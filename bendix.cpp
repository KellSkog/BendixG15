#include <stdlib.h>
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "ws2812.pio.h"

#include "bendix.hpp"

constexpr bool IS_RGBW = false;

// Data will be copied from blinkenLights to dst
std::array<uint32_t, NUM_PIXELS> blinkenLights;
volatile bool dma_done = false;
int dma_chan;

void dma_handler() {
    dma_hw->ints0 = 1u << dma_chan; // Clear the interrupt request 
    dma_done = true;
} 
bool repeating_timer_callback([[maybe_unused]] repeating_timer *t) {
    if (dma_done) {
        dma_done = false;
        dma_channel_transfer_from_buffer_now(dma_chan, blinkenLights.data(), NUM_PIXELS);
        }
    return true; // Keep repeating
}

void setup_uart() {
    // UART defines
    // By default the stdout UART is `uart0`, so we will use the second one
    constexpr uint BAUD_RATE = 115200;

    // Use pins 4 and 5 for UART1
    // Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
    constexpr uint UART0_TX_PIN = 0;
    constexpr uint UART0_RX_PIN = 1;
    // constexpr uint UART1_TX_PIN = 4;
    // constexpr uint UART1_RX_PIN = 5;

    uart_init(uart0, BAUD_RATE);
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);
    // uart_init(uart1, BAUD_RATE);
    // gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    // gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
}

int main()
{
    stdio_init_all();

    setup_uart();
    
    // Send out a string, with CR/LF conversions
    uart_puts(uart1, " Hello, UART!\r\n");
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

    PIO pio;
    uint sm;
    uint offset;

    // This will find a free pio and state machine for our program and load it for us
    // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
    // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);

    // Get a free channel, panic() if there are none
    dma_chan = dma_claim_unused_channel(true);

    // DREQ is selected, so the DMA transfers as fast TX FIFO can handle.
    dma_channel_config dcc = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&dcc, DMA_SIZE_32);
    channel_config_set_read_increment(&dcc, true);
    channel_config_set_write_increment(&dcc, false);
    channel_config_set_dreq(&dcc, pio_get_dreq(pio, sm, true));

    dma_channel_configure(
        dma_chan,           // Channel to be configured
        &dcc,            // The configuration we just created
        &pio->txf[sm],   // Write address
        blinkenLights.data(),            // The initial read address
        NUM_PIXELS,     // Number of transfers; in this case each is 4 byte.
        false            // Start later.
    );

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

// Set up DMA interrupt
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    dma_channel_set_irq0_enabled(dma_chan, true);
    
    // Set up repeating timer to start transfer @50 Hz.
    struct repeating_timer timer;
    add_repeating_timer_ms(20, repeating_timer_callback, NULL, &timer);
    
    // Start the first DMA transfer
    dma_channel_transfer_from_buffer_now(dma_chan, blinkenLights.data(), NUM_PIXELS);

    std::array<char, 256> dataBuffer;
    dataBuffer[0] = 0;
    Drum drum{};
    Input input{uart0, dataBuffer};
    Output output{blinkenLights, uart0, dataBuffer};
    Processor processor{drum, input, output};

    while (true) {
        input.scan();
        processor.process();
        output.update();
        sleep_ms(62);
    }
}

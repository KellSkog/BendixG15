#include "bendix.hpp"
#include "hardware/gpio.h"

int add(int a, int b) {
    return a + b;
}
Input::Input(){
    gpio_init(RESET_BUTTON); // Direction is input by default
}
void Input::scan() {
    static int count = 0;
    if(gpio_get(RESET_BUTTON)) {
        count++;
    }
    else {
        count = 0;
        // is_reset = false; // Let the reset handler clear
    }

    if(count > 10) {
        is_reset = true;
        count = 0;
    }
}

Output::Output(std::array<uint32_t, NUM_PIXELS> &blinkenLights) : blinkenLights_{blinkenLights} {
    gpio_init(BELL_PIN);
    gpio_set_dir(BELL_PIN, GPIO_OUT);
    gpio_put(BELL_PIN, 0);
}
void Output::ringBell() {
    gpio_put(BELL_PIN, 1);
    bellOn = true;
}
void Output::update() {
    static uint32_t count = 0;
    if(bellOn) {
        count++;
    }
    if(count >= 16) {
        count = 0;
        gpio_put(BELL_PIN, 0);
        bellOn = false;
    }
}
inline uint32_t whitePix(uint32_t intensity) {
    return intensity << 24 | intensity << 16 | intensity << 8;
}

void Processor::process() {
    static uint32_t pix = 0;
    static uint32_t bootDelay = 0;
    switch(state_) {
        // case CPU_STATE::OFF : if(input_.is_reset) {
        case CPU_STATE::OFF : bootDelay++;
            // if(input_.is_reset) {
            if(bootDelay >= 64) {
                bootDelay = 0;
            state_ = CPU_STATE::BOOTINGREAD;
            input_.is_reset = false;
            output_.ringBell();
        }
            break;
        case CPU_STATE::BOOTINGREAD : state_ = CPU_STATE::RD_CMD;
            // Read in two blocks of tape
            break;
        case CPU_STATE::RD_CMD : output_.blinkenLights_[pix] =  static_cast<uint32_t>(0);
            pix++;
            state_ = CPU_STATE::TRANSFER;
            break;
        case CPU_STATE::WAIT_TO_TRANFER : ;
            break;
        case CPU_STATE::TRANSFER :
            if(pix >= NUM_PIXELS) {
                pix = 0;
            }
            output_.blinkenLights_[pix] = whitePix(0x3);
            state_ = CPU_STATE::RD_CMD;
            break;
        case CPU_STATE::WAIT_TO_RD : ;
    }
}
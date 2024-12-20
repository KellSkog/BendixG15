#include <array>
#include "pico/stdlib.h"

enum class CPU_STATE{OFF, BOOTINGREAD, RD_CMD, WAIT_TO_TRANFER, TRANSFER, WAIT_TO_RD};
enum class SYSTEM_STATE{OFF, READY};

constexpr uint32_t RESET_BUTTON = 6; // Pin 9

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 2
#endif
constexpr uint32_t  NUM_PIXELS = 10;
constexpr uint32_t  BELL_PIN = PICO_DEFAULT_LED_PIN;


// Check testing works
int add(int a, int b);

class Drum{

};

struct Input{
    Input();
    void scan();
    volatile bool is_reset = false;
};
// struct Bell {
//     Bell(uint32_t bellPin) : bellPin_{bellPin} {}

//     uint32_t bellPin_;
// };
struct Output {
    Output(std::array<uint32_t, NUM_PIXELS> &blinkenLights);
    void ringBell();
    void update();
    std::array<uint32_t, NUM_PIXELS> &blinkenLights_;
    bool bellOn = false;
};

class Processor {
    public:
    Processor(Drum &drum, Input &input, Output &output) : drum_{drum}, input_{input}, output_{output} {}

    void process();

    private:
    CPU_STATE state_ = CPU_STATE::OFF;
    Drum &drum_;
    Input &input_;
    Output &output_;
};

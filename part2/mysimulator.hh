#ifndef _MYSIMULATOR_HH_
#define _MYSIMULATOR_HH_

#include <cstdint>

#include "timer.hh"
#include "simulator.hh"

namespace brown {

class MySimulator: public Simulator {
public:
    MySimulator(Timer& timx, Timer& timy):
        Simulator(), timx(timx), timy(timy) {};
    void hardwareLoop();
    std::int64_t getTick();

private:
    uint64_t clk = 0;
    unsigned int step = 0;
    bool init = false;
    unsigned int N;
    short x_prev = 0;
    short y_prev = 0;

    float start_rate = 400; // pulses per second
    float max_rate = 9000; // 12.5 cm/s
    float curr_rate;
    uint64_t acc_time = 100000000;

    float pulse_per_rev = 800;
    float cm_per_rev = 1.0;
    float cm_per_pulse;
    
    Timer& timx;
    Timer& timy;
};

} // namespace brown

#endif // _MYSIMULATOR_HH_

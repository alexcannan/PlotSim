#ifndef _MYSIMULATOR_HH_
#define _MYSIMULATOR_HH_

#include <cstdint>

#include "simulator.hh"

namespace brown {

/**
 * @brief Simulator subclass with direct hardware control.
 *
 * You have no further restrictions on the implementation of this class beyond
 * inheriting class `brown::Simulator`.
 */
class MySimulator: public Simulator {
public:
    MySimulator(): Simulator() {};

    /**
     * @brief Pulse generation logic for plotting.
     * @note You must override this pure virtual method.
     *
     * This method will be invoked repeatedly and indefinitely by the
     * simulator. You are recommended to handle one point at a time.
     * You can invoke `this->setpin` to generate pulses.
     */
    void hardwareLoop();

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


};

} // namespace brown

#endif // _MYSIMULATOR_HH_

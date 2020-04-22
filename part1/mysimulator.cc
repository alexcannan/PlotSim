#include <cstdlib>

#include "mysimulator.hh"
#include "ri.hh"

namespace brown {

void MySimulator::hardwareLoop() {
    // your sketchy code
    if (!this->init) {
        this->N =  sizeof state / sizeof state[0]; 
        this->cm_per_pulse = this->cm_per_rev / this->pulse_per_rev;
        this->init = true;
    }
    if (this->step < this->N) {
        short* t = state[this->step];
        printf("%u %i %i %i\n", this->step, t[0], t[1], t[2]);
        short x_ray = t[1] - this->x_prev;
        short y_ray = t[2] - this->y_prev;

        bool which_ax = (abs(x_ray) >= abs(y_ray) ? true : false); // x~T y~F
        bool x_dir = (x_ray >= 0 ? true : false);
        bool y_dir = (y_ray >= 0 ? true : false);
        float distance_cm = sqrt(pow((float)(x_ray), 2.0) + pow((float)(y_ray), 2.0)) * 0.01;
        int n_x_pulses = (int)((float)(x_ray) * 0.01 / this->cm_per_pulse);
        int n_y_pulses = (int)((float)(y_ray) * 0.01 / this->cm_per_pulse);
        short major_ax = (which_ax ? abs(n_x_pulses) : abs(n_y_pulses));
        short minor_ax = (which_ax ? abs(n_y_pulses) : abs(n_x_pulses));
        printf("%f cm in %i x pulses and %i y pulses\n", distance_cm, n_x_pulses, n_y_pulses);
        this->curr_rate = this->start_rate;
        // TODO: Figure out alg for accelerating curr_rate var
        for (int i = 0; i < abs(major_ax); i++) {
            // TODO: Figure out alg to set pulses for minor axis
            bool x_pul = (which_ax ? true : false);
            bool y_pul = (which_ax ? false : true);
            this->clk += (100000000 / this->curr_rate);
            this->setpin(this->clk, x_pul, y_pul, x_dir, y_dir, t[0]);
        }

        this->step += 1;
        this->x_prev = t[1];
        this->y_prev = t[2];
    }

}

} // namespace brown

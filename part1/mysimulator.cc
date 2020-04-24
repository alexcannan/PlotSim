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
        //printf("%u %i %i %i\n", this->step, t[0], t[1], t[2]);
        short x_ray = t[1] - this->x_prev;
        short y_ray = t[2] - this->y_prev;
        printf("Ray number %i travelling (%i, %i)\n", this->step, x_ray, y_ray);

        bool which_ax = (abs(x_ray) >= abs(y_ray) ? true : false); // x~T y~F
        bool x_dir = (x_ray >= 0 ? true : false);
        bool y_dir = (y_ray >= 0 ? true : false);
        float distance_cm = sqrt(pow((float)(x_ray), 2.0) + pow((float)(y_ray), 2.0)) * 0.01;
        int n_x_pulses = (int)((float)(x_ray) * 0.01 / this->cm_per_pulse);
        int n_y_pulses = (int)((float)(y_ray) * 0.01 / this->cm_per_pulse);
        short major_ax = (which_ax ? abs(n_x_pulses) : abs(n_y_pulses));
        short minor_ax = (which_ax ? abs(n_y_pulses) : abs(n_x_pulses));
        //printf("%f cm in %i x pulses and %i y pulses\n", distance_cm, n_x_pulses, n_y_pulses);
        this->curr_rate = this->start_rate;
        float new_rate;
        float exp_term;
        uint64_t step_start = this->clk;
        uint64_t step_half;
        uint64_t step_full;
        bool half_flag = false;
        int prev_min_pulse = -1;
        int est_min_pulse;
        printf("%d ~ x: %d dir with %i pulses; y: %d dir with %i pulses\n", which_ax, x_dir, n_x_pulses, y_dir, n_y_pulses);
        for (int i = 0; i < abs(major_ax); i++) {
            //printf("Pulse number %i rate %f clk %" PRIu64 "\n", i, this->curr_rate, this->clk);
            bool minor_pulse = false;
            est_min_pulse = (int)((float)(i * minor_ax) / (float)(major_ax) + 0.5);
            if (est_min_pulse > prev_min_pulse) {
                minor_pulse = true;
                prev_min_pulse = est_min_pulse;
            }
            bool x_pul = (which_ax ? true : minor_pulse);
            bool y_pul = (which_ax ? minor_pulse : true);
            this->clk += (uint64_t)((float)(100000000) / (float)(this->curr_rate));
            this->setpin(this->clk, x_pul, y_pul, x_dir, y_dir, t[0]);
            if (i < (abs(major_ax) / 2)) {
                // R_s - (R_s - R_0)e^-4t/T
                exp_term = 6.0 * (float)(this->clk - step_start) / (float)(this->acc_time);
                new_rate = this->max_rate - (this->max_rate - this->start_rate)*exp(-exp_term);
            } else {
                if (!half_flag) {
                    step_half = this->clk;
                    step_full = (step_half - step_start) * 2 - 1;
                    half_flag = true;
                    //printf("Half flag tripped\n");
                }
                exp_term = 6.0 * -((float)(this->clk - step_start) - (float)(step_full)) / (float)(this->acc_time);
                new_rate = this->max_rate - (this->max_rate - this->start_rate)*exp(-exp_term);
            }
            this->curr_rate = new_rate;
        }

        this->step += 1;
        this->x_prev = t[1];
        this->y_prev = t[2];
    }

}

} // namespace brown

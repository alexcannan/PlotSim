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
        this->timx.start();
    }
    if (this->step < this->N) {
        short* t = state[this->step];
        //printf("%u %i %i %i\n", this->step, t[0], t[1], t[2]);
        short x_ray = t[1] - this->x_prev;
        short y_ray = t[2] - this->y_prev;
        printf("tick number %" PRIu64 "\n", this->getTick());
        printf("Ray number %i travelling (%i, %i)\n", this->step, x_ray, y_ray);

        bool which_ax = (abs(x_ray) >= abs(y_ray) ? true : false); // x~T y~F
        bool x_dir = (x_ray >= 0 ? true : false);
        bool y_dir = (y_ray >= 0 ? true : false);
        int n_x_pulses = (int)((double)(x_ray) * 0.01 / this->cm_per_pulse);
        int n_y_pulses = (int)((double)(y_ray) * 0.01 / this->cm_per_pulse);
        short major_ax = (which_ax ? abs(n_x_pulses) : abs(n_y_pulses));
        short minor_ax = (which_ax ? abs(n_y_pulses) : abs(n_x_pulses));
        this->curr_rate = this->start_rate;
        double new_rate;
        double exp_term;
        double prev_min_double = 0;
        uint64_t step_start = this->clk;
        uint64_t step_half;
        uint64_t step_full;
        bool half_flag = false;
        int prev_min_pulse = -1;
        int prev_maj_pulse = -1;
        int est_min_pulse;
        int est_maj_pulse;
        double resolution = 100;
        // printf("%d ~ x: %d dir with %i pulses; y: %d dir with %i pulses\n", which_ax, x_dir, n_x_pulses, y_dir, n_y_pulses);
        uint64_t total_steps = (uint64_t)((double)(abs(major_ax)-1) * resolution) + 1;
        uint64_t base_clk_step = uint64_t((double)(100000000) / resolution);
        for (uint64_t i = 0; i < total_steps; i++) {
            // printf("Pulse number %i rate %f clk %" PRIu64 "\n", i, this->curr_rate, this->clk);
            bool minor_pulse = false;
            est_min_pulse = (int)((double)(i * minor_ax) / (double)(major_ax) / resolution + 0.5);
            if (est_min_pulse > prev_min_pulse && est_min_pulse != abs(minor_ax)) {
                minor_pulse = true;
                prev_min_pulse = est_min_pulse;
            }
            bool major_pulse = false;
            est_maj_pulse = (int)((double)(i) / resolution + 0.5);
            if (est_maj_pulse > prev_maj_pulse && est_maj_pulse != abs(major_ax)) {
                major_pulse = true;
                prev_maj_pulse = est_maj_pulse;
            }
            bool x_pul = (which_ax ? major_pulse : minor_pulse);
            bool y_pul = (which_ax ? minor_pulse : major_pulse);
            
            if (x_pul) {
                this->setpin(this->clk, x_pul, 0, x_dir, y_dir, t[0]);
            }
            if (y_pul) {
                this->setpin(this->clk, 0, y_pul, x_dir, y_dir, t[0]);
            }
            if (i <= (total_steps / 2)) {
                // R_s - (R_s - R_0)e^-4t/T
                exp_term = 6.0 * (double)(this->clk - step_start) / (double)(this->acc_time);
                new_rate = this->max_rate - (this->max_rate - this->start_rate)*exp(-exp_term);
            } else {
                if (!half_flag) {
                    step_half = this->clk;
                    step_full = (uint64_t)((double)(step_half - step_start) * 2);
                    half_flag = true;
                    //printf("Half flag tripped\n");
                }
                exp_term = 6.0 * -((double)(this->clk - step_start) - (double)(step_full)) / (double)(this->acc_time);
                new_rate = this->max_rate - (this->max_rate - this->start_rate)*exp(-exp_term);
            }
            this->curr_rate = new_rate;
            this->clk += (uint64_t)((double)(base_clk_step) / (double)(this->curr_rate));
        }

        this->step += 1;
        this->x_prev = t[1];
        this->y_prev = t[2];
    }
}

std::int64_t MySimulator::getTick() {
    // your amazing code
    return this->timx.t();
}

} // namespace brown

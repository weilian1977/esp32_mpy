#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
//#include "system_management.h"
#include "drv_step_motor.h"
#include "esp_err.h"
#include "esp_log.h"

#include "py/mpthread.h"


// generated using https://daycounter.com/Calculators/Sine-Generator-Calculator.phtml
static const char *TAG = "STEP TASK";
// uint16_t sin_data[] =
// {
//     500,506,512,518,525,531,537,543,549,555,561,567,573,579,585,592,
//     598,604,610,616,621,627,633,639,645,651,657,663,668,674,680,686,
//     691,697,703,708,714,719,725,730,736,741,746,752,757,762,767,773,
//     778,783,788,793,798,803,808,812,817,822,827,831,836,840,845,849,
//     854,858,862,866,870,875,879,883,887,890,894,898,902,905,909,912,
//     916,919,922,926,929,932,935,938,941,944,947,949,952,955,957,960,
//     962,964,966,969,971,973,975,977,978,980,982,983,985,986,988,989,
//     990,992,993,994,995,995,996,997,998,998,999,999,999,1000,1000,1000,
//     1000,1000,1000,1000,999,999,999,998,998,997,996,995,995,994,993,992,
//     990,989,988,986,985,983,982,980,978,977,975,973,971,969,966,964,
//     962,960,957,955,952,949,947,944,941,938,935,932,929,926,922,919,
//     916,912,909,905,902,898,894,890,887,883,879,875,870,866,862,858,
//     854,849,845,840,836,831,827,822,817,812,808,803,798,793,788,783,
//     778,773,767,762,757,752,746,741,736,730,725,719,714,708,703,697,
//     691,686,680,674,668,663,657,651,645,639,633,627,621,616,610,604,
//     598,592,585,579,573,567,561,555,549,543,537,531,525,518,512,506,
//     500,494,488,482,475,469,463,457,451,445,439,433,427,421,415,408,
//     402,396,390,384,379,373,367,361,355,349,343,337,332,326,320,314,
//     309,303,297,292,286,281,275,270,264,259,254,248,243,238,233,227,
//     222,217,212,207,202,197,192,188,183,178,173,169,164,160,155,151,
//     146,142,138,134,130,125,121,117,113,110,106,102,98,95,91,88,
//     84,81,78,74,71,68,65,62,59,56,53,51,48,45,43,40,
//     38,36,34,31,29,27,25,23,22,20,18,17,15,14,12,11,
//     10,8,7,6,5,5,4,3,2,2,1,1,1,0,0,0,
//     0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,8,
//     10,11,12,14,15,17,18,20,22,23,25,27,29,31,34,36,
//     38,40,43,45,48,51,53,56,59,62,65,68,71,74,78,81,
//     84,88,91,95,98,102,106,110,113,117,121,125,130,134,138,142,
//     146,151,155,160,164,169,173,178,183,188,192,197,202,207,212,217,
//     222,227,233,238,243,248,254,259,264,270,275,281,286,292,297,303,
//     309,314,320,326,332,337,343,349,355,361,367,373,379,384,390,396,
//     402,408,415,421,427,433,439,445,451,457,463,469,475,482,488,494,
// };
uint16_t sin_data[] =
{
    500,512,525,537,549,561,573,585,598,610,621,633,645,657,668,680,
    691,703,714,725,736,746,757,767,778,788,798,808,817,827,836,845,
    854,862,870,879,887,894,902,909,916,922,929,935,941,947,952,957,
    962,966,971,975,978,982,985,988,990,993,995,996,998,999,999,1000,
    1000,1000,999,999,998,996,995,993,990,988,985,982,978,975,971,966,
    962,957,952,947,941,935,929,922,916,909,902,894,887,879,870,862,
    854,845,836,827,817,808,798,788,778,767,757,746,736,725,714,703,
    691,680,668,657,645,633,621,610,598,585,573,561,549,537,525,512,
    500,488,475,463,451,439,427,415,402,390,379,367,355,343,332,320,
    309,297,286,275,264,254,243,233,222,212,202,192,183,173,164,155,
    146,138,130,121,113,106,98,91,84,78,71,65,59,53,48,43,
    38,34,29,25,22,18,15,12,10,7,5,4,2,1,1,0,
    0,0,1,1,2,4,5,7,10,12,15,18,22,25,29,34,
    38,43,48,53,59,65,71,78,84,91,98,106,113,121,130,138,
    146,155,164,173,183,192,202,212,222,233,243,254,264,275,286,297,
    309,320,332,343,355,367,379,390,402,415,427,439,451,463,475,488,
};

motion_data_type motion_data = {0};
int32_t motion_max_speed = 700 * STEP_SUBDIVISION;

static void sleep_pin_init(void)
{ 
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins
    io_conf.pin_bit_mask = SLEEP_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

static void set_spwm_freq(ledc_timer_t timer_num, uint32_t freq_hz)
{
    ledc_set_freq(LEDC_LOW_SPEED_MODE, timer_num, freq_hz);
    if(MOTOR_LEFT_TIME == timer_num)
    {
        motion_data.motor_data[MOTOR_LEFT].current_spwm_freq = freq_hz;
    }
    else
    {
        motion_data.motor_data[MOTOR_RIGHT].current_spwm_freq = freq_hz;
    }
}


static void spwm_stop(motor_configure_type motor)
{
    if(MOTOR_LEFT == motor)
    {
       ledc_timer_pause(MOTOR_LEFT_SPEED_MODE, MOTOR_LEFT_TIME);
    }
    else
    {
       ledc_timer_pause(MOTOR_RIGHT_SPEED_MODE, MOTOR_RIGHT_TIME);
    }
}

static void spwm_resume(motor_configure_type motor)
{
    if(MOTOR_LEFT == motor)
    {
       ledc_timer_resume(MOTOR_LEFT_SPEED_MODE, MOTOR_LEFT_TIME);
    }
    else
    {
       ledc_timer_resume(MOTOR_RIGHT_SPEED_MODE, MOTOR_RIGHT_TIME);
    }
}

static void step_motor_spwm_init(void)
{
/*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    int ch;
    ledc_timer_config_t ledc_timer = 
    {
        .duty_resolution = LEDC_TIMER_10_BIT, // resolution of PWM duty
        .freq_hz = DEFAULT_MIN_SPEED,         // frequency of PWM signal
        .speed_mode = LEDC_LOW_SPEED_MODE,    // timer mode
        .timer_num = MOTOR_LEFT_TIME,         // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    // Set configuration of timer0 timer1 for high speed channels
    ledc_timer_config(&ledc_timer);
    ledc_timer.speed_mode = MOTOR_LEFT_SPEED_MODE;
    ledc_timer.timer_num = MOTOR_LEFT_TIME;
    ledc_timer_config(&ledc_timer);
    ledc_timer.speed_mode = MOTOR_RIGHT_SPEED_MODE;
    ledc_timer.timer_num = MOTOR_RIGHT_TIME;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t motor_pwm_channel[MOTOR_CHANNEL_NUM] = 
    {
        {
            .channel    = MOTOR_LEFT_INA_CHANNEL,
            .duty       = 0,
            .gpio_num   = MOTOR_LEFT_INA_PIN,
            .speed_mode = MOTOR_LEFT_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = MOTOR_LEFT_TIME
        },
        {
            .channel    = MOTOR_LEFT_INB_CHANNEL,
            .duty       = 0,
            .gpio_num   = MOTOR_LEFT_INB_PIN,
            .speed_mode = MOTOR_LEFT_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = MOTOR_LEFT_TIME
        },
        {
            .channel    = MOTOR_RIGHT_INA_CHANNEL,
            .duty       = 0,
            .gpio_num   = MOTOR_RIGHT_INA_PIN,
            .speed_mode = MOTOR_RIGHT_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = MOTOR_RIGHT_TIME
        },
        {
            .channel    = MOTOR_RIGHT_INB_CHANNEL,
            .duty       = 0,
            .gpio_num   = MOTOR_RIGHT_INB_PIN,
            .speed_mode = MOTOR_RIGHT_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = MOTOR_RIGHT_TIME
        },
    };

    // Set LED Controller with previously prepared configuration
    for(ch = 0; ch < MOTOR_CHANNEL_NUM; ch++) 
    {
        ledc_channel_config(&motor_pwm_channel[ch]);
    }
    REG_CLR_BIT(LEDC_INT_ENA_REG, LEDC_LSTIMER0_OVF_INT_ENA);
    REG_CLR_BIT(LEDC_INT_ENA_REG, LEDC_LSTIMER1_OVF_INT_ENA);
    spwm_stop(MOTOR_LEFT);
    spwm_stop(MOTOR_RIGHT);
}

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static long distance_to_go(motor_configure_type motor)
{
    if(motor < MOTOR_MAX)
    {
        return motion_data.motor_data[motor].target_pos - motion_data.motor_data[motor].current_pos;
    }
    else
    {
        return 0;
    }
}

static void step_left_start(void)
{
    int16_t table_pos_ina;
    int16_t table_pos_inb;

    step_sleep(MOTOR_LEFT, false);
    spwm_resume(MOTOR_LEFT);

    table_pos_ina = motion_data.motor_data[MOTOR_LEFT].step_pos % MICRO_STEP;
    set_spwm_freq(MOTOR_LEFT_TIME, DEFAULT_MIN_SPEED);

    // update duty, shift the duty 4 bits to the left due to ESP32 register format
    // REG_WRITE(LEDC_LSCH0_DUTY_REG, map(sin_data[2 * table_pos_ina], 0, 1000, 102, 921) << 4);
    // REG_SET_BIT(LEDC_LSCH0_CONF1_REG, LEDC_DUTY_START_LSCH0);

    ledc_set_duty(MOTOR_LEFT_SPEED_MODE, MOTOR_LEFT_INA_CHANNEL, map(sin_data[2 * table_pos_ina], 0, 1000, 102, 921));

    if(table_pos_ina < (MICRO_STEP >> 2) * 3)
    {
        table_pos_inb = 2 * (table_pos_ina + (MICRO_STEP >> 2));
    }
    else
    {
        table_pos_inb = 2 * (table_pos_ina - (MICRO_STEP >> 2) * 3);
    }
    // update duty, shift the duty 4 bits to the left due to ESP32 register format
    // REG_WRITE(LEDC_LSCH1_DUTY_REG, map(sin_data[table_pos_inb], 0, 1000, 102, 921) << 4);
    // REG_SET_BIT(LEDC_LSCH1_CONF1_REG, LEDC_DUTY_START_LSCH1);

    ledc_set_duty(MOTOR_LEFT_SPEED_MODE, MOTOR_LEFT_INB_CHANNEL, map(sin_data[table_pos_inb], 0, 1000, 102, 921));

    REG_SET_BIT(LEDC_INT_ENA_REG, LEDC_LSTIMER0_OVF_INT_ENA);
}

static void step_right_start(void)
{
    int16_t table_pos_ina;
    int16_t table_pos_inb;

    step_sleep(MOTOR_RIGHT, false);
    spwm_resume(MOTOR_RIGHT);

    table_pos_ina = motion_data.motor_data[MOTOR_RIGHT].step_pos % MICRO_STEP;
    set_spwm_freq(MOTOR_RIGHT_TIME, DEFAULT_MIN_SPEED);
    // update duty, shift the duty 4 bits to the left due to ESP32 register format
    // REG_WRITE(LEDC_LSCH2_DUTY_REG, map(sin_data[2 * table_pos_ina], 0, 1000, 102, 921) << 4);
    // REG_SET_BIT(LEDC_LSCH2_CONF1_REG, LEDC_DUTY_START_LSCH2);

    ledc_set_duty(MOTOR_RIGHT_SPEED_MODE, MOTOR_RIGHT_INA_CHANNEL, map(sin_data[2 * table_pos_ina], 0, 1000, 102, 921));

    if(table_pos_ina < (MICRO_STEP >> 2) * 3)
    {
        table_pos_inb = 2 * (table_pos_ina + (MICRO_STEP >> 2));
    }
    else
    {
        table_pos_inb = 2 * (table_pos_ina - (MICRO_STEP >> 2) * 3);
    }
    // update duty, shift the duty 4 bits to the left due to ESP32 register format
    // REG_WRITE(LEDC_LSCH3_DUTY_REG, map(sin_data[table_pos_inb], 0, 1000, 102, 921) << 4);
    // REG_SET_BIT(LEDC_LSCH3_CONF1_REG, LEDC_DUTY_START_LSCH3);

    ledc_set_duty(MOTOR_RIGHT_SPEED_MODE, MOTOR_RIGHT_INB_CHANNEL, map(sin_data[table_pos_inb], 0, 1000, 102, 921));

    REG_SET_BIT(LEDC_INT_ENA_REG, LEDC_LSTIMER1_OVF_INT_ENA);
}

static void motor_move_to_cfg(motor_configure_type motor, long absolute)
{
    motion_data.motor_data[motor].motion_status = POS_MOVE;
    if(motion_data.motor_data[motor].target_pos != absolute)
    {
        motion_data.motor_data[motor].target_pos = absolute;
    }
    if(motion_data.motor_data[motor].target_pos != motion_data.motor_data[motor].current_pos)
    {
        compute_new_speed(motor);
        step_start(motor);
    }
}


static void compute_motor_new_speed(motor_configure_type motor)
{
    long motor_distance_to_go = distance_to_go(motor);
    // long motor_steps_to_stop = (long)(pow(motion_data.motor_data[motor].speed, 2)) / (2 * motion_data.motor_data[motor].acceleration);
    long motor_steps_to_stop = (long)((motion_data.motor_data[motor].speed * motion_data.motor_data[motor].speed)) / \
                                           (2 * motion_data.motor_data[motor].acceleration);

    if((motion_data.motor_data[motor].motion_status != STOP_MOVE) && \
       (abs(motor_distance_to_go) < 1) && \
       (motor_steps_to_stop <= (DEFAULT_MIN_SPEED * DEFAULT_MIN_SPEED / (2 * motion_data.motor_data[motor].acceleration))))
    {
        // We are at the target and its time to stop
        motion_data.motor_data[motor].speed = 0;
        motion_data.motor_data[motor]._n = 0;
        if(motor == MOTOR_LEFT)
        {
            REG_CLR_BIT(LEDC_INT_ENA_REG, LEDC_LSTIMER0_OVF_INT_ENA);
        }
        else
        {
            REG_CLR_BIT(LEDC_INT_ENA_REG, LEDC_LSTIMER1_OVF_INT_ENA);

        }
        spwm_stop(motor);
        step_sleep(motor, true);
        motion_data.motor_data[motor].motion_status = STOP_MOVE;
        return;
    }
    if(motor_distance_to_go > 0)
    {
        // We are anticlockwise from the target
        // Need to go clockwise from here, maybe decelerate now
        if(motion_data.motor_data[motor]._n > 0)
        {
            // Currently accelerating, need to decel now? Or maybe going the wrong way?
            if((motor_steps_to_stop >= motor_distance_to_go) || motion_data.motor_data[motor].dir == DIR_CCW)
            {
                motion_data.motor_data[motor]._n = -motor_steps_to_stop;              // Start deceleration
            }
        }
        else if(motion_data.motor_data[motor]._n < 0)
        {
            // Currently decelerating, need to accel again?
            if((motor_steps_to_stop < motor_distance_to_go) && motion_data.motor_data[motor].dir == DIR_CW)
            {
                motion_data.motor_data[motor]._n = -motion_data.motor_data[motor]._n; // Start accceleration
            }
        }

    }
    else if(motor_distance_to_go < 0)
    {
        // We are clockwise from the target
        // Need to go anticlockwise from here, maybe decelerate
        if(motion_data.motor_data[motor]._n > 0)
        {
            // Currently accelerating, need to decel now? Or maybe going the wrong way?
            if((motor_steps_to_stop >= -motor_distance_to_go) || motion_data.motor_data[motor].dir  == DIR_CW)
            {
                motion_data.motor_data[motor]._n  = -motor_steps_to_stop;             // Start deceleration
            }
        }
        else if(motion_data.motor_data[motor]._n < 0)
        {
            // Currently decelerating, need to accel again?
            if((motor_steps_to_stop < -motor_distance_to_go) && motion_data.motor_data[motor].dir == DIR_CCW)
            {
                motion_data.motor_data[motor]._n = -motion_data.motor_data[motor]._n; // Start accceleration
            }
        }
    }
    // Need to accelerate or decelerate
    if(motion_data.motor_data[motor]._n == 0)
    { 
        // First step from stopped
        motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._c0;
        motion_data.motor_data[motor].dir = (motor_distance_to_go > 0) ? DIR_CW : DIR_CCW;
    }
    else
    {
        // Subsequent step. Works for accel (n is +_ve) and decel (n is -ve).
        int32_t temp_ve;
        // temp_ve = 2 * motion_data.motor_data[MOTOR_LEFT]._cn / ((4 * motion_data.motor_data[MOTOR_LEFT]._n) + 1);
        if(motion_data.motor_data[motor]._n > 0)
        {
            temp_ve = max((2 * motion_data.motor_data[motor]._cn) / ((4 * motion_data.motor_data[motor]._n) + 1), 1);
        }
        else
        {
            temp_ve = min((2 * motion_data.motor_data[motor]._cn) / ((4 * motion_data.motor_data[motor]._n) + 1), -1);
        }
        motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._cn - temp_ve;    //Equation 13
        motion_data.motor_data[motor]._cn = max(motion_data.motor_data[motor]._cn, motion_data.motor_data[motor]._cmin);
    }
    motion_data.motor_data[motor]._n++ ;
    motion_data.motor_data[motor].speed = 1000000000 / motion_data.motor_data[motor]._cn;
    if(motion_data.motor_data[motor].dir == DIR_CCW)
    {
        motion_data.motor_data[motor].speed = -motion_data.motor_data[motor].speed;
    }
}

static void compute_motor_new_speed_by_speed(motor_configure_type motor)
{
    if(motion_data.motor_data[motor]._n == 0)
    { 
        // First step from stopped
        motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._c0;
    }
    else
    {
        // Subsequent step. Works for accel (n is +_ve) and decel (n is -ve).
        int32_t temp_ve;
        if(abs(motion_data.motor_data[motor].speed) < abs(motion_data.motor_data[motor].target_speed))
        {
            temp_ve = max((2 * motion_data.motor_data[motor]._cn) / ((4 * motion_data.motor_data[motor]._n) + 1), 1);
            motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._cn - temp_ve;      //Equation 13
            motion_data.motor_data[motor]._cn = max(motion_data.motor_data[motor]._cn, (1000000000 / abs(motion_data.motor_data[motor].target_speed)));
        }
        else
        {
            temp_ve = min((2 * motion_data.motor_data[motor]._cn) / ((4 * motion_data.motor_data[motor]._n) + 1), -1);
            motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._cn - temp_ve;      //Equation 13
            motion_data.motor_data[motor]._cn = min(motion_data.motor_data[motor]._cn, (1000000000 / abs(motion_data.motor_data[motor].target_speed)));
        }
    }
    motion_data.motor_data[motor].speed = 1000000000 / motion_data.motor_data[motor]._cn;
    if(motion_data.motor_data[motor].dir == DIR_CCW)
    {
        motion_data.motor_data[motor].speed = -motion_data.motor_data[motor].speed;
    }
    if(motion_data.motor_data[motor].target_speed != motion_data.motor_data[motor].speed)
    {
        motion_data.motor_data[motor]._n++;
    }
}

void IRAM_ATTR spwm_timer_overflow_isr(void *arg)
{
    int16_t table_pos_ina;
    int16_t table_pos_inb;
    // clear the interrupt
    if(REG_GET_BIT(LEDC_INT_RAW_REG, LEDC_LSTIMER0_OVF_INT_CLR) != 0)
    {
        REG_SET_BIT(LEDC_INT_CLR_REG, LEDC_LSTIMER0_OVF_INT_CLR);

        if(motion_data.motor_data[MOTOR_LEFT].dir == DIR_CW)
        {
            motion_data.motor_data[MOTOR_LEFT].step_pos = motion_data.motor_data[MOTOR_LEFT].step_pos + 1;
            motion_data.motor_data[MOTOR_LEFT].current_pos = motion_data.motor_data[MOTOR_LEFT].current_pos + 1;
            if(motion_data.motor_data[MOTOR_LEFT].step_pos >= STEP_POS_MAX)
            {
                motion_data.motor_data[MOTOR_LEFT].step_pos = 0;
            }
        }
        else
        {
            motion_data.motor_data[MOTOR_LEFT].step_pos = motion_data.motor_data[MOTOR_LEFT].step_pos - 1;
            motion_data.motor_data[MOTOR_LEFT].current_pos = motion_data.motor_data[MOTOR_LEFT].current_pos - 1;
            if(motion_data.motor_data[MOTOR_LEFT].step_pos <= -STEP_POS_MAX)
            {
                motion_data.motor_data[MOTOR_LEFT].step_pos = 0;
            }
        }

        compute_new_speed(MOTOR_LEFT);

        table_pos_ina = motion_data.motor_data[MOTOR_LEFT].step_pos % MICRO_STEP;
        if(table_pos_ina < 0)
        {
            table_pos_ina = table_pos_ina + MICRO_STEP;
        }
        // update duty, shift the duty 4 bits to the left due to ESP32 register format
        // REG_WRITE(LEDC_LSCH0_DUTY_REG, map(sin_data[2 * table_pos_ina], 0, 1000, 102, 921) << 4);
        // REG_SET_BIT(LEDC_LSCH0_CONF1_REG, LEDC_DUTY_START_LSCH0);

        ledc_set_duty(MOTOR_LEFT_SPEED_MODE, MOTOR_LEFT_INA_CHANNEL, map(sin_data[2 * table_pos_ina], 0, 1000, 102, 921));

        if(table_pos_ina < (MICRO_STEP >> 2) * 3)
        {
            table_pos_inb = 2 * (table_pos_ina + (MICRO_STEP >> 2));
        }
        else
        {
            table_pos_inb = 2 * (table_pos_ina - (MICRO_STEP >> 2) * 3);
        }
        // update duty, shift the duty 4 bits to the left due to ESP32 register format
        // REG_WRITE(LEDC_LSCH1_DUTY_REG, map(sin_data[table_pos_inb], 0, 1000, 102, 921) << 4);
        // REG_SET_BIT(LEDC_LSCH1_CONF1_REG, LEDC_DUTY_START_LSCH1);

        ledc_set_duty(MOTOR_LEFT_SPEED_MODE, MOTOR_LEFT_INB_CHANNEL, map(sin_data[table_pos_inb], 0, 1000, 102, 921));
    }

    if(REG_GET_BIT(LEDC_INT_RAW_REG, LEDC_LSTIMER1_OVF_INT_CLR) != 0)
    {

        REG_SET_BIT(LEDC_INT_CLR_REG, LEDC_LSTIMER1_OVF_INT_CLR);

        if(motion_data.motor_data[MOTOR_RIGHT].dir == DIR_CW)
        {
            motion_data.motor_data[MOTOR_RIGHT].step_pos = motion_data.motor_data[MOTOR_RIGHT].step_pos + 1;
            motion_data.motor_data[MOTOR_RIGHT].current_pos = motion_data.motor_data[MOTOR_RIGHT].current_pos + 1;
            if(motion_data.motor_data[MOTOR_RIGHT].step_pos >= STEP_POS_MAX)
            {
                motion_data.motor_data[MOTOR_RIGHT].step_pos = 0;
            }
        }
        else
        {
            motion_data.motor_data[MOTOR_RIGHT].step_pos = motion_data.motor_data[MOTOR_RIGHT].step_pos - 1;
            motion_data.motor_data[MOTOR_RIGHT].current_pos = motion_data.motor_data[MOTOR_RIGHT].current_pos - 1;
            if(motion_data.motor_data[MOTOR_RIGHT].step_pos <= -STEP_POS_MAX)
            {
                motion_data.motor_data[MOTOR_RIGHT].step_pos = 0;
            }
        }

        compute_new_speed(MOTOR_RIGHT);

        table_pos_ina = motion_data.motor_data[MOTOR_RIGHT].step_pos % MICRO_STEP;
        if(table_pos_ina < 0)
        {
            table_pos_ina = table_pos_ina + MICRO_STEP;
        }
        // update duty, shift the duty 4 bits to the left due to ESP32 register format
        // REG_WRITE(LEDC_LSCH2_DUTY_REG, map(sin_data[2 * table_pos_ina], 0, 1000, 102, 921) << 4);
        // REG_SET_BIT(LEDC_LSCH2_CONF1_REG, LEDC_DUTY_START_LSCH2);

        ledc_set_duty(MOTOR_RIGHT_SPEED_MODE, MOTOR_RIGHT_INA_CHANNEL, map(sin_data[2 * table_pos_ina], 0, 1000, 102, 921));

        if(table_pos_ina < (MICRO_STEP >> 2) * 3)
        {
            table_pos_inb = 2 * (table_pos_ina + (MICRO_STEP >> 2));
        }
        else
        {
            table_pos_inb = 2 * (table_pos_ina - (MICRO_STEP >> 2) * 3);
        }
        // update duty, shift the duty 4 bits to the left due to ESP32 register format
        // REG_WRITE(LEDC_LSCH3_DUTY_REG, map(sin_data[table_pos_inb], 0, 1000, 102, 921) << 4);
        // REG_SET_BIT(LEDC_LSCH3_CONF1_REG, LEDC_DUTY_START_LSCH3);

        ledc_set_duty(MOTOR_RIGHT_SPEED_MODE, MOTOR_RIGHT_INB_CHANNEL, map(sin_data[table_pos_inb], 0, 1000, 102, 921));
    }
}

void step_motor_init(void)
{
    sleep_pin_init();
    step_sleep(MOTOR_LEFT, true);
    step_sleep(MOTOR_RIGHT, true);
    step_motor_spwm_init();

    // register overflow interrupt handler for pwm timer
    // ESP_INTR_FLAG_LEVEL1 ESP_INTR_FLAG_IRAM
    ledc_isr_register(spwm_timer_overflow_isr, NULL, ESP_INTR_FLAG_LEVEL1, NULL);

    motion_data.motor_data[MOTOR_LEFT].step_pos = 0;
    motion_data.motor_data[MOTOR_RIGHT].step_pos = 0;
    motion_data.motor_data[MOTOR_LEFT].dir = DIR_CW;
    motion_data.motor_data[MOTOR_RIGHT].dir = DIR_CW;
    motion_data.motor_data[MOTOR_LEFT]._n = 0;
    motion_data.motor_data[MOTOR_RIGHT]._n = 0;
    
    motion_data.motor_data[MOTOR_LEFT].motion_status = STOP_MOVE;
    motion_data.motor_data[MOTOR_RIGHT].motion_status = STOP_MOVE;
}

void step_sleep(motor_configure_type motor, bool enable)
{
    if(MOTOR_LEFT == motor)
    {
        gpio_set_level(MOTOR_LEFT_SLEEP_PIN, !enable);
    }
    else if(MOTOR_RIGHT == motor)
    {
        gpio_set_level(MOTOR_RIGHT_SLEEP_PIN, !enable);
    }
}

void step_start(motor_configure_type motor)
{
    if(MOTOR_LEFT == motor)
    {
        step_left_start();
    }
    else
    {
        step_right_start();
    }
}

void compute_new_speed(motor_configure_type motor)
{
    if(motion_data.motor_data[motor].motion_status == SPEED_MOVE)
    {
        compute_motor_new_speed_by_speed(motor);
    }
    else
    {
        compute_motor_new_speed(motor);
    }
}

void motor_set_move_speed_max(int32_t max_speed)
{
    motion_max_speed = max_speed;
    return;
}

void set_max_speed(motor_configure_type motor, int32_t speed)
{
    int32_t speed_temp;
    if(speed < 0)
    {
        speed_temp = -speed;
    }
    else
    {
        speed_temp = speed;
    }
    if(motion_data.motor_data[motor].max_speed != speed_temp)
    {
        motion_data.motor_data[motor].max_speed = speed_temp;
        motion_data.motor_data[motor]._cmin = 1000000000 / speed_temp;
        // Recompute _n from current speed and adjust speed if accelerating or cruising
        if(motion_data.motor_data[motor]._n > 0)
        {
            motion_data.motor_data[motor]._n  = (int32_t)round((motion_data.motor_data[motor].speed * motion_data.motor_data[motor].speed) / \
                                                      (2.0 * motion_data.motor_data[motor].acceleration)); // Equation 16
            ESP_LOGD(TAG, "set_max_speed(%d)\r\n", speed);
            compute_new_speed(motor);
        }
    }
}

void set_acceleration(motor_configure_type motor, int32_t acceleration)
{
    int32_t acceleration_temp;
    if(acceleration == 0)
    {
        return;
    }
    if(acceleration < 0)
    {
        acceleration_temp = -acceleration;
    }
    else
    {
        acceleration_temp = acceleration;
    }
    if(motion_data.motor_data[motor].acceleration != acceleration_temp)
    {
        // Recompute _n per Equation 17
        motion_data.motor_data[motor]._n = (motion_data.motor_data[motor]._n * (motion_data.motor_data[motor].acceleration  / acceleration_temp));
        // New c0 per Equation 7, with correction per Equation 15
        motion_data.motor_data[motor]._c0 = (int32_t)round(0.676 * sqrt(2.0 / acceleration_temp) * 1000000000.0); // Equation 15
        motion_data.motor_data[motor].acceleration = acceleration_temp;
        ESP_LOGI(TAG, "set_acceleration(%d)\r\n", motion_data.motor_data[motor]._c0);
        compute_new_speed(motor);
    }
}

long get_target_position(motor_configure_type motor)
{
    if((motor < MOTOR_MAX))
    {
        return motion_data.motor_data[motor].target_pos;
    }
    else
    {
        return 0;
    }
}

long get_current_position(motor_configure_type motor)
{
    if((motor < MOTOR_MAX))
    {
        return motion_data.motor_data[motor].current_pos;
    }
    else
    {
        return 0;
    }
}

void set_current_position(motor_configure_type motor, long position)
{
    if((motor < MOTOR_MAX))
    {
        motion_data.motor_data[motor].target_pos = position;
        motion_data.motor_data[motor].current_pos = position;
        motion_data.motor_data[motor]._n = 0;
        motion_data.motor_data[motor].speed = 0;
    }
}

void motor_move_to(motor_configure_type motor, long absolute, bool sync)
{
    if(motion_data.motor_data[motor].current_pos == absolute)
    {
        motor_stop(motor);
        return;
    }
    set_max_speed(motor, motion_max_speed);
    set_acceleration(motor, 4000 * STEP_SUBDIVISION);
    xSemaphoreTake(motion_data.motion_task_init_mutex, portMAX_DELAY);
    motor_move_to_cfg(motor, absolute);
    xSemaphoreGive(motion_data.motion_task_init_mutex);
    if(sync == true)
    {
        MP_THREAD_GIL_EXIT();
        while(motion_data.motor_data[motor].motion_status != STOP_MOVE)
        {
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        MP_THREAD_GIL_ENTER();
    }
}

void motor_move(motor_configure_type motor, long relative, bool sync)
{
    long absolute_position;
    set_current_position(motor, 0);
    if(MOTOR_LEFT == motor)
    {
        absolute_position = motion_data.motor_data[MOTOR_LEFT].current_pos + relative;
    }
    else
    {
        absolute_position = motion_data.motor_data[MOTOR_RIGHT].current_pos + relative;
    }
    motor_move_to(motor, absolute_position, sync);
}

void motor_set_speed(motor_configure_type motor, int32_t run_speed)
{
    set_max_speed(motor, 1200 * STEP_SUBDIVISION);
    set_acceleration(motor, 4000 * STEP_SUBDIVISION); 
    xSemaphoreTake(motion_data.motion_task_init_mutex, portMAX_DELAY); 
    if(run_speed == 0)
    {
        motor_stop(motor);
    }
    else
    {   
        if((motion_data.motor_data[motor].speed / run_speed) < 0)
        {
            motor_stop(motor);
        }
        motion_data.motor_data[motor].target_speed = run_speed;
        ESP_LOGD(TAG, "target speed:%d\r\n", motion_data.motor_data[motor].target_speed);
        motion_data.motor_data[motor].dir = (motion_data.motor_data[motor].target_speed > 0) ? DIR_CW : DIR_CCW;
        motion_data.motor_data[motor].motion_status = SPEED_MOVE;
        if(motor == MOTOR_LEFT)
        {
            step_left_start();
        }
        else
        {
            step_right_start();
        }
    }
    xSemaphoreGive(motion_data.motion_task_init_mutex);
}

int32_t motor_get_speed(motor_configure_type motor)
{
    return motion_data.motor_data[motor].speed;
}

void motor_run_speed(int32_t left_run_speed, int32_t right_run_speed)
{
    motor_set_speed(MOTOR_LEFT, left_run_speed);
    motor_set_speed(MOTOR_RIGHT, right_run_speed);
}

void motor_stop(motor_configure_type motor)
{
    if(MOTOR_LEFT == motor)
    {
        motion_data.motor_data[MOTOR_LEFT].speed = 0;
        motion_data.motor_data[MOTOR_LEFT]._n = 0;
        motion_data.motor_data[MOTOR_LEFT].target_pos = motion_data.motor_data[MOTOR_LEFT].current_pos;
        REG_CLR_BIT(LEDC_INT_ENA_REG, LEDC_LSTIMER0_OVF_INT_ENA);
        spwm_stop(MOTOR_LEFT);
        step_sleep(MOTOR_LEFT, true);
        motion_data.motor_data[MOTOR_LEFT].motion_status = STOP_MOVE;
    }
    else
    {
        motion_data.motor_data[MOTOR_RIGHT].speed = 0;
        motion_data.motor_data[MOTOR_RIGHT]._n = 0;
        motion_data.motor_data[MOTOR_RIGHT].target_pos = motion_data.motor_data[MOTOR_RIGHT].current_pos;
        REG_CLR_BIT(LEDC_INT_ENA_REG, LEDC_LSTIMER1_OVF_INT_ENA);
        spwm_stop(MOTOR_RIGHT);
        step_sleep(MOTOR_RIGHT, true);
        motion_data.motor_data[MOTOR_RIGHT].motion_status = STOP_MOVE;
    }
}

void motor_run(void)
{
    static long previous_motor_left_pos = 0;
    static long previous_motor_right_pos = 0;
    if(motion_data.motor_data[MOTOR_LEFT].motion_status != STOP_MOVE)
    {
        if(abs(previous_motor_left_pos - motion_data.motor_data[MOTOR_LEFT].current_pos) >= 64)
        {
            volatile uint32_t left_freq = abs(motion_data.motor_data[MOTOR_LEFT].speed);
            if(left_freq < DEFAULT_MIN_SPEED)
            {
                left_freq = DEFAULT_MIN_SPEED;
            }
            else if(left_freq > DEFALUT_MAX_SPEED)
            {
                left_freq = DEFALUT_MAX_SPEED;
            }
            if(motion_data.motor_data[MOTOR_LEFT].current_spwm_freq != left_freq)
            {
                ESP_LOGD(TAG, "left speed:%d", left_freq);
                set_spwm_freq(MOTOR_LEFT_TIME, left_freq);
            }
            previous_motor_left_pos = motion_data.motor_data[MOTOR_LEFT].current_pos;
            ESP_LOGD(TAG, "left speed:%d\r\n", (int32_t)motion_data.motor_data[MOTOR_LEFT].speed);
        }
    }
    else
    {
        previous_motor_left_pos = 0;
        //vTaskDelay(SYSTEM_POLLING_TIME * 2 / portTICK_PERIOD_MS);
        vTaskDelay(10 * 2 / portTICK_PERIOD_MS);
    }

    if(motion_data.motor_data[MOTOR_RIGHT].motion_status != STOP_MOVE)
    {
        if(abs(previous_motor_right_pos - motion_data.motor_data[MOTOR_RIGHT].current_pos) >= 64)
        {
            volatile uint32_t right_freq = abs(motion_data.motor_data[MOTOR_RIGHT].speed);
            if(right_freq < DEFAULT_MIN_SPEED)
            {
                right_freq = DEFAULT_MIN_SPEED;
            }
            else if(right_freq > DEFALUT_MAX_SPEED)
            {
                right_freq = DEFALUT_MAX_SPEED;
            }
            if(motion_data.motor_data[MOTOR_RIGHT].current_spwm_freq != right_freq)
            {
                ESP_LOGD(TAG, "right speed:%d", right_freq);
                set_spwm_freq(MOTOR_RIGHT_TIME, right_freq);
            }
            ESP_LOGD(TAG, "right speed:%d\r\n", (int32_t)motion_data.motor_data[MOTOR_RIGHT].speed);
            previous_motor_right_pos = motion_data.motor_data[MOTOR_RIGHT].current_pos;
        }
    }
    else
    {
        previous_motor_right_pos = 0;
        //vTaskDelay(SYSTEM_POLLING_TIME * 2 / portTICK_PERIOD_MS);
        vTaskDelay(10 * 2 / portTICK_PERIOD_MS);
    }
}

void step_motor_task(void *pvParameter)
{
    motion_data.motion_task_init_mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(motion_data.motion_task_init_mutex);
    if(motion_data.motion_task_init_mutex == NULL)
    {
        ESP_LOGE(TAG, "step motor task init mutex failed!");
    }

    xSemaphoreTake(motion_data.motion_task_init_mutex, portMAX_DELAY);
    step_motor_init();
    set_max_speed(MOTOR_LEFT, 1000 * STEP_SUBDIVISION);
    set_max_speed(MOTOR_RIGHT, 1000 * STEP_SUBDIVISION);
    set_acceleration(MOTOR_LEFT, DEFALUT_ACCELERATION);
    set_acceleration(MOTOR_RIGHT, DEFALUT_ACCELERATION);
    set_current_position(MOTOR_LEFT, 0);
    set_current_position(MOTOR_RIGHT, 0);
    xSemaphoreGive(motion_data.motion_task_init_mutex);
    while(true)
    {
        motor_run();
    }
}

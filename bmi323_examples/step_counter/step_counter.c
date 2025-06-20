/**\
 * Copyright (c) 2024 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmi323.h"
#include "common.h"

/******************************************************************************/
/*!         Static Function Declaration                                       */

/*!
 *  @brief This internal API is used to set configurations for step counter interrupt.
 *
 *  @param[in] dev       : Structure instance of bmi3_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_feature_config(struct bmi3_dev *dev);

/******************************************************************************/
/*!            Functions                                                      */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of API are returned to this variable. */
    int8_t rslt;

    /* Variable to get step counter interrupt status. */
    uint16_t int_status = 0;

    /* Sensor initialization configuration. */
    struct bmi3_dev dev = { 0 };

    /* Feature enable initialization. */
    struct bmi3_feature_enable feature = { 0 };

    /* Interrupt mapping structure. */
    struct bmi3_map_int map_int = { 0 };

    /* Structure to store sensor data */
    struct bmi3_sensor_data sensor_data = { 0 };

    /* Sensor type of sensor to get data. */
    sensor_data.type = BMI323_STEP_COUNTER;

    /* Function to select interface between SPI and I2C, according to that the device structure gets updated.
     * Interface reference is given as a parameter
     * For I2C : BMI3_I2C_INTF
     * For SPI : BMI3_SPI_INTF
     */
    rslt = bmi3_interface_init(&dev, BMI3_SPI_INTF);
    bmi3_error_codes_print_result("bmi3_interface_init", rslt);

    if (rslt == BMI323_OK)
    {
        /* Initialize bmi323. */
        rslt = bmi323_init(&dev);
        bmi3_error_codes_print_result("bmi323_init", rslt);

        if (rslt == BMI323_OK)
        {
            /* Set feature configurations for step counter interrupt. */
            rslt = set_feature_config(&dev);

            if (rslt == BMI323_OK)
            {
                feature.step_counter_en = BMI323_ENABLE;

                /* Enable the selected sensors. */
                rslt = bmi323_select_sensor(&feature, &dev);
                bmi3_error_codes_print_result("Sensor enable", rslt);

                if (rslt == BMI323_OK)
                {
                    map_int.step_counter_out = BMI3_INT1;

                    /* Map the feature interrupt for step counter. */
                    rslt = bmi323_map_interrupt(map_int, &dev);
                    bmi3_error_codes_print_result("Map interrupt", rslt);
                    printf("Move the board in steps\n");

                    /* Loop to get step counter interrupt. */
                    do
                    {
                        /* Clear buffer. */
                        int_status = 0;

                        /* To get the interrupt status of step counter. */
                        rslt = bmi323_get_int1_status(&int_status, &dev);
                        bmi3_error_codes_print_result("Get interrupt status", rslt);

                        /* To check the interrupt status of step counter. */
                        if (int_status & BMI3_INT_STATUS_STEP_COUNTER)
                        {
                            printf("Step counter interrupt is generated\n");

                            /* Get step counter output. */
                            rslt = bmi323_get_sensor_data(&sensor_data, 1, &dev);
                            bmi3_error_codes_print_result("Get interrupt status", rslt);

                            /* Print the step counter output. */
                            printf("No of steps counted  = %ld\n", (long int)sensor_data.sens_data.step_counter_output);
                            break;
                        }
                    } while (rslt == BMI323_OK);
                }
            }
        }
    }

    bmi3_coines_deinit();

    return rslt;
}

/*!
 * @brief This internal API is used to set configurations for step counter interrupt.
 */
static int8_t set_feature_config(struct bmi3_dev *dev)
{
    /* Status of API are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi3_sens_config config[2] = { { 0 } };

    /* Configure the type of feature. */
    config[0].type = BMI323_ACCEL;
    config[1].type = BMI323_STEP_COUNTER;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi323_get_sensor_config(config, 2, dev);
    bmi3_error_codes_print_result("Get sensor config", rslt);

    if (rslt == BMI323_OK)
    {
        /* Enable accel by selecting the mode. */
        config[0].cfg.acc.acc_mode = BMI3_ACC_MODE_NORMAL;

        /* Enable water-mark level for to get interrupt after 20 step counts. */
        config[1].cfg.step_counter.watermark_level = 1;

        config[1].cfg.step_counter.activity_detection_factor = 4;
        config[1].cfg.step_counter.activity_detection_thres = 2;
        config[1].cfg.step_counter.env_coef_down = 0xD939;
        config[1].cfg.step_counter.env_coef_up = 0xF1CC;
        config[1].cfg.step_counter.env_min_dist_down = 0x85;
        config[1].cfg.step_counter.env_min_dist_up = 0x131;
        config[1].cfg.step_counter.filter_cascade_enabled = 1;
        config[1].cfg.step_counter.mcr_threshold = 5;
        config[1].cfg.step_counter.mean_crossing_pp_enabled = 0;
        config[1].cfg.step_counter.mean_step_dur = 0xFD54;
        config[1].cfg.step_counter.mean_val_decay = 0xEAC8;
        config[1].cfg.step_counter.peak_duration_min_running = 0x0C;
        config[1].cfg.step_counter.peak_duration_min_walking = 0x0C;
        config[1].cfg.step_counter.reset_counter = 0;
        config[1].cfg.step_counter.step_buffer_size = 5;
        config[1].cfg.step_counter.step_counter_increment = 0x100;
        config[1].cfg.step_counter.step_duration_max = 0x40;
        config[1].cfg.step_counter.step_duration_pp_enabled = 1;
        config[1].cfg.step_counter.step_duration_thres = 1;
        config[1].cfg.step_counter.step_duration_window = 0x0A;

        /* Set new configurations. */
        rslt = bmi323_set_sensor_config(config, 2, dev);
        bmi3_error_codes_print_result("Set sensor config", rslt);
    }

    return rslt;
}

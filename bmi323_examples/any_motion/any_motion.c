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
 *  @brief This internal API is used to set configurations for any-motion.
 *
 *  @param[in] dev       : Structure instance of bmi3_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_feature_config(struct bmi3_dev *dev);

/******************************************************************************/
/*!               Functions                                                   */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of API are returned to this variable. */
    int8_t rslt;

    /* Variable to get any-motion interrupt status. */
    uint16_t int_status = 0;

    /* Sensor initialization configuration. */
    struct bmi3_dev dev = { 0 };

    /* Feature enable initialization. */
    struct bmi3_feature_enable feature = { 0 };

    /* Interrupt mapping structure. */
    struct bmi3_map_int map_int = { 0 };

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
            /* Set feature configurations for any-motion. */
            rslt = set_feature_config(&dev);

            if (rslt == BMI323_OK)
            {
                feature.any_motion_x_en = BMI323_ENABLE;
                feature.any_motion_y_en = BMI323_ENABLE;
                feature.any_motion_z_en = BMI323_ENABLE;

                /* Enable the selected sensors. */
                rslt = bmi323_select_sensor(&feature, &dev);
                bmi3_error_codes_print_result("Sensor select", rslt);

                if (rslt == BMI323_OK)
                {
                    map_int.any_motion_out = BMI3_INT1;

                    /* Map the feature interrupt for any-motion. */
                    rslt = bmi323_map_interrupt(map_int, &dev);
                    bmi3_error_codes_print_result("Map interrupt", rslt);
                    printf("Move the board\n");

                    /* Loop to get any-motion interrupt. */
                    do
                    {
                        /* Clear buffer. */
                        int_status = 0;

                        /* To get the interrupt status of any-motion. */
                        rslt = bmi323_get_int1_status(&int_status, &dev);
                        bmi3_error_codes_print_result("Get interrupt status", rslt);

                        /* To check the interrupt status of any-motion. */
                        if (int_status & BMI3_INT_STATUS_ANY_MOTION)
                        {
                            printf("Any-motion interrupt is generated\n");
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
 * @brief This internal API is used to set configurations for any-motion.
 */
static int8_t set_feature_config(struct bmi3_dev *dev)
{
    /* Status of API are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi3_sens_config config[2] = { { 0 } };

    /* Configure the type of feature. */
    config[0].type = BMI323_ACCEL;
    config[1].type = BMI323_ANY_MOTION;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi323_get_sensor_config(config, 2, dev);
    bmi3_error_codes_print_result("Get sensor config", rslt);

    if (rslt == BMI323_OK)
    {
        /* Enable accel by selecting the mode. */
        config[0].cfg.acc.acc_mode = BMI3_ACC_MODE_NORMAL;

        /* Minimum slope of acceleration signal for motion detection. Range = 0 to 4095. */
        config[1].cfg.any_motion.slope_thres = 9;

        /* Hysteresis for the slope of the acceleration signal. Range = 0 to 1023. */
        config[1].cfg.any_motion.hysteresis = 5;

        /* Minimum duration for which the slope shall be greater than threshold for motion detection.
         * Range = 0 to 8191.
         */
        config[1].cfg.any_motion.duration = 9;

        /* Mode of the acceleration reference update. Range = 0 to 1.
         * Value    Name    Description
         *  0      OnEvent   On detection of the event
         *  1      Always    On update of acceleration signal
         */
        config[1].cfg.any_motion.acc_ref_up = 1;

        /* Wait time for clearing the event after slope is below threshold. Range = 0 to 7. */
        config[1].cfg.any_motion.wait_time = 5;

        /* Set new configurations. */
        rslt = bmi323_set_sensor_config(config, 2, dev);
        bmi3_error_codes_print_result("Set sensor config", rslt);
    }

    return rslt;
}

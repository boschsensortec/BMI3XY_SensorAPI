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
 *  @brief This internal API is used to set configurations for step detector interrupt by enabling accel.
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

    /* Variable to get step detector interrupt status. */
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
    rslt = bmi3_interface_init(&dev, BMI3_I2C_INTF);
    bmi3_error_codes_print_result("bmi3_interface_init", rslt);

    if (rslt == BMI323_OK)
    {
        /* Initialize bmi323. */
        rslt = bmi323_init(&dev);
        bmi3_error_codes_print_result("bmi323_init", rslt);

        if (rslt == BMI323_OK)
        {
            feature.step_detector_en = BMI323_ENABLE;

            /* Enable the selected sensors. */
            rslt = bmi323_select_sensor(&feature, &dev);
            bmi3_error_codes_print_result("Sensor enable", rslt);

            if (rslt == BMI323_OK)
            {
                rslt = set_feature_config(&dev);

                if (rslt == BMI323_OK)
                {
                    map_int.step_detector_out = BMI3_INT1;

                    /* Map the feature interrupt for step detector. */
                    rslt = bmi323_map_interrupt(map_int, &dev);
                    bmi3_error_codes_print_result("Map interrupt", rslt);
                    printf("Move the board in steps\n");

                    /* Loop to get step detector interrupt. */
                    for (;;)
                    {
                        /* Clear buffer. */
                        int_status = 0;

                        /* To get the interrupt status of step detector. */
                        rslt = bmi323_get_int1_status(&int_status, &dev);
                        bmi3_error_codes_print_result("Get interrupt status", rslt);

                        /* To check the interrupt status of step detector. */
                        if (int_status & BMI3_INT_STATUS_STEP_DETECTOR)
                        {
                            printf("Step detector interrupt is generated\n");
                            break;
                        }
                    }
                }
            }
        }
    }

    bmi3_coines_deinit();

    return rslt;
}

/*!
 * @brief This internal API is used to set configurations for step detector interrupt by enabling accel.
 */
static int8_t set_feature_config(struct bmi3_dev *dev)
{
    /* Status of API are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi3_sens_config config = { 0 };

    /* Configure the type of feature. */
    config.type = BMI323_ACCEL;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi323_get_sensor_config(&config, 1, dev);
    bmi3_error_codes_print_result("Get sensor config", rslt);

    if (rslt == BMI323_OK)
    {
        /* Enable accel by selecting the mode. */
        config.cfg.acc.acc_mode = BMI3_ACC_MODE_NORMAL;

        /* Set new configurations. */
        rslt = bmi323_set_sensor_config(&config, 1, dev);
        bmi3_error_codes_print_result("Set sensor config", rslt);
    }

    return rslt;
}

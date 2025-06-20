/**\
 * Copyright (c) 2024 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmi330.h"
#include "common.h"

/******************************************************************************/
/*!         Static Function Declaration                                       */

/*!
 *  @brief This internal API is used to set configurations for any-motion and no-motion.
 *
 *  @param[in] dev       : Structure instance of bmi3_dev.
 *
 *  @return Status of execution.
 */
static int8_t set_feature_config(struct bmi3_dev *dev);

/*!
 *  @brief This internal API is used to read interrupt status for any-motion and no-motion.
 *
 *  @param[in] dev       : Structure instance of bmi3_dev.
 *
 *  @return Status of execution.
 */
static int8_t motion_interrupt(struct bmi3_dev *dev);

/******************************************************************************/
/*!               Functions                                                   */

/* This function starts the execution of program. */
int main(void)
{
    /* Status of API are returned to this variable. */
    int8_t rslt;

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

    if (rslt == BMI330_OK)
    {
        /* Initialize bmi330. */
        printf("Uploading configuration file\n");
        rslt = bmi330_init(&dev);
        bmi3_error_codes_print_result("bmi330_init", rslt);

        printf("Configuration file uploaded\n");
        printf("Chip ID :0x%x\n", dev.chip_id);

        rslt = bmi330_configure_enhanced_flexibility(&dev);
        bmi3_error_codes_print_result("bmi330_configure_enhanced_flexibility", rslt);

        if (rslt == BMI330_OK)
        {
            /* Set feature configurations for any-motion and no-motion. */
            rslt = set_feature_config(&dev);

            if (rslt == BMI330_OK)
            {
                map_int.any_motion_out = BMI3_INT1;
                map_int.no_motion_out = BMI3_INT1;

                /* Map the feature interrupt for any-motion and no-motion. */
                printf("Interrupt configuration\n");
                rslt = bmi330_map_interrupt(map_int, &dev);
                bmi3_error_codes_print_result("Map interrupt", rslt);

                if (rslt == BMI330_OK)
                {
                    printf("Interrupt Enabled: \t %s\n", enum_to_string(BMI3_ANY_MOTION));
                    printf("Interrupt Mapped to: \t %s\n", enum_to_string(BMI3_INT1));

                    printf("Interrupt Enabled: \t %s\n", enum_to_string(BMI3_NO_MOTION));
                    printf("Interrupt Mapped to: \t %s\n", enum_to_string(BMI3_INT1));

                }

                printf(
                    "Move the board to get any-motion interrupt and do not move the board to get no-motion interrupt\n");

                if (rslt == BMI330_OK)
                {
                    printf("\nCASE 1: Any-motion and No-motion enable\n");

                    printf("\nPress enter to continue\n\n");
                    (void)getchar();

                    feature.any_motion_x_en = BMI330_ENABLE;
                    feature.any_motion_y_en = BMI330_ENABLE;
                    feature.any_motion_z_en = BMI330_ENABLE;
                    feature.no_motion_x_en = BMI330_ENABLE;
                    feature.no_motion_y_en = BMI330_ENABLE;
                    feature.no_motion_z_en = BMI330_ENABLE;

                    /* Enable the selected sensors. */
                    rslt = bmi330_select_sensor(&feature, &dev);
                    bmi3_error_codes_print_result("Sensor select", rslt);

                    if (rslt == BMI330_OK)
                    {
                        rslt = motion_interrupt(&dev);
                    }
                }

                if (rslt == BMI330_OK)
                {
                    printf("\nCASE 2: Any-motion disable and No-motion enable\n");

                    printf("\nPress enter to continue\n\n");
                    (void)getchar();

                    feature.any_motion_x_en = BMI330_DISABLE;
                    feature.any_motion_y_en = BMI330_DISABLE;
                    feature.any_motion_z_en = BMI330_DISABLE;
                    feature.no_motion_x_en = BMI330_ENABLE;
                    feature.no_motion_y_en = BMI330_ENABLE;
                    feature.no_motion_z_en = BMI330_ENABLE;

                    /* Enable the selected sensors. */
                    rslt = bmi330_select_sensor(&feature, &dev);
                    bmi3_error_codes_print_result("Sensor select", rslt);

                    if (rslt == BMI330_OK)
                    {
                        rslt = motion_interrupt(&dev);
                    }
                }

                if (rslt == BMI330_OK)
                {
                    printf("\nCASE 3: Any-motion enable and No-motion disable\n");

                    printf("\nPress enter to continue\n\n");
                    (void)getchar();

                    feature.any_motion_x_en = BMI330_ENABLE;
                    feature.any_motion_y_en = BMI330_ENABLE;
                    feature.any_motion_z_en = BMI330_ENABLE;
                    feature.no_motion_x_en = BMI330_DISABLE;
                    feature.no_motion_y_en = BMI330_DISABLE;
                    feature.no_motion_z_en = BMI330_DISABLE;

                    /* Enable the selected sensors. */
                    rslt = bmi330_select_sensor(&feature, &dev);
                    bmi3_error_codes_print_result("Sensor select", rslt);

                    if (rslt == BMI330_OK)
                    {
                        rslt = motion_interrupt(&dev);
                    }
                }
            }
        }
    }

    bmi3_coines_deinit();

    return rslt;
}

/*!
 * @brief This internal API is used to set configurations for any-motion and no-motion.
 */
static int8_t set_feature_config(struct bmi3_dev *dev)
{
    /* Status of API are returned to this variable. */
    int8_t rslt;

    /* Structure to define the type of sensor and its configurations. */
    struct bmi3_sens_config config[3] = { { 0 } };

    /* Configure the type of feature. */
    config[0].type = BMI330_ACCEL;
    config[1].type = BMI330_ANY_MOTION;
    config[2].type = BMI330_NO_MOTION;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi330_get_sensor_config(config, BMI3_N_SENSE_COUNT_3, dev);
    bmi3_error_codes_print_result("Get sensor config", rslt);

    if (rslt == BMI330_OK)
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

        /* Minimum slope of acceleration signal for motion detection. Range = 0 to 4095. */
        config[2].cfg.no_motion.slope_thres = 9;

        /* Hysteresis for the slope of the acceleration signal. Range = 0 to 1023. */
        config[2].cfg.no_motion.hysteresis = 5;

        /* Minimum duration for which the slope shall be greater than threshold for motion detection.
         * Range = 0 to 8191. */
        config[2].cfg.no_motion.duration = 9;

        /* Mode of the acceleration reference update. Range = 0 to 1.
         * Value    Name    Description
         *  0      OnEvent   On detection of the event
         *  1      Always    On update of acceleration signal
         */
        config[2].cfg.no_motion.acc_ref_up = 1;

        /* Wait time for clearing the event after slope is below threshold. Range = 0 to 7. */
        config[2].cfg.no_motion.wait_time = 5;

        /* Set new configurations. */
        rslt = bmi330_set_sensor_config(config, BMI3_N_SENSE_COUNT_3, dev);
        bmi3_error_codes_print_result("Set sensor config", rslt);

        /* Set No motion configuration settings. */
        printf("*************************************\n");
        printf("No motion Configuration:\n");
        printf("Threshold: %u\n", config[2].cfg.no_motion.slope_thres);
        printf("Hysterisis: %u\n", config[2].cfg.no_motion.hysteresis);
        printf("Duration: %u\n", config[2].cfg.no_motion.duration);
        printf("Acceleration Reference Update: %u\n", config[2].cfg.no_motion.acc_ref_up);
        printf("Wait Time: %u\n", config[2].cfg.no_motion.wait_time);
        printf("\n");

        /* Set Any motion configuration settings. */
        printf("*************************************\n");
        printf("Any motion Configuration:\n");
        printf("Threshold: %u\n", config[1].cfg.any_motion.slope_thres);
        printf("Hysterisis: %u\n", config[1].cfg.any_motion.hysteresis);
        printf("Duration: %u\n", config[1].cfg.any_motion.duration);
        printf("Acceleration Reference Update: %u\n", config[1].cfg.any_motion.acc_ref_up);
        printf("Wait Time: %u\n", config[1].cfg.any_motion.wait_time);
        printf("\n");
    }

    return rslt;
}

/*!
 *  @brief This internal API is used to read interrupt status for any-motion and no-motion.
 */
static int8_t motion_interrupt(struct bmi3_dev *dev)
{
    /* Status of API are returned to this variable. */
    int8_t rslt;

    /* Variable to get motion interrupt status. */
    uint16_t int_status = 0;

    uint8_t loop = 0;

    /* To get the interrupt status of any-motion and no-motion. */
    rslt = bmi330_get_int1_status(&int_status, dev);
    bmi3_error_codes_print_result("Get interrupt status", rslt);

    /* Loop to get motion interrupt. */
    for (;;)
    {
        /* Clear buffer. */
        int_status = 0;

        /* To get the interrupt status of any-motion and no-motion. */
        rslt = bmi330_get_int1_status(&int_status, dev);
        bmi3_error_codes_print_result("Get interrupt status", rslt);

        /* To check the interrupt status of any-motion. */
        if (int_status & BMI3_INT_STATUS_ANY_MOTION)
        {
            printf("Any-motion interrupt is generated\n");

            loop++;
        }

        /* To check the interrupt status of no-motion. */
        if (int_status & BMI3_INT_STATUS_NO_MOTION)
        {
            printf("No-motion interrupt is generated\n");

            loop++;
        }

        if (loop > 20)
        {
            break;
        }
    }

    return rslt;
}

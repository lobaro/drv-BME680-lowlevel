//
// Created by TheoRohde on 12/4/2019.
//
#include <stdint.h>
#include <stdbool.h>
#include "drv_bme680_lowlevel.h"
#include <github.com/lobaro/c-utils/lobaroAssert.h>

// more inspiration see: https://github.com/imorygh/bme680driver/blob/07d58fdaea3b9152e61bd6df7aa7efe35068cb54/bme680-i2c.c

bool isPrepared = false;
uint16_t meas_period = 0;

static int setupBME68xProfile(struct bme680_dev* pGas_sensor){
    uint8_t set_required_settings;
    int8_t rslt = BME680_OK;

    /* Set the temperature, pressure and humidity settings */
    pGas_sensor->tph_sett.os_hum = BME680_OS_2X;
    pGas_sensor->tph_sett.os_pres = BME680_OS_4X;
    pGas_sensor->tph_sett.os_temp = BME680_OS_8X;
    pGas_sensor->tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    pGas_sensor->gas_sett.run_gas = BME680_DISABLE_GAS_MEAS; // BME680_ENABLE_GAS_MEAS; // heater not used in this profile
    /* Create a ramp heat waveform in 3 steps */
    pGas_sensor->gas_sett.heatr_temp = 320; /* degree Celsius */
    pGas_sensor->gas_sett.heatr_dur = 150;  /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    pGas_sensor->power_mode = BME680_FORCED_MODE;

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings, pGas_sensor);
    if(rslt != BME680_OK){
        isPrepared = false;
        return rslt;
    }
}

/* Get the total measurement duration so as to sleep or wait till the
  * measurement is complete */

uint16_t drv_bme680_lowlevel_getMeasTime_ms(){
    return meas_period;
}

int drv_bme680_lowlevel_init(  struct bme680_dev* pGas_sensor, bme680_com_fptr_t i2cRead_fn, bme680_com_fptr_t i2cWrite_fn, bme680_delay_fptr_t delay_fn)
{
    int8_t rslt;

    pGas_sensor->dev_id = BME680_I2C_ADDR_PRIMARY;
    pGas_sensor->intf = BME680_I2C_INTF;
    pGas_sensor->read = i2cRead_fn;
    pGas_sensor->write = i2cWrite_fn;
    pGas_sensor->delay_ms = delay_fn;
    pGas_sensor->power_mode = 1;
    pGas_sensor->amb_temp = 25;
    rslt = bme680_init(pGas_sensor);
    if(rslt != BME680_OK){
        isPrepared = false;
        return rslt;
    }

    setupBME68xProfile(pGas_sensor);

    /* Set the power mode */
    rslt = bme680_set_sensor_mode(pGas_sensor);
    if(rslt != BME680_OK){
        isPrepared = false;
        return rslt;
    }

    /* Get the total measurement duration so as to sleep or wait till the measurement is complete */
    bme680_get_profile_dur(&meas_period, pGas_sensor);

    isPrepared = true;
    return BME680_OK;
}

int drv_bme680_lowlevel_measurement(struct bme680_dev* pGas_sensor, struct bme680_field_data* results){
    int8_t rslt;
    lobaroASSERT(pGas_sensor->delay_ms);

    if(!isPrepared){
        return BME680_E_DEV_NOT_FOUND;
    }

    bme680_set_sensor_mode(pGas_sensor);
    pGas_sensor->delay_ms(drv_bme680_lowlevel_getMeasTime_ms());
    return bme680_get_sensor_data(results, pGas_sensor);
}
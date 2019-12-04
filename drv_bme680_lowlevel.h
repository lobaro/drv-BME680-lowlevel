//
// Created by TheoRohde on 12/4/2019.
//

#ifndef APP_NRF9160_BME688_DRV_BME680_LOWLEVEL_H
#define APP_NRF9160_BME688_DRV_BME680_LOWLEVEL_H

/*! CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif

#include "bme680.h"

/*
* I2C READ Data on the bus should be like (or repeated start after write)
* |------------+---------------------|
* | I2C action | Data                |
* |------------+---------------------|
* | Start      | -                   |
* | Write      | (reg_addr)          |
* | Stop       | -                   |
* | Start      | -                   |
* | Read       | (reg_data[0])       |
* | Read       | (....)              |
* | Read       | (reg_data[len - 1]) |
* | Stop       | -                   |
* |------------+---------------------|
*/

/*
*  I2C WRITE Data on the bus should be like
* |------------+---------------------|
* | I2C action | Data                |
* |------------+---------------------|
* | Start      | -                   |
* | Write      | (reg_addr)          |
* | Write      | (reg_data[0])       |
* | Write      | (....)              |
* | Write      | (reg_data[len - 1]) |
* | Stop       | -                   |
* |------------+---------------------|
*/

/* Simple usage example
 *
 *  hal_i2c_init(K100, 0,1 );
    struct bme680_dev gas_sensor;
    int8_t rslt = drv_bme680_lowlevel_init(&gas_sensor,user_i2c_read,user_i2c_write,user_delay_ms);
    Log("BME68x result init: %d, measurement time: %dms\n",rslt, drv_bme680_lowlevel_getMeasTime_ms());

    struct bme680_field_data data;
    while(1){

       rslt = drv_bme680_lowlevel_measurement(&gas_sensor, &data);
        if(rslt == 0){
          Log("temp=%.2f degree C hum=%.2f%% pres=%.2f hPa\n",((float)data.temperature)/100.0, ((float)data.humidity) / 1000.0,  ((float)data.pressure)/100.0);
        }else{
            Log("BME68x read sensor data error: %d\n",rslt);
        }
    }
 *
 * */

int drv_bme680_lowlevel_init(  struct bme680_dev* pGas_sensor, bme680_com_fptr_t i2cRead_fn, bme680_com_fptr_t i2cWrite_fn, bme680_delay_fptr_t delay_fn); // setup bme680 operating mode

/*! Temperature in degree celsius x100 */
/*! Pressure in Pascal */
/*! Humidity in % relative humidity x1000 */
int drv_bme680_lowlevel_measurement(struct bme680_dev* pGas_sensor, struct bme680_field_data* results);

uint16_t drv_bme680_lowlevel_getMeasTime_ms();

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif //APP_NRF9160_BME688_DRV_BME680_LOWLEVEL_H

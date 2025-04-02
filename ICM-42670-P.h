/**
  * @file    icm42670.h
  * @brief   Driver for ICM42670 6-axis IMU sensor
  * @author  [Your Name]
  * @date    [Date]
  * @version 1.0
  *
  * This driver follows MISRA C:2012 guidelines where applicable
  */

#ifndef ICM42670_H
#define ICM42670_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"

/* Public defines -----------------------------------------------------------*/
/** @defgroup ICM42670_Defines ICM42670 Defines
  * @{
  */
#define ICM42670_PACKET_SIZE                (16U)
#define ICM42670_BUFFER_SIZE                (1024U)

/* I2C Addresses */
#define ICM42670_DEFAULT_ADDRESS            (0x69U) /* If AD0 = VCC then 0x69 else 0x68 */
#define ICM42670_WHO_AM_I_VALUE             (0x67U)

/* Register addresses */
#define ICM42670_REG_MCLK_RDY               (0x00U)
#define ICM42670_REG_INT_STATUS             (0x3AU)
#define ICM42670_REG_INT_STATUS2            (0x3BU)
#define ICM42670_REG_WHO_AM_I               (0x75U)
#define ICM42670_REG_PWR_MGMT0              (0x1FU)
#define ICM42670_REG_ACCEL_CONFIG0          (0x21U)
#define ICM42670_REG_GYRO_CONFIG0           (0x20U)

/* Data registers */
#define ICM42670_REG_ACCEL_DATA_X1          (0x0BU)
#define ICM42670_REG_ACCEL_DATA_X0          (0x0CU)
#define ICM42670_REG_ACCEL_DATA_Y1          (0x0DU)
#define ICM42670_REG_ACCEL_DATA_Y0          (0x0EU)
#define ICM42670_REG_ACCEL_DATA_Z1          (0x0FU)
#define ICM42670_REG_ACCEL_DATA_Z0          (0x10U)
#define ICM42670_REG_GYRO_DATA_X1           (0x11U)
#define ICM42670_REG_GYRO_DATA_X0           (0x12U)
#define ICM42670_REG_GYRO_DATA_Y1           (0x13U)
#define ICM42670_REG_GYRO_DATA_Y0           (0x14U)
#define ICM42670_REG_GYRO_DATA_Z1           (0x15U)
#define ICM42670_REG_GYRO_DATA_Z0           (0x16U)
#define ICM42670_REG_TEMP_DATA1             (0x09U)
#define ICM42670_REG_TEMP_DATA0             (0x0AU)

/* APEX configuration registers */
#define ICM42670_REG_APEX_CONFIG0           (0x25U)
#define ICM42670_REG_APEX_CONFIG1           (0x26U)
#define ICM42670_REG_INT_SOURCE6            (0x2FU) /* MREG1 */
#define ICM42670_REG_APEX_CONFIG2           (0x44U) /* MREG1 */
#define ICM42670_REG_APEX_CONFIG3           (0x45U) /* MREG1 */
#define ICM42670_REG_APEX_CONFIG4           (0x46U) /* MREG1 */
#define ICM42670_REG_APEX_CONFIG5           (0x47U) /* MREG1 */
#define ICM42670_REG_APEX_CONFIG9           (0x48U) /* MREG1 */
#define ICM42670_REG_APEX_CONFIG10          (0x49U) /* MREG1 */
#define ICM42670_REG_APEX_CONFIG11          (0x4AU) /* MREG1 */
#define ICM42670_REG_ACCEL_WOM_X_TH         (0x4BU) /* MREG1 */
#define ICM42670_REG_ACCEL_WOM_Y_TH         (0x4CU) /* MREG1 */
#define ICM42670_REG_ACCEL_WOM_Z_TH         (0x4DU) /* MREG1 */

/* WOM configuration */
#define ICM42670_REG_WOM_CONFIG             (0x27U)

/* Other registers */
#define ICM42670_REG_INT_SOURCE1            (0x2CU)
#define ICM42670_REG_GYRO_CONFIG1           (0x23U)

/* Bank selection registers */
#define ICM42670_REG_BLK_SEL_R              (0x7CU)
#define ICM42670_REG_BLK_SEL_W              (0x79U)
#define ICM42670_REG_M_R                    (0x7EU)
#define ICM42670_REG_M_W                    (0x7BU)
#define ICM42670_REG_MADDR_R                (0x7DU)
#define ICM42670_REG_MADDR_W                (0x7AU)

/* FIFO registers */
#define ICM42670_REG_FIFO_CONFIG1           (0x28U)
#define ICM42670_REG_FIFO_CONFIG2           (0x29U)
#define ICM42670_REG_FIFO_CONFIG3           (0x2AU)
#define ICM42670_REG_FIFO_COUNT_H           (0x3DU)
#define ICM42670_REG_FIFO_COUNT_L           (0x3EU)
#define ICM42670_REG_FIFO_DATA              (0x3FU)
#define ICM42670_REG_FIFO_LOST_PKT0         (0x2FU) /* 7:0 */
#define ICM42670_REG_FIFO_LOST_PKT1         (0x30U) /* 15:8 */

/* Bank 1 FIFO configuration */
#define ICM42670_REG_BK1_FIFO_CONFIG5       (0x01U)
#define ICM42670_REG_BK1_FIFO_CONFIG6       (0x02U)

/* Configuration values */
#define ICM42670_FIFO_RESET                 (0x02U)
#define ICM42670_REG_INT_SOURCE0            (0x2BU)

/* Gyro full scale range settings */
#define ICM42670_GYRO_FS_2000_DPS           (0x00U)
#define ICM42670_GYRO_FS_1000_DPS           (0x20U)
#define ICM42670_GYRO_FS_500_DPS            (0x40U)
#define ICM42670_GYRO_FS_250_DPS            (0x60U)

/* Accelerometer full scale range settings */
#define ICM42670_ACCEL_FS_16G               (0x00U)
#define ICM42670_ACCEL_FS_8G                (0x20U)
#define ICM42670_ACCEL_FS_4G                (0x40U)
#define ICM42670_ACCEL_FS_2G                (0x60U)

/* Output data rates */
#define ICM42670_ODR_1600_HZ                (0x05U)
#define ICM42670_ODR_800_HZ                 (0x06U)
#define ICM42670_ODR_400_HZ                 (0x07U)
#define ICM42670_ODR_200_HZ                 (0x08U)
#define ICM42670_ODR_100_HZ                 (0x09U)
#define ICM42670_ODR_50_HZ                  (0x0AU)
#define ICM42670_ODR_25_HZ                  (0x0BU)
#define ICM42670_ODR_12_5_HZ                (0x0CU)
#define ICM42670_ODR_6_25_HZ                (0x0DU)
#define ICM42670_ODR_3_125_HZ               (0x0EU)
#define ICM42670_ODR_1_5625_HZ              (0x0FU)
/**
  * @}
  */

/* Public types -------------------------------------------------------------*/
/** @defgroup ICM42670_Types ICM42670 Types
  * @{
  */

/**
  * @brief DMP output data rate frequencies
  */
typedef enum
{
  ICM42670_DMP_ODR_25HZ   = 0x00U,
  ICM42670_DMP_ODR_400HZ  = 0x01U,
  ICM42670_DMP_ODR_50HZ   = 0x10U,
  ICM42670_DMP_ODR_100HZ  = 0x11U
} ODR_FREQ;

/**
  * @brief Accelerometer output data rates
  */
typedef enum
{
  ICM42670_ACCEL_ODR_RESERVED0   = 0x00U,  /* Reserved */
  ICM42670_ACCEL_ODR_RESERVED1   = 0x01U,  /* Reserved */
  ICM42670_ACCEL_ODR_RESERVED2   = 0x02U,  /* Reserved */
  ICM42670_ACCEL_ODR_RESERVED3   = 0x03U,  /* Reserved */
  ICM42670_ACCEL_ODR_RESERVED4   = 0x04U,  /* Reserved */
  ICM42670_ACCEL_ODR_1600_HZ     = 0x05U,  /* 1.6 kHz (LN mode) */
  ICM42670_ACCEL_ODR_800_HZ      = 0x06U,  /* 800 Hz (LN mode) */
  ICM42670_ACCEL_ODR_400_HZ      = 0x07U,  /* 400 Hz (LP or LN mode) */
  ICM42670_ACCEL_ODR_200_HZ      = 0x08U,  /* 200 Hz (LP or LN mode) */
  ICM42670_ACCEL_ODR_100_HZ      = 0x09U,  /* 100 Hz (LP or LN mode) */
  ICM42670_ACCEL_ODR_50_HZ       = 0x0AU,  /* 50 Hz (LP or LN mode) */
  ICM42670_ACCEL_ODR_25_HZ       = 0x0BU,  /* 25 Hz (LP or LN mode) */
  ICM42670_ACCEL_ODR_12_5_HZ     = 0x0CU,  /* 12.5 Hz (LP or LN mode) */
  ICM42670_ACCEL_ODR_6_25_HZ     = 0x0DU,  /* 6.25 Hz (LP mode) */
  ICM42670_ACCEL_ODR_3_125_HZ    = 0x0EU,  /* 3.125 Hz (LP mode) */
  ICM42670_ACCEL_ODR_1_5625_HZ   = 0x0FU   /* 1.5625 Hz (LP mode) */
} ACCEL_ODR;

/**
  * @brief Tilt wait time selection
  */
typedef enum
{
  ICM42670_TILT_WAIT_0SEC  = 0x00U,
  ICM42670_TILT_WAIT_2SEC  = 0x01U,
  ICM42670_TILT_WAIT_4SEC  = 0x02U,
  ICM42670_TILT_WAIT_6SEC  = 0x03U
} icm42670_tilt_wait_t;

/**
  * @brief 3-axis sensor data structure
  */
typedef struct
{
  int16_t x;  /* X-axis data */
  int16_t y;  /* Y-axis data */
  int16_t z;  /* Z-axis data */
} sensorXYZ;

/**
  * @brief ICM42670 device structure
  */
typedef struct
{
  I2C_HandleTypeDef *hi2c;   /* I2C handle */
  uint8_t address;           /* I2C device address */
  uint16_t accel_calib;      /* Accelerometer calibration value */
  float gyro_calib;          /* Gyroscope calibration value */
} ICM42670;

/**
  * @}
  */

/* Public function prototypes -----------------------------------------------*/
/** @defgroup ICM42670_Functions ICM42670 Functions
  * @{
  */

/* Initialization functions */
HAL_StatusTypeDef icm42670_init(ICM42670 *sensor, uint8_t addr, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef icm42670_reset_config(ICM42670 *sensor);
uint8_t icm42670_who_am_i(ICM42670 *sensor);
void icm42670_power_down(ICM42670 *sensor);

/* Clock management */
HAL_StatusTypeDef icm42670_mclk_on(ICM42670 *sensor);
HAL_StatusTypeDef icm42670_mclk_off(ICM42670 *sensor);

/* Sensor control */
HAL_StatusTypeDef icm42670_start_accel(ICM42670 *sensor, uint8_t scale, uint8_t freq);
HAL_StatusTypeDef icm42670_start_gyro(ICM42670 *sensor, uint8_t rate, uint8_t freq);

/* Data acquisition */
sensorXYZ icm42670_read_accel(ICM42670 *sensor);
sensorXYZ icm42670_read_gyro(ICM42670 *sensor);
int16_t icm42670_read_temp(ICM42670 *sensor);

/* Register access */
HAL_StatusTypeDef icm42670_write(ICM42670 *sensor, uint8_t reg, const uint8_t *buffer, uint8_t len);
HAL_StatusTypeDef icm42670_read_register(ICM42670 *sensor, uint8_t reg, uint8_t *buffer, uint8_t len);
HAL_StatusTypeDef icm42670_bank1_read(ICM42670 *sensor, uint8_t reg, uint8_t *result);
HAL_StatusTypeDef icm42670_bank1_write(ICM42670 *sensor, uint8_t reg, uint8_t value);

/* FIFO management */
HAL_StatusTypeDef icm42670_reset_fifo(ICM42670 *sensor);
HAL_StatusTypeDef icm42670_set_fifo_level(ICM42670 *sensor, uint16_t level);
HAL_StatusTypeDef icm42670_read_fifo_counter(ICM42670 *sensor, uint16_t *level);
HAL_StatusTypeDef icm42670_read_fifo_data(ICM42670 *sensor, uint8_t *data, uint16_t len);
HAL_StatusTypeDef icm42670_init_fifo(ICM42670 *sensor, uint16_t fifo_level_threshold);
HAL_StatusTypeDef icm42670_read_fifo_lost_packets(ICM42670 *sensor, uint16_t *lost);
uint8_t icm42670_decode_packet(const uint8_t *buffer, 
		 sensorXYZ *accel, 
     sensorXYZ *gyro, uint16_t *temp, uint16_t *time);

/* APEX features */
HAL_StatusTypeDef icm42670_apex_init_tilt(ICM42670 *sensor, uint8_t tilt_wait_time, uint8_t enable_int);
HAL_StatusTypeDef icm42670_apex_init_pedometer(ICM42670 *sensor, 
			uint8_t low_energy_amp_th,
      uint8_t ped_amp_th, uint8_t ped_step_cnt_th,
      uint8_t ped_step_det_th, uint8_t ped_sb_timer_th,
			uint8_t ped_hi_energy_th, uint8_t ped_sensitivity,
			uint8_t enable_int);
HAL_StatusTypeDef icm42670_apex_init_smd(ICM42670 *sensor, uint8_t smd_sensitivity, uint8_t enable_int);
HAL_StatusTypeDef icm42670_apex_init_wake_on_motion(ICM42670 *sensor, 
			uint8_t accel_wom_x_th,
      uint8_t accel_wom_y_th, uint8_t accel_wom_z_th,
      uint8_t wom_mode, uint8_t wom_int_mode,
      uint8_t wom_int_dur, uint8_t enable_int);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* ICM42670_H */

/*
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Pedometer Arguments:
	//9-Enable Interrupt Generation------------------------   //0 = Disable ,1 = Enable
	//8-pedometer Sensivity Mode(1)----------------------  |  //0 = Normal(default) ,1=Slow Walk
	//7-pedometer High Energy threshold(1:0)-----------  | |  //from 87.89mg to 155.27mg(def=1)
	//6-pedometer SB timer threshold(4:2)------------  | | |  //from 50 to 225 sample(def=4)
	//5-pedometer step detection threshold(7:5)----  | | | |  //from 0 to 7 step(def=2)
	//4-pedometer step counter threshold(3:0)----  | | | | |  //from 0 to 15 step(def=5)
	//3-Pedometer amplitude threshold(7:4)-----  | | | | | |  //from 30mg to 90mg(def=8)
	//2-Low Energy amplitude threshold(7:4)--  | | | | | | |  //from 30mg to 105mg(def=10)
	//1-IMU sensor definition-------------   | | | | | | | |
	//////////////////////////////////    |  | | | | | | | |  //////////////////////////////////
	//ICM42670_APEX_INITIALIZE_PEDOMETER(&imu,10,8,5,2,4,1,0,0);//(&imu,10,8,5,1,4,2,0);
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//3-Enable Interrupt Generation-------   //0 = Disable ,1 = Enable
	//2-smd_sensivity(2:0)--------------  |  //(316<<smd_sensivity[2:0])-1
	//1-IMU sensor definition-------    | |
	////////////////////////////    |   | |    ///////////////////////////////////////////////////	
	//ICM42670_APEX_INITIALIZE_SMD(&imu,1,1);
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//3-Enable Interrupt Generation---------------------      //0 = Disable ,1 = Enable		
	//2-TITL_WAIT_TIME---------------------------       |			//0 =0sec, 1 =2sec, 2 =4sec, 3=6sec
	//1-IMU sensor definition---------           |      |
	///////////////////////////////   |          |      |     /////////////////////////////////////	
	//ICM42670_APEX_INITIALIZE_TITL(&imu,TILT_WAIT_0SEC,1);
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//Wake On Motion Arguments:
	//8-Enable Interrupt Generation----------------------------  //0 = Disable ,1-7 = Enable each X,Y,Z WOM interrupt(combined)
	//7-WOM_INT_DUR(4:3)-------------------------------------  |  //from 1 to 4 threshold event to trigger
	//6-WOM_INT_MODE(1)------------------------------------  | |  //0=OR all, 1=AND all
	//5-WOM_MODE(1)--------------------------------------  | | |  //0=Diffrential , 1=Absolute
	//4-ACCE_WOM_X_TH(7:0)----------------------------   | | | |  //1g/256 x value
	//3-ACCE_WOM_X_TH(7:0)-------------------------   |  | | | |  //1g/256 x value
	//2-ACCE_WOM_X_TH(7:0)---------------------    |  |  | | | |  //1g/256 x value
	//1-IMU sensor definition---------------   |   |  |  | | | | 
	//////////////////////////////////      |  |   |  |  | | | |   //////////////////////////////////
	ICM42670_APEX_INITIALIZE_WAKEONMOTION(&imu,99,99, 5, 1,0,1,4);
*/


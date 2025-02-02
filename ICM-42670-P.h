#ifndef ICM42670_H
#define ICM42670_H

#include "stm32wbxx_hal.h"

// I2C Address
#define ICM42670_DEFAULT_ADDRESS    (0x69)//IF AD0 = VCC THEN 0X69 ELSE 0X68
#define ICM42670_WHO_AM_I           (0x67)//

// Registers
#define WHO_AM_I_REG                (0x75)//
#define ICM42670_REG_PWR_MGMT0      (0x1F)//
#define ICM42670_REG_ACCEL_CONFIG0  (0x21)//
#define ICM42670_REG_GYRO_CONFIG0   (0x20)//
#define ICM42670_REG_ACCEL_DATA_X1  (0x0B)//
#define ICM42670_REG_ACCEL_DATA_X0  (0x0C)//
#define ICM42670_REG_ACCEL_DATA_Y1  (0x0D)//
#define ICM42670_REG_ACCEL_DATA_Y0  (0x0E)//
#define ICM42670_REG_ACCEL_DATA_Z1  (0x0F)//
#define ICM42670_REG_ACCEL_DATA_Z0  (0x10)//
#define ICM42670_REG_GYRO_DATA_X1   (0x11)//
#define ICM42670_REG_GYRO_DATA_X0   (0x12)//
#define ICM42670_REG_GYRO_DATA_Y1   (0x13)//
#define ICM42670_REG_GYRO_DATA_Y0   (0x14)//
#define ICM42670_REG_GYRO_DATA_Z1   (0x15)//
#define ICM42670_REG_GYRO_DATA_Z0   (0x16)//
#define ICM42670_REG_TEMP_DATA1     (0x09)//
#define ICM42670_REG_TEMP_DATA0     (0x0A)//

// Calibration
// GYRO
#define ICM42670_CONFIG_GYRO_2k_DPS     (0x00)
#define ICM42670_CONFIG_GYRO_1k_DPS     (0x20)
#define ICM42670_CONFIG_GYRO_500_DPS    (0x40)
#define ICM42670_CONFIG_GYRO_250_DPS    (0x60)

// ACCEL
#define ICM42670_CONFIG_ACCEL_16_G      (0x00)
#define ICM42670_CONFIG_ACCEL_8_G       (0x20)
#define ICM42670_CONFIG_ACCEL_4_G       (0x40)
#define ICM42670_CONFIG_ACCEL_2_G       (0x60)

// RATE
#define ICM42670_CONFIG_RATE_1p6_kHz    (0x05)
#define ICM42670_CONFIG_RATE_800_Hz     (0x06)
#define ICM42670_CONFIG_RATE_400_Hz     (0x07)
#define ICM42670_CONFIG_RATE_200_Hz     (0x08)
#define ICM42670_CONFIG_RATE_100_Hz     (0x09)
#define ICM42670_CONFIG_RATE_50_Hz      (0x0A)
#define ICM42670_CONFIG_RATE_25_Hz      (0x0B)
#define ICM42670_CONFIG_RATE_12p5_Hz    (0x0C)

// Sensor Data Structure
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} sensorXYZ;

// ICM42670 Structure
typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t addr;
    uint16_t accelCalib;
    float gyroCalib;
} ICM42670;

// Function Prototypes
uint8_t InitMPU(ICM42670 *sensor, uint8_t addr, I2C_HandleTypeDef *hi2c);
uint8_t ICM42670_WhoAmI(ICM42670 *sensor);
uint8_t ICM42670_SensorConf(ICM42670 *sensor);
uint8_t ICM42670_StartAccel(ICM42670 *sensor, uint8_t scale, uint8_t freq);
uint8_t ICM42670_StartGyro(ICM42670 *sensor, uint8_t rate, uint8_t freq);
sensorXYZ ICM42670_GetAccel(ICM42670 *sensor);
sensorXYZ ICM42670_GetGyro(ICM42670 *sensor);
int16_t ICM42670_GetTemp(ICM42670 *sensor);
uint8_t ICM42670_Write(ICM42670 *sensor, uint8_t reg, uint8_t *buffer, uint8_t len);
uint8_t ICM42670_ReadRegister(ICM42670 *sensor, uint8_t reg, uint8_t *buffer, uint8_t len);

#endif // ICM42670_H

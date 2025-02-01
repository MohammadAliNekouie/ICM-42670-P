#include "ICM-42670-P.h"

// Initialize the sensor
uint8_t InitMPU(ICM42670 *sensor, uint8_t addr, I2C_HandleTypeDef *hi2c) {
    sensor->hi2c = hi2c; // Set I2C handle
    sensor->addr = addr; // Set I2C address
    uint8_t whoAmI = ICM42670_WhoAmI(sensor); // Check WHO_AM_I register
    if (whoAmI == ICM42670_WHO_AM_I) {
				ICM42670_SensorConf(sensor);
        return HAL_OK; // Success
    } else {
        return HAL_ERROR; // Failure
    }
}

// Read the WHO_AM_I register
uint8_t ICM42670_WhoAmI(ICM42670 *sensor) {
    uint8_t buffer[1];
    if (ICM42670_ReadRegister(sensor, WHO_AM_I_REG, buffer, 1)) {
        return buffer[0]; // Return WHO_AM_I value
    }
    return 0xFF; // Error value
}

// Configure the sensor
uint8_t ICM42670_SensorConf(ICM42670 *sensor) {
    uint8_t sensorConf = 0x0F;
    return ICM42670_Write(sensor, ICM42670_REG_PWR_MGMT0, &sensorConf, 1);
}

// Start the accelerometer with given scale and frequency
uint8_t ICM42670_StartAccel(ICM42670 *sensor, uint8_t scale, uint8_t freq) {
    switch (scale) {
        case ICM42670_CONFIG_ACCEL_16_G:
            sensor->accelCalib = 2048;
            break;
        case ICM42670_CONFIG_ACCEL_8_G:
            sensor->accelCalib = 4096;
            break;
        case ICM42670_CONFIG_ACCEL_4_G:
            sensor->accelCalib = 8192;
            break;
        case ICM42670_CONFIG_ACCEL_2_G:
            sensor->accelCalib = 16384;
            break;
        default:
            break;
    }
    uint8_t accelConf = scale | freq;
    uint8_t accelConfOld;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_ACCEL_CONFIG0, &accelConfOld, 1)) {
        return HAL_ERROR; // Failure
    }
    if (accelConfOld == accelConf) {
        return HAL_OK; // Already configured
    }
    return ICM42670_Write(sensor, ICM42670_REG_ACCEL_CONFIG0, &accelConf, 1);
}

// Start the gyroscope with given rate and frequency
uint8_t ICM42670_StartGyro(ICM42670 *sensor, uint8_t rate, uint8_t freq) {
    switch (rate) {
        case ICM42670_CONFIG_GYRO_2k_DPS:
            sensor->gyroCalib = 16.4;
            break;
        case ICM42670_CONFIG_GYRO_1k_DPS:
            sensor->gyroCalib = 32.8;
            break;
        case ICM42670_CONFIG_GYRO_500_DPS:
            sensor->gyroCalib = 65.5;
            break;
        case ICM42670_CONFIG_GYRO_250_DPS:
            sensor->gyroCalib = 131;
            break;
        default:
            break;
    }
    uint8_t gyroConf = rate | freq;
    uint8_t gyroConfOld;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_GYRO_CONFIG0, &gyroConfOld, 1)) {
        return HAL_ERROR; // Failure
    }
    if (gyroConfOld == gyroConf) {
        return HAL_OK; // Already configured
    }
    uint8_t res = ICM42670_Write(sensor, ICM42670_REG_GYRO_CONFIG0, &gyroConf, 1);
    HAL_Delay(20); // Gyro needs a few milliseconds to reconfigure
    return res;
}

// Get accelerometer data
sensorXYZ ICM42670_GetAccel(ICM42670 *sensor) {
    uint8_t readBuffer[1];
    sensorXYZ sensorData = {0, 0, 0};
    sensorXYZ raw = {0, 0, 0};

    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_ACCEL_DATA_X1, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.x = readBuffer[0] << 8;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_ACCEL_DATA_X0, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.x |= readBuffer[0];

    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_ACCEL_DATA_Y1, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.y = readBuffer[0] << 8;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_ACCEL_DATA_Y0, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.y |= readBuffer[0];

    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_ACCEL_DATA_Z1, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.z = readBuffer[0] << 8;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_ACCEL_DATA_Z0, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.z |= readBuffer[0];

    sensorData.x = (raw.x * 1000) / sensor->accelCalib;
    sensorData.y = (raw.y * 1000) / sensor->accelCalib;
    sensorData.z = (raw.z * 1000) / sensor->accelCalib;
    return sensorData;
}

// Get gyroscope data
sensorXYZ ICM42670_GetGyro(ICM42670 *sensor) {
    uint8_t readBuffer[1];
    sensorXYZ sensorData = {0, 0, 0};
    sensorXYZ raw = {0, 0, 0};

    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_GYRO_DATA_X1, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.x = readBuffer[0] << 8;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_GYRO_DATA_X0, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.x |= readBuffer[0];

    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_GYRO_DATA_Y1, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.y = readBuffer[0] << 8;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_GYRO_DATA_Y0, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.y |= readBuffer[0];

    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_GYRO_DATA_Z1, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.z = readBuffer[0] << 8;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_GYRO_DATA_Z0, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw.z |= readBuffer[0];

    sensorData.x = (raw.x * 1000) / sensor->gyroCalib;
    sensorData.y = (raw.y * 1000) / sensor->gyroCalib;
    sensorData.z = (raw.z * 1000) / sensor->gyroCalib;
    return sensorData;
}

// Get temperature data
int16_t ICM42670_GetTemp(ICM42670 *sensor) {
    uint8_t readBuffer[1];
    int16_t sensorData = 0;
    int16_t raw = 0;

    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_TEMP_DATA1, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw = readBuffer[0] << 8;
    if (!ICM42670_ReadRegister(sensor, ICM42670_REG_TEMP_DATA0, readBuffer, 1)) {
        return sensorData; // Failure
    }
    raw |= readBuffer[0];

    sensorData = (raw / 128) + 25;
    return sensorData;
}

// Write data to a register
uint8_t ICM42670_Write(ICM42670 *sensor, uint8_t reg, uint8_t *buffer, uint8_t len) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(sensor->hi2c, sensor->addr << 1, reg, I2C_MEMADD_SIZE_8BIT, buffer, len, HAL_MAX_DELAY);
    return (status == HAL_OK) ? 1 : 0;
}

// Read data from a register
uint8_t ICM42670_ReadRegister(ICM42670 *sensor, uint8_t reg, uint8_t *buffer, uint8_t len) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(sensor->hi2c, sensor->addr << 1, reg, I2C_MEMADD_SIZE_8BIT, buffer, len, HAL_MAX_DELAY);
    return (status == HAL_OK) ? 1 : 0;
}

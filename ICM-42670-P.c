#include "ICM-42670-P.h"


uint8_t ICM42670_switch_on_mclk(ICM42670 *sensor)
{
	uint8_t i;
	uint8_t sensorConf = 0x1F;
	
	if (!ICM42670_Write(sensor, ICM42670_REG_PWR_MGMT0, &sensorConf,1))
	{
		return HAL_ERROR;
	}
	/* Check if MCLK is ready */
	for (i = 0; i < 10; ++i) {
		if (ICM42670_ReadRegister(sensor, ICM42670_MCLK_RDY, &sensorConf, 1)) {
			if((sensorConf&0x08) == 0x08)
			{
				return HAL_OK;
			}				
		}
	}
	return HAL_ERROR;
}

uint8_t ICM42670_switch_off_mclk(ICM42670 *sensor)
{
	uint8_t sensorConf = 0x0F;
	if (!ICM42670_Write(sensor, ICM42670_REG_PWR_MGMT0, &sensorConf,1))	
	{
		return HAL_ERROR;	
	}
	return HAL_OK;
}
//To achieve a 10 microsecond delay using HAL libraries, we'll use the DWT (Data Watchpoint and Trace)
void DWT_Delay_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0; // Reset the cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // Enable the cycle counter
}

//function to delay for a specified number of microseconds
void HAL_Delay_us(uint32_t microseconds) {
    uint32_t start = DWT->CYCCNT;
    uint32_t end = start + (HAL_RCC_GetHCLKFreq() / 1000000) * microseconds;
    while (DWT->CYCCNT < end) {
        // Wait until the cycle counter reaches the desired value
    }
}

// Initialize the sensor
uint8_t InitMPU(ICM42670 *sensor, uint8_t addr, I2C_HandleTypeDef *hi2c) {
		uint8_t i,Status_register,current_value;
    sensor->hi2c = hi2c; // Set I2C handle
    sensor->addr = addr; // Set I2C address
    uint8_t whoAmI = ICM42670_WhoAmI(sensor); // Check WHO_AM_I register
    if (whoAmI != ICM42670_WHO_AM_I) {
			return HAL_ERROR; // Failure
    } 
		
		//Config us delay
		DWT_Delay_Init();
		
		//Read Current Status
    if (!ICM42670_ReadRegister(sensor, ICM42670_INT_STATUS, &Status_register, 1)) {
        return HAL_ERROR; // Failure
    }
		
		if(ICM42670_RESET_CONFIG(sensor)==HAL_ERROR)
		{
			return HAL_ERROR; // Failure
		}
				
		for(i=0;i<20;i++)
		{
			HAL_Delay(1);
			if (!ICM42670_ReadRegister(sensor, ICM42670_INT_STATUS, &current_value, 1)) {
					return HAL_ERROR; // Failure
			}
			if(current_value == 0x10)//Reset INT DONE
			{
				break;
			}
		}
		
		if(current_value != 0x10)//Reset INT DONE
		{
				return HAL_ERROR; // Failure
		}
		
		
		return HAL_OK; // Success
}

// Read the WHO_AM_I register
uint8_t ICM42670_WhoAmI(ICM42670 *sensor) {
    uint8_t buffer[1];
    if (ICM42670_ReadRegister(sensor, ICM42670_WHO_AM_I_REG, buffer, 1)) {
        return buffer[0]; // Return WHO_AM_I value
    }
    return 0xFF; // Error value
}

void IMU_PowerDown(ICM42670 *sensor) {
    // Disable accelerometer and gyroscope by writing to PWR_MGMT0
		uint8_t sensorConf = 0x00;
    ICM42670_Write(sensor, ICM42670_REG_PWR_MGMT0, &sensorConf,1);
    HAL_Delay(10); // Wait for the sensor to enter standby mode
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

//Reset FIFO
uint8_t ICM42670_RESET_FIFO(ICM42670 *sensor)
{
		uint8_t temp_buffer[1];
		temp_buffer[0]=0x04;
		if (!ICM42670_Write(sensor, ICM42670_FIFO_RESET, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		HAL_Delay_us(10); // Delay for 10 microseconds
		return HAL_OK;
}

//Reset CONFIG
uint8_t ICM42670_RESET_CONFIG(ICM42670 *sensor)
{
		uint8_t temp_buffer[1];
		temp_buffer[0]=0x10;
		if (!ICM42670_Write(sensor, ICM42670_FIFO_RESET, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		HAL_Delay_us(10); // Delay for 10 microseconds
		return HAL_OK;
}

//Set FIFO level (watermark)
uint8_t ICM42670_SetFIFO_Level(ICM42670 *sensor,uint16_t level)
{
		uint8_t temp_buffer[1];
		temp_buffer[0]=level&0xFF;
		if (!ICM42670_Write(sensor, ICM42670_FIFO_CONFIG2, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		temp_buffer[0]=(level>>8)&0x0F;
		if (!ICM42670_Write(sensor, ICM42670_FIFO_CONFIG3, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		return HAL_OK;
}

//Read FIFO level counter
uint8_t ICM42670_Read_FIFO_Counter(ICM42670 *sensor,uint16_t *level)
{
		uint8_t temp_buffer[2];
		if (!ICM42670_ReadRegister(sensor, ICM42670_FIFO_COUNT_H, temp_buffer, 2)) {
			return HAL_ERROR;
		}
		*level = (temp_buffer[0]<<8) | temp_buffer[1];
		return HAL_OK;
}

//Read FIFO LOST counter
uint8_t ICM42670_Read_FIFO_Lost_Packets(ICM42670 *sensor,uint16_t *lost)
{
		uint8_t temp_buffer[2];
		if (!ICM42670_ReadRegister(sensor, ICM42670_FIFO_LOST_PKT0, temp_buffer, 2)) {
			return HAL_ERROR;
		}
		*lost = (temp_buffer[1]<<8) | temp_buffer[0];
		return HAL_OK;
}

//Read FIFO Data
uint8_t ICM42670_Read_FIFO_Data(ICM42670 *sensor,uint8_t *data,uint16_t len)
{
			if (!ICM42670_ReadRegister(sensor, ICM42670_FIFO_DATA,data, len)) {
				return HAL_ERROR;
			}	
		return HAL_OK;
}

//Decode FIFO PACKET
//it will decode a 16 byte buffer as packet into acc , gyro , temp and time information
//it also return the header of the packet as result of the fucntion.
uint8_t ICM42670_Decode_Packet(uint8_t *buffer, sensorXYZ *ACCEL, sensorXYZ *GYRO, uint16_t *TEMP, uint16_t *TIME)
{
    ACCEL->x = (buffer[1] << 8) | (buffer[2]);
    ACCEL->y = (buffer[3] << 8) | (buffer[4]);
    ACCEL->z = (buffer[5] << 8) | (buffer[6]);

    GYRO->x = (buffer[7] << 8) | (buffer[8]);
    GYRO->y = (buffer[9] << 8) | (buffer[10]);
    GYRO->z = (buffer[11] << 8) | (buffer[12]);

    *TEMP = (buffer[13] / 2) + 25;

    *TIME = (buffer[14] << 8) | (buffer[15]);

    return buffer[0];
}


//Enable FIFO
uint8_t ICM42670_Initilize_FIFO(ICM42670 *sensor,uint16_t FIFO_LEVEL_THERESHOLD)
{
		uint8_t temp_buffer[1];
	
    //Enable RC oscillator
		if(ICM42670_switch_on_mclk(sensor)==HAL_ERROR)
		{
			return HAL_ERROR;
		}

		//Enable ACC and Gryo to go into FIFO
		if (ICM42670_BANK1_WRITE(sensor,ICM42670_BK1_FIFO_CONFIG5,0x03)==HAL_ERROR){
			return HAL_ERROR;
		}		
		
		//Verify Enable ACC and Gryo in FIFO
		temp_buffer[0]=0;
		if (ICM42670_BANK1_READ(sensor,ICM42670_BK1_FIFO_CONFIG5,temp_buffer)==HAL_ERROR){
			return HAL_ERROR;
		}				
		if(temp_buffer[0]!=0x03)
		{
			return HAL_ERROR;
		}

		//Disable RC oscillator
		if(ICM42670_switch_off_mclk(sensor)==HAL_ERROR)
		{
			return HAL_ERROR;
		}
		
		//Set FIFO level thershold
		if (ICM42670_SetFIFO_Level(sensor,FIFO_LEVEL_THERESHOLD)==HAL_ERROR){
			return HAL_ERROR;
		}		
		
		//Enable FIFO threshold interrupt
		temp_buffer[0] = 0x04;
		if (!ICM42670_Write(sensor, ICM42670_INT_SOURCE0, temp_buffer, 1)) {
			return HAL_ERROR;
		}	
		
		//Flush FIFO Buffer
		if (ICM42670_RESET_FIFO(sensor)==HAL_ERROR){
			return HAL_ERROR;
		}
		
		HAL_Delay_us(10); // Delay for 10 microseconds
		
		//Disable FIFO Bypassing and Enable FIFO Streaming to make data go into FIFO
		temp_buffer[0] = 0x00;
		if (!ICM42670_Write(sensor, ICM42670_FIFO_CONFIG1, temp_buffer, 1)) {
			return HAL_ERROR;
		}		

		return HAL_OK;
}

//Read Register from other Banks
uint8_t ICM42670_BANK1_READ(ICM42670 *sensor, uint8_t reg, uint8_t *result)
{
		uint8_t temp_buffer[1];
	
		//check MCLK is ready
		if (!ICM42670_ReadRegister(sensor, ICM42670_MCLK_RDY, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		if((temp_buffer[0]&0x08)==0)//if clock is not running
		{
			return HAL_ERROR;
		}
		
		temp_buffer[0]=0;
		if (!ICM42670_Write(sensor, ICM42670_REG_BLK_SEL_R, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		
		temp_buffer[0]=reg;
		if (!ICM42670_Write(sensor, ICM42670_REG_MADDR_R, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		
		HAL_Delay_us(10); // Delay for 10 microseconds
		
		if (!ICM42670_ReadRegister(sensor, ICM42670_REG_M_R, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		
		HAL_Delay_us(10); // Delay for 10 microseconds

		*result=temp_buffer[0];
		return HAL_OK;
}

//Write Register from other Banks
uint8_t ICM42670_BANK1_WRITE(ICM42670 *sensor, uint8_t reg, uint8_t value)
{
		uint8_t temp_buffer[1];
	
		//check MCLK is ready
		if (!ICM42670_ReadRegister(sensor, ICM42670_MCLK_RDY, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		if((temp_buffer[0]&0x08)==0)//if clock is not running
		{
			return HAL_ERROR;
		}
		
		temp_buffer[0]=0;
		if (!ICM42670_Write(sensor, ICM42670_REG_BLK_SEL_W, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		
		temp_buffer[0]=reg;
		if (!ICM42670_Write(sensor, ICM42670_REG_MADDR_W, temp_buffer, 1)) {
			return HAL_ERROR;
		}

		temp_buffer[0]=value;
		if (!ICM42670_Write(sensor, ICM42670_REG_M_W, temp_buffer, 1)) {
			return HAL_ERROR;
		}
		
		HAL_Delay_us(10); // Delay for 10 microseconds
		
		return HAL_OK;
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

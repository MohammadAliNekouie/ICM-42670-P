# ICM-42670-P
It's a Startup library for ICM42670 the IMU sensor form TDK in C for STM32 series using HAL




How to use(BASIC MODE):
```C
#include "ICM-42670-P.h"
//MPU Variables
ICM42670 imu;
I2C_HandleTypeDef hi2c1;

//ICM42670 Init
if(InitMPU(&imu, ICM42670_DEFAULT_ADDRESS, &hi2c1)!=HAL_OK)
{
	LOG_RTT("MPU INIT FAILED\r\n");	
}

//Setup rate & scale
ICM42670_StartAccel(&imu, ICM42670_CONFIG_ACCEL_4_G, ICM42670_CONFIG_RATE_100_Hz);
ICM42670_StartGyro(&imu, ICM42670_CONFIG_GYRO_500_DPS, ICM42670_CONFIG_RATE_100_Hz);

//IMU TEST LOOP
while (1) {
	sensorXYZ accel = ICM42670_GetAccel(&imu);
	sensorXYZ gyro = ICM42670_GetGyro(&imu);
	int16_t temp = ICM42670_GetTemp(&imu);
	// Print or process sensor data
	sprintf(buff,"AX=%d,AY=%d,AZ=%d\r\nGX=%d,GY=%d,GZ=%d\r\nTemp=%d\r\n",accel.x,accel.y,accel.z,gyro.x,gyro.y,gyro.z,temp);
	LOG_RTT(buff);
	HAL_Delay(1000);
}
```


How to use(FIFO MODE):
```C
#include "ICM-42670-P.h"
//MPU Variables
ICM42670 imu;
I2C_HandleTypeDef hi2c1;
uint8_t		IMU_BUFFER[16];
uint16_t  IMU_FIFO_LOST=0;
sensorXYZ accel;
sensorXYZ gyro;
uint16_t temperature;
uint16_t timestamp;

//ICM42670 Init
if(InitMPU(&imu, ICM42670_DEFAULT_ADDRESS, &hi2c1)!=HAL_OK)
{
	LOG_RTT("MPU INIT FAILED\r\n");	
}

while(ICM42670_Initilize_FIFO(&imu,1024)!=HAL_OK)
{
	LOG_RTT("FIFO INIT FAILED\r\n");
	HAL_Delay(300);
}	

//Setup rate & scale
ICM42670_StartAccel(&imu, ICM42670_CONFIG_ACCEL_4_G, ICM42670_CONFIG_RATE_100_Hz);
ICM42670_StartGyro(&imu, ICM42670_CONFIG_GYRO_500_DPS, ICM42670_CONFIG_RATE_100_Hz);

//IMU FIFO TEST LOOP
while (1) {
if(ICM42670_Read_FIFO_Counter(&imu,&IMU_FIFO_LEN)==HAL_OK)
{
	if(ICM42670_Read_FIFO_Lost_Packets(&imu,&IMU_FIFO_LOST)==HAL_OK)
	{
		if(IMU_FIFO_LEN>16)//16Byte is minimum FIFO lenght
		{
			ICM42670_Read_FIFO_Data(&imu,IMU_BUFFER,16);
			ICM42670_Decode_Packet(IMU_BUFFER,&accel,&gyro,&temperature,&timestamp);
			sprintf(buff,"%d,%d,%d,%d,%d,%d,%d--%d\r\n",accel.x,accel.y,accel.z,gyro.x,gyro.y,gyro.z,temperature,timestamp);
			LOG_RTT(buff);
		}
	}
}
HAL_Delay(15);
}
```

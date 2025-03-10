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

//Init FIFO
while(ICM42670_Initilize_FIFO(&imu,1024)!=HAL_OK)
{
	LOG_RTT("FIFO INIT FAILED\r\n");
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


How to use(APEX MOTION PROCESSING FUNCTIONS):
```C
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
	ICM42670_APEX_INITIALIZE_PEDOMETER(&imu,10,8,5,2,4,1,0,0);//(&imu,10,8,5,1,4,2,0);
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//3-Enable Interrupt Generation-------   //0 = Disable ,1 = Enable
	//2-smd_sensivity(2:0)--------------  |  //(316<<smd_sensivity[2:0])-1
	//1-IMU sensor definition-------    | |
	////////////////////////////    |   | |    ///////////////////////////////////////////////////	
	ICM42670_APEX_INITIALIZE_SMD(&imu,1,1);
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	//3-Enable Interrupt Generation---------------------      //0 = Disable ,1 = Enable		
	//2-TITL_WAIT_TIME---------------------------       |			//0 =0sec, 1 =2sec, 2 =4sec, 3=6sec
	//1-IMU sensor definition---------           |      |
	///////////////////////////////   |          |      |     /////////////////////////////////////	
	ICM42670_APEX_INITIALIZE_TITL(&imu,TILT_WAIT_0SEC,1);
	
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
```

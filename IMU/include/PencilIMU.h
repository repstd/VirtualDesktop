#pragma once
#include "hidapi.h"
//#include <thread>
using namespace std;

extern "C" int hid_init();
extern "C" int hid_exit();
extern "C" hid_device * hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number);
extern "C" int hid_write(hid_device *device, const unsigned char *data, size_t length);
extern "C" int hid_read(hid_device *device, unsigned char *data, size_t length);
extern "C" int hid_set_nonblocking(hid_device *device, int nonblock);
extern "C" void hid_close(hid_device *device);
extern "C" const wchar_t *hid_error(hid_device *device);
//extern "C" int hidInit();

class IMUCalibration{


};
const int ACC_X = 0;
const int ACC_Y = 1;
const int ACC_Z = 2;
const int GYRO_X = 3;
const int GYRO_Y = 4;
const int GYRO_Z = 5;
const int MAG_X = 6;
const int MAG_Y = 7;
const int MAG_Z = 8;
const int  ROTATION_R = 9;
const int ROTATION_I = 10;
const int ROTATION_J = 11;
const int ROTATION_K = 12;
const int VENDOR_ID = 0x7792;
const int PRODUCT_ID = 0x9981;

//start  新版本功能  liang

#define INIT_SUCCESS  0
#define EXIT_SUCCESS  0 


unsigned char *cmd_write_data = new unsigned char[4];

const unsigned char CMD_SET_GET_CMD_ID = 0x81;//设置回读数据命令  后接CMD_GET_xxx_ID

const unsigned char CMD_GET_SERIAL_ID = 0x01;//获得串口号
const unsigned char CMD_GET_RAW_DATA_ID = 0x02;//获得原始数据
const unsigned char CMD_GET_ANGLE_DATA_ID = 0x03;//获得角度数据

const unsigned char CMD_SET_IMU_RESET_ID = 0x82;//九轴复位命令
const unsigned char CMD_SET_LED_DATA_ID = 0x83;//设置LED灯状态
const unsigned char CMD_SET_VOLUME_DATA_ID = 0x84;//设置音量大小
const unsigned char CMD_SET_BACKLIGNT_DATA_ID = 0x85;//设置显示背光
const unsigned char CMD_SET_TC35887_DATA_ID = 0x86;//设置显示复位
const unsigned char CMD_SET_IMU_CAL_ID = 0x87;//九轴校准

//end  新版本功能  liang
// 原始数据长度
const int MULTI_DATA_COUNT = 7;//组合数据长度
//std::thread mUpdateThread;//线程
hid_device *mDevice;//USB HID设备
bool mHidInit;//HID设备初始化 状态
bool mCalibration;//校准标志
//bool mOnExit;//退出读数据标志
bool mToggleLED;//设置LED状态
bool mSerialNum;//获得串口号

bool mTogglrLED_Flag = true;//LED灯状态反转标志

unsigned char  *mMultiSrcData = new unsigned char[4 + 4 * MULTI_DATA_COUNT];//4 为首4字节  表示ID(1字节)+Seq(1字节)+2字节保留

float mMultiData[MULTI_DATA_COUNT];
const unsigned char *mSerialNumData = new const unsigned char[64];

bool imu_connectToDevice()
{
	if (!mHidInit)
	{
		mHidInit = (hid_init() == INIT_SUCCESS);
	}
	if (mHidInit)
	{
		printf("成功初始HIDAPI.");
		mDevice = hid_open(VENDOR_ID, PRODUCT_ID, NULL);
		if (mDevice != nullptr)
		{
			hid_set_nonblocking(mDevice, 1);
			printf("成功连接设备.");
			return true;
		}
		else
		{
			printf("连接设备失败.");
		}
	}
	else
	{
		printf("初始化HIDAPI失败.");
	}
	return false;
}

float *imu_readRawData()
{
	// read raw data
	// int res = HIDAPI.hidWrite(mDevice, CMD_RAW_DATA_READ, CMD_RAW_DATA_READ.Length);
	int res = hid_read(mDevice, mMultiSrcData, 4 + 4 * MULTI_DATA_COUNT);
	if (res > 0)
	{
		if (mMultiSrcData[0] == CMD_GET_RAW_DATA_ID)
		{
			
			for (int i = 0; i < MULTI_DATA_COUNT - 1; i++)
			{
				float *s = (float *)(&mMultiSrcData[4 + i * 4]);
				mMultiData[i] = *s;
			}
			return mMultiData;
		}
		//if (mMultiSrcData[0] == CMD_GET_SERIAL_ID)
		//{
		//    for (int i = 0; i < 16; i++)
		//    {
		//        mSerialNumData[i] = mMultiSrcData[i];
		//    }
		//    return null;
		//}

	}
	return NULL;
}

void imu_exit() {

	hid_close(mDevice);
	mHidInit = !(hid_exit == EXIT_SUCCESS);

	mCalibration = false;
}

//void run()
//{
//	int tt = 0;
//	float *rawData;
//	while (true)
//	{
//		rawData = readRawData();
//		//printf("读数据中:%f,%f,%f,%f\n", mMultiData[1], mMultiData[0], mMultiData[2], mMultiData[3]);
//		if (mOnExit)
//		{
//			exit();
//
//			break;
//		}
//	}
//
//}

//void startRead()
//{
//
//	thread mUpdateThread(run);
//	mCalibration = false;
//	mOnExit = false;
//	printf("开始读取传感器数据.");
//	//mUpdateThread.join();
//	mUpdateThread.detach();
//
//
//
//}

bool imu_calibration()
{

	cmd_write_data[0] = CMD_SET_GET_CMD_ID;
	cmd_write_data[1] = CMD_SET_IMU_CAL_ID;
	cmd_write_data[2] = 0;//保留
	cmd_write_data[3] = 0;//保留

	int res = hid_write(mDevice, cmd_write_data, 4);
	if (res > -1)
	{
		return true;
	}
	return false;

}

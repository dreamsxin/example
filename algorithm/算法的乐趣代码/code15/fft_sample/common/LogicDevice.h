#ifndef __LOGIC_DEVICE_H__
#define __LOGIC_DEVICE_H__

void SetXLogicRange(double min, double max);
void SetYLogicRange(double min, double max);
void SetDevicePointRange(int left, int top, int right, int bottom);
int DeviceXFromLogicX(double x);
int DeviceYFromLogicY(double y);

void GetDeviceCenter(int& cx, int& cy);

#endif //__LOGIC_DEVICE_H__
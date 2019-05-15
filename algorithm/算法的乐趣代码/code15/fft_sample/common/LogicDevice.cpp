#include "stdafx.h"
#include "LogicDevice.h"
#include "math.h"

int wndRgLeft = 0;
int wndRgRight = 100;
int wndRgTop = 0;
int wndRgBottom = 100;
int centerX = 50;
int centerY = 50;

double luX = 1.0;
double luY = 1.0;

double x_min = 0.0;
double x_max = 100.0;
double y_min = 0.0;
double y_max = 100.0;
double x_center = 50.0;
double y_center = 50.0;

void SetXLogicRange(double min, double max)
{
    x_min = min;
    x_max = max;
    luX = (wndRgRight - wndRgLeft) / fabs(x_max - x_min);
    if((x_min >= 0) && (x_max >= 0))
    {
        centerX = wndRgLeft;
        x_center = x_min;
    }
    else if((x_min <= 0) && (x_max <= 0))
    {
        centerX = wndRgRight;
        x_center = x_max;
    }
    else
    {
        double scale = fabs(x_min) / fabs(x_max - x_min);
        centerX = wndRgLeft + int((wndRgRight - wndRgLeft) * scale);
        x_center = 0.0;
    }
}

void SetYLogicRange(double min, double max)
{
    y_min = min;
    y_max = max;
    luY = (wndRgBottom - wndRgTop) / fabs(y_max - y_min);
    if((y_min >= 0) && (y_max >= 0))
    {
        centerY = wndRgBottom;
        y_center = y_min;
    }
    else if((y_min <= 0) && (y_max <= 0))
    {
        centerY = wndRgTop;
        y_center = y_max;
    }
    else
    {
        double scale = fabs(y_min) / fabs(y_max - y_min);
        centerY = wndRgBottom - int((wndRgBottom - wndRgTop) * scale);
        y_center = 0.0;
    }
}

void SetDevicePointRange(int left, int top, int right, int bottom)
{
    wndRgLeft = left;
    wndRgRight = right;
    wndRgTop = top;
    wndRgBottom = bottom;

    SetXLogicRange(x_min, x_max);
    SetYLogicRange(y_min, y_max);
}

int DeviceXFromLogicX(double x)
{
    if((x < x_min) || (x > x_max))
    {
        return 0;
    }
    int devX = centerX + int((x - x_center) * luX + 0.5);

    return devX;
}

int DeviceYFromLogicY(double y)
{
    if((y < y_min) || (y > y_max))
    {
        return 0;
    }

    int devY = centerY - int((y - y_center) * luY + 0.5);

    return devY;
}

void GetDeviceCenter(int& cx, int& cy)
{
    cx = centerX;
    cy = centerY;
}


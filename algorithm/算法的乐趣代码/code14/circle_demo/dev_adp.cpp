#include "stdafx.h"
#include "dev_adp.h"

HDC g_DevHandle = NULL;
COLORREF gDevColor = RGB(255,0,0);
int gDevWidth = 1280;
int gDevHeight = 800;
int gCenterX = 640;
int gCenterY = 400;


HDC SetCurrentDevice(HDC hDC)
{
    HDC tmp = g_DevHandle;
    g_DevHandle = hDC;

    return tmp;
}

COLORREF SetDeviceColor(COLORREF color)
{
    COLORREF tmp = gDevColor;
    gDevColor = color;

    return tmp;
}

void SetDevicePixel(int x, int y)
{
    int devX,devY;

    if(g_DevHandle != NULL)
    {
        devX = gCenterX + x;
        devY = gDevHeight - (gCenterY + y);
        ::SetPixel(g_DevHandle, devX, devY, gDevColor);
    }
}

void SetDeviceWidth(int width)
{
    gDevWidth = width;
    gCenterX = gDevWidth / 2;
}

void SetDeviceHeight(int height)
{
    gDevHeight = height;
    gCenterY = gDevHeight / 2;
}

void DrawAxis()
{
    if(g_DevHandle != NULL)
    {
        ::MoveToEx(g_DevHandle, 0,gCenterY, (LPPOINT) NULL); 
        ::LineTo(g_DevHandle, gDevWidth, gCenterY); 
        ::MoveToEx(g_DevHandle, gCenterX, 0, (LPPOINT) NULL); 
        ::LineTo(g_DevHandle, gCenterX, gDevHeight); 
    }
}

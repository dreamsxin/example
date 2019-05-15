#ifndef __DEV_ADP_H__
#define __DEV_ADP_H__

HDC SetCurrentDevice(HDC hDC);
COLORREF SetDeviceColor(COLORREF color);
void SetDevicePixel(int x, int y);
void SetDeviceWidth(int width);
void SetDeviceHeight(int height);
void DrawAxis();

#endif /*__DEV_ADP_H__*/


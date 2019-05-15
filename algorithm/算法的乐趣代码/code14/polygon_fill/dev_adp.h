#ifndef __DEV_ADP_H__
#define __DEV_ADP_H__

const int MAX_MAP_WIDTH = 16;
const int MAX_MAP_HEIGHT = 16;

int GetPixelColor(int x, int y);
void SetPixelColor(int x, int y, int color);
void DrawHorizontalLine(int x1, int x2, int y, int color);
void FillBackground(int color);

void SetPixelMap(int map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT], int width, int height);
void PrintCurrentPixelMap();

#endif /*__DEV_ADP_H__*/


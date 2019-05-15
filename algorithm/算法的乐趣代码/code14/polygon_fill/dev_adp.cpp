#include "stdafx.h"
#include <iostream>
#include <cassert>
#include "dev_adp.h"

int color_pixel_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = { { 0 } };
int map_width = 0;
int map_height = 0;

int GetPixelColor(int x, int y)
{
    assert((x >= 0) && (x < MAX_MAP_WIDTH));
    assert((y >= 0) && (y < MAX_MAP_HEIGHT));

    return color_pixel_map[y][x];
}

void SetPixelColor(int x, int y, int color)
{
    assert((x >= 0) && (x < MAX_MAP_WIDTH));
    assert((y >= 0) && (y < MAX_MAP_HEIGHT));

    color_pixel_map[y][x] = color;
}

void DrawHorizontalLine(int x1, int x2, int y, int color)
{
    y = (MAX_MAP_HEIGHT - 1) - y;
    //for(int i = x1; i < x2; i++) //×ó±ÕÓÒ¿ª
    for(int i = x1; i <= x2; i++)
    {
        SetPixelColor(i, y, color);
    }
}

void FillBackground(int color)
{
    for(int i = 0; i < map_width; i++)
    {
        for(int j = 0; j < map_height; j++)
        {
            color_pixel_map[i][j] = color;
        }
    }
}

void SetPixelMap(int map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT], int width, int height)
{
    assert((width > 0) && (width <= MAX_MAP_WIDTH));
    assert((height > 0) && (height <= MAX_MAP_HEIGHT));
    
    map_width = width;
    map_height = height;
    for(int i = 0; i < map_width; i++)
    {
        for(int j = 0; j < map_height; j++)
        {
            color_pixel_map[i][j] = map[i][j];
        }
    }
}

void PrintCurrentPixelMap()
{
    for(int i = 0; i < map_width; i++)
    {
        for(int j = 0; j < map_height; j++)
        {
            std::cout << color_pixel_map[i][j] << ' ';
        }

        std::cout << std::endl;
    }
    std::cout << std::endl<< std::endl;
}


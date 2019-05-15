// polygon_fill.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "polygon.h"
#include "dev_adp.h"
#include "recursion_seed_fill.h"
#include "scanline_seed_fill.h"
#include "scanline_fill.h"
#include "scanline_fill_o.h"
#include "edge_center_fill.h"
#include "edge_mark_fill.h"

int flood_seed_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = 
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0 },
    { 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0 },
    { 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 0, 0, 0, 0 },
    { 0, 0, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0 },
    { 0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0 },
    { 0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

int boundary_seed_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = 
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0 },
    { 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0 },
    { 0, 1, 2, 2, 3, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0 },
    { 0, 1, 2, 2, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0 },
    { 0, 1, 3, 2, 3, 2, 2, 3, 1, 1, 2, 2, 1, 1, 0, 0 },
    { 0, 1, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 3, 1, 1, 0 },
    { 0, 1, 1, 2, 2, 2, 1, 1, 2, 2, 2, 3, 2, 2, 1, 0 },
    { 0, 0, 1, 2, 2, 2, 1, 1, 2, 2, 3, 2, 2, 2, 1, 0 },
    { 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 3, 2, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 2, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
/*seed (6,7)*/
int scanline_seed_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = 
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
    { 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
    { 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

int scanline_poly_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = 
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

int edge_center_map[MAX_MAP_WIDTH][MAX_MAP_HEIGHT] = 
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};


void TestFloodSeedFill()
{
    SetPixelMap(flood_seed_map, 16, 16);
    PrintCurrentPixelMap();
    FloodSeedFill(4, 5, 2, 4);
    PrintCurrentPixelMap();
}

void TestBoundarySeedFill()
{
    SetPixelMap(boundary_seed_map, 16, 16);
    PrintCurrentPixelMap();
    BoundarySeedFill(4, 5, 4, 1);
    PrintCurrentPixelMap();
}

void TestScanLineSeedFill()
{
    SetPixelMap(scanline_seed_map, 16, 16);
    PrintCurrentPixelMap();
    ScanLineSeedFill(6, 7, 4, 1);
    PrintCurrentPixelMap();
}

void TestScanLineFill()
{
    POINT pts[] = { {2, 8}, {1, 3}, {5, 1}, {13, 5}, {9, 10}, {7, 6} };
    Polygon py = Polygon(pts, COUNT_OF(pts));

    SetPixelMap(scanline_poly_map, 16, 16);

    ScanLinePolygonFill(py, 2);

    PrintCurrentPixelMap();
}

void TestScanLineFill2()
{
    POINT pts[] = { {2, 8}, {1, 3}, {5, 1}, {13, 5}, {9, 10}, {7, 6} };
    Polygon py = Polygon(pts, COUNT_OF(pts));

    SetPixelMap(scanline_poly_map, 16, 16);

    ScanLinePolygonFill2(py, 2);

    PrintCurrentPixelMap();
}

void TestEdgeCenterMarkFill()
{
    POINT pts[] = { {2, 1}, {12, 1}, {12, 14}, {5, 8}, {2, 11} };
    Polygon py = Polygon(pts, COUNT_OF(pts));

    SetPixelMap(edge_center_map, 16, 16);

    EdgeCenterMarkFill(py, 2);

    PrintCurrentPixelMap();
}

void TestEdgeFenceMarkFill()
{
    POINT pts[] = { {2, 1}, {12, 1}, {12, 14}, {5, 8}, {2, 11} };
    Polygon py = Polygon(pts, COUNT_OF(pts));

    SetPixelMap(edge_center_map, 16, 16);

    EdgeFenceMarkFill(py, 5, 2);

    PrintCurrentPixelMap();
}

int main(int argc, char* argv[])
{
    //TestFloodSeedFill();
    //TestBoundarySeedFill();
    //TestScanLineSeedFill();

    //TestScanLineFill();

    //TestScanLineFill2();

    //TestEdgeCenterMarkFill();

    TestEdgeFenceMarkFill();

	return 0;
}

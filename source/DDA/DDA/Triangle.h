#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

struct Point2d
{
	float x;
	float y;
};
struct Triangle
{
	Point2d p[3];
};

#define AREA_CELL_WIDTH 8
#define AREA_CELL_HEIGHT 8
#define MAX_TRIANGLE 1000
// each triangle max in 4 quads
#define MAX_TRIANGLE_IDS 4000 

#endif
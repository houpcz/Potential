//----------------------------------------------------------------------------------------
/**
 * \file       Triangle.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains Triangle class and few global constants
 *
 *  Simple triangle struct
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

struct Point2d
{
	float x;	///< x coordinate of point
	float y;	///< y coordinate of point
};
struct Triangle
{
	Point2d p[3];	///< triangle points
};

#define AREA_CELL_WIDTH 8
#define AREA_CELL_HEIGHT 8
#define MAX_TRIANGLE 1000
// each triangle max in 4 quads
#define MAX_TRIANGLE_IDS 4000 

#endif
//----------------------------------------------------------------------------------------
/**
 * \file       Point2D.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Simple Point2D class.
 *
 *  2D point containing method Distance to another point and a few usefull methods.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _POINT2D_H_
#define _POINT2D_H_

class Point2D
{
private:
	float length;		///< length of vector
	float x;			///< x coord
	float y;			///< y coord

	void ComputeLength();
public:
	Point2D(float _x, float _y);
	~Point2D(void);
	float X() { return x; };
	float Y() { return y; };
	void Set(float _x, float _y);
	float Distance(const Point2D & pointB);
	void SetLength(float _length);
};

#endif


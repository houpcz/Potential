#include "Point2D.h"
#include <math.h>

Point2D::Point2D(float _x, float _y)
{
	x = _x;
	y = _y;
	ComputeLength();
}

void Point2D::Set(float _x, float _y)
{
	x = _x;
	y = _y;
	ComputeLength();
}

Point2D::~Point2D(void)
{
}

void Point2D::ComputeLength()
{
	length = sqrt(x * x + y * y);
}

void Point2D::SetLength(float _length)
{
	x = (x / length) * _length;
	y = (y / length) * _length;
	length = _length;
}

float Point2D::Distance(const Point2D & pointB)
{
	float dx = x - pointB.x;
	float dy = y - pointB.y;

	return sqrt(dx*dx + dy*dy);
}

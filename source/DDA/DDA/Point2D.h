#pragma once

class Point2D
{
private:
	float length;
	float x;
	float y;

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


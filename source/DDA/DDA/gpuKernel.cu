#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <device_functions.h>
#include <math.h>
#include "Triangle.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

__device__ bool pointTriangleTest(Point2d & p, Triangle & t)
{
	float ab = (t.p[0].x - p.x) * (t.p[1].y - p.y) - (t.p[1].x - p.x) * (t.p[0].y - p.y);
	float bc = (t.p[1].x - p.x) * (t.p[2].y - p.y) - (t.p[2].x - p.x) * (t.p[1].y - p.y);
	float ca = (t.p[2].x - p.x) * (t.p[0].y - p.y) - (t.p[0].x - p.x) * (t.p[2].y - p.y);
	
	return (ab <= 0.0f && bc <= 0.0f && ca <= 0.0f) || (ab >= 0.0f && bc >= 0.0f && ca >= 0.0f);
}

__device__ float countFieldTile(int x, int y, int goalX, int goalY)
{
	float dx = x - goalX;
	float dy = y - goalY;
	float initValue = sqrtf(dx * dx + dy * dy);

	/*
	bool inTriangle = false;
	for(int loop1 = 0; loop1 < obstacle->size(); loop1++)
	{
		if((*obstacle)[loop1]->polygon().containsPoint(QPointF(x, y), Qt::FillRule::WindingFill))
		{
			inTriangle = true;
			break;
		}
	}
	float obst = inTriangle ? PotentialField::OBSTACLE : 0;
	*/
	return initValue; // + obst;
}

__global__ void countFields(float * devPotentialField, int fieldWidth, int tileWidth, float * fieldCenterX, float * fieldCenterY, int * goalX, int * goalY)
{
	int fieldID = blockIdx.x;
	int threadX = threadIdx.x;
	int threadY = threadIdx.y;
	int myWorkFieldWidth = fieldWidth / blockDim.x;
	int myWorkFieldHeight = fieldWidth / blockDim.y;
	int endWorkX = MAX((threadX + 1) * myWorkFieldWidth, fieldWidth);
	int endWorkY = MAX((threadY + 1) * myWorkFieldHeight, fieldWidth);
	int x, y;

	for(int loop1 = threadY * myWorkFieldWidth; loop1 < endWorkY; loop1++)
	{
		for(int loop2 = threadX * myWorkFieldWidth; loop2 < endWorkX; loop2++)
		{
			x = (int) fieldCenterX[fieldID] + (loop2 - (fieldWidth / 2)) * tileWidth; 
			y = (int) fieldCenterY[fieldID] + (loop1 - (fieldWidth / 2)) * tileWidth;
			devPotentialField[fieldID * fieldWidth * fieldWidth + loop1 * fieldWidth + loop2] = countFieldTile(x, y, goalX[fieldID], goalY[fieldID]);
		}
	}
}
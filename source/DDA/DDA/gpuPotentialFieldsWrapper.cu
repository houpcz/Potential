
#include <stdio.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>
#include <device_functions.h>
#include <math.h>
#include <algorithm>
#include "Triangle.h"

#define OBSTACLE 100000.0f
#define SMALL_OBSTACLE 10000.0f;
__global__ void countFields(float * devPotentialField, int fieldWidth, int tileWidth, float * fieldCenterX, float * fieldCenterY, int * goalX, int * goalY);
__global__ void smoothFields(float * devPotentialFieldIn, float *devPotentialFieldOut, int fieldWidth, int tileWidth);

inline void
check_cuda_errors (const char *filename, const int line_number)
{
  cudaThreadSynchronize ();
  cudaError_t error = cudaGetLastError ();
  if (error != cudaSuccess)
    {
      printf ("CUDA error at %s:%i: %s\n", filename, line_number,
	      cudaGetErrorString (error));
      exit (-1);
    }
}


static void
HandleError (cudaError_t error, const char *file, int line)
{
  if (error != cudaSuccess)
    {
      printf ("%s in %s at line %d\n", cudaGetErrorString (error), file,
	      line);
      exit (EXIT_FAILURE);
    }
}

#define CHECK_ERROR( error ) ( HandleError( error, __FILE__, __LINE__ ) )

#define OBST_AREA_LEFT 0
#define OBST_AREA_TOP 1
#define CELL_WIDTH 2
#define CELL_HEIGHT 3
__constant__ int param[4];

__constant__ int quadTree[AREA_CELL_WIDTH * AREA_CELL_HEIGHT];
__constant__ Triangle triangle[MAX_TRIANGLE];
__constant__ int triangleIDs[MAX_TRIANGLE_IDS];

float * cpuPotentialField;
float * devPotentialFieldIn;
float * devPotentialFieldOut;
float * devFieldCenterX;
float * devFieldCenterY;
int * devGoalX;
int * devGoalY;

int numberAgents;
int fieldWidth;
int tileWidth;

size_t memPotentialField;
size_t memGoal;
size_t memFieldCenter;

void gpuAllocMemory(int _numberAgents, int _fieldWidth, int _tileWidth)
{
	numberAgents = _numberAgents;
	fieldWidth = _fieldWidth;
	tileWidth = _tileWidth;

	memPotentialField = numberAgents * fieldWidth * fieldWidth * sizeof(float);
	memFieldCenter = numberAgents * sizeof(float);
	memGoal = numberAgents * sizeof(int);

	cpuPotentialField = (float *) malloc(memPotentialField);
	cudaMalloc( (void**)&devPotentialFieldIn, memPotentialField);
	cudaMalloc( (void**)&devPotentialFieldOut, memPotentialField);
	cudaMalloc( (void**)&devFieldCenterX, memFieldCenter);
	cudaMalloc( (void**)&devFieldCenterY, memFieldCenter);
	cudaMalloc( (void**)&devGoalX, memGoal);
	cudaMalloc( (void**)&devGoalY, memGoal);
}

void gpuAllocObstacles(int _obstAreaLeft, int _obstAreaTop, int _cellWidth, int _cellHeight, int * _quadTree, Triangle *_triangle, int triangleSize, int * _triangleIDs, int triangleIDsSize)
{
	int _param[4];
	_param[0] = _obstAreaLeft;
	_param[1] = _obstAreaTop;
	_param[2] = _cellWidth;
	_param[3] = _cellHeight;

	cudaMemcpyToSymbol((const char *) param, _param, sizeof(int) * 4) ;
	cudaMemcpyToSymbol((const char *) triangle, _triangle, sizeof(Triangle) * triangleSize) ;
	cudaMemcpyToSymbol((const char *) quadTree, _quadTree, sizeof(int) * AREA_CELL_WIDTH * AREA_CELL_HEIGHT) ;
	cudaMemcpyToSymbol((const char *) triangleIDs, _triangleIDs, sizeof(int) * triangleIDsSize) ;
}

void gpuCountPotentialFields(float *** potentialField, float * cpuFieldCenterX, float * cpuFieldCenterY, int * cpuGoalX, int * cpuGoalY)
{
	// grid configuration1
    dim3 gridRes(numberAgents, 1, 1);
    dim3 blockRes(fieldWidth / 2, fieldWidth / 2, 1);

	cudaMemcpy( devFieldCenterX, cpuFieldCenterX, memFieldCenter, cudaMemcpyHostToDevice ) ;
	cudaMemcpy( devFieldCenterY, cpuFieldCenterY, memFieldCenter, cudaMemcpyHostToDevice ) ;
	cudaMemcpy( devGoalX, cpuGoalX, memGoal, cudaMemcpyHostToDevice ) ;
	cudaMemcpy( devGoalY, cpuGoalY, memGoal, cudaMemcpyHostToDevice ) ;

    countFields<<< gridRes, blockRes >>>( devPotentialFieldIn, fieldWidth, tileWidth, devFieldCenterX, devFieldCenterY, devGoalX, devGoalY);
	CHECK_ERROR( cudaGetLastError() );
	cudaThreadSynchronize();
	smoothFields<<< gridRes, blockRes >>> (devPotentialFieldIn, devPotentialFieldOut, fieldWidth, tileWidth);
	cudaMemcpy( cpuPotentialField, devPotentialFieldOut, memPotentialField, cudaMemcpyDeviceToHost ) ;
	//cudaMemcpy( cpuPotentialField, devPotentialFieldIn, memPotentialField, cudaMemcpyDeviceToHost ) ;
	
	for(int loop1 = 0; loop1 < numberAgents; loop1++)
		for(int loop2 = 0; loop2 < fieldWidth; loop2++)
			for(int loop3 = 0; loop3 < fieldWidth; loop3++)
				potentialField[loop1][loop2][loop3] = cpuPotentialField[loop1 * fieldWidth * fieldWidth + loop2 * fieldWidth + loop3];
	
}

void gpuFreeMemory()
{
	free(cpuPotentialField);
	cudaFree(devPotentialFieldIn);
	cudaFree(devPotentialFieldOut);
	cudaFree(devFieldCenterX);
	cudaFree(devFieldCenterY);
	cudaFree(devGoalX);
	cudaFree(devGoalY);
}

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

__device__ bool pointTriangleTest1(int x, int y, const Triangle & t)
{
	float ab = (t.p[0].x - x) * (t.p[1].y - y) - (t.p[1].x - x) * (t.p[0].y - y);
	float bc = (t.p[1].x - x) * (t.p[2].y - y) - (t.p[2].x - x) * (t.p[1].y - y);
	float ca = (t.p[2].x - x) * (t.p[0].y - y) - (t.p[0].x - x) * (t.p[2].y - y);

	return ((ab <= 0.0f && bc <= 0.0f && ca <= 0.0f) || (ab >= 0.0f && bc >= 0.0f && ca >= 0.0f));
}

__device__ bool pointTriangleTest2(int x, int y, const Triangle & t)
{
	float p0x = t.p[0].x - x;
	float p0y = t.p[0].y - y;
	float p1x = t.p[1].x - x;
	float p1y = t.p[1].y - y;
	float p2x = t.p[2].x - x;
	float p2y = t.p[2].y - y;
	float ab = p0x * p1y - p1x * p0y;
	float bc = p1x * p2y - p2x * p1y;
	float ca = p2x * p0y - p0x * p2y;

	return ((ab <= 0.0f && bc <= 0.0f && ca <= 0.0f) || (ab >= 0.0f && bc >= 0.0f && ca >= 0.0f));
}

__device__ bool pointTriangleTest3(int x, int y, const Triangle & t)
{
	float p0x = t.p[0].x - x;
	float p0y = t.p[0].y - y;
	float p1x = t.p[1].x - x;
	float p1y = t.p[1].y - y;
	if(p0x * p1y - p1x * p0y < 0.0f) // ab
		return false;
	float p2x = t.p[2].x - x;
	float p2y = t.p[2].y - y;
	float bc = p1x * p2y - p2x * p1y;
	float ca = p2x * p0y - p0x * p2y;

	return bc >= 0.0f && ca >= 0.0f;
}

__device__ bool pointTriangleTest(int x, int y, const Triangle & t)
{
	float p0x = t.p[0].x - x;
	float p0y = t.p[0].y - y;
	float p1x = t.p[1].x - x;
	float p1y = t.p[1].y - y;
	float p2x = t.p[2].x - x;
	float p2y = t.p[2].y - y;

	return (p0x * p1y - p1x * p0y) >= 0.0f && (p1x * p2y - p2x * p1y) >= 0.0f && (p2x * p0y - p0x * p2y) >= 0.0f;
}

__device__ float countFieldTile(int x, int y, int goalX, int goalY)
{
	float dx = x - goalX;
	float dy = y - goalY;
	float initValue = sqrtf(dx * dx + dy * dy);

	int areaX = x - param[OBST_AREA_LEFT];
	int areaY = y - param[OBST_AREA_TOP];
	float obst = 0.0f;

	if(areaX >= 0 && areaY >= 0 && 
	   areaX < param[CELL_WIDTH] * AREA_CELL_WIDTH &&
	   areaY < param[CELL_HEIGHT] * AREA_CELL_HEIGHT)
	{
		int yid = areaY / param[CELL_HEIGHT];
		int xid = areaX / param[CELL_WIDTH];
		int id = xid + yid * AREA_CELL_WIDTH;
		int startX;
		if(id == 0)
			startX = 0;
		else
			startX = quadTree[id - 1];
		int endX = quadTree[id];

		for(int loop1 = startX; loop1 < endX; loop1++)
		{
			int triangleID = triangleIDs[loop1];
			if(pointTriangleTest(x, y, triangle[triangleID]))
			{
				obst = OBSTACLE;
				break;
			}
		}
	}
	
	return initValue + obst;
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

__global__ void smoothFields(float * devPotentialFieldIn, float *devPotentialFieldOut, int fieldWidth, int tileWidth)
{
	int fieldID = blockIdx.x;
	int threadX = threadIdx.x;
	int threadY = threadIdx.y;
	int myWorkFieldWidth = fieldWidth / blockDim.x;
	int myWorkFieldHeight = fieldWidth / blockDim.y;
	int endWorkX = MAX((threadX + 1) * myWorkFieldWidth, fieldWidth);
	int endWorkY = MAX((threadY + 1) * myWorkFieldHeight, fieldWidth);
	int arrayID;
	int fieldOffset = fieldID * fieldWidth * fieldWidth;
	for(int loop1 = threadY * myWorkFieldWidth; loop1 < endWorkY; loop1++)
	{
		for(int loop2 = threadX * myWorkFieldWidth; loop2 < endWorkX; loop2++)
		{
			arrayID = fieldOffset + loop1 * fieldWidth + loop2;
			if((loop2 > 0 && devPotentialFieldOut[arrayID - 1] > OBSTACLE) ||
			   (loop2 < fieldWidth - 1 && devPotentialFieldOut[arrayID + 1] > OBSTACLE) ||
			   (loop1 > 0 && devPotentialFieldOut[arrayID - fieldWidth] > OBSTACLE) ||
			   (loop1 < fieldWidth - 1 && devPotentialFieldOut[arrayID + fieldWidth] > OBSTACLE)) 
			{
				devPotentialFieldOut[arrayID] = devPotentialFieldIn[arrayID] + SMALL_OBSTACLE;
			} else 
			{
				devPotentialFieldOut[arrayID] = devPotentialFieldIn[arrayID];
			}
		}
	}
}

#include <stdio.h>
#include <cuda.h>
#include <cuda_runtime.h>

__global__ void countFields(float * devPotentialField, int fieldWidth, int tileWidth, float * fieldCenterX, float * fieldCenterY, int * goalX, int * goalY);

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

float * cpuPotentialField;
float * devPotentialField;
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
	cudaMalloc( (void**)&devPotentialField, memPotentialField);
	cudaMalloc( (void**)&devFieldCenterX, memFieldCenter);
	cudaMalloc( (void**)&devFieldCenterY, memFieldCenter);
	cudaMalloc( (void**)&devGoalX, memGoal);
	cudaMalloc( (void**)&devGoalY, memGoal);
}

void gpuCountPotentialFields(float *** potentialField, float * cpuFieldCenterX, float * cpuFieldCenterY, int * cpuGoalX, int * cpuGoalY)
{
	// grid configuration1
    dim3 gridRes(numberAgents, 1, 1);
    dim3 blockRes(fieldWidth / 4, fieldWidth / 4, 1);

	cudaMemcpy( devFieldCenterX, cpuFieldCenterX, memFieldCenter, cudaMemcpyHostToDevice ) ;
	cudaMemcpy( devFieldCenterY, cpuFieldCenterY, memFieldCenter, cudaMemcpyHostToDevice ) ;
	cudaMemcpy( devGoalX, cpuGoalX, memGoal, cudaMemcpyHostToDevice ) ;
	cudaMemcpy( devGoalY, cpuGoalY, memGoal, cudaMemcpyHostToDevice ) ;

    countFields<<< gridRes, blockRes >>>( devPotentialField, fieldWidth, tileWidth, devFieldCenterX, devFieldCenterY, devGoalX, devGoalY);
	CHECK_ERROR( cudaGetLastError() );
	cudaThreadSynchronize();

	cudaMemcpy( cpuPotentialField, devPotentialField, memPotentialField, cudaMemcpyDeviceToHost ) ;

	for(int loop1 = 0; loop1 < numberAgents; loop1++)
		for(int loop2 = 0; loop2 < fieldWidth; loop2++)
			for(int loop3 = 0; loop3 < fieldWidth; loop3++)
				potentialField[loop1][loop2][loop3] = cpuPotentialField[loop1 * fieldWidth * fieldWidth + loop2 * fieldWidth + loop3];
}

void gpuFreeMemory()
{
	free(cpuPotentialField);
	cudaFree(devPotentialField);
	cudaFree(devFieldCenterX);
	cudaFree(devFieldCenterY);
	cudaFree(devGoalX);
	cudaFree(devGoalY);
}
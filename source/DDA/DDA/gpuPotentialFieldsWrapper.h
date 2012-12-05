//----------------------------------------------------------------------------------------
/**
 * \file       gpuPotentialFieldsWrapper.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains functions which uses GPU
 *
 *  Methods to allocate, free memory on gpu. For running kernel.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _GPUPOTENTIALFIELDSWRAPPER_H_
#define _GPUPOTENTIALFIELDSWRAPPER_H_

void gpuAllocMemory(int _numberAgents, int _fieldWidth, int _tileWidth);
void gpuAllocObstacles(int _obstAreaLeft, int _obstAreaTop, int _cellWidth, int _cellHeight, int * _quadTree, Triangle *_triangle, int triangleSize, int * _triangleIDs, int triangleIDsSize);
void gpuCountPotentialFields(float ***potentialField, int threadBlockWidth, int threadBlockHeight, float * cpuFieldCenterX, float * cpuFieldCenterY, int * cpuGoalX, int * cpuGoalY);
void gpuFreeMemory();

#endif
#ifndef _GPUPOTENTIALFIELDSWRAPPER_H_
#define _GPUPOTENTIALFIELDSWRAPPER_H_

void gpuAllocMemory(int _numberAgents, int _fieldWidth, int _tileWidth);
void gpuAllocObstacles(int _obstAreaLeft, int _obstAreaTop, int _cellWidth, int _cellHeight, int * _quadTree, Triangle *_triangle, int triangleSize, int * _triangleIDs, int triangleIDsSize);
void gpuCountPotentialFields(float ***potentialField, float * cpuFieldCenterX, float * cpuFieldCenterY, int * cpuGoalX, int * cpuGoalY);
void gpuFreeMemory();

#endif
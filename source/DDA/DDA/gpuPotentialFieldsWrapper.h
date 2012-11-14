#ifndef _GPUPOTENTIALFIELDSWRAPPER_H_
#define _GPUPOTENTIALFIELDSWRAPPER_H_

void gpuAllocMemory(int _numberAgents, int _fieldWidth, int _tileWidth);
void gpuCountPotentialFields(float ***potentialField, float * cpuFieldCenterX, float * cpuFieldCenterY, int * cpuGoalX, int * cpuGoalY);
void gpuFreeMemory();

#endif
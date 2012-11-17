#ifndef _POTENTIALFIELDWORKER_H_
#define _POTENTIALFIELDWORKER_H_

#include <QThread>
#include "PotentialField.h"
#include "Obstacle.h"
#include "Triangle.h"

using namespace std;

class PotentialFieldWorker  : public QThread {
    Q_OBJECT

private:
	int agentSize;
	vector<Agent *> *agent;
	vector<Obstacle *> *obstacle;

	int obstAreaTop;
	int obstAreaBottom;
	int obstAreaRight;
	int obstAreaLeft;
	int cellWidth;
	int cellHeight;
	static const int AREA_CELL_WIDTH = 8;
	static const int AREA_CELL_HEIGHT = 8;
	Triangle *triangle;
	int * quadTree;			// in each cell is end index to triangleIds, for eaxample potential tile goes to quadTree[0], triangles in this tree are with indeces triangleIDs[0] to triangleIDs[quadTree[0] - 1]
	int * triangleIDs;		// indexes to triangle

	int * goalX;
	int * goalY;
	float * fieldCenterX;
	float * fieldCenterY;
	float ** tempField;
	float *** manyPotentialFields;

	bool requestForNewFields;
	bool newFieldsPrepared;
	bool shouldBeRunning;
	bool isFinished;
	int lastTimeElapsed;

	bool PointTriangleTest(Point2d & p, Triangle & t);
	void BuildSimpleQuadTree();

public:
	PotentialFieldWorker(vector<Agent *> *_agent, vector<Obstacle *> *_obstacle);
	~PotentialFieldWorker(void);

	void CountPotentialField(int agentID);
	float CountPotentialFieldTile(int agentID, int x, int y, int goalX, int goalY);
	float CountPotentialFieldTilePostProcess(int row, int col, float ** potentialField);
	
	
	void SendPotentionalFieldToAgent(int agentID, PotentialField * field);

	void Kill() { shouldBeRunning = false; }
	bool GetIsFinished() { return isFinished; }
	bool GetNewFieldsPrepared() { return newFieldsPrepared; }
	int GetLastTimeElapsedMS() { return lastTimeElapsed; }
	void RequestForNewFields() { newFieldsPrepared = false; requestForNewFields = true; }
    void run();
};

#endif
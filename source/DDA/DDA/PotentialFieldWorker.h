#ifndef _POTENTIALFIELDWORKER_H_
#define _POTENTIALFIELDWORKER_H_

#include <QThread>
#include "PotentialField.h"
#include "Obstacle.h"

using namespace std;

class PotentialFieldWorker  : public QThread {
    Q_OBJECT

private:
	int agentSize;
	vector<Agent *> *agent;
	vector<Obstacle *> *obstacle;

	int * goalX;
	int * goalY;
	float * fieldCenterX;
	float * fieldCenterY;
	float *** manyPotentialFields;

	bool requestForNewFields;
	bool newFieldsPrepared;
	bool shouldBeRunning;
	bool isFinished;
	int lastTimeElapsed;
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
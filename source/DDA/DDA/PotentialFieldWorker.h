//----------------------------------------------------------------------------------------
/**
 * \file       PotentialFieldWorker.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains PotentialFieldWorker class for thread
 *
 *  Thread which counts potential fields of agents in the never ending loop
 *
*/
//----------------------------------------------------------------------------------------

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
	int agentSize;				///< agent count
	vector<Agent *> *agent;		///< pointers to all agents
	vector<Obstacle *> *obstacle; ///< pointers to all obstacles

	int obstAreaTop;		///< top y coord of  bounding box of all triangles
	int obstAreaBottom;	    ///< bottom y coord of bounding box of all triangles
	int obstAreaRight;		///< right x coord of bounding box of all triangles
	int obstAreaLeft;		///< left x coord of bounding box of all triangles
	int cellWidth;			///< quadtree cell width in pixels
	int cellHeight;			///< quadtree cell height in pixels
	Triangle *triangle;		///< obstacles as triangles
	int * quadTree;			///< in each cell is end index to triangleIds, for eaxample potential tile goes to quadTree[0], triangles in this tree are with indeces triangleIDs[0] to triangleIDs[quadTree[0] - 1]
	int * triangleIDs;		///< indexes to triangle

	int * goalX;			///< array of agents goals x coords
	int * goalY;			///< array of agents goals y coords
	float * fieldCenterX;	///< array of x coordinates where agent was when computing field values started
	float * fieldCenterY;	///< array of y coordinates where agent was when computing field values started
	float ** tempField;		///< temp field used in more methods
	float *** manyPotentialFields;	///< array of all potential fields

	bool gpu;					///< true if it should be counted on gpu
	bool requestForNewFields;	///< true if worker should count potential fields again
	bool newFieldsPrepared;		///< new fields are prepered, someone should take them 
	bool shouldBeRunning;		///< when true, "infinite" loop ends
	bool isFinished;			///< true, if worker thread has ended
	int lastTimeElapsed;		///< time needed to count all potential fields for all agents

	bool PointTriangleTest(Point2d & p, Triangle & t);
	void BuildSimpleQuadTree();

public:
	PotentialFieldWorker(vector<Agent *> *_agent, vector<Obstacle *> *_obstacle, bool _gpu);
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
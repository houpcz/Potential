#include "PotentialFieldWorker.h"
#include "gpuPotentialFieldsWrapper.h"
#include <set>

#define _GPU_

PotentialFieldWorker::PotentialFieldWorker(vector<Agent *> *_agent, vector<Obstacle *> *_obstacle, bool _gpu)
{
	newFieldsPrepared = false;
	lastTimeElapsed = 0;
	gpu = _gpu;
	agent = _agent;
	obstacle = _obstacle;
	agentSize = agent->size();
}


PotentialFieldWorker::~PotentialFieldWorker(void)
{
	if(manyPotentialFields != NULL)
	{
		for(int loop2 = 0; loop2 < agentSize; loop2++)
		{
			for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
				delete [] manyPotentialFields[loop2][loop1];

			delete [] manyPotentialFields[loop2];
		}

		for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
			delete tempField[loop1];
		delete [] tempField;
			
		delete [] manyPotentialFields;
		delete [] fieldCenterX;
		delete [] fieldCenterY;
		delete [] goalX;
		delete [] goalY;
		delete [] triangle;
		delete [] quadTree;
		delete [] triangleIDs;
	}
	

	gpuFreeMemory();

	qDebug("Finished worker", shouldBeRunning);
}

void PotentialFieldWorker::BuildSimpleQuadTree()
{
	if(obstacle->size() == 0)
		return;

	triangle = new Triangle[obstacle->size()];

	set<int> helpQuadTree[AREA_CELL_HEIGHT][AREA_CELL_WIDTH];

	float x, y;

	obstAreaLeft = (int) (*obstacle)[0]->polygon().at(0).x();
	obstAreaRight = (int) (*obstacle)[0]->polygon().at(0).x() + 1;
	obstAreaTop = (int) (*obstacle)[0]->polygon().at(0).y();
	obstAreaBottom = (int) (*obstacle)[0]->polygon().at(0).y() + 1;
	for(int loop1 = 0; loop1 < obstacle->size(); loop1++)
	{
		for(int loop2 = 0; loop2 < 3; loop2++)
		{
			x = (*obstacle)[loop1]->polygon().at(loop2).x();
			y = (*obstacle)[loop1]->polygon().at(loop2).y();
			triangle[loop1].p[loop2].x = x;
			triangle[loop1].p[loop2].y = y;

			if(x < obstAreaLeft)
				obstAreaLeft = (int) x;
			if(x > obstAreaRight)
				obstAreaRight = (int) x + 1;
			if(y < obstAreaTop)
				obstAreaTop = (int) y;
			if(y > obstAreaBottom)
				obstAreaBottom = (int) y + 1;
		}
	}

	int areaWidth = obstAreaRight - obstAreaLeft;
	int areaHeight = obstAreaBottom - obstAreaTop;
	cellWidth = areaWidth / AREA_CELL_WIDTH + 1;
	cellHeight = areaHeight / AREA_CELL_HEIGHT + 1;

	for(int loop1 = 0; loop1 < obstacle->size(); loop1++)
	{
		for(int loop2 = 0; loop2 < 3; loop2++)
		{
			int areaX = (int) triangle[loop1].p[loop2].x - obstAreaLeft;
			int areaY = (int) triangle[loop1].p[loop2].y - obstAreaTop;
			int yid = areaY / cellHeight;
			int xid = areaX / cellWidth;
			helpQuadTree[yid][xid].insert(loop1);
		}
	}
	quadTree = new int[AREA_CELL_WIDTH * AREA_CELL_HEIGHT];

	int sumId = 0;
	for(int loop1 = 0; loop1 < AREA_CELL_HEIGHT; loop1++)
	{
		for(int loop2 = 0; loop2 < AREA_CELL_WIDTH; loop2++)
		{
			sumId += helpQuadTree[loop1][loop2].size();
			quadTree[loop2 + loop1 * AREA_CELL_WIDTH] = sumId;	
		}
	}
	triangleIDs = new int[sumId];
	int id = 0;
	for(int loop1 = 0; loop1 < AREA_CELL_HEIGHT; loop1++)
	{
		for(int loop2 = 0; loop2 < AREA_CELL_WIDTH; loop2++)
		{
			set<int>::iterator myIterator;
			for(myIterator = helpQuadTree[loop1][loop2].begin();
				myIterator != helpQuadTree[loop1][loop2].end();
				myIterator++)
			{
				triangleIDs[id++] = *myIterator;			
			}
		}
	}

	gpuAllocObstacles(obstAreaLeft, obstAreaTop, cellWidth, cellHeight, quadTree, triangle, obstacle->size(), triangleIDs, id);
}

bool PotentialFieldWorker::PointTriangleTest(Point2d & p, Triangle & t)
{
	float ab = (t.p[0].x - p.x) * (t.p[1].y - p.y) - (t.p[1].x - p.x) * (t.p[0].y - p.y);
	float bc = (t.p[1].x - p.x) * (t.p[2].y - p.y) - (t.p[2].x - p.x) * (t.p[1].y - p.y);
	float ca = (t.p[2].x - p.x) * (t.p[0].y - p.y) - (t.p[0].x - p.x) * (t.p[2].y - p.y);
	
	return (ab <= 0.0f && bc <= 0.0f && ca <= 0.0f) || (ab >= 0.0f && bc >= 0.0f && ca >= 0.0f);
}

void PotentialFieldWorker::run()
{
	manyPotentialFields = new float**[agentSize];
	fieldCenterX = new float[agentSize];
	fieldCenterY = new float[agentSize];
	goalX = new int[agentSize];
	goalY = new int[agentSize];

	BuildSimpleQuadTree();

	tempField = new float*[PotentialField::FIELD_WIDTH];
	for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
		tempField[loop1] = new float[PotentialField::FIELD_WIDTH];

	for(int loop2 = 0; loop2 < agentSize; loop2++)
	{
		manyPotentialFields[loop2] = new float*[PotentialField::FIELD_WIDTH];
		for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
			manyPotentialFields[loop2][loop1] = new float[PotentialField::FIELD_WIDTH];
	}

	newFieldsPrepared = false;
	isFinished = false;
	shouldBeRunning = true;
	requestForNewFields = false;

	gpuAllocMemory(agentSize, PotentialField::FIELD_WIDTH, PotentialField::TILE_WIDTH);

	lastTimeElapsed = 0;

	QTime time;
	time.start();
	while(shouldBeRunning)
	{
		time.restart();
		for(int loop1 = 0; loop1 < agentSize; loop1++)
		{
			Point2D fieldCenter = (*agent)[loop1]->FieldCenter();
			fieldCenterX[loop1] = fieldCenter.X();
			fieldCenterY[loop1] = fieldCenter.Y();
			goalX[loop1] = (*agent)[loop1]->GoalX();
			goalY[loop1] = (*agent)[loop1]->GoalY();
		}

		
		if(gpu)
			gpuCountPotentialFields(manyPotentialFields, fieldCenterX, fieldCenterY, goalX, goalY);
		else {
			for(int loop1 = 0; loop1 < agentSize; loop1++)
			{
				if(!shouldBeRunning)
					break;
				CountPotentialField(loop1);
			}
		}


		qDebug("Elapsed time during counting %d p. fields = %dms", agentSize, time.elapsed());
		lastTimeElapsed = time.elapsed();

		newFieldsPrepared = true;
		while(!requestForNewFields && shouldBeRunning)
			this->msleep(1);
		requestForNewFields = false;
		newFieldsPrepared = false;
	}
	
	isFinished = true;
}

void PotentialFieldWorker::SendPotentionalFieldToAgent(int agentID, PotentialField * field)
{
	field->SetPotentialField(manyPotentialFields[agentID], fieldCenterX[agentID], fieldCenterY[agentID]);
}
	 
void PotentialFieldWorker::CountPotentialField(int agentID)
{
	int x, y;
	int gX = goalX[agentID];
	int gY = goalY[agentID];

	for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < PotentialField::FIELD_WIDTH; loop2++)
		{
			x = (int) fieldCenterX[agentID] + (loop2 - (PotentialField::FIELD_WIDTH / 2)) * PotentialField::TILE_WIDTH;
			y = (int) fieldCenterY[agentID] + (loop1 - (PotentialField::FIELD_WIDTH / 2)) * PotentialField::TILE_WIDTH;
			tempField[loop1][loop2] = CountPotentialFieldTile(agentID, x, y, gX, gY);
		}
	}
	if(!shouldBeRunning)
		return;

	
	for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < PotentialField::FIELD_WIDTH; loop2++)
		{
			manyPotentialFields[agentID][loop1][loop2] = tempField[loop1][loop2] + CountPotentialFieldTilePostProcess(loop1, loop2, tempField);
		}
	}
}

float PotentialFieldWorker::CountPotentialFieldTilePostProcess(int row, int col, float ** potentialField)
{
	int n = 0;
	if(row > 0)
	{
		if(potentialField[row - 1][col] > PotentialField::OBSTACLE_IN_FIELD)
			++n;
	}
	if(col > 0)
	{
		if(potentialField[row][col - 1] > PotentialField::OBSTACLE_IN_FIELD)
			++n;
	}
	if(row < PotentialField::FIELD_WIDTH - 1)
	{
		if(potentialField[row + 1][col] > PotentialField::OBSTACLE_IN_FIELD)
			++n;
	}
	if(col < PotentialField::FIELD_WIDTH - 1)
	{
		if(potentialField[row][col + 1] > PotentialField::OBSTACLE_IN_FIELD)
			++n;
	}

	return (n > 0) ? PotentialField::OBSTACLE_IN_FIELD / 100: 0.0f;
}

float PotentialFieldWorker::CountPotentialFieldTile(int agentID, int x, int y, int goalX, int goalY)
{
	float dx = x - goalX;
	float dy = y - goalY;
	float initValue = sqrt(dx * dx + dy * dy);

	bool inTriangle = false;

	int areaX = x - obstAreaLeft;
	int areaY = y - obstAreaTop;

	if(areaX >= 0 && areaY >= 0 && 
	   areaX < cellWidth * AREA_CELL_WIDTH &&
	   areaY < cellHeight * AREA_CELL_HEIGHT)
	{
		int yid = areaY / cellHeight;
		int xid = areaX / cellWidth;
		int id = xid + yid * AREA_CELL_WIDTH;
		int startX;
		if(id == 0)
			startX = 0;
		else
			startX = quadTree[id - 1];
		int endX = quadTree[id];
		Point2d p;
		p.x = x;
		p.y = y;
		for(int loop1 = startX; loop1 < endX; loop1++)
		{
			if(PointTriangleTest(p, triangle[triangleIDs[loop1]]))
			{
				inTriangle = true;
				break;
			}
		}
	}

	float obst = inTriangle ? PotentialField::OBSTACLE_IN_FIELD : 0;
	
	return initValue + obst;
}
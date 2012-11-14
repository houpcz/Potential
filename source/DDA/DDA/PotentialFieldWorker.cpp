#include "PotentialFieldWorker.h"
#include "gpuPotentialFieldsWrapper.h"

PotentialFieldWorker::PotentialFieldWorker(vector<Agent *> *_agent, vector<Obstacle *> *_obstacle)
{
	lastTimeElapsed = 0;
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
			
		delete [] manyPotentialFields;
	}
	delete [] fieldCenterX;
	delete [] fieldCenterY;
	delete [] goalX;
	delete [] goalY;

	gpuFreeMemory();

	qDebug("Finished worker", shouldBeRunning);
}

void PotentialFieldWorker::run()
{
	manyPotentialFields = new float**[agentSize];
	fieldCenterX = new float[agentSize];
	fieldCenterY = new float[agentSize];
	goalX = new int[agentSize];
	goalY = new int[agentSize];
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

		/*
		for(int loop1 = 0; loop1 < agentSize; loop1++)
		{
			if(!shouldBeRunning)
				break;
			CountPotentialField(loop1);
		}*/

		gpuCountPotentialFields(manyPotentialFields, fieldCenterX, fieldCenterY, goalX, goalY);

		qDebug("Elapsed time during counting %d p. fields = %dms", agentSize, time.elapsed());
		lastTimeElapsed = time.elapsed();

		newFieldsPrepared = true;
		while(!requestForNewFields && shouldBeRunning)
			this->msleep(10);
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
			manyPotentialFields[agentID][loop1][loop2] = CountPotentialFieldTile(agentID, x, y, gX, gY);
		}
	}
	if(!shouldBeRunning)
		return;

	for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < PotentialField::FIELD_WIDTH; loop2++)
		{
			manyPotentialFields[agentID][loop1][loop2] += CountPotentialFieldTilePostProcess(loop1, loop2, manyPotentialFields[agentID]);
		}
	}
}

float PotentialFieldWorker::CountPotentialFieldTilePostProcess(int row, int col, float ** potentialField)
{
	int n = 0;
	if(row > 0)
	{
		if(potentialField[row - 1][col] > PotentialField::OBSTACLE)
			++n;
	}
	if(col > 0)
	{
		if(potentialField[row][col - 1] > PotentialField::OBSTACLE)
			++n;
	}
	if(row < PotentialField::FIELD_WIDTH - 1)
	{
		if(potentialField[row + 1][col] > PotentialField::OBSTACLE)
			++n;
	}
	if(col < PotentialField::FIELD_WIDTH - 1)
	{
		if(potentialField[row][col + 1] > PotentialField::OBSTACLE)
			++n;
	}

	return (n > 0) ? PotentialField::OBSTACLE / 100: 0.0f;
}

float PotentialFieldWorker::CountPotentialFieldTile(int agentID, int x, int y, int goalX, int goalY)
{
	float dx = x - goalX;
	float dy = y - goalY;
	float initValue = sqrt(dx * dx + dy * dy);

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

	/*
	bool inAgent = false;
	for(int loop1 = 0; loop1 < agent->size(); loop1++)
	{
		if(loop1 == agentID)
			continue;

		if((*agent)[loop1]->RealPos().Distance(Point2D(x, y)) < 5)
		{
			inAgent = true;
			break;
		}
	}
	float ag = inAgent ? PotentialField::OBSTACLE / 10 : 0;
	*/
	return initValue + obst;
}
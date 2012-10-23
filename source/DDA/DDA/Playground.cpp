#include <QHBoxLayout>
#include <QColor>
#include <math.h>
#include "Playground.h"

Playground::Playground(QWidget *parent)
     : QWidget(parent)
{
	numberAgents = 20;
	numberObstacles = 100;
	SetEnvironment();
	QGraphicsView * view = new QGraphicsView(&scene);
	QHBoxLayout * layout = new QHBoxLayout();
	layout->addWidget(view);
	this->setLayout(layout);

	timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Tick()));
    timer->start(0);

	time.start();

	potentialField = new float*[PotentialField::FIELD_WIDTH];
	for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
		potentialField[loop1] = new float[PotentialField::FIELD_WIDTH];

	// each agent needs its field 
	/*
	for(int loop1 = 0; loop1 < agent.size(); loop1++)
	{
		CountPotentialField(loop1);
	}*/

	lastAgentFieldID = 0;
}

Playground::~Playground(void)
{
	delete timer;

	for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
		delete [] potentialField[loop1];
	delete [] potentialField;

	ClearEnvironment();
}

void Playground::Tick()
{
	int tickMillis = time.elapsed();
	time.restart();

	for(int loop1 = 0; loop1 < agent.size(); loop1++)
	{
		if(loop1 == lastAgentFieldID)
			CountPotentialField(loop1);

		agent[loop1]->Tick(tickMillis);
	}
	++lastAgentFieldID;
	lastAgentFieldID = lastAgentFieldID % agent.size();

}

void Playground::ClearEnvironment()
{
	for(int loop1 = 0; loop1 < agent.size(); loop1++)
	{
		scene.removeItem(agent[loop1]);
		delete agent[loop1];
		scene.removeItem(field[loop1]);
		delete field[loop1];
		scene.removeItem(goalPoint[loop1]);
		delete goalPoint[loop1];
	}
	goalPoint.clear();
	agent.clear();
	field.clear();

	for(int loop1 = 0; loop1 < obstacle.size(); loop1++)
	{
		scene.removeItem(obstacle[loop1]);
		delete obstacle[loop1];
	}
	obstacle.clear();
}

void Playground::CountPotentialField(int agentID)
{
	fieldCenterX = agent[agentID]->rect().x() + agent[agentID]->pos().x() + agent[agentID]->rect().width()/2;
	fieldCenterY = agent[agentID]->rect().y() + agent[agentID]->pos().y() + agent[agentID]->rect().height()/2;
	//fieldCenterX = fieldCenterX - fmod(fieldCenterX, PotentialField::TILE_WIDTH);
	//fieldCenterY = fieldCenterY - fmod(fieldCenterY, PotentialField::TILE_WIDTH);

	int x, y;
	int goalX = agent[agentID]->GoalX();
	int goalY = agent[agentID]->GoalY();
	for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < PotentialField::FIELD_WIDTH; loop2++)
		{
			x = (int) fieldCenterX + (loop2 - (PotentialField::FIELD_WIDTH / 2)) * PotentialField::TILE_WIDTH;
			y = (int) fieldCenterY + (loop1 - (PotentialField::FIELD_WIDTH / 2)) * PotentialField::TILE_WIDTH;
			potentialField[loop1][loop2] = CountPotentialFieldTile(agentID, x, y, goalX, goalY);
		}
	}
	for(int loop1 = 0; loop1 < PotentialField::FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < PotentialField::FIELD_WIDTH; loop2++)
		{
			potentialField[loop1][loop2] += CountPotentialFieldTilePostProcess(loop1, loop2);
		}
	}
	field[agentID]->SetPotentialField(potentialField, fieldCenterX, fieldCenterY);
}

float Playground::CountPotentialFieldTilePostProcess(int row, int col)
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

float Playground::CountPotentialFieldTile(int agentID, int x, int y, int goalX, int goalY)
{
	float dx = x - goalX;
	float dy = y - goalY;
	float initValue = sqrt(dx * dx + dy * dy);

	bool inTriangle = false;
	for(int loop1 = 0; loop1 < obstacle.size(); loop1++)
	{
		if(obstacle[loop1]->polygon().containsPoint(QPointF(x, y), Qt::FillRule::WindingFill))
		{
			inTriangle = true;
			break;
		}
	}
	float obst = inTriangle ? PotentialField::OBSTACLE : 0;

	bool inAgent = false;
	for(int loop1 = 0; loop1 < agent.size(); loop1++)
	{
		if(loop1 == agentID)
			continue;

		if(agent[loop1]->RealPos().Distance(Point2D(x, y)) < 5)
		{
			inAgent = true;
			break;
		}
	}
	float ag = inAgent ? PotentialField::OBSTACLE / 10 : 0;

	return initValue + obst + ag;
}

void Playground::SetEnvironment()
{
	ClearEnvironment();

	srand(numberAgents * numberObstacles);
	//srand(121);
	Obstacle * tempObstacle;
	QPolygonF polygon;
	int centerX, centerY;
	qreal koef = 0.5;
	for(int loop1 = 0; loop1 < numberObstacles; loop1++)
	{
		tempObstacle = new Obstacle();
		polygon.clear();
		centerX = rand() % 10 * 100 - 25 + rand() % 50;
		centerY = rand() % 10 * 100 - 25 + rand() % 50;
		
		int size = 30;
		switch(rand() % 3)
		{
			case 0:
				polygon.push_back(QPointF(koef * (centerX - size), koef * (centerY - size))); 
				polygon.push_back(QPointF(koef * (centerX + size), koef * (centerY - size))); 
				polygon.push_back(QPointF(koef * (centerX), koef * (centerY + size))); 
				break;
			case 1:
				polygon.push_back(QPointF(koef * (centerX - size), koef * (centerY - size))); 
				polygon.push_back(QPointF(koef * (centerX + size), koef * (centerY))); 
				polygon.push_back(QPointF(koef * (centerX - size), koef * (centerY + size))); 
				break;
			case 2:
				polygon.push_back(QPointF(koef * (centerX - size), koef * (centerY - size))); 
				polygon.push_back(QPointF(koef * (centerX + size), koef * (centerY - size))); 
				polygon.push_back(QPointF(koef * (centerX - size), koef * (centerY + size))); 
				break;
		}
		tempObstacle->setPolygon(polygon);
		obstacle.push_back(tempObstacle);
		scene.addItem(tempObstacle);
	}

	Agent * tempAgent;
	PotentialField * tempField;
	GoalPoint * tempGoal;

	int agentX, agentY;
	int agentWidth = 8;
	for(int loop1 = 0; loop1 < numberAgents; loop1++)
	{
		bool cond = false;
		do {
			//agentX = koef * (rand() % 80 * 12 + rand() % 7);
			//agentY = koef * (rand() % 80 * 12 + rand() % 7);
			agentX = koef * (rand() % 10 * 100 + 40);
			agentY = koef * (rand() % 10 * 100 - 40);

			cond = scene.itemAt(agentX, agentY) != NULL;/* ||
				        scene.itemAt(agentX + agentWidth, agentY) != NULL ||
						scene.itemAt(agentX, agentY + agentWidth) != NULL ||
						scene.itemAt(agentX - agentWidth, agentY) != NULL ||
						scene.itemAt(agentX, agentY - agentWidth) != NULL;*/
			cond = false;
		} while(cond);

		tempAgent = new Agent(agentX, agentY, agentWidth, agentWidth, 0);
		tempAgent->SetGoal(500 - agentX, 500 - agentY);
		tempGoal = new GoalPoint(500 - agentX, 500 - agentY, 10, 10, 0);
		tempField = new PotentialField(tempAgent, agentX, agentY, 0);

		agent.push_back(tempAgent);
		scene.addItem(tempAgent);
		field.push_back(tempField);
		scene.addItem(tempField);
		goalPoint.push_back(tempGoal);
		scene.addItem(tempGoal);
	}

}
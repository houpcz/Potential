#include <QHBoxLayout>
#include <QColor>
#include <math.h>
#include "Playground.h"

Playground::Playground(QWidget *parent)
     : QWidget(parent)
{
	numberAgents = 10;
	numberObstacles = 50;
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
		CountPotentialField(loop1);
		agent[loop1]->Tick(tickMillis);
	}
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
	field[agentID]->SetPotentialField(potentialField, fieldCenterX, fieldCenterY);
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
	float obst = inTriangle ? 100 : 0;
	return initValue + obst;
}

void Playground::SetEnvironment()
{
	ClearEnvironment();

	srand(120);

	Obstacle * tempObstacle;
	QPolygonF polygon;
	int centerX, centerY;
	qreal koef = 0.5;
	for(int loop1 = 0; loop1 < numberObstacles; loop1++)
	{
		tempObstacle = new Obstacle();
		polygon.clear();
		centerX = rand() % 20 * 50;
		centerY = rand() % 20 * 50;
		
		switch(rand() % 3)
		{
			case 0:
				polygon.push_back(QPointF(koef * (centerX - 50 - rand() % 40), koef * (centerY - 50 - rand() % 30))); 
				polygon.push_back(QPointF(koef * (centerX + 40 - rand() % 30), koef * (centerY - 50 + rand() % 20))); 
				polygon.push_back(QPointF(koef * (centerX - 50 - rand() % 30), koef * (centerY + 60 + rand() % 30))); 
				break;
			case 1:
				polygon.push_back(QPointF(koef * (centerX - 50 + rand() % 40), koef * (centerY + 50 - rand() % 30))); 
				polygon.push_back(QPointF(koef * (centerX + 40 - rand() % 30), koef * (centerY + 50 + rand() % 20))); 
				polygon.push_back(QPointF(koef * (centerX - 50 - rand() % 30), koef * (centerY - 60 + rand() % 30))); 
				break;
			case 2:
				polygon.push_back(QPointF(koef * (centerX - 50 + rand() % 40), koef * (centerY + 50 - rand() % 30))); 
				polygon.push_back(QPointF(koef * (centerX + 40 + rand() % 30), koef * (centerY - 50 + rand() % 20))); 
				polygon.push_back(QPointF(koef * (centerX + 50 - rand() % 30), koef * (centerY + 60 - rand() % 30))); 
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
			agentX = koef * (rand() % 19 * 50 + rand() % 50);
			agentY = koef * (rand() % 19 * 50 + rand() % 50);

			cond = scene.itemAt(agentX, agentY) != NULL ||
				        scene.itemAt(agentX + agentWidth, agentY) != NULL ||
						scene.itemAt(agentX, agentY + agentWidth) != NULL ||
						scene.itemAt(agentX - agentWidth, agentY) != NULL ||
						scene.itemAt(agentX, agentY - agentWidth) != NULL;
		} while(cond);

		tempAgent = new Agent(agentX, agentY, agentWidth, agentWidth, 0);
		tempAgent->SetGoal(500 - agentX, 500 - agentY);
		tempGoal = new GoalPoint(500 - agentX, 500 - agentY, 10, 10, 0);
		tempField = new PotentialField(agentX, agentY, 0);

		agent.push_back(tempAgent);
		scene.addItem(tempAgent);
		field.push_back(tempField);
		scene.addItem(tempField);
		goalPoint.push_back(tempGoal);
		scene.addItem(tempGoal);
	}

}
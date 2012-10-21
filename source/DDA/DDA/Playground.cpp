#include <QHBoxLayout>
#include <QColor>
#include "Playground.h"

Playground::Playground(QWidget *parent)
     : QWidget(parent)
{
	numberAgents = 1;
	numberObstacles = 50;
	SetEnvironment();
	QGraphicsView * view = new QGraphicsView(&scene);
	QHBoxLayout * layout = new QHBoxLayout();
	layout->addWidget(view);
	this->setLayout(layout);

	timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Tick()));
    timer->start(10);

	time.start();

	potentialField = new float*[Agent::FIELD_WIDTH];
	for(int loop1 = 0; loop1 < Agent::FIELD_WIDTH; loop1++)
		potentialField[loop1] = new float[Agent::FIELD_WIDTH];
}

Playground::~Playground(void)
{
	delete timer;

	for(int loop1 = 0; loop1 < Agent::FIELD_WIDTH; loop1++)
		delete [] potentialField[loop1];
	delete [] potentialField;
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
	}
	agent.clear();

	for(int loop1 = 0; loop1 < obstacle.size(); loop1++)
	{
		scene.removeItem(obstacle[loop1]);
		delete obstacle[loop1];
	}
	obstacle.clear();
}

void Playground::CountPotentialField(int agentID)
{
	fieldCenterX = agent[agentID]->rect().x();
	fieldCenterY = agent[agentID]->rect().y();

	int x, y;
	for(int loop1 = 0; loop1 < Agent::FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < Agent::FIELD_WIDTH; loop2++)
		{
			x = (int) fieldCenterX + (loop2 - (Agent::FIELD_WIDTH / 2)) * Agent::TILE_WIDTH;
			y = (int) fieldCenterY + (loop1 - (Agent::FIELD_WIDTH / 2)) * Agent::TILE_WIDTH;
			potentialField[loop1][loop2] = CountPotentialFieldTile(agentID, x, y);
		}
	}
	agent[agentID]->SetPotentialField(potentialField, fieldCenterX, fieldCenterY);
}

float Playground::CountPotentialFieldTile(int agentID, int x, int y)
{
	return 1;
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
		agent.push_back(tempAgent);
		scene.addItem(tempAgent);
	}

}
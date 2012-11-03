#include <QHBoxLayout>
#include <QColor>
#include <math.h>
#include "Playground.h"

Playground::Playground(QWidget *parent)
     : QWidget(parent)
{
	numberAgents = 50;
	numberObstacles = 99;
	SetEnvironment();
	QGraphicsView * view = new QGraphicsView(&scene);
	QHBoxLayout * layout = new QHBoxLayout();
	layout->addWidget(view);
	this->setLayout(layout);

	timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Tick()));
    timer->start(0);

	worker = new PotentialFieldWorker(&agent, &obstacle);
	worker->start();

	time.start();
}

Playground::~Playground(void)
{
	delete timer;
	worker->Kill();
	while(!worker->GetIsFinished())
		;
	ClearEnvironment();
}

void Playground::Tick()
{
	int tickMillis = time.elapsed();
	time.restart();
	if(tickMillis > 100)
		tickMillis = 0;

	for(int loop1 = 0; loop1 < agent.size(); loop1++)
	{
		agent[loop1]->Tick(tickMillis);
	}

	if(worker->GetNewFieldsPrepared())
	{
		for(int loop1 = 0; loop1 < agent.size(); loop1++)
		{
			worker->SendPotentionalFieldToAgent(loop1, field[loop1]);
		}
		worker->RequestForNewFields();
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
		switch(loop1 % 4)
		{
			case 0 :
				agentX = -40;
				agentY = rand() % 100 * 5;
				break;
			case 1 :
				agentX = 510;
				agentY = rand() % 100 * 5;
				break;

			case 2 :
				agentY = -40;
				agentX = rand() % 100 * 5;
				break;
			case 3 :
				agentY = 510;
				agentX = rand() % 100 * 5;
				break;
		}

		tempAgent = new Agent(agentX, agentY, agentWidth, agentWidth, 0);
		tempAgent->SetGoal(470 - agentX, 470 - agentY);
		tempGoal = new GoalPoint(470 - agentX, 470 - agentY, 10, 10, 0);
		tempField = new PotentialField(tempAgent, agentX, agentY, 0);

		agent.push_back(tempAgent);
		scene.addItem(tempAgent);
		field.push_back(tempField);
		scene.addItem(tempField);
		goalPoint.push_back(tempGoal);
		scene.addItem(tempGoal);
	}

}
#pragma once
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QGraphicsScene.h>
#include <QGraphicsView.h>
#include <QGraphicsItem>
#include <vector>
#include <QThread>

#include "PotentialFieldWorker.h"
#include "Agent.h"
#include "Obstacle.h"
#include "PotentialField.h"
#include "GoalPoint.h"

using namespace std;

class Playground :
	public QWidget
{
Q_OBJECT

private :
	QTimer * timer;
	QGraphicsScene scene;
	QGraphicsItem * item;

	int numberAgents;
	int numberObstacles;
	vector<Agent *> agent;
	vector<Obstacle *> obstacle;
	vector<PotentialField *> field;
	vector<GoalPoint *> goalPoint;
	QTime time;

	QThread* thread;
	PotentialFieldWorker* worker;
public:
	Playground(QWidget *parent = 0);
	~Playground(void);
	void SetEnvironment();
	void ClearEnvironment();
public slots:
	void Tick();
};


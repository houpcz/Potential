//----------------------------------------------------------------------------------------
/**
 * \file       Playground.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains Playground class
 *
 *  Playground is main class which works with agents, obstacle, potential fields.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _PLAYGROUND_H_
#define _PLAYGROUND_H_

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
	QTimer * timer;				///< timer for Tick method
	QGraphicsScene scene;		///< scene containg all graphics items
	QGraphicsItem * item;		///< temp graphics item
	QGraphicsSimpleTextItem * countingTimeText;	///< counting all fields time in scene

	int numberAgents;					///< count of agents
	int numberObstacles;				///< count of obstacles
	vector<Agent *> agent;				///< array of all agents
	vector<Obstacle *> obstacle;		///< array of all obstacles
	vector<PotentialField *> field;		///< array of all fields
	vector<GoalPoint *> goalPoint;		///< array of all goal points
	QTime time;							///< time between ticks
		
	PotentialFieldWorker* worker;		///< worker thread
public:
	Playground(QWidget *parent = 0, int _numberAgents = 10, int _numberObstacles = 50, bool gpu = true);
	~Playground(void);
	void SetEnvironment();
	void ClearEnvironment();
public slots:
	void Tick();
};

#endif


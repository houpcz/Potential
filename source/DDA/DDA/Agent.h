#pragma once

#include <QGraphicsItem>
#include <queue>
#include "Point2D.h"

using namespace std;

class Agent : public QGraphicsEllipseItem
{
private:
	int goalX, goalY;
	Point2D velocity;
	Point2D realPos;
	queue<Point2D> path;
public:
	Agent(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 );
	~Agent(void);
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	void Tick(int millis);
	void SetGoal(int _goalX, int _goalY);
	int GoalX() { return goalX; };
	int GoalY() { return goalY; };
	Point2D RealPos() { return realPos; };
};


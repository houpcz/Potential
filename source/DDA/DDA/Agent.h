#ifndef _AGENT_H_
#define _AGENT_H_

#include <QGraphicsItem>
#include <queue>
#include <stack>
#include "Point2D.h"

using namespace std;

class Agent : public QGraphicsEllipseItem
{
private:
	static const int SPEED_PX_PER_S = 20;
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
	Point2D FieldCenter();
	void SetPath(stack<Point2D> * _path);
};

#endif
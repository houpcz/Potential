#pragma once

#include <QGraphicsItem>


class Agent : public QGraphicsEllipseItem
{
private:
	int goalX, goalY;
	float posX, posY;
	float vX, vY;
public:
	Agent(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 );
	~Agent(void);
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	void Tick(int millis);
	void SetGoal(int _goalX, int _goalY) { goalX = _goalX; goalY = _goalY; };
	int GoalX() { return goalX; };
	int GoalY() { return goalY; };
};


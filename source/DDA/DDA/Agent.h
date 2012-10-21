#pragma once

#include <QGraphicsItem>


class Agent : public QGraphicsEllipseItem
{
public :
	static const int FIELD_WIDTH = 20;
	static const int TILE_WIDTH = 5;
private:
	int goalX, goalY;
	float posX, posY;
	float vX, vY;

	qreal fieldCenterX;
	qreal fieldCenterY;
	float potentialField[FIELD_WIDTH][FIELD_WIDTH];
public:
	Agent(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 );
	~Agent(void);
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	void Tick(int millis);
	void SetGoal(int _goalX, int _goalY) { goalX = _goalX; goalY = _goalY; };
	void SetPotentialField(float ** _potentialField, qreal _fieldCenterX, qreal _fieldCenterY);
};


#pragma once

#include <QGraphicsRectItem>
#include "Agent.h"

class PotentialField : public QGraphicsRectItem
{
public :
	static const int FIELD_WIDTH = 30;
	static const int TILE_WIDTH = 5;
	static const int OBSTACLE = 100000;
private:
	Agent * agent;
	qreal fieldCenterX;
	qreal fieldCenterY;
	float minValue;
	float maxValue;
	float potentialField[FIELD_WIDTH][FIELD_WIDTH];

	queue<Point2D> path;

	void PushToPath(int fieldIdX, int fieldIdY);
	void FindPath();
public:
	PotentialField(Agent * _agent, qreal x, qreal y, QGraphicsItem * parent = 0 );
	~PotentialField(void);
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	void SetPotentialField(float ** _potentialField, qreal _fieldCenterX, qreal _fieldCenterY);
};


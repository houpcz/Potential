#pragma once

#include <QGraphicsRectItem>
#include <stack>
#include <QTime>
#include "Agent.h"

class PotentialField : public QGraphicsRectItem
{
public :
	static const int FIELD_WIDTH = 32;
	static const int TILE_WIDTH = 5;
	static const int OBSTACLE = 100000;
private:
	static const char ON_ROAD = 'R';
	static const char START = - 'S';
	static const char NO_DIR = '?';
	static const char UP_LEFT = '7';
	static const char UP_CENTER = '8';
	static const char UP_RIGHT = '9';
	static const char LEFT = '4';
	static const char RIGHT = '6';
	static const char DOWN_LEFT = '1';
	static const char DOWN_CENTER = '2';
	static const char DOWN_RIGHT = '3';

	Agent * agent;
	int agentCenterX;
	int agentCenterY;
	qreal fieldCenterX;
	qreal fieldCenterY;
	float minValue;
	float maxValue;
	float potentialField[FIELD_WIDTH][FIELD_WIDTH];
	char road[FIELD_WIDTH][FIELD_WIDTH];

	stack<Point2D> path;

	void PushToPath(int fieldIdX, int fieldIdY);
	void FindPath();

	QTime * time;
public:
	PotentialField(Agent * _agent, qreal x, qreal y, QGraphicsItem * parent = 0 );
	~PotentialField(void);
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	void SetPotentialField(float ** _potentialField, qreal _fieldCenterX, qreal _fieldCenterY);
};


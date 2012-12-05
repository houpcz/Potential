//----------------------------------------------------------------------------------------
/**
 * \file       PotentialField.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains PotentialField class
 *
 *  Potential field visualisation and method for finding path between agents and the global minimum in the field.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _POTENTIALFIELD_H_
#define _POTENTIALFIELD_H_

#include <QGraphicsRectItem>
#include <stack>
#include <QTime>
#include "Agent.h"

class PotentialField : public QGraphicsRectItem
{
public :
	static const int OBSTACLE_IN_FIELD = 100000;
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
	static int fieldWidth;
	static int tileWidth;
	static bool show1Field;					///< true if field number 1 should be visible
	static int potentialFieldCount;			///< number of potential fields
	int fieldID;							///< this field ID

	Agent * agent;							///< pointer to agent whose the field this is
	int agentCenterX;						///< agent pos x coord
	int agentCenterY;						///< agent pos y coord
	qreal fieldCenterX;						///< agent pos x coord when was the field counted
	qreal fieldCenterY;						///< agent pos y coord when was the field counted
	float minValue;							///< min value in field
	float maxValue;							///< max value in the field
	float potentialField[FIELD_WIDTH][FIELD_WIDTH];	///< the potential field itself
	char road[FIELD_WIDTH][FIELD_WIDTH];	///< "pointers" to ancestors of all field tiles when finding path
	bool fieldPrepared;
	stack<Point2D> path;					///< path in road/potentialField

	void PushToPath(int fieldIdX, int fieldIdY);

	QTime * time;
public:
	PotentialField(Agent * _agent, qreal x, qreal y, QGraphicsItem * parent = 0 );
	~PotentialField(void);
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	void SetPotentialField(float ** _potentialField, qreal _fieldCenterX, qreal _fieldCenterY);
	static void SetShow1Field(bool show) { show1Field = show; };
	void FindPath();
};

#endif
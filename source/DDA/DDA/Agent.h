//----------------------------------------------------------------------------------------
/**
 * \file       Agent.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains Agent class
 *
 *  Agent class containing necessery methods for moving, painting agent.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _AGENT_H_
#define _AGENT_H_

#include <QGraphicsItem>
#include <queue>
#include <stack>
#include "Point2D.h"

class PotentialField;
using namespace std;

/// Class that holds info about an agent.
/**
  This class contains information about the agen, i.e. its position,
  velocity, goal, current path.
*/

class Agent : public QGraphicsEllipseItem
{
private:
	static const int SPEED_PX_PER_S = 15;	///< Lenght of velocity vector
	int goalX;						///< Agent goal x position
	int goalY;						///< Agent goal y position
	Point2D velocity;				///< velocity vector
	Point2D realPos;				///< position
	PotentialField * field;			///< agents potential field
	queue<Point2D> path;			///< waypoints of current path in queue
public:
	/// Constructor.
	/**
	Creates a new agent with given position, width and height of ellipse.

	\param[in] x Origin x position
	\param[in] y Origin y position
	\param[in] width Agent width
	\param[in] height Agent height
	\param[in] parent possible QGraphicsItem parent
	*/
	Agent(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 );
	~Agent(void);

	/// Paints agent as circle
	/**
	\param[in] painter Painter which paints agent.
	\param[in] option QStyleOption.
	\param[in] widget Parent widget.
	*/
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

	/// Updates agent position towards goal.
	/**
	\param[in] millis How many milliseconds lasted from last tick.
	*/
	void Tick(int millis);
	/// Set new goal position
	/**
	\param[in] _goalX Goal x position
	\param[in] _goalY Goal y position
	*/
	void SetGoal(int _goalX, int _goalY);
	int GoalX() { return goalX; };
	int GoalY() { return goalY; };
	Point2D RealPos() { return realPos; };
	Point2D FieldCenter();
	void SetPath(stack<Point2D> * _path);
	void SetField(PotentialField * _field) { field = _field; };
};

#endif
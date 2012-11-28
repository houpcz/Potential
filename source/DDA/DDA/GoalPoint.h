//----------------------------------------------------------------------------------------
/**
 * \file       GoalPoint.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains GoalPoint class declaration
 *
 *  Graphics class to show path ends (goal) of each agent.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _GOALPOINT_H_
#define _GOALPOINT_H_

#include <QGraphicsEllipseItem>

class GoalPoint : public QGraphicsEllipseItem
{
public:
	GoalPoint(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 );
	~GoalPoint(void);
};

#endif


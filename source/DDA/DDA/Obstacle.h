//----------------------------------------------------------------------------------------
/**
 * \file       Obstacle.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains Obstacle class
 *
 *  Obstacle graphics item.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

#include <QGraphicsItem>
class Obstacle : public QGraphicsPolygonItem
{
public:
	Obstacle(void);
	~Obstacle(void);
};

#endif


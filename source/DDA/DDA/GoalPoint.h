#pragma once

#include <QGraphicsEllipseItem>

class GoalPoint : public QGraphicsEllipseItem
{
public:
	GoalPoint(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 );
	~GoalPoint(void);
};


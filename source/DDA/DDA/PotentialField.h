#pragma once

#include <QGraphicsRectItem>

class PotentialField : public QGraphicsRectItem
{
public:
	PotentialField( qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 );
	~PotentialField(void);
};


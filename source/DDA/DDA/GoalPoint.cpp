#include "GoalPoint.h"
#include <QPen>

GoalPoint::GoalPoint(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent) : QGraphicsEllipseItem (x, y, width, height, parent)
{
	QPen pen;
	pen.setColor(QColor(0, 20, 20));
	QBrush brush;
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	brush.setColor(QColor(200, 200, 250));
	this->setPen(pen);
	this->setBrush(brush);
}


GoalPoint::~GoalPoint(void)
{
}

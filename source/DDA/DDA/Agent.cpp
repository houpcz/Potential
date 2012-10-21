#include "Agent.h"
#include <QBrush>
#include <QPainter>

Agent::Agent(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent) : QGraphicsEllipseItem (x, y, width, height, parent)
{
	QBrush brush;
	brush.setColor(QColor(255, 0, 0));
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	setBrush(brush);
	
	vY = 15;
	vX = 0;
	this->setZValue(10);
}


Agent::~Agent(void)
{
}

void Agent::Tick(int millis)
{
	setX(x() + vX * millis / 1000.0);
	setY(y() + vY * millis / 1000.0);
}

void Agent::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QBrush brush;
	brush.setColor(QColor(200, 200, 250));
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	painter->setBrush(brush);
	painter->drawEllipse(goalX, goalY, 10, 10);

	QGraphicsEllipseItem::paint(painter, option, widget);
}

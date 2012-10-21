#include <QPen>
#include "Obstacle.h"


Obstacle::Obstacle(void)
{
	QPen pen;
	pen.setColor(QColor(0, 20, 20));
	QBrush brush;
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	brush.setColor(QColor(150, 150, 240));
	this->setPen(pen);
	this->setBrush(brush);
}


Obstacle::~Obstacle(void)
{
}

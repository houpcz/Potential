#include "Agent.h"
#include <QBrush>
#include <QPainter>

Agent::Agent(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent) : QGraphicsEllipseItem (x, y, width, height, parent)
{
	QBrush brush;
	brush.setColor(QColor(255, 0, 0));
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	setBrush(brush);
	
	fieldCenterX = 0.0f;
	fieldCenterY = 0.0f;
	vY = 7;
	vX = 0;
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
	for(int loop1 = 0; loop1 < FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < FIELD_WIDTH; loop2++)
		{
			int tileX = (int) fieldCenterX + (loop2 - (Agent::FIELD_WIDTH / 2)) * TILE_WIDTH;
			int tileY = (int) fieldCenterY + (loop1 - (Agent::FIELD_WIDTH / 2)) * TILE_WIDTH;
			painter->drawRect(tileX, tileY, TILE_WIDTH, TILE_WIDTH);
		}
	}

	QBrush brush;
	brush.setColor(QColor(200, 200, 250));
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	painter->setBrush(brush);
	painter->drawEllipse(goalX, goalY, 10, 10);

	QGraphicsEllipseItem::paint(painter, option, widget);
}

void Agent::SetPotentialField(float ** _potentialField, qreal _fieldCenterX, qreal _fieldCenterY)
{
	fieldCenterX = _fieldCenterX;
	fieldCenterY = _fieldCenterY;

	for(int loop1 = 0; loop1 < FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < FIELD_WIDTH; loop2++)
		{
			potentialField[loop1][loop2] = _potentialField[loop1][loop2];
		}
	}
}
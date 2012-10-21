#include <QPainter>
#include "PotentialField.h"


PotentialField::PotentialField( qreal x, qreal y, QGraphicsItem * parent) :  QGraphicsRectItem( x - (FIELD_WIDTH / 2) * TILE_WIDTH, y  - (FIELD_WIDTH / 2) * TILE_WIDTH, TILE_WIDTH * FIELD_WIDTH, TILE_WIDTH * FIELD_WIDTH, parent )
{
	fieldCenterX = 0.0f;
	fieldCenterY = 0.0f;
}


PotentialField::~PotentialField(void)
{
}

void PotentialField::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	float variance = maxValue - minValue;
	if(variance == 0.0f)
		variance = 1.0f;

	QBrush brush;
	brush.setStyle(Qt::BrushStyle::SolidPattern);

	for(int loop1 = 0; loop1 < FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < FIELD_WIDTH; loop2++)
		{
			int tileX = (int) fieldCenterX + loop2 * TILE_WIDTH;
			int tileY = (int) fieldCenterY + loop1 * TILE_WIDTH;
			int value = (int) (235 * (potentialField[loop1][loop2] - minValue) / variance) + 20;
			brush.setColor(QColor(255 - value, 0, 255 - value));
			painter->setBrush(brush);
			painter->drawRect(tileX, tileY, TILE_WIDTH, TILE_WIDTH);
		}
	}
}

void PotentialField::SetPotentialField(float ** _potentialField, qreal _fieldCenterX, qreal _fieldCenterY)
{
	fieldCenterX = _fieldCenterX - (FIELD_WIDTH / 2) * TILE_WIDTH;
	fieldCenterY = _fieldCenterY - (FIELD_WIDTH / 2) * TILE_WIDTH;

	minValue = maxValue = potentialField[0][0];
	for(int loop1 = 0; loop1 < FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < FIELD_WIDTH; loop2++)
		{
			potentialField[loop1][loop2] = _potentialField[loop1][loop2];
			if(potentialField[loop1][loop2] < minValue)
				minValue = potentialField[loop1][loop2];
			else if(potentialField[loop1][loop2] > maxValue)
				maxValue= potentialField[loop1][loop2];
		}
	}
	
	setRect(fieldCenterX, fieldCenterY, rect().width(), rect().height());
}
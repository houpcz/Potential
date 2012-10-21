#include <QPainter>
#include "PotentialField.h"


PotentialField::PotentialField(Agent * _agent,  qreal x, qreal y, QGraphicsItem * parent) :  QGraphicsRectItem( x - (FIELD_WIDTH / 2) * TILE_WIDTH, y  - (FIELD_WIDTH / 2) * TILE_WIDTH, TILE_WIDTH * FIELD_WIDTH, TILE_WIDTH * FIELD_WIDTH, parent )
{
	agent = _agent;
	fieldCenterX = 0.0f;
	fieldCenterY = 0.0f;
}


PotentialField::~PotentialField(void)
{
}

void PotentialField::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	return;

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
			int value = ((int) (235 * (potentialField[loop1][loop2] - minValue) / variance) + 20);		
			if(value > 255)
				value = 255;
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

	maxValue = -1;
	minValue = potentialField[0][0];
	for(int loop1 = 0; loop1 < FIELD_WIDTH; loop1++)
	{
		for(int loop2 = 0; loop2 < FIELD_WIDTH; loop2++)
		{
			potentialField[loop1][loop2] = _potentialField[loop1][loop2];
			if(potentialField[loop1][loop2] < minValue)
				minValue = potentialField[loop1][loop2];
			else if(potentialField[loop1][loop2] > maxValue && potentialField[loop1][loop2] < OBSTACLE - 1.0f)
				maxValue= potentialField[loop1][loop2];
		}
	}
	
	setRect(fieldCenterX, fieldCenterY, rect().width(), rect().height());

	FindPath();
}

void PotentialField::FindPath()
{
	int centerX = FIELD_WIDTH / 2;
	int centerY = FIELD_WIDTH / 2;
	int minCenter;
	int minX, minY;
	minCenter = potentialField[centerY][centerX];
	minX = centerX;
	minY = centerY;

	if(potentialField[centerY - 1][centerX] < minCenter) 
	{
		minX = centerX;
		minY = centerY - 1;
		minCenter = potentialField[centerY - 1][centerX];
	}
	if(potentialField[centerY - 1][centerX - 1] < minCenter) 
	{
		minX = centerX - 1;
		minY = centerY - 1;
		minCenter = potentialField[centerY - 1][centerX - 1];
	}
	if(potentialField[centerY][centerX - 1] < minCenter) 
	{
		minX = centerX - 1;
		minY = centerY;
		minCenter = potentialField[centerY][centerX - 1];
	}

	int lastMinX;
	int lastMinY;
	bool isNewOptimum;
	do {
		PushToPath(minX, minY);
		lastMinX = minX;
		lastMinY = minY;
		isNewOptimum = false;

		if(lastMinY > 0 && potentialField[lastMinY - 1][lastMinX] < minCenter) 
		{
			isNewOptimum = true;
			minX = lastMinX;
			minY = lastMinY - 1;
			minCenter = potentialField[lastMinY - 1][lastMinX];
		}
		if(lastMinY < FIELD_WIDTH - 1 && potentialField[lastMinY + 1][lastMinX] < minCenter) 
		{
			isNewOptimum = true;
			minX = lastMinX;
			minY = lastMinY + 1;
			minCenter = potentialField[lastMinY + 1][lastMinX];
		}
		if(lastMinX > 0 && potentialField[lastMinY][lastMinX - 1] < minCenter) 
		{
			isNewOptimum = true;
			minX = lastMinX - 1;
			minY = lastMinY;
			minCenter = potentialField[lastMinY][lastMinX - 1];
		}
		if(lastMinX < FIELD_WIDTH - 1 && potentialField[lastMinY][lastMinX + 1] < minCenter) 
		{
			isNewOptimum = true;
			minX = lastMinX + 1;
			minY = lastMinY;
			minCenter = potentialField[lastMinY][lastMinX + 1];
		}
		if(lastMinY > 0 && lastMinX > 0 && potentialField[lastMinY - 1][lastMinX - 1] < minCenter) 
		{
			isNewOptimum = true;
			minX = lastMinX - 1;
			minY = lastMinY - 1;
			minCenter = potentialField[lastMinY - 1][lastMinX - 1];
		}
		if(lastMinY < FIELD_WIDTH - 1 && lastMinX < FIELD_WIDTH - 1 && potentialField[lastMinY + 1][lastMinX + 1] < minCenter) 
		{
			isNewOptimum = true;
			minX = lastMinX + 1;
			minY = lastMinY + 1;
			minCenter = potentialField[lastMinY + 1][lastMinX + 1];
		}
		if(lastMinX > 0 && lastMinY < FIELD_WIDTH - 1 && potentialField[lastMinY + 1][lastMinX - 1] < minCenter) 
		{
			isNewOptimum = true;
			minX = lastMinX - 1;
			minY = lastMinY + 1;
			minCenter = potentialField[lastMinY + 1][lastMinX - 1];
		}
		if(lastMinX < FIELD_WIDTH - 1 && lastMinY > 0 && potentialField[lastMinY - 1][lastMinX + 1] < minCenter) 
		{
			isNewOptimum = true;
			minX = lastMinX + 1;
			minY = lastMinY - 1;
			minCenter = potentialField[lastMinY - 1][lastMinX + 1];
		}
	}while(isNewOptimum);

	agent->SetPath(&path);
}

void PotentialField::PushToPath(int fieldIdX, int fieldIdY)
{
	path.push(Point2D(fieldIdX * TILE_WIDTH + fieldCenterX, fieldIdY * TILE_WIDTH + fieldCenterY));
}
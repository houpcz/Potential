#include <QPainter>
#include <queue>
#include "PotentialField.h"
#include "BestFirstSearchNode.h"

using namespace std;

bool operator<(const BestFirstSearchNode & node1, const BestFirstSearchNode & node2)
{
	return node1.value > node2.value;
}

int PotentialField::tileWidth = 4;
int PotentialField::fieldWidth = 32;
int PotentialField::potentialFieldCount = 0;
bool PotentialField::show1Field = false;

PotentialField::PotentialField(Agent * _agent,  qreal x, qreal y, QGraphicsItem * parent) :  QGraphicsRectItem( x - (fieldWidth / 2) * tileWidth, y  - (fieldWidth / 2) * tileWidth, tileWidth * fieldWidth, tileWidth * fieldWidth, parent )
{
	agent = _agent;
	fieldID = potentialFieldCount++;
	fieldCenterX = 0.0f;
	fieldCenterY = 0.0f;
	time = NULL;
	fieldPrepared = false;
	potentialField = new float*[fieldWidth];
	road = new char*[fieldWidth];
	for(int loop1 = 0; loop1 < fieldWidth; loop1++)
	{
		potentialField[loop1] = new float[fieldWidth];
		road[loop1] = new char[fieldWidth];
	}
}


PotentialField::~PotentialField(void)
{
	if(time != NULL)
		delete time;

	for(int loop1 = 0; loop1 < fieldWidth; loop1++)
	{
		delete [] potentialField[loop1];
		delete [] road[loop1];
	}
	delete [] potentialField;
	delete [] road;
}

void PotentialField::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	if(fieldID != 0 || !show1Field)
		return;

	float variance = maxValue - minValue;
	if(variance == 0.0f)
		variance = 1.0f;

	QBrush brush;
	brush.setStyle(Qt::BrushStyle::SolidPattern);

	for(int loop1 = 0; loop1 < fieldWidth; loop1++)
	{
		for(int loop2 = 0; loop2 < fieldWidth; loop2++)
		{
			int tileX = (int) fieldCenterX + loop2 * tileWidth;
			int tileY = (int) fieldCenterY + loop1 * tileWidth;
			int value = ((int) (235 * (potentialField[loop1][loop2] - minValue) / variance) + 20);		
			if(value > 255)
				value = 255;

			//if(road[loop1][loop2] == ON_ROAD)
			//	brush.setColor(QColor(0, 255, 0));
			//else
			brush.setColor(QColor(255 - value, 0, 255 - value));

			painter->setBrush(brush);
			painter->drawRect(tileX, tileY, tileWidth, tileWidth);
		}
	}
}

void PotentialField::SetPotentialField(float ** _potentialField, qreal _fieldCenterX, qreal _fieldCenterY)
{
	fieldCenterX = _fieldCenterX - (fieldWidth / 2) * tileWidth;
	fieldCenterY = _fieldCenterY - (fieldWidth / 2) * tileWidth;

	Point2D fieldCenter = agent->FieldCenter();
	int plusX = (fieldCenter.X() - _fieldCenterX) / tileWidth;
	int plusY = (fieldCenter.Y() - _fieldCenterY) / tileWidth;
	agentCenterX = fieldWidth / 2 + plusX;
	agentCenterY = fieldWidth / 2 + plusY;

	maxValue = -1;
	minValue = potentialField[0][0];
	for(int loop1 = 0; loop1 < fieldWidth; loop1++)
	{
		for(int loop2 = 0; loop2 < fieldWidth; loop2++)
		{
			potentialField[loop1][loop2] = _potentialField[loop1][loop2];
			if(potentialField[loop1][loop2] < minValue)
				minValue = potentialField[loop1][loop2];
			else if(potentialField[loop1][loop2] > maxValue && potentialField[loop1][loop2] < OBSTACLE_IN_FIELD / 100)
				maxValue= potentialField[loop1][loop2];
		}
	}
	
	setRect(fieldCenterX, fieldCenterY, rect().width(), rect().height());

	fieldPrepared = true;
}

void PotentialField::FindPath()
{
	if(!fieldPrepared)
		return;

	int centerX = agentCenterX;
	int centerY = agentCenterY;
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

	priority_queue<BestFirstSearchNode> queueField;
	queueField.push(BestFirstSearchNode(minX, minY, minCenter));
	BestFirstSearchNode tempNode(0, 0, 0);
	/* CLOSE SEZNAM */
	for(int loop1 = 0; loop1 < fieldWidth; loop1++)
	{
		for(int loop2 = 0; loop2 < fieldWidth; loop2++)
		{
			road[loop1][loop2] = NO_DIR;
		}
	}
	road[minY][minX] = START;

	int lastMinY;
	int lastMinX;

	bool pathFound = false;
	while(!queueField.empty())
	{
		tempNode = queueField.top();
		queueField.pop();

		lastMinY = tempNode.y;
		lastMinX = tempNode.x;

		if(tempNode.value <= minValue + 0.00001f)
		{
			//PushToPath(lastMinX, lastMinY);
			pathFound = true;
			break;
		}

		if(lastMinY > 0 && road[lastMinY - 1][lastMinX] == NO_DIR) 
		{
			minX = lastMinX;
			minY = lastMinY - 1;
			minCenter = potentialField[lastMinY - 1][lastMinX];
			queueField.push(BestFirstSearchNode(minX, minY, minCenter));
			road[minY][minX] = UP_CENTER;
		}
		if(lastMinY < fieldWidth - 1 && road[lastMinY + 1][lastMinX] == NO_DIR) 
		{
			minX = lastMinX;
			minY = lastMinY + 1;
			minCenter = potentialField[lastMinY + 1][lastMinX];
			queueField.push(BestFirstSearchNode(minX, minY, minCenter));
			road[minY][minX] = DOWN_CENTER;
		}
		if(lastMinX > 0 && road[lastMinY][lastMinX - 1] == NO_DIR) 
		{
			minX = lastMinX - 1;
			minY = lastMinY;
			minCenter = potentialField[lastMinY][lastMinX - 1];
			queueField.push(BestFirstSearchNode(minX, minY, minCenter));
			road[minY][minX] = LEFT;
		}
		if(lastMinX < fieldWidth - 1 && road[lastMinY][lastMinX + 1] == NO_DIR) 
		{
			minX = lastMinX + 1;
			minY = lastMinY;
			minCenter = potentialField[lastMinY][lastMinX + 1];
			queueField.push(BestFirstSearchNode(minX, minY, minCenter));
			road[minY][minX] = RIGHT;
		}
		if(lastMinY > 0 && lastMinX > 0  && road[lastMinY - 1][lastMinX - 1] == NO_DIR) 
		{
			minX = lastMinX - 1;
			minY = lastMinY - 1;
			minCenter = potentialField[lastMinY - 1][lastMinX - 1];
			queueField.push(BestFirstSearchNode(minX, minY, minCenter));
			road[minY][minX] = UP_LEFT;
		}
		if(lastMinY < fieldWidth - 1 && lastMinX < fieldWidth - 1  && road[lastMinY + 1][lastMinX + 1] == NO_DIR) 
		{
			minX = lastMinX + 1;
			minY = lastMinY + 1;
			minCenter = potentialField[lastMinY + 1][lastMinX + 1];
			queueField.push(BestFirstSearchNode(minX, minY, minCenter));
			road[minY][minX] = DOWN_RIGHT;
		}
		if(lastMinX > 0 && lastMinY < fieldWidth - 1  && road[lastMinY + 1][lastMinX - 1] == NO_DIR) 
		{
			minX = lastMinX - 1;
			minY = lastMinY + 1;
			minCenter = potentialField[lastMinY + 1][lastMinX - 1];
			queueField.push(BestFirstSearchNode(minX, minY, minCenter));
			road[minY][minX] = DOWN_LEFT;
		}
		if(lastMinX < fieldWidth - 1 && lastMinY > 0 && road[lastMinY - 1][lastMinX + 1] == NO_DIR) 
		{
			minX = lastMinX + 1;
			minY = lastMinY - 1;
			minCenter = potentialField[lastMinY - 1][lastMinX + 1];
			queueField.push(BestFirstSearchNode(minX, minY, minCenter));
			road[minY][minX] = UP_RIGHT;
		}
	}

	if(!pathFound)
		return;

	int newMinX = lastMinX, newMinY = lastMinY;
	int checkPointNumber = 0;
	while(road[newMinY][newMinX] != START)
	{
		lastMinX = newMinX;
		lastMinY = newMinY;
		switch(road[lastMinY][lastMinX])
		{
			case UP_LEFT :
				newMinX = lastMinX + 1;
				newMinY = lastMinY + 1;
				break;
			case UP_CENTER :
				newMinY = lastMinY + 1;
				break; 
			case UP_RIGHT :
				newMinX = lastMinX - 1;
				newMinY = lastMinY + 1;
				break;
			case LEFT :
				newMinX = lastMinX + 1;
				break;
			case RIGHT :
				newMinX = lastMinX - 1;
				break;
			case DOWN_LEFT :
				newMinX = lastMinX + 1;
				newMinY = lastMinY - 1;
				break;
			
			case DOWN_CENTER :
				newMinY = lastMinY - 1;
				break;
			case DOWN_RIGHT :
				newMinX = lastMinX - 1;
				newMinY = lastMinY - 1;
				break;
		}

		road[lastMinY][lastMinX] = ON_ROAD;
		if(road[newMinY][newMinX] != START)
		{
			checkPointNumber++;
			//if(checkPointNumber % 3 == 0)
			PushToPath(newMinX, newMinY);
		}
	}

	agent->SetPath(&path);
}

void PotentialField::PushToPath(int fieldIdX, int fieldIdY)
{
	path.push(Point2D(fieldIdX * tileWidth + fieldCenterX, fieldIdY * tileWidth + fieldCenterY));
}
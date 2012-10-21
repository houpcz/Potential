#pragma once

#include <QGraphicsRectItem>

class PotentialField : public QGraphicsRectItem
{
public :
	static const int FIELD_WIDTH = 20;
	static const int TILE_WIDTH = 5;
private:
	qreal fieldCenterX;
	qreal fieldCenterY;
	float minValue;
	float maxValue;
	float potentialField[FIELD_WIDTH][FIELD_WIDTH];
public:
	PotentialField( qreal x, qreal y, QGraphicsItem * parent = 0 );
	~PotentialField(void);
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	void SetPotentialField(float ** _potentialField, qreal _fieldCenterX, qreal _fieldCenterY);
};


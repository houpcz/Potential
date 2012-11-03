#include "Agent.h"
#include <QBrush>
#include <QPainter>
#include <QDebug>

Agent::Agent(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent) : realPos(x, y), velocity(0, 0), QGraphicsEllipseItem (x, y, width, height, parent)
{
	QBrush brush;
	brush.setColor(QColor(255, 0, 0));
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	setBrush(brush);

	this->setZValue(10);
}

void Agent::SetGoal(int _goalX, int _goalY) 
{ 
	goalX = _goalX; 
	goalY = _goalY; 

	//path.push(Point2D(goalX, goalY));
}

Agent::~Agent(void)
{
}

void Agent::Tick(int millis)
{
	float dx = velocity.X() * millis / 1000.0f;
	float dy = velocity.Y() * millis / 1000.0f;
	setX(x() + dx);
	setY(y() + dy);
	float whatX = x();
	float whatY = y();

	realPos.Set(x() + rect().x(), y() + rect().y());

	if(!path.empty())
	{
		Point2D waypoint(0, 0);
		
		bool wasPop;
		do {
			wasPop = false;
			waypoint = path.front();
			float realPosDist = realPos.Distance(waypoint);
			if(realPosDist < 3 || realPos.Distance(Point2D(goalX, goalY)) < 4 )
			{
				path.pop();
				wasPop = true;
			} 
		}while(wasPop && !path.empty());

		velocity.Set(waypoint.X() - realPos.X(), waypoint.Y() - realPos.Y());
		velocity.SetLength(25.0f);
	} else {
		velocity.SetLength(0.0f);
	}
}

void Agent::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QBrush brush;
	brush.setColor(QColor(200, 200, 250));
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	painter->setBrush(brush);

	QGraphicsEllipseItem::paint(painter, option, widget);
}

void Agent::SetPath(stack<Point2D> * _path)
{
	if(realPos.Distance(Point2D(goalX, goalY)) < 4)
		return;

	while(!path.empty())
		path.pop();

	while(!_path->empty())
	{
		path.push(_path->top());
		_path->pop();
	}
}

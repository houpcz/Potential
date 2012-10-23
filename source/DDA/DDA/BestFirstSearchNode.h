#ifndef _BESTFIRSTSEARCHNODE_H_
#define _BESTFIRSTSEARCHNODE_H_

class BestFirstSearchNode
{
public:
	int x;
	int y;
	float value;
	BestFirstSearchNode * parent;
public:
	BestFirstSearchNode(int _x, int _y, float _value);
	~BestFirstSearchNode(void);
};

#endif

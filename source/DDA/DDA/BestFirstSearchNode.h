//----------------------------------------------------------------------------------------
/**
 * \file       BestFirstSearchNode.h
 * \author     Lukas Beran
 * \date       2012/11/25
 * \brief      Contains BestFirstSearchNode class
 *
 *  Node class used in A Star path finding.
 *
*/
//----------------------------------------------------------------------------------------

#ifndef _BESTFIRSTSEARCHNODE_H_
#define _BESTFIRSTSEARCHNODE_H_

class BestFirstSearchNode
{
public:
	int x;							///< column of potential field
	int y;							///< row of potential field
	float value;					///< value of potential field tile on pos x, y
	BestFirstSearchNode * parent;	///< parent node to build path
public:
	BestFirstSearchNode(int _x, int _y, float _value);
	~BestFirstSearchNode(void);
};

#endif

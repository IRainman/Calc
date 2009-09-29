#include "stdafx.h"
#include "Calculator.h"

bool Analize()
{
	return false;//true;
}

inline int GetPriorityOnLineExpression(char a)
{
	switch(a)
	{
		case '^':
			return 4;

		case '*':
		case '/':
			return 3;

		case '-':
		case '+':
			return 2;

		case '(':
			return 1;

		default:
			return -1;
	}
}

int GetPriority()
{

}
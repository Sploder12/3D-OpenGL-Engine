#include "objectClasses.h"

//Base Shape
void shape::translate(float x, float y, float z)
{
	for (size_t vertIt = 0; vertIt < this->vertCnt; vertIt++)
	{
		this->vertices[vertIt * 3] += x;
		this->vertices[vertIt * 3 + 1] += y;
		this->vertices[vertIt * 3 + 2] += z;
	}
}

void shape::scale(float x, float y, float z)
{

}

void shape::rotate(float pitch, float roll, float yaw)
{

}
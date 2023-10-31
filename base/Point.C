#include "Point.h"

using namespace img;

Point::Point() : 
    x(0),
    y(0)
{
}


Point::Point(float x, float y) : 
    x(x),
    y(y)
{
}


Point::Point(const Point& point)
{
    x = point.x;
    y = point.y;
}



float Point::getX() const{
    return x;
}

float Point::getY() const{
    return y;
}

void Point::setX(float newX){
    x = newX;
}
void Point::setY(float newY){
    y = newY;
}





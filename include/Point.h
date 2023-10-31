#ifndef STARTER_POINT_H
#define STARTER_POINT_H



namespace img{

class Point{
    public:
        Point();

        Point(float x, float y);

        Point(const Point& p);

        float getX() const;
        float getY() const;

        void setX(float newX);
        void setY(float newY);

    private:
        float x;
        float y;


};


}

#endif


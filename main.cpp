#include <iostream>
#include <cmath>

using namespace std;

class Point
{
    int x;
    int y;

public:
    Point()
    {
        x = 0;
        y = 0;
    }

    Point(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void setPoint(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    int getX()
    {
        return x;
    }

    int getY()
    {
        return y;
    }

    bool operator ==(Point& other){
        return x == other.x && y == other.y;
    }
};

class Circle
{
public:
    Circle(Point &center, const int radius)
    {
        this->center = center;
        this->radius = radius;
        calcPoints();
    }

    int getPointsLen(){
        return pointsLen;
    }

    Point* & getPoints(){
        return points;
    }

    bool isPointOnCircle(Point& point)
    {
        for (int i = 0; i < pointsLen; i++)
        {
            if (point == points[i]) return true;
        }
        return false;
    }

    bool isPointInCircle(Point &point) // except those on circle
    {
        int squareRadius = radius * radius;
        int value = round(pow(point.getX() - center.getX(), 2) + pow(point.getY() - center.getX(), 2));
        return value < squareRadius;
    }

private:
    Point center;
    int radius;

    Point *points;
    int pointsLen;

    void calcPoints()
    {
        int projectionOnAxisLen = 2 * radius;
        pointsLen = projectionOnAxisLen * 4 + 4; // 4 slots for projections (max lengths) and 4 places for furthest points
        points = new Point[pointsLen];

        int quarter = radius - 1;

        int shift = 0;

        for (int i = 0; i < quarter; i++)
        {
            shift = round( sqrt(pow(radius, 2) - pow(i + 1, 2)) );

            // calculating by X axis
            points[i            ] = Point(center.getX() + (i + 1), center.getY() + shift); // top right quarter
            points[i +   quarter] = Point(center.getX() - (i + 1), center.getY() + shift); // top left quarter
            points[i + 2*quarter] = Point(center.getX() + (i + 1), center.getY() - shift); // bottom right quarter
            points[i + 3*quarter] = Point(center.getX() - (i + 1), center.getY() - shift); // bottom left quarter

            // calculating by Y axis
            Point topRight    (center.getX() + shift, center.getY() + (i + 1));
            Point topLeft     (center.getX() + shift, center.getY() - (i + 1));
            Point bottomRight (center.getX() - shift, center.getY() + (i + 1));
            Point bottomLeft  (center.getX() - shift, center.getY() - (i + 1));

            if (!isPointOnCircle(topRight))    points[i + 4*quarter] = topRight;
            if (!isPointOnCircle(topLeft))     points[i + 5*quarter] = topLeft;
            if (!isPointOnCircle(bottomRight)) points[i + 6*quarter] = bottomRight;
            if (!isPointOnCircle(bottomLeft))  points[i + 7*quarter] = bottomLeft;

            // adding furthest points
            points[8*quarter    ] = Point(center.getX(), center.getY() + radius); // top
            points[8*quarter + 1] = Point(center.getX(), center.getY() - radius); // bottom
            points[8*quarter + 2] = Point(center.getX() + radius, center.getY()); // right
            points[8*quarter + 3] = Point(center.getX() - radius, center.getY()); // left
        }
        
    }
};

class Axis
{
    int size;
    char **ax;

public:
    Axis(const int size)
    {
        this->size = size;
        ax = new char *[size];
        for (int i = 0; i < size; i++)
        {
            ax[i] = new char[size];
            for (int j = 0; j < size; j++)
            {
                ax[i][j] = '.';
            }
        }
    }

    template <class T>
    void drawObject(T &object, const char symbol)
    {
        if (object.getPointsLen() > 0)
        {
            int x, col;
            int y, row;
            for (int i = 0; i < object.getPointsLen(); i++)
            {
                x = object.getPoints()[i].getX();
                y = object.getPoints()[i].getY();

                row = y;
                col = x;

                ax[row][col] = symbol;
            }
        }
        
    }

    void printAxis()
    {
        for (int i = size - 1; i >= 0; i--)
        {
            for (int j = 0; j < size; j++)
            {
                cout << ax[i][j] << ' ';
            }
            cout << endl;
        }
    }

    ~Axis()
    {
        for (int i = 0; i < size; i++)
        {
            delete[] ax[i];
        }
        delete[] ax;
    }
};

int main()
{
    Axis ax(11);

    Point center(5, 5);
    Circle mainCircle(center, 3);

    ax.drawObject(mainCircle, '#');
    ax.printAxis();

    return 0;
}
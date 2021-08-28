#include <iostream>
#include <cmath>
#include <random>
#include <ctime>

using namespace std;


class Point
{
private:
    int x;
    int y;

public:
    Point()
    {
        x = -1;
        y = -1;
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

class Shape
{
protected:
    Point* points = nullptr;
    int pointsLen;
public:
    ~Shape()
    {
        delete[] points;
    }

    int getPointsLen(){
        return pointsLen;
    }

    Point* & getPoints(){
        return points;
    }

    void clearRedundantPoints()  // constraining array with skipping redundant points
    {
        int realLen = 0;
        for (int i = 0; i < pointsLen; i++)
        {
            if (points[i].getX() != -1)
            {
                realLen++;
            }
        }
        Point* temp = new Point[realLen];
        for (int i = 0, j = 0; i < pointsLen; i++)
        {
            if (points[i].getX() != -1)
            {
                temp[j] = points[i];
                 j++;
            }
        }
        delete[] points;
        points = temp;
        pointsLen = realLen;
    }
};

class Circle : public Shape
{
private:
    Point center;
    int radius;

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

public:
    Circle(Point &center, const int radius)
    {
        this->center = center;
        this->radius = radius;
        calcPoints();
        clearRedundantPoints();        
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

    Point & getRandomPointOnCircle()  // todo
    {
        int randomIndex = rand() % (pointsLen - 1);
        return points[randomIndex];
    }
};

class Line : public Shape
{
private:
    Point a;
    Point b;
    
    void calculatePoints()
    {
        // getting number of points
        int xProjectionLen = abs(b.getX() - a.getX());
        int yProjectionLen = abs(b.getY() - a.getY());

        pointsLen = xProjectionLen + yProjectionLen + 2;
        points = new Point[pointsLen];

        // a and b on circle
        points[xProjectionLen + yProjectionLen] = a;
        points[xProjectionLen + yProjectionLen + 1] = b;

        
        int startY, startX;
        a.getY() <= b.getY() ? startY = a.getY() : startY = b.getY();
        a.getX() <= b.getX() ? startX = a.getX() : startX = b.getX();

        if (xProjectionLen == 0)
        {
            for (int i = 0; i < yProjectionLen; i++) // only by y axis
            {
                points[i] = Point(startX, startY + (i+1));
            }
        }
        else if (yProjectionLen == 0)
        {
            for (int i = 0; i < xProjectionLen; i++) // only by x axis
            {
                points[i] = Point(startX + (i+1), startY);
            }
        }
        else
        {
            float k = (float) (b.getY() - a.getY()) / (b.getX() - a.getX());
            float c = (float) a.getY() - k * a.getX();

            for (int i = 0; i < xProjectionLen; i++) // by x axis
            {
                points[i] = Point(startX + (i+1), round(k*(startX + (i+1)) + c) ); // y = k*x + b
            }
            for (int i = 0; i < yProjectionLen; i++) // by y axis
            {
                points[xProjectionLen + i] = Point(round((startY + (i+1) - c)/k), startY + (i+1) ); // x = (y - c)/k
            }
        }
    }

public:
    Line(Point &a, Point &b)
    {
        this->a = a;
        this->b = b;
        calculatePoints();
        clearRedundantPoints();
    }
};

class Axis
{
private:
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

    void drawShape(Shape &shape, const char symbol)
    {
        if (shape.getPointsLen() > 0)
        {
            int x, col;
            int y, row;
            for (int i = 0; i < shape.getPointsLen(); i++)
            {
                x = shape.getPoints()[i].getX();
                y = shape.getPoints()[i].getY();

                if (x < 0 || y < 0 || x > size || y > size)
                {
                    cout << "Wrong coordinates of figure! Skipping..." << endl;
                    return;
                }
                

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

    ax.drawShape(mainCircle, '#');

    Point a;
    Point b;

    srand(time(NULL));
    a = mainCircle.getRandomPointOnCircle();
    b = mainCircle.getRandomPointOnCircle();

    Line line(a, b);

    ax.drawShape(line, '*');

    ax.printAxis();

    return 0;
}
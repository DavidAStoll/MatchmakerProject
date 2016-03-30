#ifndef MATHALGORITHMS_H
#define MATHALGORITHMS_H

#include <QVector>
#include <QGLWidget>


class MathAlgorithms
{
public:
    struct Vertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;

        Vertex()
        {
            x = 0;
            y = 0;
            z = 0;
        }

        Vertex(GLfloat xLocation, GLfloat yLocation)
        {
            x = xLocation;
            y = yLocation;
            z = 0;
        }

        bool operator==(Vertex& otherVertex)
        {
            return x == otherVertex.x && y == otherVertex.y;
        }
    };

    struct Triangle
    {
        Vertex point1;
        Vertex point2;
        Vertex point3;
    };




   static QVector<Triangle> getDelaunayTriangulation(QVector<Vertex> aPoints);
   static QVector<Triangle> getDelaunayTriangulation(QVector<Vertex> aFixedPoints, QVector<Vertex> aRelativePoints);
   static void createCounterClockwiseTriangle(QVector<Triangle>& triangleStorage, Vertex point1, Vertex point2, Vertex point3);
};

#endif // MATHALGORITHMS_H

#ifndef MATHALGORITHMS_H
#define MATHALGORITHMS_H

#include <QVector>

class MathAlgorithms
{
public:
    struct Vertex
    {
        float x;
        float y;
        float z;
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

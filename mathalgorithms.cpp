#include "mathalgorithms.h"
#include <cmath>

QVector<MathAlgorithms::Triangle> MathAlgorithms::getDelaunayTriangulation(QVector<MathAlgorithms::Vertex> aPoints)
{
    //Algorithm follows: https://en.wikipedia.org/wiki/Delaunay_triangulation
    //basically create all possible triangles with the given vertices and then remove all that are invalid ones (intesect with another valid triangle)
    //thus we are left with the full set of valid triangles for the set of vertices

    QVector<MathAlgorithms::Triangle> trianglesForPoints;

    //create all possible triangles for the given points
    for(int outerIndex = 0; outerIndex < aPoints.size(); outerIndex++)
    {
        MathAlgorithms::Vertex currentPoint = aPoints[outerIndex];

        for(int nextPointIndex = outerIndex + 1; nextPointIndex < aPoints.size(); nextPointIndex++)
        {
            MathAlgorithms::Vertex currentPointPlus1 = aPoints[nextPointIndex];

            for(int nextNextPointIndex = nextPointIndex + 1; nextNextPointIndex < aPoints.size(); nextNextPointIndex++)
            {
                MathAlgorithms::Vertex currentPointPlus2 = aPoints[nextNextPointIndex];
                createCounterClockwiseTriangle(trianglesForPoints, currentPoint, currentPointPlus1, currentPointPlus2);
            }
        }
    }

    //check for all triangles if any vertex that is not part of the triangle is inside that that triangle
    //if it is that triangle is not a delaunay triangle and has to be removed
    for(int pointIndex = 0; pointIndex < aPoints.size(); pointIndex++)
    {
        MathAlgorithms::Vertex currentPoint = aPoints[pointIndex];

        for(int triangleIndex = 0; triangleIndex < trianglesForPoints.size(); triangleIndex++)
        {
            MathAlgorithms::Triangle& triangle = trianglesForPoints[triangleIndex];

            //point is not part of the triangle
            if(!(triangle.point1.x == currentPoint.x && triangle.point1.y == currentPoint.y && triangle.point1.z == currentPoint.z)
            && !(triangle.point2.x == currentPoint.x && triangle.point2.y == currentPoint.y && triangle.point2.z == currentPoint.z)
            && !(triangle.point3.x == currentPoint.x && triangle.point3.y == currentPoint.y && triangle.point3.z == currentPoint.z))
            {
                //determine if inside the circumcircle
                float circumMatrix [3][3];
                //column 1
                circumMatrix [0][0] = triangle.point1.x - currentPoint.x;
                circumMatrix [1][0] = triangle.point2.x - currentPoint.x;
                circumMatrix [2][0] = triangle.point3.x - currentPoint.x;
                //column 2
                circumMatrix [0][1] = triangle.point1.y - currentPoint.y;
                circumMatrix [1][1] = triangle.point2.y - currentPoint.y;
                circumMatrix [2][1] = triangle.point3.y - currentPoint.y;
                //column 3
                circumMatrix [0][2] = (pow(triangle.point1.x, 2) - pow(currentPoint.x, 2)) + (pow(triangle.point1.y, 2) - pow(currentPoint.y, 2));
                circumMatrix [1][2] = (pow(triangle.point2.x, 2) - pow(currentPoint.x, 2)) + (pow(triangle.point2.y, 2) - pow(currentPoint.y, 2));
                circumMatrix [2][2] = (pow(triangle.point3.x, 2) - pow(currentPoint.x, 2)) + (pow(triangle.point3.y, 2) - pow(currentPoint.y, 2));

                //calculate determine of matrix
                double determinantLeftPart = (circumMatrix[0][0] * circumMatrix[1][1] * circumMatrix[2][2]) + (circumMatrix[0][1] * circumMatrix[1][2] * circumMatrix[2][0]) + (circumMatrix[0][2] * circumMatrix[1][0] * circumMatrix[2][1]);
                double determinantRightPart = (circumMatrix[0][2] * circumMatrix[1][1] * circumMatrix[2][0]) + (circumMatrix[0][1] * circumMatrix[1][0] * circumMatrix[2][2]) + (circumMatrix[0][0] * circumMatrix[1][2] * circumMatrix[2][1]);
                double determinant = determinantLeftPart - determinantRightPart;
                if(determinant > 0) //only true if point is inside circumcircle of triangle
                {
                    //triangle is no good, need to remove
                    trianglesForPoints.removeAt(triangleIndex);
                    triangleIndex--; // since array shrinked by 1 otherwise we skip the next triangle in the list
                }
            }
        }
    }

    return trianglesForPoints;
}

QVector<MathAlgorithms::Triangle> MathAlgorithms::getDelaunayTriangulation(QVector<MathAlgorithms::Vertex> aFixedPoints, QVector<MathAlgorithms::Vertex> aRelativePoints)
{
    QVector<MathAlgorithms::Triangle> trianglesForPoints;

    //add fixed points to relative points
    for(int i = 0; i < aFixedPoints.size(); i++)
    {
        aRelativePoints.append(aFixedPoints[i]);
    }

    //create all possible triangles for the given points
    for(int outerIndex = 0; outerIndex < aFixedPoints.size(); outerIndex++)
    {
        MathAlgorithms::Vertex& currentPoint = aFixedPoints[outerIndex];

        for(int index = 0; index < aRelativePoints.size(); index++)
        {
            MathAlgorithms::Vertex& currentPointPlus1 = aRelativePoints[index];

            if(!(currentPointPlus1.x == currentPoint.x && currentPointPlus1.y == currentPoint.y))
            {
                for(int nextIndex = index + 1; nextIndex < aRelativePoints.size(); nextIndex++)
                {
                    MathAlgorithms::Vertex& currentPointPlus2 = aRelativePoints[nextIndex];

                    if(!(currentPointPlus2.x == currentPoint.x && currentPointPlus2.y == currentPoint.y))
                    {
                        createCounterClockwiseTriangle(trianglesForPoints, currentPoint, currentPointPlus1, currentPointPlus2);
                    }
                }
            }
        }
    }

    //check for all triangles if any vertex that is not part of the triangle is inside that that triangle
    //if it is that triangle is not a delaunay triangle and has to be removed
    for(int pointIndex = 0; pointIndex < aRelativePoints.size(); pointIndex++)
    {
        MathAlgorithms::Vertex& currentPoint = aRelativePoints[pointIndex];

        for(int triangleIndex = 0; triangleIndex < trianglesForPoints.size(); triangleIndex++)
        {
            MathAlgorithms::Triangle& triangle = trianglesForPoints[triangleIndex];

            //point is not part of the triangle
            if(!(triangle.point1.x == currentPoint.x && triangle.point1.y == currentPoint.y && triangle.point1.z == currentPoint.z)
            && !(triangle.point2.x == currentPoint.x && triangle.point2.y == currentPoint.y && triangle.point2.z == currentPoint.z)
            && !(triangle.point3.x == currentPoint.x && triangle.point3.y == currentPoint.y && triangle.point3.z == currentPoint.z))
            {
                //determine if inside the circumcircle
                float circumMatrix [3][3];
                //column 1
                circumMatrix [0][0] = triangle.point1.x - currentPoint.x;
                circumMatrix [1][0] = triangle.point2.x - currentPoint.x;
                circumMatrix [2][0] = triangle.point3.x - currentPoint.x;
                //column 2
                circumMatrix [0][1] = triangle.point1.y - currentPoint.y;
                circumMatrix [1][1] = triangle.point2.y - currentPoint.y;
                circumMatrix [2][1] = triangle.point3.y - currentPoint.y;
                //column 3
                circumMatrix [0][2] = (pow(triangle.point1.x, 2) - pow(currentPoint.x, 2)) + (pow(triangle.point1.y, 2) - pow(currentPoint.y, 2));
                circumMatrix [1][2] = (pow(triangle.point2.x, 2) - pow(currentPoint.x, 2)) + (pow(triangle.point2.y, 2) - pow(currentPoint.y, 2));
                circumMatrix [2][2] = (pow(triangle.point3.x, 2) - pow(currentPoint.x, 2)) + (pow(triangle.point3.y, 2) - pow(currentPoint.y, 2));

                //calculate determine of matrix
                double determinantLeftPart = (circumMatrix[0][0] * circumMatrix[1][1] * circumMatrix[2][2]) + (circumMatrix[0][1] * circumMatrix[1][2] * circumMatrix[2][0]) + (circumMatrix[0][2] * circumMatrix[1][0] * circumMatrix[2][1]);
                double determinantRightPart = (circumMatrix[0][2] * circumMatrix[1][1] * circumMatrix[2][0]) + (circumMatrix[0][1] * circumMatrix[1][0] * circumMatrix[2][2]) + (circumMatrix[0][0] * circumMatrix[1][2] * circumMatrix[2][1]);
                double determinant = determinantLeftPart - determinantRightPart;
                if(determinant > 0) //only true if point is inside circumcircle of triangle
                {
                    //triangle is no good, need to remove
                    trianglesForPoints.removeAt(triangleIndex);
                    triangleIndex--; // since array shrinked by 1 otherwise we skip the next triangle in the list
                }
            }
        }
    }

    //there could be potential duplicate triangles
    for(int i = 0; i <  trianglesForPoints.size(); i++)
    {
        MathAlgorithms::Triangle& currentTriangle = trianglesForPoints[i];

        for(int ii = i + 1; ii <  trianglesForPoints.size(); ii++)
        {
            MathAlgorithms::Triangle& checkTriangle = trianglesForPoints[ii];

            if((checkTriangle.point1.x == currentTriangle.point1.x && checkTriangle.point1.y == currentTriangle.point1.y)
            || (checkTriangle.point2.x == currentTriangle.point1.x && checkTriangle.point2.y == currentTriangle.point1.y)
            || (checkTriangle.point3.x == currentTriangle.point1.x && checkTriangle.point3.y == currentTriangle.point1.y))
            {
                //point 1 is the same
                if((checkTriangle.point1.x == currentTriangle.point2.x && checkTriangle.point1.y == currentTriangle.point2.y)
                || (checkTriangle.point2.x == currentTriangle.point2.x && checkTriangle.point2.y == currentTriangle.point2.y)
                || (checkTriangle.point3.x == currentTriangle.point2.x && checkTriangle.point3.y == currentTriangle.point2.y))
                {
                    //point 2 is the same

                    if((checkTriangle.point1.x == currentTriangle.point3.x && checkTriangle.point1.y == currentTriangle.point3.y)
                    || (checkTriangle.point2.x == currentTriangle.point3.x && checkTriangle.point2.y == currentTriangle.point3.y)
                    || (checkTriangle.point3.x == currentTriangle.point3.x && checkTriangle.point3.y == currentTriangle.point3.y))
                    {
                       //point 3 is the same
                       //both are the same triangle, remove one
                       trianglesForPoints.remove(ii);
                       ii--; // since size decreased
                    }
                }
            }
        }
    }

    return trianglesForPoints;
}

void MathAlgorithms::createCounterClockwiseTriangle(QVector<MathAlgorithms::Triangle>& triangleStorage, MathAlgorithms::Vertex point1, MathAlgorithms::Vertex point2, MathAlgorithms::Vertex point3)
{
    MathAlgorithms::Triangle triangle;
    triangle.point1 = point1;

    MathAlgorithms::Vertex vectorA;
    vectorA.x = point2.x - point1.x;
    vectorA.y = point2.y - point1.y;
    vectorA.z = point2.z - point1.z;

    //normalize A
    float normOfA = std::sqrt(vectorA.x * vectorA.x + vectorA.y * vectorA.y);
    vectorA.x /= normOfA;
    vectorA.y /= normOfA;

    MathAlgorithms::Vertex vectorB;
    vectorB.x = point3.x - point1.x;
    vectorB.y = point3.y - point1.y;
    vectorB.z = point3.z - point1.z;

    //normalize B
    float normOfB = std::sqrt(vectorB.x * vectorB.x + vectorB.y * vectorB.y);
    vectorB.x /= normOfB;
    vectorB.y /= normOfB;

    //do cross product but only for the z component, since that is the only one we are interested in
    float signOfCrossProduct = (vectorA.x * vectorB.y) - (vectorA.y * vectorB.x);
    if(signOfCrossProduct < 0)
    {
        //need to go to B to make it counter-clockwise
        triangle.point2 = point3;
        triangle.point3 = point2;
    }
    else
    {
        //need to go to A to make it counter-clockwise
        triangle.point2 = point2;
        triangle.point3 = point3;
    }
    if(signOfCrossProduct != 0) //is  a valid triangle
    {
        triangleStorage.append(triangle);
    }
}

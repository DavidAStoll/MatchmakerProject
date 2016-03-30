#ifndef GLPROGRESSWIDGET_H
#define GLPROGRESSWIDGET_H

#include <QGLWidget>
#include <QVector>
#include "mathalgorithms.h"
#include "glmeshselectwidget.h"
#define GL_PROGRESSWIDGET_CONSTRAINT_SIZE 1

class glProgressWidget : public QGLWidget
{
public:

    struct ConstraintMatch
    {
        MathAlgorithms::Vertex vertexInMesh;
        MathAlgorithms::Vertex vertexInTexture;
    };

    struct edgeWalker
    {
        bool success;
        glMeshSelectWidget::Vertex* startVertex;
        glMeshSelectWidget::Vertex* targetVertex;
        QVector<glMeshSelectWidget::Edge*> edgesIWalked;
    };

    struct validateTriangulation
    {
        edgeWalker* edgeA;
        edgeWalker* edgeB;
        edgeWalker* edgeC;
    };

    struct constraintOrientation
    {
        MathAlgorithms::Vertex constraintVertex;
        float sign;
    };

    struct constraintOrientationSet
    {
        glMeshSelectWidget::Vertex* startConstraint;
        QVector<constraintOrientation> constraintOrientations;
    };

    glProgressWidget();
    ~glProgressWidget();
    void performMatch();
    //void performEmbed();

    void addConstraintMatchForMesh(MathAlgorithms::Vertex aVertex);
    void addConstraintMatchForTexture(MathAlgorithms::Vertex aVertex);

    QVector<validateTriangulation> GetValidTriangulations();

protected:

    //default widget gl functions
    void initializeGL(); //called once before drawing happens
    void paintGL(); //called during draw call back
    void resizeGL(int width, int height); //called when widget size changes
    void mousePressEvent(QMouseEvent *event); //when mouse is pressed inside the area
    void walkToVertex(edgeWalker* walker, constraintOrientationSet& constraintOrientations);
    edgeWalker* constraintEdgeDoesExist(glMeshSelectWidget::Vertex* vertexA, glMeshSelectWidget::Vertex* vertexB);
    constraintOrientationSet createConstraintOrientationSet(glMeshSelectWidget::Vertex* vertexA, glMeshSelectWidget::Vertex* vertexB);
    float crossProduct(MathAlgorithms::Vertex point1, glMeshSelectWidget::Vertex* point2, glMeshSelectWidget::Vertex* point3);

    QVector<ConstraintMatch> ConstraintMatches;
    QVector<glMeshSelectWidget::Vertex*>* meshVertices;
    QVector<glMeshSelectWidget::Edge*>* meshEdges;
    QVector<glMeshSelectWidget::Triangle*>* meshTriangles;
    QVector<edgeWalker*> constraintEdgesWithPoints;
    QVector<glMeshSelectWidget::Vertex*> doNotWalkVertices;
    QVector<validateTriangulation> validTriangulations;
};

#endif // GLPROGRESSWIDGET_H

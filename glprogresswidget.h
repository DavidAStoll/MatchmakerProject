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
        glMeshSelectWidget::vertex* startVertex;
        glMeshSelectWidget::vertex* targetVertex;
        QVector<glMeshSelectWidget::edge*> edgesIWalked;
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
        glMeshSelectWidget::vertex* startConstraint;
        QVector<constraintOrientation> constraintOrientations;
    };

    glProgressWidget();
    ~glProgressWidget();
    void performMatch();
    //void performEmbed();

    void addConstraintMatchAddVertexInMesh(MathAlgorithms::Vertex aVertex);
    void addConstraintMatchAddVertexInTexture(MathAlgorithms::Vertex aVertex);

    QVector<validateTriangulation> GetValidTriangulations();

protected:

    //default widget gl functions
    void initializeGL(); //called once before drawing happens
    void paintGL(); //called during draw call back
    void resizeGL(int width, int height); //called when widget size changes
    void mousePressEvent(QMouseEvent *event); //when mouse is pressed inside the area
    void walkToVertex(edgeWalker* walker, constraintOrientationSet& constraintOrientations);
    edgeWalker* constraintEdgeDoesExist(glMeshSelectWidget::vertex* vertexA, glMeshSelectWidget::vertex* vertexB);
    constraintOrientationSet createConstraintOrientationSet(glMeshSelectWidget::vertex* vertexA, glMeshSelectWidget::vertex* vertexB);
    float crossProduct(MathAlgorithms::Vertex point1, glMeshSelectWidget::vertex* point2, glMeshSelectWidget::vertex* point3);

    QVector<ConstraintMatch> ConstraintMatches;
    QVector<glMeshSelectWidget::vertex*>* meshVertices;
    QVector<glMeshSelectWidget::edge*>* meshEdges;
    QVector<glMeshSelectWidget::triangle*>* meshTriangles;
    QVector<edgeWalker*> constraintEdgesWithPoints;
    QVector<glMeshSelectWidget::vertex*> doNotWalkVertices;
    QVector<validateTriangulation> validTriangulations;
};

#endif // GLPROGRESSWIDGET_H

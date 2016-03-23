#ifndef GLFINALWIDGET_H
#define GLFINALWIDGET_H

#include <QGLWidget>
#include <QVector>
#include <QVector>
#include "mathalgorithms.h"
#include "glmeshselectwidget.h"
#include "glprogresswidget.h"
#include <GL/gl.h>
#include <GL/glu.h>

class glFinalWidget : public QGLWidget
{
public:
    glFinalWidget();
    ~glFinalWidget();

    void performEmbed();

protected:

    //default widget gl functions
    void initializeGL(); //called once before drawing happens
    void paintGL(); //called during draw call back
    void resizeGL(int width, int height); //called when widget size changes
    void mousePressEvent(QMouseEvent *event); //when mouse is pressed inside the area

private:

    void EmbedForPatches();
    void RepositionEdgePoints(glProgressWidget::edgeWalker* currentPath);
    void RepositionInteriorPoints();

    GLfloat Sign(const glMeshSelectWidget::vertex& p1, const glMeshSelectWidget::vertex& p2, const glMeshSelectWidget::vertex& p3);
    bool PointInTriangle(const glMeshSelectWidget::vertex& pt, const glMeshSelectWidget::vertex& v1, const glMeshSelectWidget::vertex& v2, const glMeshSelectWidget::vertex& v3);
    bool HasThePoint(const glMeshSelectWidget::vertex* v);

    QVector<glMeshSelectWidget::vertex*>* m_qVertices;
    QVector<glMeshSelectWidget::edge*>* m_qEdges;
    QVector<glMeshSelectWidget::triangle*>* m_qTriangles;

    QVector<glProgressWidget::validateTriangulation> m_validTriangulations;

    QVector<glMeshSelectWidget::vertex*> m_fixedPoints;
};

#endif // GLFINALWIDGET_H

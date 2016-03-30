#include "glfinalwidget.h"
#include "QMouseEvent"
#include "qimage.h"
#include "qbuffer.h"
#include "qbytearray.h"
#include "mainwindow.h"

glFinalWidget::glFinalWidget()
{
    m_qVertices = 0;
    m_qEdges = 0;
    m_qTriangles = 0;
}

glFinalWidget::~glFinalWidget()
{
    //nothing owned

}

void glFinalWidget::initializeGL()
{
    //setup OpenGl
    //glEnable(GL_TEXTURE_2D); //enable textures
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void glFinalWidget::paintGL()
{
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if (m_qTriangles)
    {
        // Draw mesh
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin( GL_TRIANGLES );
        for ( int i = 0; i < m_qTriangles->size(); ++i )
        {
            glMeshSelectWidget::Triangle* t = m_qTriangles->value(i);
            glMeshSelectWidget::Vertex a, b, c;
            a = *(t->vertexA);
            b = *(t->vertexB);
            c = *(t->vertexC);

            glVertex3f(a.vertexCor.x, a.vertexCor.y, a.vertexCor.z);
            glVertex3f(b.vertexCor.x, b.vertexCor.y, b.vertexCor.z);
            glVertex3f(c.vertexCor.x, c.vertexCor.y, c.vertexCor.z);
        }
        glEnd();

        // Draw path
        glColor3f(0.5f, 0.0f, 0.5f);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < m_validTriangulations.size(); ++i)
        {
            glProgressWidget::validateTriangulation currentTriangle = m_validTriangulations[i];

            glMeshSelectWidget::Vertex* vertexA;
            glMeshSelectWidget::Vertex* vertexB;
            glMeshSelectWidget::Vertex* vertexC;

            vertexA = currentTriangle.edgeA->startVertex;
            if (vertexA == currentTriangle.edgeB->startVertex)
            {
                vertexB = currentTriangle.edgeB->targetVertex;
            }
            else
            {
                vertexB = currentTriangle.edgeB->startVertex;
            }

            if (currentTriangle.edgeC->startVertex == vertexA || currentTriangle.edgeC->startVertex == vertexB)
            {
                vertexC = currentTriangle.edgeC->targetVertex;
            }
            else
            {
                vertexC = currentTriangle.edgeC->startVertex;
            }

            glVertex3f(vertexA->vertexCor.x, vertexA->vertexCor.y, vertexA->vertexCor.z);
            glVertex3f(vertexB->vertexCor.x, vertexB->vertexCor.y, vertexB->vertexCor.z);
            glVertex3f(vertexC->vertexCor.x, vertexC->vertexCor.y, vertexC->vertexCor.z);
        }
        glEnd();

        // draw border points
        /*glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        for(int i = 0; i < m_fixedPoints.size(); ++i)
        {
            glBegin(GL_TRIANGLE_STRIP);
            glMeshSelectWidget::vertex* v = m_fixedPoints[i];
            glVertex3f(v->x + 1, v->y + 1, v->z);
            glVertex3f(v->x + 1, v->y - 1, v->z);
            glVertex3f(v->x - 1, v->y - 1, v->z);
            glVertex3f(v->x - 1, v->y + 1, v->z);
            glEnd();
        }*/
    }
}

void glFinalWidget::resizeGL(int width, int height)
{
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-75, 75, 0, 150, -90, 160);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void glFinalWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
    //redraw glWidget
    updateGL();
}

void glFinalWidget::performEmbed()
{
    m_qVertices = MainWindow::globalInstance->glMeshWidget->GetVertices();
    m_qEdges = MainWindow::globalInstance->glMeshWidget->GetEdges();
    m_qTriangles = MainWindow::globalInstance->glMeshWidget->GetTriangles();
    m_validTriangulations = MainWindow::globalInstance->progressWidget->GetValidTriangulations();

    // Find fixed points that should not be repositioned while embeddings
    for (int i = 0; i < m_validTriangulations.size(); ++i)
    {
        glProgressWidget::validateTriangulation currentTriangle = m_validTriangulations[i];

        glMeshSelectWidget::Vertex* vertexA;
        glMeshSelectWidget::Vertex* vertexB;
        glMeshSelectWidget::Vertex* vertexC;

        vertexA = currentTriangle.edgeA->startVertex;
        if (vertexA == currentTriangle.edgeB->startVertex)
        {
            vertexB = currentTriangle.edgeB->targetVertex;
        }
        else
        {
            vertexB = currentTriangle.edgeB->startVertex;
        }

        if (currentTriangle.edgeC->startVertex == vertexA || currentTriangle.edgeC->startVertex == vertexB)
        {
            vertexC = currentTriangle.edgeC->targetVertex;
        }
        else
        {
            vertexC = currentTriangle.edgeC->startVertex;
        }

        if (!HasThePoint(vertexA))
        {
            m_fixedPoints.push_back(vertexA);
        }
        if (!HasThePoint(vertexB))
        {
            m_fixedPoints.push_back(vertexB);
        }
        if (!HasThePoint(vertexC))
        {
            m_fixedPoints.push_back(vertexC);
        }
    }

    EmbedForPatches();

    for (int i = 0; i < 10; ++i)
    {
        RepositionInteriorPoints();
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-75, 75, 0, 150, -90, 160);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    updateGL();
}

bool glFinalWidget::HasThePoint(const glMeshSelectWidget::Vertex *v)
{
    bool hasThePoint = false;
    for(int i = 0; i < m_fixedPoints.size(); ++i)
    {
        if (v == m_fixedPoints[i])
        {
            hasThePoint = true;
            break;
        }
    }
    return hasThePoint;
}

void glFinalWidget::EmbedForPatches()
{
    for (int i = 0; i < m_validTriangulations.size(); ++i)
    {
        glProgressWidget::validateTriangulation currentTriangle = m_validTriangulations[i];

        glProgressWidget::edgeWalker* currentPath = currentTriangle.edgeA;
        RepositionEdgePoints(currentPath);

        currentPath = currentTriangle.edgeB;
        RepositionEdgePoints(currentPath);

        currentPath = currentTriangle.edgeC;
        RepositionEdgePoints(currentPath);
    }
}

void glFinalWidget::RepositionEdgePoints(glProgressWidget::edgeWalker* currentPath)
{
    int numOfVerticesAlongPath = currentPath->edgesIWalked.size() - 1;
    if (numOfVerticesAlongPath <= 0)
    {
        return;
    }

    glMeshSelectWidget::Vertex* start = currentPath->startVertex;
    glMeshSelectWidget::Vertex* target = currentPath->targetVertex;

    GLfloat deltaX = (target->vertexCor.x - start->vertexCor.x)/(currentPath->edgesIWalked.size());
    GLfloat deltaY = (target->vertexCor.y - start->vertexCor.y)/(currentPath->edgesIWalked.size());

    glMeshSelectWidget::Vertex* startingPoint = start;
    for(int j = 0; j < numOfVerticesAlongPath; ++j)
    {
        glMeshSelectWidget::Edge* currentEdge = currentPath->edgesIWalked[j];
        glMeshSelectWidget::Vertex* vertexToReposition;
        if (currentEdge->vertexA == startingPoint)
        {
            vertexToReposition = currentEdge->vertexB;
        }
        else
        {
            vertexToReposition = currentEdge->vertexA;
        }
        vertexToReposition->vertexCor.x = start->vertexCor.x + deltaX * (j + 1);
        vertexToReposition->vertexCor.y = start->vertexCor.y + deltaY * (j + 1);

        startingPoint = vertexToReposition;

        if (!HasThePoint(vertexToReposition))
        {
            m_fixedPoints.push_back(vertexToReposition);
        }
    }
}

GLfloat glFinalWidget::Sign(const glMeshSelectWidget::Vertex& p1, const glMeshSelectWidget::Vertex& p2, const glMeshSelectWidget::Vertex& p3)
{
    return (p1.vertexCor.x - p3.vertexCor.x) * (p2.vertexCor.y - p3.vertexCor.y) - (p2.vertexCor.x - p3.vertexCor.x) * (p1.vertexCor.y - p3.vertexCor.y);
}

bool glFinalWidget::PointInTriangle(const glMeshSelectWidget::Vertex& pt, const glMeshSelectWidget::Vertex& v1, const glMeshSelectWidget::Vertex& v2, const glMeshSelectWidget::Vertex& v3)
{
  bool b1, b2, b3;

  b1 = Sign(pt, v1, v2) < 0.0f;
  b2 = Sign(pt, v2, v3) < 0.0f;
  b3 = Sign(pt, v3, v1) < 0.0f;

  return ((b1 == b2) && (b2 == b3));
}

void glFinalWidget::RepositionInteriorPoints()
{
    for (int vIndex = 0; vIndex < m_qVertices->size(); ++vIndex)
    {
        glMeshSelectWidget::Vertex* currentVertex = m_qVertices->value(vIndex);

        if (HasThePoint(currentVertex))
        {
            continue;
        }

        bool pointInPatch = false;
        for (int pIndex = 0; pIndex < m_validTriangulations.size(); ++pIndex)
        {
            glProgressWidget::validateTriangulation currentTriangle = m_validTriangulations[pIndex];

            glMeshSelectWidget::Vertex* vertexA;
            glMeshSelectWidget::Vertex* vertexB;
            glMeshSelectWidget::Vertex* vertexC;

            vertexA = currentTriangle.edgeA->startVertex;
            if (vertexA == currentTriangle.edgeB->startVertex)
            {
                vertexB = currentTriangle.edgeB->targetVertex;
            }
            else
            {
                vertexB = currentTriangle.edgeB->startVertex;
            }

            if (currentTriangle.edgeC->startVertex == vertexA || currentTriangle.edgeC->startVertex == vertexB)
            {
                vertexC = currentTriangle.edgeC->targetVertex;
            }
            else
            {
                vertexC = currentTriangle.edgeC->startVertex;
            }

            pointInPatch = PointInTriangle(*currentVertex, *vertexA, *vertexB, *vertexC);

            // if patch is found
            if (pointInPatch)
            {
                // Find all neighbors
                QVector<glMeshSelectWidget::Vertex*> neighbors;
                for (int eIndex = 0; eIndex < currentVertex->edgeIndicies.size(); ++eIndex)
                {
                    int edgeIndex = currentVertex->edgeIndicies[eIndex];
                    if (m_qEdges->value(edgeIndex)->vertexA == currentVertex)
                    {
                        neighbors.push_back(m_qEdges->value(edgeIndex)->vertexB);
                    }
                    else if (m_qEdges->value(edgeIndex)->vertexB == currentVertex)
                    {
                        neighbors.push_back(m_qEdges->value(edgeIndex)->vertexA);
                    }
                }

                // re-compute the position
                glMeshSelectWidget::Vertex sum;
                sum.vertexCor.x = 0.0;
                sum.vertexCor.y = 0.0;
                sum.vertexCor.z = 0.0;

                for (int nIndex = 0; nIndex < neighbors.size(); ++nIndex)
                {
                    sum.vertexCor.x += neighbors[nIndex]->vertexCor.x;
                    sum.vertexCor.y += neighbors[nIndex]->vertexCor.y;
                    sum.vertexCor.z += neighbors[nIndex]->vertexCor.z;
                }

                int num = neighbors.size();
                if (num > 0)
                {
                    sum.vertexCor.x = sum.vertexCor.x/(GLfloat)num;
                    sum.vertexCor.y = sum.vertexCor.y/(GLfloat)num;
                    sum.vertexCor.z = sum.vertexCor.z/(GLfloat)num;

                    // re-assign
                    currentVertex->vertexCor.x = sum.vertexCor.x;
                    currentVertex->vertexCor.y = sum.vertexCor.y;
                    currentVertex->vertexCor.z = sum.vertexCor.z;
                }
                break;
            }
        }
    }
}

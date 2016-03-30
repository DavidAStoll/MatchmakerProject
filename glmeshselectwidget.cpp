#include "glmeshselectwidget.h"

#include "mathalgorithms.h"

#include <cmath>
#include <QFileDialog>
#include <QMouseEvent>
#include "mainwindow.h"

glMeshSelectWidget::glMeshSelectWidget()
    : m_meshLoaded( false )
{
    enableSetConstraint = true;
}

glMeshSelectWidget::~glMeshSelectWidget()
{
    //nothing owned
}

void glMeshSelectWidget::initializeGL()
{
    //setup OpenGl
    glEnableClientState(GL_VERTEX_ARRAY);
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_NORMALIZE );
}

void glMeshSelectWidget::paintGL()
{
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Draw meshes
    glColor3f(0.0f, 1.0f, 0.0f);
    DrawObject();

    // Draw border points
    if ( m_meshLoaded )
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glColor3f(0.5f, 0.0f, 0.5f);

        for(int i = 0; i < m_borderPoints.size(); i++)
        {
            glVertexPointer(3, GL_FLOAT, 0,  &m_borderPoints[i].rightBottom.x);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        for(int i = 0; i < m_userConstraints.size(); i++)
        {
            glVertexPointer(3, GL_FLOAT, 0,  &m_userConstraints[i].rightBottom.x);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
}

void glMeshSelectWidget::resizeGL(int width, int height)
{
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_widgetWidth = width;
    m_widgetHeight = height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if ( m_meshLoaded )
    {
        glOrtho(-75, 75, 0, 150, -90, 160);
    }
    else
    {
        glOrtho(-125, 125, -50, 200, -90, 160);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    updateGL();
}

void glMeshSelectWidget::mousePressEvent(QMouseEvent *event)
{   
    if(m_meshLoaded && enableSetConstraint)
    {
        //coordinates in window coordinates
        float windowX = event->x();
        float windowY = event->y();

        //convert to OpenGL ones
        int glXLocation = ((float) windowX / m_widgetWidth) * GL_MESHWIDGET_CANVAS_WIDTH + X_OFFSET;
        int glYLocation = ((float)(m_widgetHeight - windowY) / m_widgetHeight) * GL_MESHWIDGET_CANVAS_HEIGHT + Y_OFFSET;

        //find the closest vertices to this event
        std::vector<GLfloat> closestVertex = GetClosestVertex(glXLocation, glYLocation);

        //add visual element for this constraint point to mesh display
        GLfloat closestXLocation = closestVertex[0];
        GLfloat closestYLocation = closestVertex[1];
        m_userConstraints.push_back(constraintPoint(closestXLocation, closestYLocation));

        //add selected vertex to match list
        MathAlgorithms::Vertex constraintVertex;
        constraintVertex.x = closestVertex[0];
        constraintVertex.y = closestVertex[1];
        constraintVertex.z = closestVertex[2];
        MainWindow::globalInstance->progressWidget->addConstraintMatchForMesh(constraintVertex);
    }

    //redraw glWidget
    this->makeCurrent();
    updateGL();
}

void glMeshSelectWidget::loadMeshFileCallback(QTextStream* fileStream)
{
    m_vertices.clear();
    m_faces.clear();

    QString v = "v";
    QString f = "f";
    QString vt = "vt";

    while(!fileStream->atEnd())
    {
        QString currentLine = fileStream->readLine();
        if (currentLine.size() < 2)
        {
            continue;
        }

        QStringList values = currentLine.split(" ");
        std::vector<GLfloat> vertex(3);
        std::vector<unsigned int> face(3);
        std::vector<GLfloat> vTexture(3);
        std::vector<unsigned int> fTexture(3);

        QString type = values[0];
        if ( type.compare(v) == 0 )
        {
            for ( unsigned int i = 0; i < 3; ++i )
            {
                vertex[i] = values[i + 2].toFloat();
            }
            m_vertices.push_back( vertex );
        }
        else if ( type.compare(f) == 0 )
        {
            for ( unsigned int i = 0; i < 3; ++i )
            {
                face[i] = (values[i + 1].split("/"))[0].toUInt();
                fTexture[i] = (values[i + 1].split("/"))[1].toUInt();
            }
            m_faces.push_back( face );
            m_fTexture.push_back( fTexture );
        }
        else if ( type.compare(vt) == 0 )
        {
            vTexture[0] = (values[1].toFloat() * 250) - 125;
            vTexture[1] = (values[2].toFloat() * 250) - 50;
            vTexture[2] = values[3].toFloat();
            m_vTexture.push_back( vTexture );
        }
    }

    m_originVertices = m_vertices;
    m_originFaces = m_faces;

    updateGL();
}

void glMeshSelectWidget::DrawObject()
{
    // Draw object
    glBegin( GL_TRIANGLES );
    for ( unsigned int i = 0; i < m_faces.size(); ++i )
    {
      //unsigned int faceIndex = i;
        for ( unsigned int j = 0; j < m_faces[i].size(); ++j )
        {
            unsigned int vertexIndex = m_faces[i][j];

            std::vector<GLfloat> currentVertex = m_vertices[ vertexIndex - 1 ];
            glVertex3f( currentVertex[0], currentVertex[1], currentVertex[2] );
        }
    }
    glEnd();
}

void glMeshSelectWidget::CreateBorder()
{
    //Setup border constraints
    //Bottom side
    m_borderPoints.push_back(CreateContraintPoint(0, m_widgetHeight));
    m_borderPoints.push_back(CreateContraintPoint(m_widgetWidth / 3, m_widgetHeight));
    m_borderPoints.push_back(CreateContraintPoint((2 * m_widgetWidth) / 3, m_widgetHeight));
    m_borderPoints.push_back(CreateContraintPoint(m_widgetWidth, m_widgetHeight));
    //Top side
    m_borderPoints.push_back(CreateContraintPoint(0, 0));
    m_borderPoints.push_back(CreateContraintPoint(m_widgetWidth / 3, 0));
    m_borderPoints.push_back(CreateContraintPoint((2 * m_widgetWidth) / 3, 0));
    m_borderPoints.push_back(CreateContraintPoint(m_widgetWidth, 0));
    //Left side
    m_borderPoints.push_back(CreateContraintPoint(0, (2 * m_widgetHeight) / 3));
    m_borderPoints.push_back(CreateContraintPoint(0, m_widgetHeight / 3));
    //Right side
    m_borderPoints.push_back(CreateContraintPoint(m_widgetWidth, (2 * m_widgetHeight) / 3));
    m_borderPoints.push_back(CreateContraintPoint(m_widgetWidth, m_widgetHeight / 3));
}

glMeshSelectWidget::constraintPoint glMeshSelectWidget::CreateContraintPoint(int x, int y)
{
    int glXLocation = ((float)x / m_widgetWidth) * GL_MESHWIDGET_CANVAS_WIDTH + X_OFFSET;
    int glYLocation = ((float)(m_widgetHeight - y) / m_widgetHeight) * GL_MESHWIDGET_CANVAS_HEIGHT + Y_OFFSET;

    return constraintPoint(glXLocation, glYLocation);
}

void glMeshSelectWidget::AddEdgesAndTriangles()
{
    for ( unsigned int fIndex = 0; fIndex < m_faces.size(); ++fIndex )
    {
        std::vector<unsigned int> currentFace = m_faces[fIndex];

        unsigned int v1 = currentFace[0] - 1;
        unsigned int v2 = currentFace[1] - 1;
        unsigned int v3 = currentFace[2] - 1;

        Triangle* t = new Triangle;
        t->vertexA = m_qVertices[v1];
        t->vertexB = m_qVertices[v2];
        t->vertexC = m_qVertices[v3];

        Edge* e1 = new Edge;
        e1->vertexA = m_qVertices[v1];
        e1->vertexB = m_qVertices[v2];

        int index = FindEdgeIndex(*e1);
        if ( index == -1)
        {
            m_qEdges.push_back(e1);
            index = m_qEdges.size() - 1;
            //e1->triangleIndicies = new QVector<int>;

            //is new edge, need to add it to the verticies as well
            t->vertexA->edgeIndicies.append(index);
            t->vertexB->edgeIndicies.append(index);
        }
        t->edgeA = m_qEdges[index];

        Edge* e2 = new Edge;
        e2->vertexA = m_qVertices[v2];
        e2->vertexB = m_qVertices[v3];

        index = FindEdgeIndex(*e2);
        if ( index == -1)
        {
            m_qEdges.push_back(e2);
            index = m_qEdges.size() - 1;
            //e2->triangleIndicies = new QVector<int>;

            //is new edge, need to add it to the verticies as well
            t->vertexB->edgeIndicies.append(index);
            t->vertexC->edgeIndicies.append(index);
        }
        t->edgeB = m_qEdges[index];

        Edge* e3 = new Edge;
        e3->vertexA = m_qVertices[v3];
        e3->vertexB = m_qVertices[v1];

        index = FindEdgeIndex(*e3);
        if ( index == -1)
        {
            m_qEdges.push_back(e3);
            index = m_qEdges.size() - 1;
            //e3->triangleIndicies = new QVector<int>;

            //is new edge, need to add it to the verticies as well
            t->vertexA->edgeIndicies.append(index);
            t->vertexC->edgeIndicies.append(index);
        }
        t->edgeC = m_qEdges[index];

        m_qTriangles.push_back(t);

        //update edges to point to this triangle
        t->edgeA->triangleIndicies.append(m_qTriangles.size() - 1);
        t->edgeB->triangleIndicies.append(m_qTriangles.size() - 1);
        t->edgeC->triangleIndicies.append(m_qTriangles.size() - 1);

        //need to update vertices to point to triangle
        t->vertexA->triangleIndicies.append(m_qTriangles.size() - 1);
        t->vertexB->triangleIndicies.append(m_qTriangles.size() - 1);
        t->vertexC->triangleIndicies.append(m_qTriangles.size() - 1);
    }

    // debug
    printf("v: %d, f: %d, e: %d\n", (int)m_qVertices.size(), (int)m_qTriangles.size(), (int)m_qEdges.size() );
}

void glMeshSelectWidget::parameterizeMesh()
{
    m_meshLoaded = true;

    // init
    m_actualVTexture = m_vTexture;
    m_actualFaces = m_originFaces;
    m_actualFTexture = m_fTexture;

    std::set<unsigned int> edgePoints;
    RemoveFacesOutsideBoundary( edgePoints );

    // projection (2D) points
    m_vertices = m_vTexture;
    m_faces = m_actualFTexture;

    //add boundary mesh as well
    AddVirtualBoundary( edgePoints );

    MakeNewStructure();

    // 3D points
    //m_vertices = m_originVertices;
    //m_faces = m_actualFaces;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-125, 125, -50, 200, -90, 160);
    glOrtho(-75, 75, 0, 150, -90, 160);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    updateGL();
}

std::vector<GLfloat> glMeshSelectWidget::GetClosestVertex( GLfloat x, GLfloat y )
{
    std::vector<GLfloat> closestVertex;
    GLfloat minDistance = 150;

    for( unsigned int i = 0; i < m_vertices.size(); ++i )
    {
        std::vector<GLfloat> currentVertex = m_vertices[i];
        if ( (currentVertex[0] < -70 || currentVertex[0] > 70) || (currentVertex[1] < 5 || currentVertex[1] > 145) )
        {
            continue;
        }

        GLfloat distance = sqrt( pow( currentVertex[0] - x, 2 ) + pow( currentVertex[1] - y ,2 ) );
        if ( distance < minDistance )
        {
            minDistance = distance;
            closestVertex = currentVertex;
        }
    }
    return closestVertex;
}

void glMeshSelectWidget::RemoveFacesOutsideBoundary(std::set<unsigned int>& edgePoints )
{
    for ( unsigned int i = 0; i < m_vTexture.size(); ++i )
    {
        std::vector<GLfloat> currentVertex = m_vTexture[i];
        // if a vertex is outside boundary
        if ( (currentVertex[0] < -70 || currentVertex[0] > 70) || (currentVertex[1] < 5 || currentVertex[1] > 145) )
        {
            // remove the vertex if it's in the neighbor set
            std::set<unsigned int>::iterator itr = edgePoints.find( i + 1 );
            if ( itr != edgePoints.end() )
            {
                edgePoints.erase( itr );
            }

            // find faces having this vertex
            for ( int j = 0; j < m_actualFTexture.size(); ++j )
            {
                std::vector<unsigned int> currentFTexture = m_actualFTexture[j];
                if ( currentFTexture[0] - 1 == i || currentFTexture[1] - 1 == i || currentFTexture[2] - 1 == i )
                {
                    // collect neighbors of the removed vertex
                    for ( unsigned int k = 0; k < 3; ++k )
                    {
                        if ( currentFTexture[k] - 1 == i )
                        {
                            edgePoints.insert( currentFTexture[(k+1)%3] );
                            edgePoints.insert( currentFTexture[(k+2)%3] );
                        }
                    }

                    // remove the face
                    m_actualFTexture.erase( m_actualFTexture.begin() + j );
                    m_actualFaces.erase( m_actualFaces.begin() + j );
                    j--;
                }
            }
        }
    }
}

void glMeshSelectWidget::AddVirtualBoundary( const std::set<unsigned int>& edgePoints )
{
    CreateBorder();

    QVector<MathAlgorithms::Vertex> fixedPoints;
    QVector<MathAlgorithms::Vertex> relativePoints;

    // for each border point
    for ( int i = 0; i < m_borderPoints.size(); ++i )
    {
        // add new boundary points to the set
        std::vector<GLfloat> newPoint(3);
        newPoint[0] = m_borderPoints[i].leftBottom.x + GL_MESHWIDGET_CONSTRAINT_SIZE;
        newPoint[1] = m_borderPoints[i].leftBottom.y + GL_MESHWIDGET_CONSTRAINT_SIZE;
        newPoint[2] = 0.0;
        m_actualVTexture.push_back(newPoint);

        MathAlgorithms::Vertex vertex;
        vertex.x = newPoint[0];
        vertex.y = newPoint[1];
        vertex.z = newPoint[2];
        fixedPoints.push_back(vertex);
    }

    for ( std::set<unsigned int>::iterator itr = edgePoints.begin(); itr != edgePoints.end(); ++itr )
    {
        unsigned int currentIndex = *itr - 1;

        MathAlgorithms::Vertex vertex;
        vertex.x = m_vTexture[currentIndex][0];
        vertex.y = m_vTexture[currentIndex][1];
        vertex.z = m_vTexture[currentIndex][2];
        relativePoints.push_back(vertex);
    }

    // get Delaunay triangles
    QVector<MathAlgorithms::Triangle> triangles = MathAlgorithms::getDelaunayTriangulation(fixedPoints, relativePoints);
    //printf("numOfTriangles: %d\n", (int)triangles.size());

    //add border to faces and vertices
    for(int i = 0; i < triangles.size(); i++)
    {
        int point1Index = -1;
        int point2Index = -1;
        int point3Index = -1;

        //check if a vertex is already in this list, don't want to introduce duplicates
        for(int ii = 0; ii < m_vertices.size(); ii++)
        {
            if(m_vertices[ii][0] == triangles[i].point1.x
            && m_vertices[ii][1] == triangles[i].point1.y
            && m_vertices[ii][2] == triangles[i].point1.z)
            {
               point1Index = ii + 1;
            }

            if(m_vertices[ii][0] == triangles[i].point2.x
            && m_vertices[ii][1] == triangles[i].point2.y
            && m_vertices[ii][2] == triangles[i].point2.z)
            {
               point2Index = ii + 1;
            }

            if(m_vertices[ii][0] == triangles[i].point3.x
            && m_vertices[ii][1] == triangles[i].point3.y
            && m_vertices[ii][2] == triangles[i].point3.z)
            {
               point3Index = ii + 1;
            }
        }

        //add vertices
        if(point1Index == -1)
        {
            std::vector<GLfloat> lVertex1;
            lVertex1.push_back(triangles[i].point1.x);
            lVertex1.push_back(triangles[i].point1.y);
            lVertex1.push_back(triangles[i].point1.z);
            m_vertices.push_back(lVertex1);
            point1Index = m_vertices.size();
        }

        if(point2Index == -1)
        {
            std::vector<GLfloat> lVertex2;
            lVertex2.push_back(triangles[i].point2.x);
            lVertex2.push_back(triangles[i].point2.y);
            lVertex2.push_back(triangles[i].point2.z);
            m_vertices.push_back(lVertex2);
            point2Index = m_vertices.size();
        }

        if(point3Index == -1)
        {
            std::vector<GLfloat> lVertex3;
            lVertex3.push_back(triangles[i].point3.x);
            lVertex3.push_back(triangles[i].point3.y);
            lVertex3.push_back(triangles[i].point3.z);
            m_vertices.push_back(lVertex3);
            point3Index = m_vertices.size();
        }

        //add to face
        std::vector<unsigned int> newFace;
        newFace.push_back(point1Index);
        newFace.push_back(point2Index);
        newFace.push_back(point3Index);
        m_faces.push_back(newFace);
    }
}

int glMeshSelectWidget::FindEdgeIndex(const Edge &e)
{
    int index = -1;
    for (unsigned int i = 0; i < m_qEdges.size(); ++i)
    {
        Edge& currentEdge = *m_qEdges[i];
        if ((currentEdge.vertexA == e.vertexA && currentEdge.vertexB == e.vertexB) || (currentEdge.vertexA == e.vertexB && currentEdge.vertexB == e.vertexA))
        {
            index = i;
            break;
        }
    }
    return index;
}

void glMeshSelectWidget::MakeNewStructure()
{
    for (unsigned int i = 0; i < m_vertices.size(); ++i)
    {
        Vertex* v = new Vertex;
        v->vertexCor.x = m_vertices[i][0];
        v->vertexCor.y = m_vertices[i][1];
        v->vertexCor.z = m_vertices[i][2];
        //v->edgeIndicies = new QVector<int>;
       // v->triangleIndicies = new QVector<int>;
        m_qVertices.push_back(v);
    }

    AddEdgesAndTriangles();
}

QVector<glMeshSelectWidget::Vertex*>* glMeshSelectWidget::GetVertices()
{
    return &m_qVertices;
}

QVector<glMeshSelectWidget::Edge*>* glMeshSelectWidget::GetEdges()
{
    return &m_qEdges;
}

QVector<glMeshSelectWidget::Triangle*>* glMeshSelectWidget::GetTriangles()
{
    return &m_qTriangles;
}

void glMeshSelectWidget::SetEnableConstraintSelection(bool aValue)
{
   enableSetConstraint = aValue;
}

QVector<MathAlgorithms::Vertex> glMeshSelectWidget::createBorderConstraints()
{
    QVector<MathAlgorithms::Vertex> fixedPoints;

    for ( int i = 0; i < m_borderPoints.size(); ++i )
    {
        // add new boundary points to the set
        std::vector<GLfloat> newPoint(3);
        newPoint[0] = m_borderPoints[i].leftBottom.x + GL_MESHWIDGET_CONSTRAINT_SIZE;
        newPoint[1] = m_borderPoints[i].leftBottom.y + GL_MESHWIDGET_CONSTRAINT_SIZE;
        newPoint[2] = 0.0;

        MathAlgorithms::Vertex vertex;
        vertex.x = newPoint[0];
        vertex.y = newPoint[1];
        vertex.z = newPoint[2];
        fixedPoints.push_back(vertex);
    }

    return fixedPoints;
}

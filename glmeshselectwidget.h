#ifndef GLMESHSELECTWIDGET_H
#define GLMESHSELECTWIDGET_H

#include <QGLWidget>
#include <QTextStream>
#include <QVector>

#include <map>
#include <set>
#include <vector>
#include "mathalgorithms.h"

#define GL_MESHWIDGET_CANVAS_WIDTH 150
#define GL_MESHWIDGET_CANVAS_HEIGHT 150
#define X_OFFSET -75
#define Y_OFFSET 0
#define GL_MESHWIDGET_CONSTRAINT_SIZE 1

class glMeshSelectWidget: public QGLWidget
{

public:

public:
    struct Vertex
    {
        MathAlgorithms::Vertex vertexCor;
        QVector<int> edgeIndicies;
        QVector<int> triangleIndicies;
    };

    struct Edge
    {
        Vertex* vertexA;
        Vertex* vertexB;
        QVector<int> triangleIndicies;
    };

    struct Triangle
    {
        Vertex* vertexA;
        Vertex* vertexB;
        Vertex* vertexC;
        Edge* edgeA;
        Edge* edgeB;
        Edge* edgeC;
    };



public:
    glMeshSelectWidget();
    ~glMeshSelectWidget();

    //is called when a the user wants to load a different mesh file
    void loadMeshFileCallback(QTextStream* fileStream);

    // called when the user wants to parameterize mesh
    void parameterizeMesh();
    void SetEnableConstraintSelection(bool aValue);
    QVector<MathAlgorithms::Vertex> createBorderConstraints();

    QVector<Vertex*>* GetVertices();
    QVector<Edge*>* GetEdges();
    QVector<Triangle*>* GetTriangles();

protected:
    void initializeGL(); //called once before drawing happens
    void paintGL(); //called during draw call back
    void resizeGL(int width, int height); //called when widget size changes
    void mousePressEvent(QMouseEvent *event); //when mouse is pressed inside the area

private:

    struct constraintVertex
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

    struct constraintPoint
    {
        GLfloat pixelXLocation;
        GLfloat pixelYLocation;

        //for displaying
        constraintVertex rightBottom;
        constraintVertex rightTop;
        constraintVertex leftBottom;
        constraintVertex leftTop;

        constraintPoint()
        {
        }

        constraintPoint(GLfloat xLocation, GLfloat yLocation)
        {
            pixelXLocation = xLocation;
            pixelYLocation = yLocation;

            //Left Bottom
            leftBottom.x = xLocation - GL_MESHWIDGET_CONSTRAINT_SIZE;
            leftBottom.y = yLocation - GL_MESHWIDGET_CONSTRAINT_SIZE;
            leftBottom.z = 0;
            //Left Top
            leftTop.x = xLocation - GL_MESHWIDGET_CONSTRAINT_SIZE;
            leftTop.y = yLocation + GL_MESHWIDGET_CONSTRAINT_SIZE;
            leftTop.z = 0;
            //Right Bottom
            rightBottom.x = xLocation + GL_MESHWIDGET_CONSTRAINT_SIZE;
            rightBottom.y = yLocation - GL_MESHWIDGET_CONSTRAINT_SIZE;
            rightBottom.z = 0;
            //Right Top
            rightTop.x = xLocation + GL_MESHWIDGET_CONSTRAINT_SIZE;
            rightTop.y = yLocation + GL_MESHWIDGET_CONSTRAINT_SIZE;
            rightTop.z = 0;
        }
    };

    void DrawObject();
    void AddEdgesAndTriangles();
    int FindEdgeIndex(const Edge& e);

    void RemoveFacesOutsideBoundary( std::set<unsigned int>& edgePoints );
    void AddVirtualBoundary( const std::set<unsigned int>& edgePoints );
    void MakeNewStructure();

    void CreateBorder();
    constraintPoint CreateContraintPoint(int x, int y);
    std::vector<GLfloat> GetClosestVertex( GLfloat x, GLfloat y );

    QVector<Vertex*> m_qVertices;
    QVector<Edge*> m_qEdges;
    QVector<Triangle*> m_qTriangles;

    std::vector< std::vector< GLfloat > > m_vertices;
    std::vector< std::vector< unsigned int > > m_faces;

    // origin mesh vertices and faces which are loaded from .obj file
    std::vector< std::vector< GLfloat > > m_originVertices;
    std::vector< std::vector< unsigned int > > m_originFaces;

    // origin texture vertices and faces
    std::vector< std::vector< GLfloat > > m_vTexture;
    std::vector< std::vector< unsigned int > > m_fTexture;

    // actual vertices and faces after cutting and adding boundary
    std::vector< std::vector< unsigned int > > m_actualFaces;   // orgin faces - faces cut
    std::vector< std::vector< GLfloat > > m_actualVTexture;     // orgin texture vertices + additional boundary points
    std::vector< std::vector< unsigned int > > m_actualFTexture;    // origin texture faces - texture faces cut

    //std::set< std::pair<unsigned int, unsigned int> > m_edges;

    int m_widgetWidth;
    int m_widgetHeight;

    QVector<constraintPoint> m_borderPoints;
    QVector<constraintPoint> m_userConstraints;
    std::vector< std::vector< GLfloat > > m_constraints;

    bool m_meshLoaded;
    bool enableSetConstraint;
};

#endif // GLMESHSELECTWIDGET_H

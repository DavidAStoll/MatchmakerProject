#ifndef GLTEXTURESELECTWIDGET_H
#define GLTEXTURESELECTWIDGET_H

#include <QGLWidget>
#include <QVector>
#include "mathalgorithms.h"
#include "glmeshselectwidget.h"

#define GL_TEXTUREWIDGET_CANVAS_WIDTH 100
#define GL_TEXTUREWIDGET_CANVAS_HEIGHT 100
#define GL_TEXTUREWIDGET_CONSTRAINT_SIZE 1

//This widget just displays one texture mapped over the entire area of the widget
//The user can then select certain constraint points on this texutre

class glTextureSelectWidget : public QGLWidget
{
    struct vertexCor
    {
        float x;
        float y;
        float z;
    };

    struct vertexTex
    {
        float s;
        float t;
    };

    struct constraintPoint
    {
        int pixelXLocation;
        int pixelYLocation;
        vertexCor rightBottom;
        vertexCor rightTop;
        vertexCor leftBottom;
        vertexCor leftTop;

        constraintPoint()
        {

        }

        constraintPoint(GLfloat xLocation, GLfloat yLocation)
        {
            pixelXLocation = xLocation;
            pixelYLocation = yLocation;
            //Left Bottom
            leftBottom.x = pixelXLocation - GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
            leftBottom.y = pixelYLocation - GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
            leftBottom.z = 0;
            //Left Top
            leftTop.x = pixelXLocation - GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
            leftTop.y = pixelYLocation + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
            leftTop.z = 0;
            //Right Bottom
            rightBottom.x = pixelXLocation + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
            rightBottom.y = pixelYLocation - GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
            rightBottom.z = 0;
            //Right Top
            rightTop.x = pixelXLocation + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
            rightTop.y = pixelYLocation + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
            rightTop.z = 0;
        }
    };

    struct rectangle //since we want to span the entire area we need a rectangle
    {
        vertexCor vertexACor;
        vertexCor vertexBCor;
        vertexCor vertexCCor;
        vertexCor vertexDCor;

        vertexTex vertexATex;
        vertexTex vertexBTex;
        vertexTex vertexCTex;
        vertexTex vertexDTex;
    };

public:
    glTextureSelectWidget();
    ~glTextureSelectWidget();

    void loadTextureFromFile(QString& fileName);
    void triangulatePoints();
    void SetEnableConstraintSelection(bool aValue);
    QVector<MathAlgorithms::Vertex> createBorderConstraints();


    QVector<glMeshSelectWidget::Vertex*>& GetVertices();
    QVector<glMeshSelectWidget::Edge*>& GetEdges();
    QVector<glMeshSelectWidget::Triangle*>& GetTriangles();

protected:

    //default widget gl functions
    void initializeGL(); //called once before drawing happens
    void paintGL(); //called during draw call back
    void resizeGL(int width, int height); //called when widget size changes
    void mousePressEvent(QMouseEvent *event); //when mouse is pressed inside the widget area

    //creates a constraintPoint given the X and Y vertex coordinates relative to the widget
    constraintPoint createContraintPoint(int x, int y);
    //create data structers used by Match function, updates information about triangles
    void buildStructers();


    int widgetWidth;
    int widgetHeight;
    bool textureLoaded;
    bool enableSetConstraint;
    rectangle textureOfFace;
    GLuint textureId;
    constraintPoint borderConstraints [12];
    QVector<constraintPoint> userConstraints;
    QVector<MathAlgorithms::Triangle> triangulatedConstraints;

    //data structers used for matching, creating during triangulation
    QVector<glMeshSelectWidget::Vertex*> triangulatedVertexes;
    QVector<glMeshSelectWidget::Edge*> triangulatedEdges;
    QVector<glMeshSelectWidget::Triangle*> triangulatedTriangles;
};

#endif // GLTEXTURESELECTWIDGET_H

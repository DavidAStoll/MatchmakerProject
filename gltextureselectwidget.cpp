#include "glTextureSelectWidget.h"
#include "QMouseEvent"
#include "qimage.h"
#include "qbuffer.h"
#include "qbytearray.h"
#include "mainwindow.h"

glTextureSelectWidget::glTextureSelectWidget()
{
    textureOfFace.vertexACor.x = 100;
    textureOfFace.vertexACor.y = 0;
    textureOfFace.vertexACor.z = 0;
    textureOfFace.vertexATex.s = 1;
    textureOfFace.vertexATex.t = 0;

    textureOfFace.vertexBCor.x = 100;
    textureOfFace.vertexBCor.y = 100;
    textureOfFace.vertexBCor.z = 0;
    textureOfFace.vertexBTex.s = 1;
    textureOfFace.vertexBTex.t = 1;

    textureOfFace.vertexCCor.x = 0;
    textureOfFace.vertexCCor.y = 0;
    textureOfFace.vertexCCor.z = 0;
    textureOfFace.vertexCTex.s = 0;
    textureOfFace.vertexCTex.t = 0;

    textureOfFace.vertexDCor.x = 0;
    textureOfFace.vertexDCor.y = 100;
    textureOfFace.vertexDCor.z = 0;
    textureOfFace.vertexDTex.s = 0;
    textureOfFace.vertexDTex.t = 1;

    textureLoaded = false;
    enableSetConstraint = false;
}

glTextureSelectWidget::~glTextureSelectWidget()
{
    //nothing owned
}

void glTextureSelectWidget::initializeGL()
{
    //setup OpenGl
    glEnable(GL_TEXTURE_2D); //enable textures
    glEnableClientState(GL_VERTEX_ARRAY); //enable drawArray vertex array
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);//enable drawArray texture array
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //-- Setup texture handling and default texture --//

    //generate TextureIdHandle
    glGenTextures(1, &textureId);

    //load default image
    QImage *default_image = new QImage(":/no_image.png");
    if(default_image->isNull())
    {
        default_image = NULL; //something went wrong with loading the file from the resources
    }
    QImage imageInOpenGLFormat = QGLWidget::convertToGLFormat(*default_image);

    //load image into openGl texture memory
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageInOpenGLFormat.width(), imageInOpenGLFormat.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageInOpenGLFormat.bits());

    //setup texture rendering options
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    //should now be in openGL memory, don't need image file anymore
    delete default_image;
}

void glTextureSelectWidget::paintGL()
{
    //draw texture
    glVertexPointer(3, GL_FLOAT, 0, &textureOfFace.vertexACor.x);
    glTexCoordPointer(2, GL_FLOAT, 0,  &textureOfFace.vertexATex.s);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if(textureLoaded)
    {
        //draw constrain points, don't want to use texture in this case
        glDisable(GL_TEXTURE_2D);

        //draw user Constraint triangles
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(0,1,0);  //use gree to draw lines
        for(int i = 0; i < triangulatedConstraints.size(); i++)
        {
            glVertexPointer(3, GL_FLOAT, 0,  &triangulatedConstraints[i].point1.x);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        //use purple for constraint points
        glColor3f(0.5, 0,0.5);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //draw border constraints
        for(int i = 0; i < 12; i++)
        {
            glVertexPointer(3, GL_FLOAT, 0,  &borderConstraints[i].rightBottom.x);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        //draw user constraints points
        for(int i = 0; i < userConstraints.size(); i++)
        {
            glVertexPointer(3, GL_FLOAT, 0,  &userConstraints[i].rightBottom.x);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        //all done, enable textures again
        glEnable(GL_TEXTURE_2D);
    }
}

void glTextureSelectWidget::resizeGL(int width, int height)
{
    widgetWidth = width;
    widgetHeight = height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, GL_TEXTUREWIDGET_CANVAS_WIDTH, 0, GL_TEXTUREWIDGET_CANVAS_HEIGHT, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void glTextureSelectWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
    //add constraint point if conditions are met
    if(textureLoaded && enableSetConstraint)
    {
         glTextureSelectWidget::constraintPoint point = createContraintPoint(mouseEvent->x(), mouseEvent->y());
         userConstraints.push_back(point);

         vertexCor& bottomLeft = point.leftBottom;
         MainWindow::globalInstance->progressWidget->addConstraintMatchForTexture(MathAlgorithms::Vertex(bottomLeft.x + GL_TEXTUREWIDGET_CONSTRAINT_SIZE, bottomLeft.y + GL_TEXTUREWIDGET_CONSTRAINT_SIZE));
    }

    //redraw glWidget
    updateGL();
}

void glTextureSelectWidget::loadTextureFromFile(QString& fileName)
{
    //load image data from file
    QImage *image = new QImage(fileName);
    if(image->isNull())
    {
        image = NULL; //something went wrong with loading the file from the resources
    }
    QImage imageInOpenGLFormat = QGLWidget::convertToGLFormat(*image);

    //replace current displayed texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageInOpenGLFormat.width(), imageInOpenGLFormat.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageInOpenGLFormat.bits());

    //setup texture rendering options
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //should now be in openGL memory, don't need image file anymore
    //delete image;
    textureLoaded = true;

    //setup border constraints, we have 4 for each side of the rectangle
    //Bottom side
    borderConstraints[0] = createContraintPoint(0, widgetHeight);
    borderConstraints[1] = createContraintPoint(widgetWidth / 3, widgetHeight);
    borderConstraints[2] = createContraintPoint((2 * widgetWidth) / 3, widgetHeight);
    borderConstraints[3] = createContraintPoint(widgetWidth, widgetHeight);
    //Top side
    borderConstraints[4] = createContraintPoint(0, 0);
    borderConstraints[5] = createContraintPoint(widgetWidth / 3, 0);
    borderConstraints[6] = createContraintPoint((2 * widgetWidth) / 3, 0);
    borderConstraints[7] = createContraintPoint(widgetWidth, 0);
    //Left side
    borderConstraints[8] = createContraintPoint(0, (2 * widgetHeight) / 3);
    borderConstraints[9] = createContraintPoint(0, widgetHeight / 3);
    //Right side
    borderConstraints[10] = createContraintPoint(widgetWidth, (2 * widgetHeight) / 3);
    borderConstraints[11] = createContraintPoint(widgetWidth, widgetHeight / 3);

    //redraw glWidget
    updateGL();
}

glTextureSelectWidget::constraintPoint glTextureSelectWidget::createContraintPoint(int x, int y)
{
    //convert to OpenGL cordinates
    int glXLocation = ((float) x / widgetWidth) * GL_TEXTUREWIDGET_CANVAS_WIDTH;
    int glYLocation = ((float) (widgetHeight - y) / widgetHeight) * GL_TEXTUREWIDGET_CANVAS_HEIGHT;

    //X and Y location is always center of the constraint point
    return constraintPoint (glXLocation, glYLocation);
}

void glTextureSelectWidget::triangulatePoints()
{
    //fill list with pre-defined border constraint points
    QVector<MathAlgorithms::Vertex> points = MainWindow::globalInstance->glTextureWidget->createBorderConstraints();

    //add user constraint points
    for(int i = 0; i < userConstraints.size(); i++)
    {
        //need to find center of the constraint point
        vertexCor& bottomLeft = userConstraints[i].leftBottom;
        points.append( MathAlgorithms::Vertex(bottomLeft.x + GL_TEXTUREWIDGET_CONSTRAINT_SIZE, bottomLeft.y + GL_TEXTUREWIDGET_CONSTRAINT_SIZE));
    }

    triangulatedConstraints = MathAlgorithms::getDelaunayTriangulation(points);

    buildStructers();

    //redraw glWidget
    updateGL();
}

void glTextureSelectWidget::SetEnableConstraintSelection(bool aValue)
{
   enableSetConstraint = aValue;
}

QVector<glMeshSelectWidget::Vertex*>& glTextureSelectWidget::GetVertices()
{
    return triangulatedVertexes;
}

QVector<glMeshSelectWidget::Edge*>& glTextureSelectWidget::GetEdges()
{
    return triangulatedEdges;
}

QVector<glMeshSelectWidget::Triangle*>& glTextureSelectWidget::GetTriangles()
{
    return triangulatedTriangles;
}

void glTextureSelectWidget::buildStructers()
{
    //reset structers first
    triangulatedTriangles.clear();
    triangulatedEdges.clear();
    triangulatedVertexes.clear();

    for (int i = 0; i < triangulatedConstraints.size(); i++ )
    {
        MathAlgorithms::Triangle currentTriangle = triangulatedConstraints[i];
        glMeshSelectWidget::Triangle* matchTriangle = new glMeshSelectWidget::Triangle;
        glMeshSelectWidget::Vertex* matchVertexA = 0;
        glMeshSelectWidget::Vertex* matchVertexB = 0;
        glMeshSelectWidget::Vertex* matchVertexC = 0;

        //chech already created vertices if the one we are looking for is already in the list, don't want duplicates
        auto findVertexInTriangulatedVertexes = [&] (glMeshSelectWidget::Vertex*& vertexPointer, MathAlgorithms::Vertex& vertexData)
        {
            for(int vertexIndex = 0; vertexIndex < triangulatedVertexes.size(); vertexIndex++)
            {
               glMeshSelectWidget::Vertex* vertex = triangulatedVertexes[vertexIndex];

               if(vertexData == vertex->vertexCor)
               {
                vertexPointer = vertex;
                return;
               }
            }
        };
        findVertexInTriangulatedVertexes(matchVertexA, currentTriangle.point1);
        findVertexInTriangulatedVertexes(matchVertexB, currentTriangle.point2);
        findVertexInTriangulatedVertexes(matchVertexC, currentTriangle.point3);

        //create vertices for those not found
        auto createVertexIfNotFound = [&](glMeshSelectWidget::Vertex*& vertex, MathAlgorithms::Vertex& vertexData)
        {
           if(vertex == 0)
           {
               //need to create a new one
               glMeshSelectWidget::Vertex* matchVertex = new glMeshSelectWidget::Vertex(vertexData.x, vertexData.y);
               triangulatedVertexes.append(matchVertex);

               //need reference to it
               vertex = matchVertex;
           }
        };
        createVertexIfNotFound(matchVertexA, currentTriangle.point1);
        createVertexIfNotFound(matchVertexB, currentTriangle.point2);
        createVertexIfNotFound(matchVertexC, currentTriangle.point3);

        //create or find edges
        glMeshSelectWidget::Edge* matchEdgeA = 0;
        glMeshSelectWidget::Edge* matchEdgeB = 0;
        glMeshSelectWidget::Edge* matchEdgeC = 0;

        //check for existing edges
        auto findEdgeWithVertices = [&] (glMeshSelectWidget::Edge*& edgePointer, glMeshSelectWidget::Vertex* vertexA, glMeshSelectWidget::Vertex* vertexB)
        {
            for(int edgeIndex = 0; edgeIndex < matchVertexA->edgeIndicies.size(); edgeIndex++)
            {
               glMeshSelectWidget::Edge* edge = triangulatedEdges[(matchVertexA->edgeIndicies[edgeIndex])];

               if((edge->vertexA == vertexA || edge->vertexB == vertexA)
               && (edge->vertexA == vertexB || edge->vertexB == vertexB))
               {
                   edgePointer = edge;
               }
            }
        };
        findEdgeWithVertices(matchEdgeA, matchVertexA, matchVertexB);
        findEdgeWithVertices(matchEdgeB, matchVertexB, matchVertexC);
        findEdgeWithVertices(matchEdgeC, matchVertexC, matchVertexA);

        //create vertices for those not found
        auto createEdgeIfNotFound = [&](glMeshSelectWidget::Edge*& edgePointer, glMeshSelectWidget::Vertex* vertexA, glMeshSelectWidget::Vertex* vertexB)
        {
           if(edgePointer == 0)
           {
               //need to create a new one
               glMeshSelectWidget::Edge* matchEdge = new glMeshSelectWidget::Edge(vertexA, vertexB);
               triangulatedEdges.append(matchEdge);

               //need reference to it
               edgePointer = matchEdge;
           }
        };
        createEdgeIfNotFound(matchEdgeA, matchVertexA, matchVertexB);
        createEdgeIfNotFound(matchEdgeB, matchVertexB, matchVertexC);
        createEdgeIfNotFound(matchEdgeC, matchVertexC, matchVertexA);

        //update triangle since we have data now
        matchTriangle->edgeA = matchEdgeA;
        matchTriangle->edgeB = matchEdgeB;
        matchTriangle->edgeC = matchEdgeC;
        matchTriangle->vertexA = matchVertexA;
        matchTriangle->vertexB = matchVertexB;
        matchTriangle->vertexC = matchVertexC;

        triangulatedTriangles.append(matchTriangle);
        int triangleIndex = triangulatedTriangles.size() - 1;

        //update edges to point to this triangle
        matchEdgeA->triangleIndicies.append(triangleIndex);
        matchEdgeB->triangleIndicies.append(triangleIndex);
        matchEdgeC->triangleIndicies.append(triangleIndex);

        //need to update vertices to point to triangle
        matchVertexA->triangleIndicies.append(triangleIndex);
        matchVertexB->triangleIndicies.append(triangleIndex);
        matchVertexC->triangleIndicies.append(triangleIndex);
    }
}

 QVector<MathAlgorithms::Vertex> glTextureSelectWidget::createBorderConstraints()
 {
     QVector<MathAlgorithms::Vertex> points;

     //add default border constraint points, calculate again to avoid roundoff error
     //Bottom
     points.append(MathAlgorithms::Vertex (0,0));
     points.append(MathAlgorithms::Vertex (GL_TEXTUREWIDGET_CANVAS_WIDTH / 3, 0));
     points.append(MathAlgorithms::Vertex ((2 * GL_TEXTUREWIDGET_CANVAS_WIDTH) / 3, 0));
     points.append(MathAlgorithms::Vertex (GL_TEXTUREWIDGET_CANVAS_WIDTH, 0));
     //Top
     points.append(MathAlgorithms::Vertex (0, GL_TEXTUREWIDGET_CANVAS_HEIGHT));
     points.append(MathAlgorithms::Vertex (GL_TEXTUREWIDGET_CANVAS_WIDTH / 3, GL_TEXTUREWIDGET_CANVAS_HEIGHT));
     points.append(MathAlgorithms::Vertex ((2 * GL_TEXTUREWIDGET_CANVAS_WIDTH) / 3, GL_TEXTUREWIDGET_CANVAS_HEIGHT));
     points.append(MathAlgorithms::Vertex (GL_TEXTUREWIDGET_CANVAS_WIDTH, GL_TEXTUREWIDGET_CANVAS_HEIGHT));
     //Left
     points.append(MathAlgorithms::Vertex (0, GL_TEXTUREWIDGET_CANVAS_HEIGHT / 3));
     points.append(MathAlgorithms::Vertex (0, (2 * GL_TEXTUREWIDGET_CANVAS_HEIGHT) / 3));
     //Right
     points.append(MathAlgorithms::Vertex (GL_TEXTUREWIDGET_CANVAS_WIDTH, GL_TEXTUREWIDGET_CANVAS_HEIGHT / 3));
     points.append(MathAlgorithms::Vertex (GL_TEXTUREWIDGET_CANVAS_WIDTH, (2 * GL_TEXTUREWIDGET_CANVAS_HEIGHT) / 3));

     return points;
 }

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

         MathAlgorithms::Vertex constraintVertex;
         constraintVertex.x = point.leftBottom.x + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
         constraintVertex.y = point.leftBottom.y + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
         constraintVertex.z = 0;

         MainWindow::globalInstance->progressWidget->addConstraintMatchAddVertexInTexture(constraintVertex);
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
    int glXLocation = ((float) x / widgetWidth) * GL_TEXTUREWIDGET_CANVAS_WIDTH;
    int glYLocation = ((float) (widgetHeight - y) / widgetHeight) * GL_TEXTUREWIDGET_CANVAS_HEIGHT;

    //X and Y location is always center of the constraint point
    constraintPoint newPoint;
    newPoint.pixelXLocation = glXLocation;
    newPoint.pixelYLocation = glYLocation;
    //Left Bottom
    newPoint.leftBottom.x = glXLocation - GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
    newPoint.leftBottom.y = glYLocation - GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
    newPoint.leftBottom.z = 0;
    //Left Top
    newPoint.leftTop.x = glXLocation - GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
    newPoint.leftTop.y = glYLocation + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
    newPoint.leftTop.z = 0;
    //Right Bottom
    newPoint.rightBottom.x = glXLocation + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
    newPoint.rightBottom.y = glYLocation - GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
    newPoint.rightBottom.z = 0;
    //Right Top
    newPoint.rightTop.x = glXLocation + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
    newPoint.rightTop.y = glYLocation + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
    newPoint.rightTop.z = 0;

    return newPoint;
}

void glTextureSelectWidget::triangulatePoints()
{
    QVector<MathAlgorithms::Vertex> points;

    //add default border constraint points, calculate again to avoid roundoff error
    //Bottom
    MathAlgorithms::Vertex vertex;
    vertex.x = 0; vertex.y = 0; vertex.z = 0;
    points.append(vertex);
    vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH / 3; vertex.y = 0; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    vertex.x = (2 * GL_TEXTUREWIDGET_CANVAS_WIDTH) / 3; vertex.y = 0; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH; vertex.y = 0; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    //Top
    vertex.x = 0; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH / 3; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    vertex.x = (2 * GL_TEXTUREWIDGET_CANVAS_WIDTH) / 3; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    //Left
    vertex.x = 0; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT / 3; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    vertex.x = 0; vertex.y = (2 * GL_TEXTUREWIDGET_CANVAS_HEIGHT) / 3; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    //Right
    vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT / 3; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));
    vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH; vertex.y = (2 * GL_TEXTUREWIDGET_CANVAS_HEIGHT) / 3; vertex.z = 0;
    points.append(MathAlgorithms::Vertex(vertex));

    //add user constraint points
    for(int i = 0; i < userConstraints.size(); i++)
    {
        //need to find center of the constraint point
        float xLocation = userConstraints[i].leftBottom.x + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
        float yLocation = userConstraints[i].leftBottom.y + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;

        vertex.x = xLocation; vertex.y = yLocation; vertex.z = 0;
        points.append(MathAlgorithms::Vertex(vertex));
    }

    triangulatedConstraints = MathAlgorithms::getDelaunayTriangulation(points);

    buildStructers();

    //redraw glWidget
    updateGL();
}

void glTextureSelectWidget::SetEnableConstraint(bool aValue)
{
   enableSetConstraint = aValue;
}

QVector<glMeshSelectWidget::vertex*>& glTextureSelectWidget::GetVertices()
{
    return triangulatedVertexes;
}

QVector<glMeshSelectWidget::edge*>& glTextureSelectWidget::GetEdges()
{
    return triangulatedEdges;
}

QVector<glMeshSelectWidget::triangle*>& glTextureSelectWidget::GetTriangles()
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
        glMeshSelectWidget::triangle* matchTriangle = new glMeshSelectWidget::triangle;
        glMeshSelectWidget::vertex* matchVertexA = 0;
        glMeshSelectWidget::vertex* matchVertexB = 0;
        glMeshSelectWidget::vertex* matchVertexC = 0;

        //check if verticies are already in list or not
        for(int vertexIndex = 0; vertexIndex < triangulatedVertexes.size(); vertexIndex++)
        {
           if(currentTriangle.point1.x == triangulatedVertexes[vertexIndex]->x
           && currentTriangle.point1.y == triangulatedVertexes[vertexIndex]->y
           && currentTriangle.point1.z == triangulatedVertexes[vertexIndex]->z)
           {
              matchVertexA = triangulatedVertexes[vertexIndex];
              break;
           }
        }
        for(int vertexIndex = 0; vertexIndex < triangulatedVertexes.size(); vertexIndex++)
        {
           if(currentTriangle.point2.x == triangulatedVertexes[vertexIndex]->x
           && currentTriangle.point2.y == triangulatedVertexes[vertexIndex]->y
           && currentTriangle.point2.z == triangulatedVertexes[vertexIndex]->z)
           {
              matchVertexB = triangulatedVertexes[vertexIndex];
              break;
           }
        }
        for(int vertexIndex = 0; vertexIndex < triangulatedVertexes.size(); vertexIndex++)
        {
           if(currentTriangle.point3.x == triangulatedVertexes[vertexIndex]->x
           && currentTriangle.point3.y == triangulatedVertexes[vertexIndex]->y
           && currentTriangle.point3.z == triangulatedVertexes[vertexIndex]->z)
           {
              matchVertexC = triangulatedVertexes[vertexIndex];
              break;
           }
        }

        //for every vertex not found create an instance for it
        if(matchVertexA == 0)
        {
            //need to create a new one
             glMeshSelectWidget::vertex* matchVertex = new glMeshSelectWidget::vertex;
             matchVertex->x = currentTriangle.point1.x;
             matchVertex->y = currentTriangle.point1.y;
             matchVertex->z = currentTriangle.point1.z;
             triangulatedVertexes.append(matchVertex);

             matchVertexA = triangulatedVertexes[triangulatedVertexes.size() - 1]; //need reference to it
        }
        if(matchVertexB == 0)
        {
            //need to create a new one
             glMeshSelectWidget::vertex* matchVertex = new glMeshSelectWidget::vertex;
             matchVertex->x = currentTriangle.point2.x;
             matchVertex->y = currentTriangle.point2.y;
             matchVertex->z = currentTriangle.point2.z;
             triangulatedVertexes.append(matchVertex);

             matchVertexB = triangulatedVertexes[triangulatedVertexes.size() - 1]; //need reference to it
        }
        if(matchVertexC == 0)
        {
            //need to create a new one
             glMeshSelectWidget::vertex* matchVertex = new glMeshSelectWidget::vertex;
             matchVertex->x = currentTriangle.point3.x;
             matchVertex->y = currentTriangle.point3.y;
             matchVertex->z = currentTriangle.point3.z;
             triangulatedVertexes.append(matchVertex);

             matchVertexC = triangulatedVertexes[triangulatedVertexes.size() - 1]; //need reference to it
        }

        //create or find edges
        glMeshSelectWidget::edge* matchEdgeA = 0;
        glMeshSelectWidget::edge* matchEdgeB = 0;
        glMeshSelectWidget::edge* matchEdgeC = 0;
        //edge is made up by vertexA and vertex B, check if vertexA has already this edge, if it has the edge is already in the edge list
        for(int edgeIndex = 0; edgeIndex < matchVertexA->edgeIndicies.size(); edgeIndex++)
        {
            if((triangulatedEdges[(matchVertexA->edgeIndicies[edgeIndex])]->vertexA == matchVertexA || triangulatedEdges[(matchVertexA->edgeIndicies[edgeIndex])]->vertexB == matchVertexA)
            && (triangulatedEdges[matchVertexA->edgeIndicies[edgeIndex]]->vertexA == matchVertexB || triangulatedEdges[matchVertexA->edgeIndicies[edgeIndex]]->vertexB == matchVertexB))
            {
                matchEdgeA = triangulatedEdges[matchVertexA->edgeIndicies[edgeIndex]];
            }
        }
        for(int edgeIndex = 0; edgeIndex < matchVertexB->edgeIndicies.size(); edgeIndex++)
        {
            if((triangulatedEdges[matchVertexB->edgeIndicies[edgeIndex]]->vertexA == matchVertexB || triangulatedEdges[matchVertexB->edgeIndicies[edgeIndex]]->vertexB == matchVertexB)
            && (triangulatedEdges[matchVertexB->edgeIndicies[edgeIndex]]->vertexA == matchVertexC || triangulatedEdges[matchVertexB->edgeIndicies[edgeIndex]]->vertexB == matchVertexC))
            {
                matchEdgeB = triangulatedEdges[matchVertexB->edgeIndicies[edgeIndex]];
            }
        }
        for(int edgeIndex = 0; edgeIndex < matchVertexC->edgeIndicies.size(); edgeIndex++)
        {
            if((triangulatedEdges[matchVertexC->edgeIndicies[edgeIndex]]->vertexA == matchVertexC || triangulatedEdges[matchVertexC->edgeIndicies[edgeIndex]]->vertexB == matchVertexC)
            && (triangulatedEdges[matchVertexC->edgeIndicies[edgeIndex]]->vertexA == matchVertexA || triangulatedEdges[matchVertexC->edgeIndicies[edgeIndex]]->vertexB == matchVertexA))
            {
                matchEdgeC = triangulatedEdges[matchVertexC->edgeIndicies[edgeIndex]];
            }
        }
        if(matchEdgeA == 0)
        {
            //need to create a new one
             glMeshSelectWidget::edge* matchEdge = new glMeshSelectWidget::edge;
             matchEdge->vertexA = matchVertexA;
             matchEdge->vertexB = matchVertexB;
             triangulatedEdges.append(matchEdge);

             matchEdgeA = triangulatedEdges[triangulatedEdges.size() - 1]; //need reference to it
        }
        if(matchEdgeB == 0)
        {
            //need to create a new one
             glMeshSelectWidget::edge* matchEdge = new glMeshSelectWidget::edge;
             matchEdge->vertexA = matchVertexB;
             matchEdge->vertexB = matchVertexC;
             triangulatedEdges.append(matchEdge);

             matchEdgeB = triangulatedEdges[triangulatedEdges.size() - 1]; //need reference to it
        }
        if(matchEdgeC == 0)
        {
            //need to create a new one
             glMeshSelectWidget::edge* matchEdge = new glMeshSelectWidget::edge;
             matchEdge->vertexA = matchVertexA;
             matchEdge->vertexB = matchVertexC;
             triangulatedEdges.append(matchEdge);

             matchEdgeC = triangulatedEdges[triangulatedEdges.size() - 1]; //need reference to it
        }

        //update triangle since we have data now
        matchTriangle->edgeA = matchEdgeA;
        matchTriangle->edgeB = matchEdgeB;
        matchTriangle->edgeC = matchEdgeC;
        matchTriangle->vertexA = matchVertexA;
        matchTriangle->vertexB = matchVertexB;
        matchTriangle->vertexC = matchVertexC;

        triangulatedTriangles.append(matchTriangle);

        //update edges to point to this triangle
        matchEdgeA->triangleIndicies.append(triangulatedTriangles.size() - 1);
        matchEdgeB->triangleIndicies.append(triangulatedTriangles.size() - 1);
        matchEdgeC->triangleIndicies.append(triangulatedTriangles.size() - 1);

        //need to update vertices to point to triangle
        matchVertexA->triangleIndicies.append(triangulatedTriangles.size() - 1);
        matchVertexB->triangleIndicies.append(triangulatedTriangles.size() - 1);
        matchVertexC->triangleIndicies.append(triangulatedTriangles.size() - 1);
    }
}

 QVector<MathAlgorithms::Vertex> glTextureSelectWidget::createBorderConstraints()
 {
     QVector<MathAlgorithms::Vertex> points;

     //add default border constraint points, calculate again to avoid roundoff error
     //Bottom
     MathAlgorithms::Vertex vertex;
     vertex.x = 0; vertex.y = 0; vertex.z = 0;
     points.append(vertex);
     vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH / 3; vertex.y = 0; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     vertex.x = (2 * GL_TEXTUREWIDGET_CANVAS_WIDTH) / 3; vertex.y = 0; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH; vertex.y = 0; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     //Top
     vertex.x = 0; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH / 3; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     vertex.x = (2 * GL_TEXTUREWIDGET_CANVAS_WIDTH) / 3; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     //Left
     vertex.x = 0; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT / 3; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     vertex.x = 0; vertex.y = (2 * GL_TEXTUREWIDGET_CANVAS_HEIGHT) / 3; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     //Right
     vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH; vertex.y = GL_TEXTUREWIDGET_CANVAS_HEIGHT / 3; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));
     vertex.x = GL_TEXTUREWIDGET_CANVAS_WIDTH; vertex.y = (2 * GL_TEXTUREWIDGET_CANVAS_HEIGHT) / 3; vertex.z = 0;
     points.append(MathAlgorithms::Vertex(vertex));

//     for ( int i = 0; i < 12; ++i )
//     {
//         MathAlgorithms::Vertex vertex;
//         vertex.x =  borderConstraints[i].leftBottom.x + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
//         vertex.y =  borderConstraints[i].leftBottom.y + GL_TEXTUREWIDGET_CONSTRAINT_SIZE;
//         vertex.z = 0;
//         points.push_back(vertex);
//     }

     return points;
 }

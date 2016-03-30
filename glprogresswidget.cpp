#include "glprogresswidget.h"
#include "QMouseEvent"
#include "qimage.h"
#include "qbuffer.h"
#include "qbytearray.h"
#include "mainwindow.h"

glProgressWidget::glProgressWidget()
{
    meshVertices = 0;
    meshEdges = 0;
    meshTriangles = 0;
}

glProgressWidget::~glProgressWidget()
{
    //nothing owned

}

void glProgressWidget::initializeGL()
{
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void glProgressWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //glShadeModel( GL_FLAT );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if(meshTriangles)
    {
        //draw mesh
        glColor3f(0.0f, 1.0f, 0.0f);

        for(int i = 0; i < meshTriangles->size(); i++)
        {
            glMeshSelectWidget::Triangle* triangle = meshTriangles->value(i);

            //draw triangle mesh
            glBegin( GL_TRIANGLES );

            //set vertex
            glVertex3f(triangle->vertexA->vertexCor.x, triangle->vertexA->vertexCor.y, triangle->vertexA->vertexCor.z);
            glVertex3f(triangle->vertexB->vertexCor.x, triangle->vertexB->vertexCor.y, triangle->vertexB->vertexCor.z);
            glVertex3f(triangle->vertexC->vertexCor.x, triangle->vertexC->vertexCor.y, triangle->vertexC->vertexCor.z);

            glEnd();
        }

        //draw edges that connect constraints
        glColor3f(0.5, 0, 0.5); //purple

        for(int i = 0; i < constraintEdgesWithPoints.size(); i++)
        {
            edgeWalker* constraintWithEdges = constraintEdgesWithPoints[i];

            //draw edges
            for(int ii = 0; ii < constraintWithEdges->edgesIWalked.size(); ii++)
            {
                glMeshSelectWidget::Vertex* vertexA = constraintWithEdges->edgesIWalked[ii]->vertexA;
                glMeshSelectWidget::Vertex* vertexB = constraintWithEdges->edgesIWalked[ii]->vertexB;

                //draw line for edge
                glBegin( GL_LINES );

                //set vertex
                glVertex3f(vertexA->vertexCor.x, vertexA->vertexCor.y, vertexA->vertexCor.z);
                glVertex3f(vertexB->vertexCor.x, vertexB->vertexCor.y, vertexB->vertexCor.z);

                glEnd();
            }

            //draw Constraint Points as well
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); //constraint points should be filled

            MathAlgorithms::Vertex bottomLeftA, bottomRightA, topRightA, topLeftA;
            bottomLeftA.x = constraintWithEdges->startVertex->vertexCor.x - GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            bottomLeftA.y = constraintWithEdges->startVertex->vertexCor.y - GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            bottomRightA.x = constraintWithEdges->startVertex->vertexCor.x + GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            bottomRightA.y = constraintWithEdges->startVertex->vertexCor.y - GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            topRightA.x = constraintWithEdges->startVertex->vertexCor.x + GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            topRightA.y = constraintWithEdges->startVertex->vertexCor.y + GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            topLeftA.x = constraintWithEdges->startVertex->vertexCor.x - GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            topLeftA.y = constraintWithEdges->startVertex->vertexCor.y + GL_PROGRESSWIDGET_CONSTRAINT_SIZE;

            MathAlgorithms::Vertex bottomLeftB, bottomRightB, topRightB, topLeftB;
            bottomLeftB.x = constraintWithEdges->targetVertex->vertexCor.x - GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            bottomLeftB.y = constraintWithEdges->targetVertex->vertexCor.y - GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            bottomRightB.x = constraintWithEdges->targetVertex->vertexCor.x + GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            bottomRightB.y = constraintWithEdges->targetVertex->vertexCor.y - GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            topRightB.x = constraintWithEdges->targetVertex->vertexCor.x + GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            topRightB.y = constraintWithEdges->targetVertex->vertexCor.y + GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            topLeftB.x = constraintWithEdges->targetVertex->vertexCor.x - GL_PROGRESSWIDGET_CONSTRAINT_SIZE;
            topLeftB.y = constraintWithEdges->targetVertex->vertexCor.y + GL_PROGRESSWIDGET_CONSTRAINT_SIZE;

            //start Vertext Constraint
            glBegin( GL_TRIANGLE_STRIP );
            glVertex2f(bottomRightA.x, bottomRightA.y);
            glVertex2f(topRightA.x, topRightA.y);
            glVertex2f(bottomLeftA.x, bottomLeftA.y);
            glVertex2f(topLeftA.x, topLeftA.y);
            glEnd();

            //target Vertext Constraint
            glBegin( GL_TRIANGLE_STRIP );
            glVertex2f(bottomRightB.x, bottomRightB.y);
            glVertex2f(topRightB.x, topRightB.y);
            glVertex2f(bottomLeftB.x, bottomLeftB.y);
            glVertex2f(topLeftB.x, topLeftB.y);
            glEnd();
        }
    }
}

void glProgressWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-75, 75, 0, 150, -90, 160);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void glProgressWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
    //redraw glWidget
    updateGL();
}

void glProgressWidget::addConstraintMatchForMesh(MathAlgorithms::Vertex aVertex)
{
    glProgressWidget::ConstraintMatch newMatch;
    newMatch.vertexInMesh = aVertex;
    ConstraintMatches.append(newMatch);

    //change flags for widgets
    MainWindow::globalInstance->glMeshWidget->SetEnableConstraintSelection(false);
    MainWindow::globalInstance->glTextureWidget->SetEnableConstraintSelection(true);
    MainWindow::globalInstance->matchAction->setEnabled(false); //since we still need another vertex for texture
}

void glProgressWidget::addConstraintMatchForTexture(MathAlgorithms::Vertex aVertex)
{
    //should already be created beforehand since Mesh selection happens always first
    ConstraintMatches[ConstraintMatches.size() - 1].vertexInTexture = aVertex;

    //change flags for widgets
    MainWindow::globalInstance->glMeshWidget->SetEnableConstraintSelection(true);
    MainWindow::globalInstance->glTextureWidget->SetEnableConstraintSelection(false);
    MainWindow::globalInstance->matchAction->setEnabled(true); //since we have a full set of pairs now
}

void glProgressWidget::performMatch()
{
    //reset what edges are part of the constraints
    for(int i = 0; i < constraintEdgesWithPoints.size(); i++)
    {
        constraintEdgesWithPoints[i]->edgesIWalked.clear();
        delete constraintEdgesWithPoints[i]; //clear up memory it points to
    }
    constraintEdgesWithPoints.clear(); //clear up memoy taken by pointers

    //need to add border constraints to list
    QVector<MathAlgorithms::Vertex> textureBorder = MainWindow::globalInstance->glTextureWidget->createBorderConstraints();
    QVector<MathAlgorithms::Vertex> meshBorder = MainWindow::globalInstance->glMeshWidget->createBorderConstraints();

    //create match for border constraints between texture and mesh view
    for(int i = 0; i < 12 ; i++)
    {
        glProgressWidget::ConstraintMatch newMatch;
        newMatch.vertexInMesh = meshBorder[i];
        newMatch.vertexInTexture= textureBorder[i];
        ConstraintMatches.append(newMatch);
    }

    //need to get edges, vertices and triangles from Mesh
    meshVertices = MainWindow::globalInstance->glMeshWidget->GetVertices();
    meshEdges = MainWindow::globalInstance->glMeshWidget->GetEdges();
    meshTriangles = MainWindow::globalInstance->glMeshWidget->GetTriangles();

    //need to get triangles fromed in texture
    QVector<glMeshSelectWidget::Triangle*>& textureTriangles = MainWindow::globalInstance->glTextureWidget->GetTriangles();

    for(int i = 0; i < textureTriangles.size(); i++)
    {
        glMeshSelectWidget::Vertex* meshVerticesA = 0;
        glMeshSelectWidget::Vertex* meshVerticesB = 0;
        glMeshSelectWidget::Vertex* meshVerticesC = 0;

        //get vertexA in mesh that corresponds to constraint in texture
        for(int ii = 0; ii < ConstraintMatches.size(); ii++)
        {
            //find what texture vertex matches the correct constrant Vertex
            if(textureTriangles[i]->vertexA->vertexCor.x == ConstraintMatches[ii].vertexInTexture.x
            && textureTriangles[i]->vertexA->vertexCor.y == ConstraintMatches[ii].vertexInTexture.y
            && textureTriangles[i]->vertexA->vertexCor.z == ConstraintMatches[ii].vertexInTexture.z)
            {
                for(int iii = 0; iii < meshVertices->size(); iii++)
                {
                    //find what constraint mesh vertex matches the meshVertices
                    if(ConstraintMatches[ii].vertexInMesh.x == meshVertices->value(iii)->vertexCor.x
                    && ConstraintMatches[ii].vertexInMesh.y == meshVertices->value(iii)->vertexCor.y
                    && ConstraintMatches[ii].vertexInMesh.z == meshVertices->value(iii)->vertexCor.z)
                    {
                        meshVerticesA = meshVertices->value(iii);
                        break;
                    }
                }
                    break;
            }
        }
        //get vertexB in mesh that corresponds to constraint in texture
        for(int ii = 0; ii < ConstraintMatches.size(); ii++)
        {
            //find what texture vertex matches the correct constrant Vertex
            if(textureTriangles[i]->vertexB->vertexCor.x == ConstraintMatches[ii].vertexInTexture.x
            && textureTriangles[i]->vertexB->vertexCor.y == ConstraintMatches[ii].vertexInTexture.y
            && textureTriangles[i]->vertexB->vertexCor.z == ConstraintMatches[ii].vertexInTexture.z)
            {
                for(int iii = 0; iii < meshVertices->size(); iii++)
                {
                    //find what constraint mesh vertex matches the meshVertices
                    if(ConstraintMatches[ii].vertexInMesh.x == meshVertices->value(iii)->vertexCor.x
                    && ConstraintMatches[ii].vertexInMesh.y == meshVertices->value(iii)->vertexCor.y
                    && ConstraintMatches[ii].vertexInMesh.z == meshVertices->value(iii)->vertexCor.z)
                    {
                        meshVerticesB = meshVertices->value(iii);
                        break;
                    }
                }
                    break;
            }
        }
        //get vertexC in mesh that corresponds to constraint in texture
        for(int ii = 0; ii < ConstraintMatches.size(); ii++)
        {
            //find what texture vertex matches the correct constrant Vertex
            if(textureTriangles[i]->vertexC->vertexCor.x == ConstraintMatches[ii].vertexInTexture.x
            && textureTriangles[i]->vertexC->vertexCor.y == ConstraintMatches[ii].vertexInTexture.y
            && textureTriangles[i]->vertexC->vertexCor.z == ConstraintMatches[ii].vertexInTexture.z)
            {
                for(int iii = 0; iii < meshVertices->size(); iii++)
                {
                    //find what constraint mesh vertex matches the meshVertices
                    if(ConstraintMatches[ii].vertexInMesh.x == meshVertices->value(iii)->vertexCor.x
                    && ConstraintMatches[ii].vertexInMesh.y == meshVertices->value(iii)->vertexCor.y
                    && ConstraintMatches[ii].vertexInMesh.z == meshVertices->value(iii)->vertexCor.z)
                    {
                        meshVerticesC = meshVertices->value(iii);
                        break;
                    }
                }
                    break;
            }
        }

        edgeWalker* walkerAToB = 0;
        edgeWalker* walkerBToC = 0;
        edgeWalker* walkerAToC = 0;

        //now we know the vertex, we need to find shortest path between them in the mesh
        if(!constraintEdgeDoesExist(meshVerticesA, meshVerticesB))
        {
            //A --> B
            walkerAToB = new edgeWalker;
            walkerAToB->startVertex = meshVerticesA;
            walkerAToB->targetVertex = meshVerticesB;
            walkerAToB->success = false;
            glProgressWidget::constraintOrientationSet set = createConstraintOrientationSet(meshVerticesA , meshVerticesB);
            walkToVertex(walkerAToB, set);//find closest path
            //done, add to constraint data-structer
            walkerAToB->startVertex = meshVerticesA; //need to reset it, since it changes due to function calls
            constraintEdgesWithPoints.append(walkerAToB);
        }
        else
        {
            printf("Error, A constraint edge does not exist!");
        }
        if(!constraintEdgeDoesExist(meshVerticesB, meshVerticesC))
        {
            //B --> C
            walkerBToC = new edgeWalker;
            walkerBToC->startVertex = meshVerticesB;
            walkerBToC->targetVertex = meshVerticesC;
            walkerBToC->success = false;
            glProgressWidget::constraintOrientationSet set = createConstraintOrientationSet(meshVerticesB , meshVerticesC);
            walkToVertex(walkerBToC, set);//find closest path
            //done, add to constraint data-structer
            walkerBToC->startVertex = meshVerticesB; //need to reset it, since it changes due to function calls
            constraintEdgesWithPoints.append(walkerBToC);
        }
        else
        {
            printf("Error, A constraint edge does not exist!");
        }
        if(!constraintEdgeDoesExist(meshVerticesA, meshVerticesC))
        {
            //A --> C
            walkerAToC = new edgeWalker;
            walkerAToC->startVertex = meshVerticesA;
            walkerAToC->targetVertex = meshVerticesC;
            walkerAToC->success = false;
            glProgressWidget::constraintOrientationSet set = createConstraintOrientationSet(meshVerticesA, meshVerticesC);
            walkToVertex(walkerAToC, set);//find closest path
            //done, add to constraint data-structer
            walkerAToC->startVertex = meshVerticesA; //need to reset it, since it changes due to function calls
            constraintEdgesWithPoints.append(walkerAToC);
        }
        else
        {
            printf("Error, A constraint edge does not exist!");
        }

        if ((constraintEdgeDoesExist(meshVerticesA, meshVerticesB)->success)
        &&  (constraintEdgeDoesExist(meshVerticesB, meshVerticesC)->success)
        &&  (constraintEdgeDoesExist(meshVerticesA, meshVerticesC)->success))
        {
          validateTriangulation properTriangulation;
          properTriangulation.edgeA = constraintEdgeDoesExist(meshVerticesA, meshVerticesB);
          properTriangulation.edgeB = constraintEdgeDoesExist(meshVerticesB, meshVerticesC);
          properTriangulation.edgeC = constraintEdgeDoesExist(meshVerticesA, meshVerticesC);

          validTriangulations.append(properTriangulation);
        }
    }

    //redraw glWidget
    updateGL();
}

void glProgressWidget::walkToVertex(edgeWalker* walker, glProgressWidget::constraintOrientationSet& constraintOrientations)
{
    QVector<float> distances;

    for(int i = 0; i < walker->startVertex->edgeIndicies.size(); i++)
    {
         glMeshSelectWidget::Edge* currentEdge = meshEdges->value(walker->startVertex->edgeIndicies[i]);
         glMeshSelectWidget::Vertex* goToVertex = 0;

         if(currentEdge->vertexA == walker->startVertex)
         {
            goToVertex = currentEdge->vertexB;
         }
         else
         {
            goToVertex = currentEdge->vertexA;
         }

         //vertex is destination?
         if(goToVertex == walker->targetVertex)
         {
             //found destination, add edge and terminate
             walker->edgesIWalked.append(currentEdge);
             walker->success = true;
             return;
         }
         else
         {
             //distance to target
             MathAlgorithms::Vertex distance;
             distance.x = walker->targetVertex->vertexCor.x - goToVertex->vertexCor.x;
             distance.y = walker->targetVertex->vertexCor.y - goToVertex->vertexCor.y;
             float dist = sqrt((distance.x * distance.x) +  (distance.y * distance.y));

             distances.append(dist);
         }
    }

    float shortestDisance = -1;
    int index = -1;
    for(int i = 0; i < distances.size(); i++)
    {
        if(distances[i] < shortestDisance || shortestDisance == -1)
        {
            bool notGoodEdge = false;
            glMeshSelectWidget::Edge* candidateEdge = meshEdges->value(walker->startVertex->edgeIndicies[i]);
            glMeshSelectWidget::Vertex* goToVertex = 0;

            //what vertex are we going to?
            if(candidateEdge->vertexA == walker->startVertex)
            {
               goToVertex = candidateEdge->vertexB;
            }
            else
            {
               goToVertex = candidateEdge->vertexA;
            }

            //check if edge already used before
            for(int ii = 0; ii < walker->edgesIWalked.count(); ii++)
            {
                if(walker->edgesIWalked[ii] == meshEdges->value(walker->startVertex->edgeIndicies[i]))
                {
                    //already walked edge
                    notGoodEdge = true;
                    break;
                }
            }


            /*
            //check if vertex has already been used by another path
            if(!notGoodEdge)
            {
                for(int vertexIndex = 0; vertexIndex < doNotWalkVertices.count(); vertexIndex++)
                {
                    if(goToVertex == doNotWalkVertices[vertexIndex])
                    {
                        //someone already used this vertex
                        notGoodEdge = true;
                        break;
                    }
                }
            }


            //check if we invalidate any of the constraint orientations
            if(!notGoodEdge)
            {
                for(int indexConstraints = 0; indexConstraints < constraintOrientations.constraintOrientations.size(); indexConstraints++)
                {
                     glMeshSelectWidget::vertex* constraintVertexStart =  walker->targetVertex;
                     glProgressWidget::constraintOrientation constraintOrient = constraintOrientations.constraintOrientations[indexConstraints];

                    float sign = crossProduct(constraintOrient.constraintVertex, goToVertex, constraintVertexStart);
                    if((sign > 0  && constraintOrient.sign < 0)
                    || (sign < 0 && constraintOrient.sign > 0)) //if signs do not agree
                    {
                        //does not work with one of the constraints
                        notGoodEdge = true;
                        break;
                    }
                }
            }
            */

            //check if walks through a constraint point, it shouldn't unless it is destination
            if(!notGoodEdge)
            {
                for(int indexConstraints = 0; indexConstraints < constraintOrientations.constraintOrientations.size(); indexConstraints++)
                {
                    glProgressWidget::constraintOrientation constraintOrient = constraintOrientations.constraintOrientations[indexConstraints];

                    if(constraintOrient.constraintVertex.x == goToVertex->vertexCor.x && constraintOrient.constraintVertex.y == goToVertex->vertexCor.y)
                    {
                        //does not work with one of the constraints
                        notGoodEdge = true;
                        break;
                    }
                }
            }


            //all good, this edge can be used
            if(!notGoodEdge)
            {
                shortestDisance = distances[i];
                index = i;
            }
        }
    }

    if(index == -1)
        return;

    //keep track of edge
    glMeshSelectWidget::Edge* bestEdge = meshEdges->value(walker->startVertex->edgeIndicies[index]);
    walker->edgesIWalked.append(bestEdge);

    //keep track of vertex used
    if(bestEdge->vertexA == walker->startVertex)
    {
        walker->startVertex = bestEdge->vertexB;
        doNotWalkVertices.append(bestEdge->vertexB); //since this vertex is part of our path, nobody else can use it
        walkToVertex(walker, constraintOrientations);
    }
    else
    {
        walker->startVertex = bestEdge->vertexA;
        doNotWalkVertices.append(bestEdge->vertexA); //since this vertex is part of our path, nobody else can use it
        walkToVertex(walker, constraintOrientations);
    }
}

glProgressWidget::edgeWalker* glProgressWidget::constraintEdgeDoesExist(glMeshSelectWidget::Vertex* vertexA, glMeshSelectWidget::Vertex* vertexB)
{
    for(int i = 0; i < constraintEdgesWithPoints.size(); i++)
    {
        if((constraintEdgesWithPoints[i]->startVertex == vertexA && constraintEdgesWithPoints[i]->targetVertex == vertexB)
        || (constraintEdgesWithPoints[i]->startVertex == vertexB && constraintEdgesWithPoints[i]->targetVertex == vertexA)) //might be switched around
        {
            return constraintEdgesWithPoints[i];
        }
    }

    return 0;
}

glProgressWidget::constraintOrientationSet glProgressWidget::createConstraintOrientationSet(glMeshSelectWidget::Vertex* vertexA, glMeshSelectWidget::Vertex* vertexB)
{
    constraintOrientationSet set;
    set.startConstraint = vertexA; //always start from A

    for(int i = 0; i < ConstraintMatches.size(); i++)
    {
        MathAlgorithms::Vertex constraintVertex = ConstraintMatches[i].vertexInMesh;

        if((constraintVertex.x == vertexA->vertexCor.x && constraintVertex.y == vertexA->vertexCor.y)
        || (constraintVertex.x == vertexB->vertexCor.x && constraintVertex.y == vertexB->vertexCor.y))
        {
            //constraint is one of the end points, don't add it
        }
        else
        {
            //calculate cross product from constraint to A times B
            float sign = crossProduct(constraintVertex, vertexA, vertexB);

            //this orientation is done
            glProgressWidget::constraintOrientation orientation;
            orientation.constraintVertex = constraintVertex;
            orientation.sign = sign;
            //add to set
            set.constraintOrientations.append(orientation);
        }
    }

    return set;
}

float glProgressWidget::crossProduct(MathAlgorithms::Vertex point1, glMeshSelectWidget::Vertex* point2, glMeshSelectWidget::Vertex* point3)
{
//    MathAlgorithms::Vertex vectorA;
//    vectorA.x = point2->x - point1.x;
//    vectorA.y = point2->y - point1.y;
//    vectorA.z = point2->z - point1.z;

//    //normalize A
//    float normOfA = sqrt(vectorA.x * vectorA.x + vectorA.y * vectorA.y);
//    vectorA.x /= normOfA;
//    vectorA.y /= normOfA;

//    MathAlgorithms::Vertex vectorB;
//    vectorB.x = point3->x - point2->x;
//    vectorB.y = point3->y - point2->y;
//    vectorB.z = point3->z - point2->z;

//    //normalize B
//    float normOfB = sqrt(vectorB.x * vectorB.x + vectorB.y * vectorB.y);
//    vectorB.x /= normOfB;
//    vectorB.y /= normOfB;

//    //do cross product but only for the z component, since that is the only one we are interested in
//    float signOfCrossProduct = (vectorA.x * vectorB.y) - (vectorA.y * vectorB.x);
    //float signOfCrossProduct = (point2->x - point3->x)*(point1.y - point3->y) > (point2->y - point3->y)*(point1.x - point3->x);
    float signOfCrossProduct = (point1.x - point3->vertexCor.x) * (point2->vertexCor.y - point3->vertexCor.y) > (point2->vertexCor.x - point3->vertexCor.x) * (point1.y - point3->vertexCor.y);
    return signOfCrossProduct;
}

/*void glProgressWidget::performEmbed()
{
    for(int i = 0; i < validTriangulations.count(); i++)
    {
        validateTriangulation triangulation = validTriangulations[i];
    }
}*/

QVector<glProgressWidget::validateTriangulation> glProgressWidget::GetValidTriangulations()
{
    return validTriangulations;
}

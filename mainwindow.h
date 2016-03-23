#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtOpenGL>
#include "glmeshselectwidget.h"
#include "gltextureselectwidget.h"
#include "glfinalwidget.h"
#include "glprogresswidget.h"

#define WINDOW_TITLE "Matchmaker"
#define WINDOW_SIZE_WIDTH 800
#define WINDOW_SIZE_HEIGHT 800

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

    //need to be public so widgets can communiate with each other
    glMeshSelectWidget* glMeshWidget;
    glTextureSelectWidget* glTextureWidget;
    glProgressWidget* progressWidget;
    glFinalWidget* finalWidget;
    QAction* matchAction; //want this public, so that widgets can enable and disable it dependent on state
    QAction* embedAction;

private slots:
    void SLOT_loadMeshFile();
    void SLOT_loadTextureFile();
    void SLOT_ParameterizeMesh();
    void SLOT_TriangulateTexture();
    void SLOT_Match();
    void SLOT_Embed();

protected:
    QWidget* mainWidget; //contains basically all layout elements, can be considered the frame of the window
    QScrollArea* MeshSelectArea;
    QScrollArea* TextureSelectArea;
    QScrollArea* FinalSelectArea;
    QScrollArea* ProgressSelectArea;
    QAction* meshParameterizationAction;
    QAction* triangulateTextureAction;


public:
    static MainWindow* globalInstance;
};

#endif // MAINWINDOW_H

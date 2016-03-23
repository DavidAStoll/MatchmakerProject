#include "mainwindow.h"
#include <QFileDialog>
#include <QTextStream>

MainWindow* MainWindow::globalInstance = 0;

MainWindow::MainWindow()
{
    //create widget that is directly used by the window
    mainWidget = new QWidget();
    setCentralWidget(mainWidget);

    //-- create areas for OpenGl displays --//

    //create GL widgets
    glMeshWidget = new glMeshSelectWidget();
    glTextureWidget = new glTextureSelectWidget();
    progressWidget = new glProgressWidget();
    finalWidget = new glFinalWidget();

    //MeshSelectArea setup
    MeshSelectArea = new QScrollArea;
    MeshSelectArea->setWidget(glMeshWidget);
    MeshSelectArea->setWidgetResizable(true);
    MeshSelectArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    MeshSelectArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    MeshSelectArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    MeshSelectArea->setMinimumSize(100, 100);

    //TextureSelectArea setup
    TextureSelectArea = new QScrollArea;
    TextureSelectArea->setWidget(glTextureWidget);
    TextureSelectArea->setWidgetResizable(true);
    TextureSelectArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    TextureSelectArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    TextureSelectArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    TextureSelectArea->setMinimumSize(100, 100);

    //ProgressArea Setup
    ProgressSelectArea = new QScrollArea;
    ProgressSelectArea->setWidget(progressWidget);
    ProgressSelectArea->setWidgetResizable(true);
    ProgressSelectArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ProgressSelectArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ProgressSelectArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ProgressSelectArea->setMinimumSize(100, 100);

    //finalWidgetArea setup
    FinalSelectArea = new QScrollArea;
    FinalSelectArea->setWidget(finalWidget);
    FinalSelectArea->setWidgetResizable(true);
    FinalSelectArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    FinalSelectArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    FinalSelectArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    FinalSelectArea->setMinimumSize(100, 100);

    //-- Set what layout and add compontents --//

    //use grid layout to split screen
    QGridLayout* centralLayout = new QGridLayout;
    centralLayout->addWidget(MeshSelectArea, 0, 0);
    centralLayout->addWidget(TextureSelectArea, 0, 1);
    centralLayout->addWidget(FinalSelectArea, 1, 0);
    centralLayout->addWidget(ProgressSelectArea, 1, 1);
    mainWidget->setLayout(centralLayout);

    //add menu bar and associated options
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu* actionMenu = menuBar()->addMenu(tr("&Action"));

    //actions for file Menu
    QAction* loadMeshFileAction = new QAction(tr("Load Mesh File"), this);
    connect(loadMeshFileAction, SIGNAL(triggered()), this, SLOT(SLOT_loadMeshFile()));
    QAction* loadTextureFileAction = new QAction(tr("Load Texture File"), this);
    connect(loadTextureFileAction, SIGNAL(triggered()), this, SLOT(SLOT_loadTextureFile()));

    //actions for Action Menu
    meshParameterizationAction = new QAction(tr("Parameterize Mesh"), this);
    connect(meshParameterizationAction, SIGNAL(triggered()), this, SLOT(SLOT_ParameterizeMesh()));
    meshParameterizationAction->setEnabled(false);

    triangulateTextureAction = new QAction(tr("Triangulate Texture"), this);
    connect(triangulateTextureAction, SIGNAL(triggered()), this, SLOT(SLOT_TriangulateTexture()));
    triangulateTextureAction->setEnabled(false);

    matchAction = new QAction(tr("Match"), this);
    connect(matchAction, SIGNAL(triggered()), this, SLOT(SLOT_Match()));
    matchAction->setEnabled(false);

    embedAction = new QAction(tr("Embed"), this);
    connect(embedAction, SIGNAL(triggered()), this, SLOT(SLOT_Embed()));
    embedAction->setEnabled(false);

    //Add actions to menus
    //fileMenu
    fileMenu->addAction(loadMeshFileAction);
    fileMenu->addAction(loadTextureFileAction);
    //actionMenu
    actionMenu->addAction(meshParameterizationAction);
    actionMenu->addAction(triangulateTextureAction);
    actionMenu->addAction(matchAction);
    actionMenu->addAction(embedAction);

    //set title and size of window
    setWindowTitle(tr(WINDOW_TITLE));
    resize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);

    //all done set global pointer
    MainWindow::globalInstance = this;
}

MainWindow::~MainWindow()
{

}

void MainWindow::SLOT_loadMeshFile()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QCoreApplication::applicationDirPath());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.obj)"));
    if(dialog.exec())
    {
       QString fileName = dialog.selectedFiles()[0];
       QFile file(fileName);
       file.open(QIODevice::ReadOnly);
       QTextStream* fileStream= new QTextStream(&file);

       //call widget handle read
       glMeshWidget->makeCurrent();
       glMeshWidget->loadMeshFileCallback(fileStream);
       meshParameterizationAction->setEnabled(true);

       //all done
       delete fileStream;
       file.close();
    }
}

void MainWindow::SLOT_loadTextureFile()
{

    QFileDialog dialog(this);
    dialog.setDirectory(QCoreApplication::applicationDirPath());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
    if(dialog.exec())
    {
       //get selected file name
       QString fileName = dialog.selectedFiles()[0];

       //call widget handle read
       glTextureWidget->makeCurrent();
       glTextureWidget->loadTextureFromFile(fileName);
       triangulateTextureAction->setEnabled(true);
    }
}

void MainWindow::SLOT_ParameterizeMesh()
{
    glMeshWidget->makeCurrent();
    glMeshWidget->parameterizeMesh();
    meshParameterizationAction->setEnabled(false);
}

void MainWindow::SLOT_TriangulateTexture()
{
    glTextureWidget->makeCurrent();
    glTextureWidget->triangulatePoints();
}

void MainWindow::SLOT_Match()
{
    //just in case triangulate since user might have called match without triangulating it first
    glTextureWidget->makeCurrent();
    glTextureWidget->triangulatePoints();

    //do matching
    progressWidget->makeCurrent();
    progressWidget->performMatch();
    embedAction->setEnabled(true);
}

void MainWindow::SLOT_Embed()
{
    finalWidget->makeCurrent();
    finalWidget->performEmbed();
}

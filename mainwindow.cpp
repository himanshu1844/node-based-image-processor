#include "mainwindow.h"
#include <QVBoxLayout>

// Assuming you'll implement your node editor solution here
// You may need to include additional headers for your node editing framework

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupNodeEditor();
    setupDockWidgets();

    // Set window title and initial size
    setWindowTitle("Node Editor");
    resize(1200, 800);
}

MainWindow::~MainWindow()
{
    // Any cleanup code here
}

void MainWindow::setupNodeEditor()
{
    // Replace this with your actual node editor setup code
    // This is just a placeholder widget
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->addWidget(new QTextEdit("Node Editor Area"));
    setCentralWidget(centralWidget);
}

void MainWindow::setupDockWidgets()
{
    // Create left dock widget
    auto *leftDock = new QDockWidget("Left Panel", this);
    leftDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    leftDock->setWidget(new QTextEdit("Settings / File Load", leftDock));
    addDockWidget(Qt::LeftDockWidgetArea, leftDock);

    // Create right dock widget
    auto *rightDock = new QDockWidget("Right Panel", this);
    rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
    rightDock->setWidget(new QTextEdit("Properties / Inspector", rightDock));
    addDockWidget(Qt::RightDockWidgetArea, rightDock);
}

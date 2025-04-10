#include "mainWindow.h"
#include "InputNode.h"

#include <QMenu>
#include <QContextMenuEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setSceneRect(0, 0, 1600, 1000);
    setCentralWidget(view);
    resize(1200, 800);
    setWindowTitle("Node Canvas");
}

MainWindow::~MainWindow() {}

void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    QAction *addInput = menu.addAction("Add Input Node");

    QAction *selectedAction = menu.exec(event->globalPos());
    if (selectedAction == addInput) {
        addInputNode(event->globalPos());
    }
}

void MainWindow::addInputNode(const QPoint &globalPos) {
    QPointF scenePos = view->mapToScene(view->mapFromGlobal(globalPos));
    auto *node = new InputNode();
    node->setPos(scenePos);
    scene->addItem(node);
}

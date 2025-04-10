#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QGraphicsScene *scene;
    QGraphicsView *view;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void addInputNode(const QPoint &globalPos);
};

#endif // MAINWINDOW_H

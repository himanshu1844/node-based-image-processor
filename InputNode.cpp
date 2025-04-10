#include "InputNode.h"
#include <QPainter>
#include <QFileDialog>

InputNode::InputNode() {
    setFlags(ItemIsMovable | ItemIsSelectable);
}

QRectF InputNode::boundingRect() const {
    return QRectF(0, 0, 150, 120);
}

void InputNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // painter->setBrush(Qt::white);
    painter->drawRect(0, 0, 150, 30);
    painter->drawText(10, 20, "Input Node");

    if (!image.isNull())
        painter->drawPixmap(10, 40, 200, 100, image);
    else
        painter->drawText(30, 80, "Double-click to load");
}

void InputNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *) {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Image", "", "Images (*.png *.jpg)");
    if (!fileName.isEmpty()) {
        image.load(fileName);
        update();
    }
}

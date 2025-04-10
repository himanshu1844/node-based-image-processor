#ifndef INPUTNODE_H
#define INPUTNODE_H

#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>

class InputNode : public QGraphicsItem {
public:
    InputNode();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPixmap image;
};

#endif // INPUTNODE_H

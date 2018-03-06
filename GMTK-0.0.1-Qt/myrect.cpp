#include "myrect.h"
#include <QGraphicsScene>

Myrect::Myrect()
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemSendsGeometryChanges|
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsFocusable);
}

Myrect::Myrect(const QRect &rect, QGraphicsScene *scene)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemSendsGeometryChanges|
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsFocusable);
    setPos(rect.center());
    setRect(QRectF(QPointF(-rect.width() / 2.0,
                           -rect.height() / 2.0), rect.size()));
    scene->clearSelection();
    scene->addItem(this);
    setSelected(true);
    setFocus();
}




QDataStream &operator<<(QDataStream &out, const Myrect &boxItem)
{
    out << boxItem.pos()
        << boxItem.zValue() << boxItem.rect();
    return out;
}


QDataStream &operator>>(QDataStream &in, Myrect &boxItem)
{
    QPointF position;
    double z;
    QRectF rect;
//    QPen pen;
//    QBrush brush;
    in >> position >> z
       >> rect;
    boxItem.setPos(position);
//    boxItem.setAngle(angle);
//    boxItem.setShear(shearHorizontal, shearVertical);
    boxItem.setZValue(z);
    boxItem.setRect(rect);
//    boxItem.setPen(pen);
//    boxItem.setBrush(brush);
    return in;
}

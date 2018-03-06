#ifndef MYRECT_H
#define MYRECT_H

#include "itemtypes.h"

#include <QtCore>
#include <QtGui>
#include <QGraphicsRectItem>

class QDataStream;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QKeyEvent;


class Myrect: public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    enum {Type = BoxItemType};

    Myrect();
    explicit Myrect(const QRect &rect, QGraphicsScene *scene);
    int type() const { return Type; }

};

QDataStream &operator<<(QDataStream &out, const Myrect &boxItem);
QDataStream &operator>>(QDataStream &in, Myrect &boxItem);

#endif // MYRECT_H

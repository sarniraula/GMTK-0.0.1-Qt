#ifndef MYLINE_H
#define MYLINE_H

#include "itemtypes.h"

#include <QtCore>
#include <QtGui>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class Myline: public QGraphicsLineItem
{
public:
    Myline();

    QRectF boundingRect();

};

#endif // MYLINE_H

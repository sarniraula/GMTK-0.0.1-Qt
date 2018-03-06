#ifndef MYCIRCLE_H
#define MYCIRCLE_H

#include <QtCore>
#include <QtGui>
#include <QGraphicsItem>

class Mycircle: public QGraphicsEllipseItem
{
public:
    Mycircle();

    QRectF boundingRect();
};

#endif // MYCIRCLE_H

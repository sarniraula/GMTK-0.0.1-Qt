#ifndef SCENE_H
#define SCENE_H

#include "myline.h"
#include "myrect.h"
#include "mycircle.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QAction>
#include <QGraphicsView>
#include <QKeyEvent>

class Scene : public QGraphicsScene
{
public:
    enum Mode {NoMode, SelectObject, DrawLine, DrawRect, DrawCircle};
    Scene(QObject* parent = 0);
    void setMode(Mode mode);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    Mode sceneMode;
    QPointF origPoint;
    //QGraphicsLineItem* itemToDraw;
    Myline* itemToDraw;
    //QGraphicsRectItem* rectToDraw;
    Myrect* rectToDraw;
    //QGraphicsEllipseItem* circleToDraw;
    Mycircle* circleToDraw;
    void makeItemsControllable(bool areControllable);

};

#endif // SCENE_H

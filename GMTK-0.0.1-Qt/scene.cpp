#include "scene.h"

Scene::Scene(QObject* parent): QGraphicsScene(parent)
{
    sceneMode = NoMode;
    itemToDraw = 0;
    rectToDraw = 0;
    circleToDraw = 0;
}

void Scene::setMode(Mode mode){
    sceneMode = mode;
    QGraphicsView::DragMode vMode =
            QGraphicsView::NoDrag;
    if(mode == DrawLine){
        makeItemsControllable(false);
        vMode = QGraphicsView::NoDrag;
    }
    else if(mode == SelectObject){
        makeItemsControllable(true);
        vMode = QGraphicsView::RubberBandDrag;
    }
    QGraphicsView* mView = views().at(0);
    if(mView)
        mView->setDragMode(vMode);
}

void Scene::makeItemsControllable(bool areControllable){
    foreach(QGraphicsItem* item, items()){
        item->setFlag(QGraphicsItem::ItemIsSelectable,
                      areControllable);
        item->setFlag(QGraphicsItem::ItemIsMovable,
                      areControllable);
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(sceneMode == DrawLine){
        origPoint = event->scenePos();
    }
    else if(sceneMode == DrawRect){
        origPoint = event->scenePos();
    }
    else if(sceneMode == DrawCircle){
        origPoint = event->scenePos();
    }
    QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(sceneMode == DrawLine){
        if(!itemToDraw){
            itemToDraw = new Myline;
            this->addItem(itemToDraw);
            itemToDraw->setPen(QPen(Qt::black, 3, Qt::SolidLine));
            itemToDraw->setPos(origPoint);
        }
        itemToDraw->setLine(0,0,
                            event->scenePos().x() - origPoint.x(),
                            event->scenePos().y() - origPoint.y());
    }
    else if(sceneMode == DrawRect){
        if(!rectToDraw){
            rectToDraw = new Myrect;
            this->addItem(rectToDraw);
            rectToDraw->setPen(QPen(Qt::black, 3));
            //rectToDraw->setPos(origPoint);

        }
        int w, h;
        w = event->scenePos().x()-origPoint.x();
        h = event->scenePos().y()-origPoint.y();


        if(w>=0 && h>=0){
        rectToDraw->setRect(origPoint.x(), origPoint.y(), w,h);
        }
        else if(w>=0 && h<0){
            rectToDraw->setRect(origPoint.x(), event->scenePos().y(), w, -h);
        }
        else if(w<0 && h>=0){
            rectToDraw->setRect(event->scenePos().x(), origPoint.y(), -w, h);
        }
        else if(w<0 && h<0){
            rectToDraw->setRect(event->scenePos().x(), event->scenePos().y(), -w, -h);
        }
    }
    else if(sceneMode == DrawCircle){
        if(!circleToDraw){
            circleToDraw = new Mycircle;
            this->addItem(circleToDraw);
            circleToDraw->setPen(QPen(Qt::black, 3));
        }
        int w, h;
        w = event->scenePos().x()-origPoint.x();
        h = event->scenePos().y()-origPoint.y();


        circleToDraw->setRect(origPoint.x(), origPoint.y(), w,h);
    }
    else
        QGraphicsScene::mouseMoveEvent(event);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    itemToDraw = 0;
    rectToDraw = 0;
    circleToDraw = 0;
    QGraphicsScene::mouseReleaseEvent(event);
}

void Scene::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Delete)
        foreach(QGraphicsItem* item, selectedItems()){
            removeItem(item);
            delete item;
        }
    else
        QGraphicsScene::keyPressEvent(event);
}

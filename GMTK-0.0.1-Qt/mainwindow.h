#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "scene.h"
#include "GraphicsView.h"

#include <QPrinter>
#include <QMainWindow>
#include <QGraphicsView>
#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QApplication>


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    QSize sizeHint() const;

    QString filenameFilter(const QString &name,
                           const QList<QByteArray> formats);

public slots:
    void setDirty(bool on=true);

private slots:

    void fileNew();
    void fileOpen();

    void actionGroupClicked(QAction*);
    bool fileSave();
    bool fileSaveAs();
    void fileExport();
    void filePrint();
    void loadFile();

    void updateUi();

private:
    void createSceneAndView();
    void createActions();
    void createMenusAndToolBars();
    void populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar,
                                QList<QAction*> actions);
    void createConnections();

    bool okToClearData();

    template<typename T>
    bool okToClearDataMessage(bool (T::*saveData)(), T *parent,
            const QString &title, const QString &text,
            const QString &detailedText=QString())
    {
        Q_ASSERT(saveData && parent);
        QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));

        messageBox->setWindowModality(Qt::WindowModal);
        messageBox->setIcon(QMessageBox::Question);
        messageBox->setWindowTitle(QString("%1 - %2")
                .arg(QApplication::applicationName()).arg(title));
        messageBox->setText(text);
        if (!detailedText.isEmpty())
            messageBox->setInformativeText(detailedText);
        messageBox->addButton(QMessageBox::Save);
        messageBox->addButton(QMessageBox::Discard);
        messageBox->addButton(QMessageBox::Cancel);
        messageBox->setDefaultButton(QMessageBox::Save);
        messageBox->exec();
        if (messageBox->clickedButton() ==
            messageBox->button(QMessageBox::Cancel))
            return false;
        if (messageBox->clickedButton() ==
            messageBox->button(QMessageBox::Save))
            return (parent->*saveData)();
        return true;
    }

    void connectItem(QObject *item);

    bool openGeometryToolkitFile(QFile *file, QDataStream &in);
    void readItems(QDataStream &in, int offset=0, bool select=false);
    void writeItems(QDataStream &out,
                    const QList<QGraphicsItem*> &items);

    void copyItems(const QList<QGraphicsItem*> &items);
    void selectItems(const QSet<QGraphicsItem*> &items);

    void exportImage(const QString &filename);
    void exportSvg(const QString &filename);
    void paintScene(QPainter *painter);
    void populateCoordinates(const Qt::Alignment &alignment,
            QVector<double> *coordinates,
            const QList<QGraphicsItem*> &items);

    bool sceneHasItems() const;



    QAction *fileNewAction;
    QAction *fileOpenAction;
    QAction *fileExportAction;
    QAction *fileSaveAction;
    QAction *fileSaveAsAction;
    QAction *filePrintAction;
    QAction *fileQuitAction;

    QAction *editCopyAction;
    QAction *editCutAction;
    QAction *editPasteAction;

    QAction *viewZoomInAction;
    QAction *viewZoomOutAction;

    QAction* lineAction;
    QAction* selectAction;
    QAction* rectAction;
    QAction* circleAction;
    QActionGroup *actionGroup;
    QToolBar* drawingToolBar;

    QPrinter *printer;
    Scene *scene;
    GraphicsView *view;
    QPoint previousPoint;

};

#endif // MAINWINDOW_H

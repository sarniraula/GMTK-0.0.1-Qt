#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QImageWriter>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>


#include <QPrintDialog>
#include <QSettings>
#include <QStatusBar>
#include <QSvgGenerator>
#include <QTimer>
#include <QToolBar>

#include <cmath>
#include <limits>

namespace {

const int StatusTimeout = 3000;
const int OffsetIncrement = 5;
const qint32 MagicNumber = 0x5A93DE5;
const qint16 VersionNumber = 1;
const QString ShowGrid("ShowGrid");
const QString MostRecentFile("MostRecentFile");
//const QString MimeType = "application/vnd.qtrac.pagedesigner";
}

MainWindow::MainWindow()
{
     printer = new QPrinter(QPrinter::HighResolution);

    createSceneAndView();
    createActions();
    createConnections();
    createMenusAndToolBars();

    QSettings settings;
    QString filename = settings.value(MostRecentFile).toString();
    if (filename.isEmpty() || filename == tr("Unnamed"))
        QTimer::singleShot(0, this, SLOT(fileNew()));
    else {
        setWindowFilePath(filename);
        QTimer::singleShot(0, this, SLOT(loadFile()));
    }
}

QSize MainWindow::sizeHint() const
{
    QSize size = printer->paperSize(QPrinter::Point).toSize() * 1.2;
    return size.boundedTo(
            QApplication::desktop()->availableGeometry().size());
}

QString MainWindow::filenameFilter(const QString &name, const QList<QByteArray> formats)
{
    QStringList fileFormatList;
    foreach (const QByteArray &ba, formats)
        fileFormatList += ba;
    if (fileFormatList.isEmpty())
        fileFormatList << "*";
    else
        fileFormatList.sort();
    QString fileFormats = QString("%1 (*.").arg(name);
    fileFormats += fileFormatList.join(" *.") + ")";
    return fileFormats;
}


void MainWindow::createSceneAndView()
{
    view = new GraphicsView;
    scene = new Scene(this);
    scene->setSceneRect(0,0,200,200);
    view->setScene(scene);
    view->setRenderHints(QPainter::Antialiasing);
    setCentralWidget(view);
}

void MainWindow::createActions()
{
    fileNewAction = new QAction(QIcon(":/images/file.png"), tr("New..."), this);
    fileNewAction->setShortcuts(QKeySequence::New);

    fileOpenAction = new QAction(QIcon(":/images/open.png"), tr("Open..."), this);
    fileOpenAction->setShortcuts(QKeySequence::Open);

    fileSaveAction = new QAction(QIcon(":/images/save.png"), tr("Save"), this);
    fileSaveAction->setShortcuts(QKeySequence::Save);

    fileSaveAsAction = new QAction(QIcon(":/images/save.png"), tr("Save As..."), this);
    fileSaveAsAction->setShortcuts(QKeySequence::SaveAs);

    fileExportAction = new QAction(QIcon(":/images/export.png"), tr("Export..."), this);
    filePrintAction = new QAction(QIcon(":/images/print.png"), tr("Print..."), this);

    fileQuitAction = new QAction(QIcon(":/images/exit.png"), tr("Quit"), this);
    fileQuitAction->setShortcuts(QKeySequence::Quit);



    lineAction = new QAction("Draw line", this);
    lineAction->setData(int(Scene::DrawLine));
    lineAction->setIcon(QIcon(":/images/line.png"));
    lineAction->setCheckable(true);

    selectAction = new QAction("Select object", this);
    selectAction->setData(int(Scene::SelectObject));
    selectAction->setIcon(QIcon(":/images/selector.png"));
    selectAction->setCheckable(true);

    rectAction = new QAction("Draw Rectangle", this);
    rectAction->setData(int(Scene::DrawRect));
    rectAction->setIcon(QIcon(":/images/rect.png"));
    rectAction->setCheckable(true);

    circleAction = new QAction("Draw Circle", this);
    circleAction->setData((int(Scene::DrawCircle)));
    circleAction->setIcon(QIcon(":/images/circle.png"));
    circleAction->setCheckable(true);

    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    actionGroup->addAction(lineAction);
    actionGroup->addAction(selectAction);
    actionGroup->addAction(rectAction);
    actionGroup->addAction(circleAction);



    viewZoomInAction = new QAction(QIcon(":/images/zoomin.png"),
                                   tr("Zoom In"), this);
    viewZoomInAction->setShortcut(tr("+"));
    viewZoomOutAction = new QAction(QIcon(":/images/zoomout.png"),
                                    tr("Zoom Out"), this);
    viewZoomOutAction->setShortcut(tr("-"));
}

void MainWindow::createMenusAndToolBars()
{
    QAction *separator = 0;

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    populateMenuAndToolBar(fileMenu, fileToolBar, QList<QAction*>()
            << fileNewAction << fileOpenAction << fileSaveAction );
    fileMenu->addAction(fileSaveAsAction);

    populateMenuAndToolBar(fileMenu, fileToolBar, QList<QAction*>()
            << separator << fileExportAction << filePrintAction);
    fileMenu->addSeparator();
    fileMenu->addAction(fileQuitAction);

    QMenu *addItemMenu = menuBar()->addMenu(tr("Add"));
    QToolBar *addItemToolBar = addToolBar(tr("Add"));
    populateMenuAndToolBar(addItemMenu, addItemToolBar, QList<QAction*>()
            << selectAction << separator << lineAction << rectAction << circleAction);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));
    QToolBar *viewToolBar = addToolBar(tr("View"));
    populateMenuAndToolBar(viewMenu, viewToolBar, QList<QAction*>()
            << viewZoomInAction << viewZoomOutAction);
}


void MainWindow::populateMenuAndToolBar(QMenu *menu,
        QToolBar *toolBar, QList<QAction*> actions)
{
    foreach (QAction *action, actions) {
        if (!action) {
            menu->addSeparator();
            toolBar->addSeparator();
        }
        else {
            menu->addAction(action);
            toolBar->addAction(action);
        }
    }
}


void MainWindow::createConnections()
{
    connect(fileNewAction, SIGNAL(triggered()),
            this, SLOT(fileNew()));
    connect(fileOpenAction, SIGNAL(triggered()),
            this, SLOT(fileOpen()));
    connect(fileSaveAction, SIGNAL(triggered()),
            this, SLOT(fileSave()));
    connect(fileSaveAsAction, SIGNAL(triggered()),
            this, SLOT(fileSaveAs()));
    connect(fileExportAction, SIGNAL(triggered()),
            this, SLOT(fileExport()));
    connect(filePrintAction, SIGNAL(triggered()),
            this, SLOT(filePrint()));
    connect(fileQuitAction, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(actionGroup, SIGNAL(triggered(QAction*)),
            this, SLOT(actionGroupClicked(QAction*)));

    connect(scene, SIGNAL(selectionChanged()),
            this, SLOT(selectionChanged()));
    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
            this, SLOT(updateUi()));

    connect(viewZoomInAction, SIGNAL(triggered()),
            view, SLOT(zoomIn()));
    connect(viewZoomOutAction, SIGNAL(triggered()),
            view, SLOT(zoomOut()));
}

void MainWindow::actionGroupClicked(QAction *action){
    scene->setMode(Scene::Mode(action->data().toInt()));
}

void MainWindow::setDirty(bool on)
{
    setWindowModified(on);
//    updateUi();
}

void MainWindow::updateUi()
{
    fileSaveAction->setEnabled(isWindowModified());
    bool hasItems = sceneHasItems();
    fileSaveAsAction->setEnabled(hasItems);
    fileExportAction->setEnabled(hasItems);
    filePrintAction->setEnabled(hasItems);
//    int selected = scene->selectedItems().count();
//    editSelectedItemAction->setEnabled(selected == 1);
//    editCopyAction->setEnabled(selected >= 1);
//    editCutAction->setEnabled(selected >= 1);
//    QClipboard *clipboard = QApplication::clipboard();
    /*const QMimeData *mimeData = clipboard->mimeData();
    editPasteAction->setEnabled(mimeData &&
            (mimeData->hasFormat(MimeType) || mimeData->hasHtml() ||
             mimeData->hasText()));*/
}


bool MainWindow::sceneHasItems() const
{
    foreach (QGraphicsItem *item, scene->items())
        return true;
    return false;
}

void MainWindow::fileNew()
{
    if (!okToClearData())
        return;
    scene->clear();
    setWindowFilePath(tr("Unnamed"));
    setDirty(false);
}


void MainWindow::fileOpen()
{
    if (!okToClearData())
        return;
    const QString &filename = QFileDialog::getOpenFileName(this,
            tr("%1 - Open").arg(QApplication::applicationName()),
            ".", tr("Geometry ToolKit (*.gmtk)"));
    if (filename.isEmpty())
        return;
    setWindowFilePath(filename);
    loadFile();
}


bool MainWindow::okToClearData()
{
    if (isWindowModified())
        return okToClearDataMessage(&MainWindow::fileSave, this,
                tr("Unsaved changes"), tr("Save unsaved changes?"));
    return true;
}


void MainWindow::loadFile()
{
    QFile file(windowFilePath());
    QDataStream in;
    if (!openGeometryToolkitFile(&file, in))
        return;
    in.setVersion(QDataStream::Qt_5_3);
    scene->clear();
    readItems(in);
//    statusBar()->showMessage(tr("Loaded %1").arg(windowFilePath()),
//                             StatusTimeout);
    setDirty(false);
}

bool MainWindow::openGeometryToolkitFile(QFile *file, QDataStream &in)
{
    if (!file->open(QIODevice::ReadOnly)) {
//        AQP::warning(this, tr("Error"), tr("Failed to open file: %1")
//                                        .arg(file->errorString()));
        return false;
    }
    in.setDevice(file);
    qint32 magicNumber;
    in >> magicNumber;
    if (magicNumber != MagicNumber) {
//        AQP::warning(this, tr("Error"),
//                tr("%1 is not a %2 file").arg(file->fileName())
//                .arg(QApplication::applicationName()));
        return false;
    }
    qint16 versionNumber;
    in >> versionNumber;
    if (versionNumber > VersionNumber) {
//        AQP::warning(this, tr("Error"),
//                tr("%1 needs a more recent version of %2")
//                .arg(file->fileName())
//                .arg(QApplication::applicationName()));
        return false;
    }
}


void MainWindow::selectItems(const QSet<QGraphicsItem*> &items)
{
    scene->clearSelection();
    foreach (QGraphicsItem *item, items)
        item->setSelected(true);
}


void MainWindow::fileExport()
{
    QString suffixes = filenameFilter(tr("Bitmap image"),
            QImageWriter::supportedImageFormats());
    suffixes += tr(";;Vector image (*.svg)");
    const QString filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Export").arg(QApplication::applicationName()),
            ".", suffixes);
    if (filename.isEmpty())
        return;
    if (filename.toLower().endsWith(".svg"))
        exportSvg(filename);
    else
        exportImage(filename);
}

bool MainWindow::fileSave()
{
    const QString filename = windowFilePath();
    if (filename.isEmpty() || filename == tr("Unnamed"))
        return fileSaveAs();
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file);
    out << MagicNumber << VersionNumber;
    out.setVersion(QDataStream::Qt_5_3);
    writeItems(out, scene->items());
    file.close();
    setDirty(false);
    return true;
}

bool MainWindow::fileSaveAs()
{
    QString filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Save As").arg(QApplication::applicationName()),
            ".", tr("Geometry Toolkit (*.gmtk)"));
    if (filename.isEmpty())
        return false;
    if (!filename.toLower().endsWith(".gmtk"))
        filename += ".gmtk";
    setWindowFilePath(filename);
    return fileSave();
}

void MainWindow::readItems(QDataStream &in, int offset, bool select)
{
    QSet<QGraphicsItem*> items;
    qint32 itemType;
    QGraphicsItem *item = 0;
    while (!in.atEnd()) {
        in >> itemType;
        switch (itemType) {
            case BoxItemType: {
                Myrect *boxItem = new Myrect(QRect(), scene);
                in >> *boxItem;
                //connectItem(boxItem);
                item = boxItem;
                break;
            }
        }
        if (item) {
            item->moveBy(offset, offset);
            if (select)
                items << item;
            item = 0;
        }
    }
    if (select)
        selectItems(items);
//    else
//        selectionChanged();
}

void MainWindow::writeItems(QDataStream &out,
                            const QList<QGraphicsItem*> &items)
{
    foreach (QGraphicsItem *item, items) {
        qint32 type = static_cast<qint32>(item->type());
        out << type;
        switch (type) {
            case BoxItemType:
                    out << *static_cast<Myrect*>(item); break;
            default: Q_ASSERT(false);
        }
    }
}


void MainWindow::exportSvg(const QString &filename)
{
    QSvgGenerator svg;
    svg.setFileName(filename);
    svg.setSize(printer->paperSize(QPrinter::Point).toSize());
    {
        QPainter painter(&svg);
        paintScene(&painter);
    }
    statusBar()->showMessage(tr("Exported %1").arg(filename),
                             StatusTimeout);
}


void MainWindow::exportImage(const QString &filename)
{
    QImage image(printer->paperSize(QPrinter::Point).toSize(),
                 QImage::Format_ARGB32);
    {
        QPainter painter(&image);
        painter.setRenderHints(QPainter::Antialiasing|
                               QPainter::TextAntialiasing);
        paintScene(&painter);
    }
    if (image.save(filename))
        statusBar()->showMessage(tr("Exported %1").arg(filename),
                                 StatusTimeout);
    else
       QMessageBox::warning(this, tr("Error"), tr("Failed to export: %1")
                                        .arg(filename));
}


void MainWindow::paintScene(QPainter *painter)
{

    QList<QGraphicsItem*> items = scene->selectedItems();
    scene->clearSelection();

    scene->render(painter);

    foreach (QGraphicsItem *item, items)
        item->setSelected(true);
    //selectionChanged();
}


void MainWindow::filePrint()
{
    QPrintDialog dialog(printer);
    if (dialog.exec()) {
        {
            QPainter painter(printer);
            paintScene(&painter);
        }
        statusBar()->showMessage(tr("Printed %1")
                .arg(windowFilePath()), StatusTimeout);
    }
}



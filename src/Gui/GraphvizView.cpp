/***************************************************************************
 *   Copyright (c) 2014 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/

# include <QApplication>
# include <QFile>
# include <QPrinter>
# include <QPrintDialog>
# include <QPrintPreviewDialog>
# include <QProcess>
# include <QSvgRenderer>
# include <QGraphicsSvgItem>
# include <QMessageBox>
# include <QGraphicsScene>
# include <QGraphicsView>
# include <QThread>
# include <QProcess>
# include <boost/bind/bind.hpp>
#include "GraphicsViewZoom.h"
#include "FileDialog.h"


#include "GraphvizView.h"
#include "Application.h"
#include "MainWindow.h"
#include "App/Application.h"
#include "App/Document.h"

using namespace Gui;
namespace bp = boost::placeholders;

namespace Gui {

/**
 * @brief The GraphvizWorker class
 *
 * Implements a QThread class that does the actual conversion from dot to
 * svg. All critical communication is done using queued signals.
 *
 */

class GraphvizWorker : public QThread {
    Q_OBJECT
public:
    GraphvizWorker(QObject * parent = 0)
        : QThread(parent)
    {
    }

    virtual ~GraphvizWorker()
    {
        dotProc.moveToThread(this);
        unflattenProc.moveToThread(this);
    }

    void setData(const QByteArray & data)
    {
        str = data;
    }

    void startThread() {
        // This doesn't actually run a thread but calls the function
        // directly in the main thread.
        // This is needed because embedding a QProcess into a QThread
        // causes some problems with Qt5.
        run();
        // Can't use the finished() signal of QThread
        emitFinished();
    }

    void run() {
        QByteArray preprocessed = str;
        
        ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/DependencyGraph");
        if(hGrp->GetBool("Unflatten", true)) {
            // Write data to unflatten process
            unflattenProc.write(str);
            unflattenProc.closeWriteChannel();
            //no error handling: unflatten is optional
            unflattenProc.waitForFinished();
                preprocessed = unflattenProc.readAll();
        } else {
            unflattenProc.closeWriteChannel();
            unflattenProc.waitForFinished();
        }
        
        dotProc.write(preprocessed);
        dotProc.closeWriteChannel();
        if (!dotProc.waitForFinished()) {
            Q_EMIT error();
            quit();
        }

        // Emit result; it will get queued for processing in the main thread
        Q_EMIT svgFileRead(dotProc.readAll());
    }

    QProcess * dotProcess() {
        return &dotProc;
    }
    
    QProcess * unflattenProcess() {
        return &unflattenProc;
    }

Q_SIGNALS:
    void svgFileRead(const QByteArray & data);
    void error();
    void emitFinished();

private:
    QProcess dotProc, unflattenProc;
    QByteArray str, flatStr;
};

}

/* TRANSLATOR Gui::GraphvizView */

GraphvizView::GraphvizView(App::Document & _doc, QWidget* parent)
  : MDIView(0, parent)
  , doc(_doc)
  , nPending(0)
{
    // Create scene
    scene = new QGraphicsScene();

    // Create item to hold the graph
    svgItem = new QGraphicsSvgItem();
    renderer = new QSvgRenderer(this);
    svgItem->setSharedRenderer(renderer);
    scene->addItem(svgItem);

    // Create view and zoomer object
    view = new QGraphicsView(scene, this);
    zoomer = new GraphicsViewZoom(view);
    zoomer->set_modifiers(Qt::NoModifier);
    view->show();

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath
            ("User parameter:BaseApp/Preferences/View");
    bool on = hGrp->GetBool("InvertZoom", true);
    zoomer->set_zoom_inverted(on);

    // Set central widget to view
    setCentralWidget(view);

    // Create worker thread
    thread = new GraphvizWorker(this);
    connect(thread, SIGNAL(emitFinished()), this, SLOT(done()));
    connect(thread, SIGNAL(finished()), this, SLOT(done()));
    connect(thread, SIGNAL(error()), this, SLOT(error()));
    connect(thread, SIGNAL(svgFileRead(const QByteArray &)), this, SLOT(svgFileRead(const QByteArray &)));

    // Connect signal from document
    recomputeConnection = _doc.signalRecomputed.connect(boost::bind(&GraphvizView::updateSvgItem, this, bp::_1));
    undoConnection = _doc.signalUndo.connect(boost::bind(&GraphvizView::updateSvgItem, this, bp::_1));
    redoConnection = _doc.signalRedo.connect(boost::bind(&GraphvizView::updateSvgItem, this, bp::_1));

    updateSvgItem(_doc);
}

GraphvizView::~GraphvizView()
{
    delete scene;
    delete view;
}

void GraphvizView::updateSvgItem(const App::Document &doc)
{
    nPending++;

    // Skip if thread is working now
    if (nPending > 1)
        return;

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Paths");
    QProcess * dotProc = thread->dotProcess();
    QProcess * flatProc = thread->unflattenProcess();
    QStringList args, flatArgs;
    args << QString("-Tsvg");
    flatArgs << QString("-c2 -l2");

#ifdef FC_OS_LINUX
    QString path = QString(hGrp->GetASCII("Graphviz", "/usr/bin").c_str());
#else
    QString path = QString(hGrp->GetASCII("Graphviz").c_str());
#endif
    bool pathChanged = false;
#ifdef FC_OS_WIN32
    QString dot = QString("\"%1/dot\"").arg(path);
    QString unflatten = QString("\"%1/unflatten\"").arg(path);
#else
    QString dot = QString("%1/dot").arg(path);
    QString unflatten = QString("%1/unflatten").arg(path);
#endif
    dotProc->setEnvironment(QProcess::systemEnvironment());
    flatProc->setEnvironment(QProcess::systemEnvironment());
    do {
        flatProc->start(unflatten, flatArgs);
        bool value = flatProc->waitForStarted();
        Q_UNUSED(value); // quieten code analyzer
        dotProc->start(dot, args);
        if (!dotProc->waitForStarted()) {
            int ret = QMessageBox::warning(Gui::getMainWindow(),
                                           tr("Graphviz not found"),
                                           QString("<html><head/><body>%1 "
                                                               "<a href=\"https://www.freecadweb.org/wiki/Std_DependencyGraph\">%2"
                                                               "</a><p>%3</p></body></html>")
                                           .arg(tr("Graphviz couldn't be found on your system."),
                                                tr("Read more about it here."),
                                                tr("Do you want to specify its installation path if it's already installed?")),
                                           QMessageBox::Yes, QMessageBox::No);
            if (ret == QMessageBox::No) {
                disconnectSignals();
                return;
            }
            path = QFileDialog::getExistingDirectory(Gui::getMainWindow(),
                                                     tr("Graphviz installation path"));
            if (path.isEmpty()) {
                disconnectSignals();
                return;
            }
            pathChanged = true;
#ifdef FC_OS_WIN32
            dot = QString("\"%1/dot\"").arg(path);
            unflatten = QString("\"%1/unflatten\"").arg(path);
#else
            dot = QString("%1/dot").arg(path);
            unflatten = QString("%1/unflatten").arg(path);
#endif
        }
        else {
            if (pathChanged)
                hGrp->SetASCII("Graphviz", (const char*)path.toUtf8());
            break;
        }
    }
    while(true);

    // Create graph in dot format
    std::stringstream stream;
    doc.exportGraphviz(stream);
    graphCode = stream.str();

    // Update worker thread, and start it
    thread->setData(QByteArray(graphCode.c_str(), graphCode.size()));
    thread->startThread();
}

void GraphvizView::svgFileRead(const QByteArray & data)
{
    // Update renderer with new SVG file, and give message if something went wrong
    if (renderer->load(data))
        svgItem->setSharedRenderer(renderer);
    else {
        QMessageBox::warning(getMainWindow(),
                             tr("Graphviz failed"),
                             tr("Graphviz failed to create an image file"));
        disconnectSignals();
    }
}

void GraphvizView::error()
{
    // If the worker fails for some reason, stop giving it more data later
    disconnectSignals();
}

void GraphvizView::done()
{
    nPending--;
    if (nPending > 0) {
        nPending = 0;
        updateSvgItem(doc);
        thread->startThread();
    }
}

void GraphvizView::disconnectSignals()
{
    recomputeConnection.disconnect();
    undoConnection.disconnect();
    redoConnection.disconnect();
}

#include <QObject>
#include <QGraphicsView>

QByteArray GraphvizView::exportGraph(const QString& format)
{
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Paths");
    QProcess dotProc, flatProc;
    QStringList args, flatArgs;
    args << QString("-T%1").arg(format);
    flatArgs << QString("-c2 -l2");

#ifdef FC_OS_LINUX
    QString path = QString(hGrp->GetASCII("Graphviz", "/usr/bin").c_str());
#else
    QString path = QString(hGrp->GetASCII("Graphviz").c_str());
#endif

#ifdef FC_OS_WIN32
    QString exe = QString("\"%1/dot\"").arg(path);
    QString unflatten = QString("\"%1/unflatten\"").arg(path);
#else
    QString exe = QString("%1/dot").arg(path);
    QString unflatten = QString("%1/unflatten").arg(path);
#endif
       
    dotProc.setEnvironment(QProcess::systemEnvironment());
    dotProc.start(exe, args);
    if (!dotProc.waitForStarted()) {
        return QByteArray();
    }
    
    ParameterGrp::handle depGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/DependencyGraph");
    if(depGrp->GetBool("Unflatten", true)) {
        flatProc.setEnvironment(QProcess::systemEnvironment());
        flatProc.start(unflatten, flatArgs);
        if (!flatProc.waitForStarted()) {
            return QByteArray();
        }
        flatProc.write(graphCode.c_str(), graphCode.size());
        flatProc.closeWriteChannel();
        if (!flatProc.waitForFinished())
            return QByteArray();
        
        dotProc.write(flatProc.readAll());
    }
    else 
        dotProc.write(graphCode.c_str(), graphCode.size());   
    
    dotProc.closeWriteChannel();
    if (!dotProc.waitForFinished())
        return QByteArray();

    return dotProc.readAll();
}

bool GraphvizView::onMsg(const char* pMsg,const char**)
{
    if (strcmp("Save",pMsg) == 0 || strcmp("SaveAs",pMsg) == 0) {
        QList< QPair<QString, QString> > formatMap;
        formatMap << qMakePair(QString("%1 (*.png)").arg(tr("PNG format")), QString("png"));
        formatMap << qMakePair(QString("%1 (*.bmp)").arg(tr("Bitmap format")), QString("bmp"));
        formatMap << qMakePair(QString("%1 (*.gif)").arg(tr("GIF format")), QString("gif"));
        formatMap << qMakePair(QString("%1 (*.jpg)").arg(tr("JPG format")), QString("jpg"));
        formatMap << qMakePair(QString("%1 (*.svg)").arg(tr("SVG format")), QString("svg"));
        formatMap << qMakePair(QString("%1 (*.pdf)").arg(tr("PDF format")), QString("pdf"));
      //formatMap << qMakePair(tr("VRML format (*.vrml)"), QString("vrml"));

        QStringList filter;
        for (QList< QPair<QString, QString> >::iterator it = formatMap.begin(); it != formatMap.end(); ++it)
            filter << it->first;

        QString selectedFilter;
        QString fn = Gui::FileDialog::getSaveFileName(this, tr("Export graph"), QString(), filter.join(QString(";;")), &selectedFilter);
        if (!fn.isEmpty()) {
            QString format;
            for (QList< QPair<QString, QString> >::iterator it = formatMap.begin(); it != formatMap.end(); ++it) {
                if (selectedFilter == it->first) {
                    format = it->second;
                    break;
                }
            }
            QByteArray buffer = exportGraph(format);
            if (buffer.isEmpty())
                return true;
            QFile file(fn);
            if (file.open(QFile::WriteOnly)) {
                file.write(buffer);
                file.close();
            }
        }
        return true;
    }
    else if (strcmp("Print",pMsg) == 0) {
        print();
        return true;
    }
    else if (strcmp("PrintPreview",pMsg) == 0) {
        printPreview();
        return true;
    }
    else if (strcmp("PrintPdf",pMsg) == 0) {
        printPdf();
        return true;
    }

    return false;
}

bool GraphvizView::onHasMsg(const char* pMsg) const
{
    if (strcmp("Save",pMsg) == 0)
        return true;
    else if (strcmp("SaveAs",pMsg) == 0)
        return true;
    else if (strcmp("Print",pMsg) == 0)
        return true; 
    else if (strcmp("PrintPreview",pMsg) == 0)
        return true; 
    else if (strcmp("PrintPdf",pMsg) == 0)
        return true; 
    return false;
}

void GraphvizView::print(QPrinter* printer)
{
    QPainter p(printer);
    QRect rect = printer->pageRect();
    view->scene()->render(&p, rect);
    //QByteArray buffer = exportGraph(QString("svg"));
    //QSvgRenderer svg(buffer);
    //svg.render(&p, rect);
    p.end();
}

void GraphvizView::print()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    printer.setOrientation(QPrinter::Landscape);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        print(&printer);
    }
}

void GraphvizView::printPdf()
{
    QStringList filter;
    filter << QString("%1 (*.pdf)").arg(tr("PDF format"));

    QString selectedFilter;
    QString fn = Gui::FileDialog::getSaveFileName(this, tr("Export graph"), QString(), filter.join(QString(";;")), &selectedFilter);
    if (!fn.isEmpty()) {
        QByteArray buffer = exportGraph(selectedFilter);
        if (buffer.isEmpty())
            return;
        QFile file(fn);
        if (file.open(QFile::WriteOnly)) {
            file.write(buffer);
            file.close();
        }
    }
}

void GraphvizView::printPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    printer.setOrientation(QPrinter::Landscape);

    QPrintPreviewDialog dlg(&printer, this);
    connect(&dlg, SIGNAL(paintRequested (QPrinter *)),
            this, SLOT(print(QPrinter *)));
    dlg.exec();
}

#include "moc_GraphvizView.cpp"
#include "moc_GraphvizView-internal.cpp"

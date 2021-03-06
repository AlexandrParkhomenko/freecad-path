/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/

#ifndef PARTGUI_DLGREVOLUTION_H
#define PARTGUI_DLGREVOLUTION_H

#include "Gui/Selection.h"
#include "Gui/InputVector.h"
#include "Gui/TaskView/TaskDialog.h"
#include "Gui/TaskView/TaskView.h"

namespace PartGui {

class Ui_DlgRevolution;
class DlgRevolution : public QDialog, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    DlgRevolution(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    ~DlgRevolution();
    void accept();

    Base::Vector3d getDirection() const;
    Base::Vector3d getPosition() const;
    void getAxisLink(App::PropertyLinkSub &lnk) const;
    double getAngle() const;

    void setDirection(Base::Vector3d dir);
    void setPosition(Base::Vector3d dir);
    void setAxisLink(const App::PropertyLinkSub &lnk);
    void setAxisLink(const char* objname, const char* subname);

    std::vector<App::DocumentObject*> getShapesToRevolve() const;

    bool validate();

protected:
    void changeEvent(QEvent *e);
    void keyPressEvent(QKeyEvent*);

private Q_SLOTS:
    void on_selectLine_clicked();
    void on_btnX_clicked();
    void on_btnY_clicked();
    void on_btnZ_clicked();
    void on_txtAxisLink_textChanged(QString);

private:
    void findShapes();
    void onSelectionChanged(const Gui::SelectionChanges& msg);

    ///returns link to any of selected source shapes. Throws if nothing is selected for extrusion.
    App::DocumentObject& getShapeToRevolve() const;

    ///automatically checks Solid checkbox depending on input shape
    void autoSolid();

private:
    //typedef Gui::LocationInterfaceComp<Ui_DlgRevolution> Ui_RevolutionComp;
    Ui_DlgRevolution* ui;
    class EdgeSelection;
    EdgeSelection* filter;
};

class TaskRevolution : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskRevolution();
    ~TaskRevolution();

public:
    bool accept();

    virtual QDialogButtonBox::StandardButtons getStandardButtons() const
    { return QDialogButtonBox::Ok | QDialogButtonBox::Cancel; }

private:
    DlgRevolution* widget;
    Gui::TaskView::TaskBox* taskbox;
};

} // namespace PartGui

#endif // PARTGUI_DLGREVOLUTION_H

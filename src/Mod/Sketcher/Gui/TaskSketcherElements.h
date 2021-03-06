/***************************************************************************
 *   Copyright (c) 2014 Abdullah Tahiri <abdullah.tahiri.yo@gmail.com	     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef GUI_TASKVIEW_TaskSketcherElements_H
#define GUI_TASKVIEW_TaskSketcherElements_H

#include "Gui/TaskView/TaskView.h"
#include "Gui/Selection.h"
#include <boost/signals2.hpp>
#include <QListWidget>

namespace App {
class Property;
}

namespace SketcherGui {

class ViewProviderSketch;
class Ui_TaskSketcherElements;

class ElementView : public QListWidget
{
    Q_OBJECT

public:
    explicit ElementView(QWidget *parent = 0);
    ~ElementView();

        
Q_SIGNALS:
    void onFilterShortcutPressed();
    void signalCloseShape();
    
protected:
    void contextMenuEvent (QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent * event);

protected Q_SLOTS:
    void deleteSelectedItems();
    // Constraints
    void doHorizontalDistance();
    void doVerticalDistance();
    void doHorizontalConstraint();
    void doVerticalConstraint();
    void doLockConstraint();
    void doPointCoincidence();
    void doParallelConstraint();
    void doPerpendicularConstraint();
    void doLengthConstraint();
    void doRadiusConstraint();
    void doDiameterConstraint();
    void doAngleConstraint();
    void doEqualConstraint();
    void doPointOnObjectConstraint();
    void doSymmetricConstraint();
    void doTangentConstraint();
    // Other Commands
    void doToggleConstruction();    
    // Acelerators
    void doCloseShape();
    void doConnect();
    void doSelectOrigin();
    void doSelectHAxis();
    void doSelectVAxis();

    void doSelectConstraints();

};

class TaskSketcherElements : public Gui::TaskView::TaskBox, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    TaskSketcherElements(ViewProviderSketch *sketchView);
    ~TaskSketcherElements();

    /// Observer message from the Selection
    void onSelectionChanged(const Gui::SelectionChanges& msg);

private:
    void slotElementsChanged(void);
    void updateIcons(int element);
    void updatePreselection();
    void clearWidget();

public Q_SLOTS:
    void on_listWidgetElements_itemSelectionChanged(void); 
    void on_listWidgetElements_itemEntered(QListWidgetItem *item);
    void on_listWidgetElements_filterShortcutPressed();
    void on_listWidgetElements_currentFilterChanged ( int index );
    void on_namingBox_stateChanged(int state);
    void on_autoSwitchBox_stateChanged(int state);

protected:
    void changeEvent(QEvent *e);
    void leaveEvent ( QEvent * event );
    ViewProviderSketch *sketchView;
    typedef boost::signals2::connection Connection;
    Connection connectionElementsChanged;

private:
    QWidget* proxy;
    Ui_TaskSketcherElements* ui;
    int focusItemIndex;
    int previouslySelectedItemIndex;
    
    bool isNamingBoxChecked;
    bool isautoSwitchBoxChecked;
    
    bool inhibitSelectionUpdate;
};

} //namespace SketcherGui

#endif // GUI_TASKVIEW_TASKAPPERANCE_H

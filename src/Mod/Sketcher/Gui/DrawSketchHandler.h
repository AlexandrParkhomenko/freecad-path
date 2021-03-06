/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef SKETCHERGUI_DrawSketchHandler_H
#define SKETCHERGUI_DrawSketchHandler_H

#include "stdexport.h"
#include "Base/Tools2D.h"
#include "Mod/Sketcher/App/Constraint.h"
#include "Gui/Selection.h"

class QPixmap;

namespace Sketcher {
    class Sketch;
    class SketchObject;
}

namespace SketcherGui {

class ViewProviderSketch;

// A Simple data type to hold basic information for suggested constraints
struct AutoConstraint
{
    enum TargetType
    {
        VERTEX,
        CURVE
    };
    Sketcher::ConstraintType Type;
    int GeoId;
    Sketcher::PointPos PosId;
};

/** Handler to create new sketch geometry
  * This class has to be reimplemented to create geometry in the
  * sketcher while its in editing.
  */
class SketcherGuiExport DrawSketchHandler
{
public:
    DrawSketchHandler();
    virtual ~DrawSketchHandler();

    virtual void activated(ViewProviderSketch *){}
    virtual void deactivated(ViewProviderSketch *){}
    virtual void mouseMove(Base::Vector2d onSketchPos)=0;
    virtual bool pressButton(Base::Vector2d onSketchPos)=0;
    virtual bool releaseButton(Base::Vector2d onSketchPos)=0;
    virtual bool onSelectionChanged(const Gui::SelectionChanges&) { return false; }
    virtual void registerPressedKey(bool /*pressed*/, int /*key*/){}

    virtual void quit(void);

    friend class ViewProviderSketch;

    // get the actual highest vertex index, the next use will be +1
    int getHighestVertexIndex(void);
    // get the actual highest edge index, the next use will be +1
    int getHighestCurveIndex(void);

    int seekAutoConstraint(std::vector<AutoConstraint> &suggestedConstraints,
                           const Base::Vector2d &Pos, const Base::Vector2d &Dir,
                           AutoConstraint::TargetType type = AutoConstraint::VERTEX);
    // createowncommand indicates whether a separate command shall be create and committed (for example for undo purposes) or not
    // is not it is the responsibility of the developer to create and commit the command appropriately.
    void createAutoConstraints(const std::vector<AutoConstraint> &autoConstrs,
                               int geoId, Sketcher::PointPos pointPos=Sketcher::none, bool createowncommand = true);

    void setPositionText(const Base::Vector2d &Pos, const SbString &text);
    void setPositionText(const Base::Vector2d &Pos);
    void resetPositionText(void);
    void renderSuggestConstraintsCursor(std::vector<AutoConstraint> &suggestedConstraints);

protected:
    // helpers
    void setCursor( const QPixmap &p,int x,int y );
    void unsetCursor(void);
    void applyCursor(void);
    void applyCursor(QCursor &newCursor);
    void setCrosshairColor();

    ViewProviderSketch *sketchgui;
    QCursor oldCursor;
    QCursor actCursor;
};


} // namespace SketcherGui


#endif // SKETCHERGUI_DrawSketchHandler_H


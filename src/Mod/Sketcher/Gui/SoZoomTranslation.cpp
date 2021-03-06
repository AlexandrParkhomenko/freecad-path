/***************************************************************************
 *   Copyright (c) 2011                                                    *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/misc/SoState.h>
# include <cmath>
# include <cfloat>

#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/nodes/SoCamera.h>

#include "Gui/Application.h"
#include "Gui/Document.h"
#include "Gui/MainWindow.h"
#include "Gui/View3DInventor.h"
#include "Gui/View3DInventorViewer.h"


#include "SoZoomTranslation.h"

// *************************************************************************

using namespace SketcherGui;

// ------------------------------------------------------

SO_NODE_SOURCE(SoZoomTranslation);

void SoZoomTranslation::initClass()
{
    SO_NODE_INIT_CLASS(SoZoomTranslation, SoTranslation, "Translation");

    // Enable elements for SoGetMatrixAction (#0002268)
    // SoCamera::initClass() enables the SoViewVolumeElement for
    // * SoGLRenderAction
    // * SoGetBoundingBoxAction
    // * SoRayPickAction
    // * SoCallbackAction
    // * SoGetPrimitiveCountAction
    // The element SoViewportRegionElement is enabled by the
    // above listed actions.
    // Additionally, SoViewVolumeElement is enabled for
    // * SoAudioRenderAction
    // * SoHandleEventAction
    // And SoViewportRegionElement is enabled for
    // * SoHandleEventAction
    // * SoGetMatrixAction
    SO_ENABLE(SoGetMatrixAction, SoViewVolumeElement);
}

float SoZoomTranslation::getScaleFactor(SoAction* action) const
{
    // Dividing by 5 seems to work well
    SbViewVolume vv = SoViewVolumeElement::get(action->getState());
    float aspectRatio = SoViewportRegionElement::get(action->getState()).getViewportAspectRatio();
    float scale = vv.getWorldToScreenScale(SbVec3f(0.f, 0.f, 0.f), 0.1f) / (5*aspectRatio);
    return scale;
}

SoZoomTranslation::SoZoomTranslation()
{
    SO_NODE_CONSTRUCTOR(SoZoomTranslation);
    SO_NODE_ADD_FIELD(abPos, (SbVec3f(0.f,0.f,0.f)));
}

void SoZoomTranslation::GLRender(SoGLRenderAction * action)
{
    SoZoomTranslation::doAction((SoAction *)action);
}

// Doc in superclass.
void SoZoomTranslation::doAction(SoAction * action)
{
    SbVec3f v;
    if (this->translation.getValue() == SbVec3f(0.0f, 0.0f, 0.0f) && this->abPos.getValue() == SbVec3f(0.0f, 0.0f, 0.0f)) {
        return;
    } else {
        SbVec3f absVtr = this->abPos.getValue();
        SbVec3f relVtr = this->translation.getValue();

        float sf = this->getScaleFactor(action);
        // For Sketcher Keep Z value the same
        relVtr[0] = (relVtr[0] != 0) ? sf * relVtr[0] : 0;
        relVtr[1] = (relVtr[1] != 0) ? sf * relVtr[1] : 0;

        v = absVtr + relVtr;
    }

    SoModelMatrixElement::translateBy(action->getState(), this, v);
}

void SoZoomTranslation::getMatrix(SoGetMatrixAction * action)
{
    SbVec3f v;
    if (this->translation.getValue() == SbVec3f(0.0f, 0.0f, 0.0f) && this->abPos.getValue() == SbVec3f(0.0f, 0.0f, 0.0f)) {
        return;
    } else {
        SbVec3f absVtr = this->abPos.getValue();
        SbVec3f relVtr = this->translation.getValue();

        float sf = this->getScaleFactor(action);
        // For Sketcher Keep Z value the same
        relVtr[0] = (relVtr[0] != 0) ? sf  * relVtr[0] : 0;
        relVtr[1] = (relVtr[1] != 0) ? sf  * relVtr[1] : 0;

        v = absVtr + relVtr;
    }

    SbMatrix m;
    m.setTranslate(v);
    action->getMatrix().multLeft(m);
    m.setTranslate(-v);
    action->getInverse().multRight(m);
}

void SoZoomTranslation::callback(SoCallbackAction * action)
{
    SoZoomTranslation::doAction((SoAction *)action);
}

void SoZoomTranslation::getBoundingBox(SoGetBoundingBoxAction * action)
{
    SoZoomTranslation::doAction((SoAction *)action);
}

void SoZoomTranslation::pick(SoPickAction * action)
{
    SoZoomTranslation::doAction((SoAction *)action);
}

// Doc in superclass.
void SoZoomTranslation::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
    SoZoomTranslation::doAction((SoAction *)action);
}

/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


# include <Bnd_Box.hxx>
# include <gp_Pln.hxx>
# include <BRep_Builder.hxx>
# include <BRepBndLib.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <BRepBuilderAPI_MakeFace.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <Geom_Plane.hxx>
# include <Geom_Surface.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Compound.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Wire.hxx>
# include <TopExp_Explorer.hxx>
# include <BRepAlgoAPI_Fuse.hxx>
# include <gp_Pln.hxx>

#include "Base/Placement.h"

#include "FeatureFace.h"
#include "FaceMaker.h"
#include "Mod/Part/App/Part2DObject.h"


using namespace Part;


PROPERTY_SOURCE(Part::Face, Part::Feature)

Face::Face()
{
    ADD_PROPERTY(Sources,(0));
    ADD_PROPERTY(FaceMakerClass,("Part::FaceMakerCheese"));//default value here is for legacy documents. Default for new objects is set in setupObject.
    Sources.setSize(0);
}

short Face::mustExecute() const
{
    if (FaceMakerClass.isTouched())
        return 1;
    if (Sources.isTouched())
        return 1;
    return Part::Feature::mustExecute();
}

void Face::setupObject()
{
    this->FaceMakerClass.setValue("Part::FaceMakerBullseye");
    Feature::setupObject();
}

App::DocumentObjectExecReturn *Face::execute(void)
{
    std::vector<App::DocumentObject*> links = Sources.getValues();
    if (links.empty())
        return new App::DocumentObjectExecReturn("No shapes linked");

    std::unique_ptr<FaceMaker> facemaker = FaceMaker::ConstructFromType(this->FaceMakerClass.getValue());

    for (std::vector<App::DocumentObject*>::iterator it = links.begin(); it != links.end(); ++it) {
        if (!(*it && (*it)->isDerivedFrom(Part::Feature::getClassTypeId())))
            return new App::DocumentObjectExecReturn("Linked object is not a Part object (has no Shape).");
        TopoDS_Shape shape = static_cast<Part::Part2DObject*>(*it)->Shape.getShape().getShape();
        if (shape.IsNull())
            return new App::DocumentObjectExecReturn("Linked shape object is empty");

        // this is a workaround for an obscure OCC bug which leads to empty tessellations
        // for some faces. Making an explicit copy of the linked shape seems to fix it.
        // The error only happens when re-computing the shape.
        /*if (!this->Shape.getValue().IsNull()) {
            BRepBuilderAPI_Copy copy(shape);
            shape = copy.Shape();
            if (shape.IsNull())
                return new App::DocumentObjectExecReturn("Linked shape object is empty");
        }*/

        if(links.size() == 1 && shape.ShapeType() == TopAbs_COMPOUND)
            facemaker->useCompound(TopoDS::Compound(shape));
        else
            facemaker->addShape(shape);
    }

    facemaker->Build();

    TopoDS_Shape aFace = facemaker->Shape();
    if (aFace.IsNull())
        return new App::DocumentObjectExecReturn("Creating face failed (null shape result)");
    this->Shape.setValue(aFace);

    return App::DocumentObject::StdReturn;
}


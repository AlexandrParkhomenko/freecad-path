/***************************************************************************
 *   Copyright (c) 2011 Juergen Riegel <juergen.riegel@web.de>             *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef PARTGUI_VIEWPROVIDERPARTEXT_H
#define PARTGUI_VIEWPROVIDERPARTEXT_H

#include "stdexport.h"
#include <Standard_math.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfDir.hxx>
#include "App/PropertyUnits.h"
#include "Gui/ViewProviderGeometryObject.h"
#include <map>

class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Wire;
class TopoDS_Face;
class SoSeparator;
class SoGroup;
class SoSwitch;
class SoVertexShape;
class SoPickedPoint;
class SoShapeHints;
class SoEventCallback;
class SbVec3f;
class SoSphere;
class SoScale;
class SoCoordinate3;
class SoIndexedFaceSet;
class SoNormal;
class SoNormalBinding;
class SoMaterialBinding;
class SoIndexedLineSet;

namespace PartGui {

class SoBrepFaceSet;
class SoBrepEdgeSet;
class SoBrepPointSet;

class PartGuiExport ViewProviderPartExt : public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER(PartGui::ViewProviderPartExt);

public:
    /// constructor
    ViewProviderPartExt();
    /// destructor
    virtual ~ViewProviderPartExt();

    // Display properties
    App::PropertyFloatConstraint Deviation;
    App::PropertyBool ControlPoints;
    App::PropertyAngle AngularDeflection;
    App::PropertyEnumeration Lighting;
    App::PropertyEnumeration DrawStyle;
    // Points
    App::PropertyFloatConstraint PointSize;
    App::PropertyColor PointColor;
    App::PropertyMaterial PointMaterial;
    App::PropertyColorList PointColorArray;
    // Lines
    App::PropertyFloatConstraint LineWidth;
    App::PropertyColor LineColor;
    App::PropertyMaterial LineMaterial;
    App::PropertyColorList LineColorArray;
    // Faces (Gui::ViewProviderGeometryObject::ShapeColor and Gui::ViewProviderGeometryObject::ShapeMaterial apply)
    App::PropertyColorList DiffuseColor;    

    virtual void attach(App::DocumentObject *);
    virtual void setDisplayMode(const char* ModeName);
    /// returns a list of all possible modes
    virtual std::vector<std::string> getDisplayModes(void) const;
    /// Update the view representation
    void reload();

    virtual void updateData(const App::Property*);

    /** @name Selection handling
     * This group of methods do the selection handling.
     * Here you can define how the selection for your ViewProfider
     * works.
     */
    //@{
    /// indicates if the ViewProvider use the new Selection model
    virtual bool useNewSelectionModel(void) const {return true;}
    /// return a hit element to the selection path or 0
    virtual std::string getElement(const SoDetail*) const;
    virtual SoDetail* getDetail(const char*) const;
    virtual std::vector<Base::Vector3d> getModelPoints(const SoPickedPoint *) const;
    /// return the highlight lines for a given element or the whole shape
    virtual std::vector<Base::Vector3d> getSelectionShape(const char* Element) const;
    //@}

    /** @name Highlight handling
    * This group of methods do the highlighting of elements.
    */
    //@{
    void setHighlightedFaces(const std::vector<App::Color>& colors);
    void setHighlightedFaces(const std::vector<App::Material>& colors);
    void unsetHighlightedFaces();
    void setHighlightedEdges(const std::vector<App::Color>& colors);
    void unsetHighlightedEdges();
    void setHighlightedPoints(const std::vector<App::Color>& colors);
    void unsetHighlightedPoints();
    //@}

    /** @name Edit methods */
    //@{
    void setupContextMenu(QMenu*, QObject*, const char*);
protected:
    bool setEdit(int ModNum);
    void unsetEdit(int ModNum);
    //@}

protected:
    /// get called by the container whenever a property has been changed
    virtual void onChanged(const App::Property* prop);
    bool loadParameter();
    void updateVisual(const TopoDS_Shape &);
    void getNormals(const TopoDS_Face&  theFace, const Handle(Poly_Triangulation)& aPolyTri,
                    TColgp_Array1OfDir& theNormals);

    // nodes for the data representation
    SoMaterialBinding * pcFaceBind;
    SoMaterialBinding * pcLineBind;
    SoMaterialBinding * pcPointBind;
    SoMaterial        * pcLineMaterial;
    SoMaterial        * pcPointMaterial;
    SoDrawStyle       * pcLineStyle;
    SoDrawStyle       * pcPointStyle;
    SoShapeHints      * pShapeHints;

    SoCoordinate3     * coords;
    SoBrepFaceSet     * faceset;
    SoNormal          * norm;
    SoNormalBinding   * normb;
    SoBrepEdgeSet     * lineset;
    SoBrepPointSet    * nodeset;

    bool VisualTouched;
    bool NormalsFromUV;

private:
    // settings stuff
    static App::PropertyFloatConstraint::Constraints sizeRange;
    static App::PropertyFloatConstraint::Constraints tessRange;
    static App::PropertyQuantityConstraint::Constraints angDeflectionRange;
    static const char* LightingEnums[];
    static const char* DrawStyleEnums[];
};

}

#endif // PARTGUI_VIEWPROVIDERPARTEXT_H


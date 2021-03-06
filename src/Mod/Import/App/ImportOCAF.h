/***************************************************************************
 *   Copyright (c) 2013 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef IMPORT_IMPORTOCAF_H
#define IMPORT_IMPORTOCAF_H

#include "stdexport.h"
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>
#include <climits>
#include <string>
#include <set>
#include <map>
#include <vector>
#include "App/Material.h"
#include "App/Part.h"
#include "Mod/Part/App/FeatureCompound.h"


class TDF_Label;
class TopLoc_Location;

namespace App {
class Document;
class DocumentObject;
}
namespace Part {
class Feature;
}

namespace Import {

class Standard_EXPORT ImportOCAF
{
public:
    ImportOCAF(Handle(TDocStd_Document) h, App::Document* d, const std::string& name);
    virtual ~ImportOCAF();
    void loadShapes();
    void setMerge(bool);

private:
    void loadShapes(const TDF_Label& label, const TopLoc_Location&, const std::string& partname, const std::string& assembly, bool isRef, std::vector<App::DocumentObject*> &);
    void createShape(const TDF_Label& label, const TopLoc_Location&, const std::string&, std::vector<App::DocumentObject*> &, bool);
    void createShape(const TopoDS_Shape& label, const TopLoc_Location&, const std::string&, std::vector<App::DocumentObject*> &);
    void loadColors(Part::Feature* part, const TopoDS_Shape& aShape);
    virtual void applyColors(Part::Feature*, const std::vector<App::Color>&){}

private:
    Handle(TDocStd_Document) pDoc;
    App::Document* doc;
    Handle(XCAFDoc_ShapeTool) aShapeTool;
    Handle(XCAFDoc_ColorTool) aColorTool;
    bool merge;
    std::string default_name;
    std::set<int> myRefShapes;
    static const int HashUpper = INT_MAX;
};

class Standard_EXPORT ImportOCAFCmd : public ImportOCAF
{
public:
    ImportOCAFCmd(Handle(TDocStd_Document) h, App::Document* d, const std::string& name);
    std::map<Part::Feature*, std::vector<App::Color> > getPartColorsMap() const {
        return partColors;
    }

private:
    void applyColors(Part::Feature* part, const std::vector<App::Color>& colors);

private:
    std::map<Part::Feature*, std::vector<App::Color> > partColors;
};

class ImportXCAF
{
public:
    ImportXCAF(Handle(TDocStd_Document) h, App::Document* d, const std::string& name);
    virtual ~ImportXCAF();
    void loadShapes();

private:
    void createShape(const TopoDS_Shape& shape, bool perface=false, bool setname=false) const;
    void loadShapes(const TDF_Label& label);
    virtual void applyColors(Part::Feature*, const std::vector<App::Color>&){}

private:
    Handle(TDocStd_Document) hdoc;
    App::Document* doc;
    Handle(XCAFDoc_ShapeTool) aShapeTool;
    Handle(XCAFDoc_ColorTool) hColors;
    std::string default_name;
    std::map<Standard_Integer, TopoDS_Shape> mySolids;
    std::map<Standard_Integer, TopoDS_Shape> myShells;
    std::map<Standard_Integer, TopoDS_Shape> myCompds;
    std::map<Standard_Integer, TopoDS_Shape> myShapes;
    std::map<Standard_Integer, Quantity_Color> myColorMap;
    std::map<Standard_Integer, std::string> myNameMap;
};

}

#endif //IMPORT_IMPORTOCAF_H

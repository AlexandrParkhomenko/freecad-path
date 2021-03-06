/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2008     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef PART_PROPERTYTOPOSHAPE_H
#define PART_PROPERTYTOPOSHAPE_H

#include "stdexport.h"
#include "TopoShape.h"
#include <TopAbs_ShapeEnum.hxx>
#include "App/DocumentObject.h"
#include "App/PropertyGeo.h"
#include <map>
#include <vector>

namespace Part
{

/** The part shape property class.
 * @author Werner Mayer
 */
class Standard_EXPORT PropertyPartShape : public App::PropertyComplexGeoData
{
    TYPESYSTEM_HEADER();

public:
    PropertyPartShape();
    ~PropertyPartShape();

    /** @name Getter/setter */
    //@{
    /// set the part shape
    void setValue(const TopoShape&);
    /// set the part shape
    void setValue(const TopoDS_Shape&);
    /// get the part shape
    const TopoDS_Shape& getValue(void) const;
    const TopoShape& getShape() const;
    const Data::ComplexGeoData* getComplexData() const;
    //@}

    /** @name Modification */
    //@{
    /// Transform the real shape data
    void transformGeometry(const Base::Matrix4D &rclMat);
    //@}

    /** @name Getting basic geometric entities */
    //@{
    /** Returns the bounding box around the underlying mesh kernel */
    Base::BoundBox3d getBoundingBox() const;
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject(void);
    void setPyObject(PyObject *value);
    //@}

    /** @name Save/restore */
    //@{
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);

    void SaveDocFile (Base::Writer &writer) const;
    void RestoreDocFile(Base::Reader &reader);

    App::Property *Copy(void) const;
    void Paste(const App::Property &from);
    unsigned int getMemSize (void) const;
    //@}

    /// Get valid paths for this property; used by auto completer
    virtual void getPaths(std::vector<App::ObjectIdentifier> & paths) const;

private:
    TopoShape _Shape;
};

struct Standard_EXPORT ShapeHistory {
    /**
    * @brief MapList: key is index of subshape (of type 'type') in source
    * shape. Value is list of indexes of subshapes in result shape.
    */
    typedef std::map<int, std::vector<int> > MapList;
    typedef std::vector<int> List;

    TopAbs_ShapeEnum type;
    MapList shapeMap;
};

class Standard_EXPORT PropertyShapeHistory : public App::PropertyLists
{
    TYPESYSTEM_HEADER();

public:
    PropertyShapeHistory();
    ~PropertyShapeHistory();

    virtual void setSize(int newSize) {
        _lValueList.resize(newSize);
    }
    virtual int getSize(void) const {
        return _lValueList.size();
    }

    /** Sets the property
     */
    void setValue(const ShapeHistory&);

    void setValues (const std::vector<ShapeHistory>& values);

    const std::vector<ShapeHistory> &getValues(void) const {
        return _lValueList;
    }

    virtual PyObject *getPyObject(void);
    virtual void setPyObject(PyObject *);

    virtual void Save (Base::Writer &writer) const;
    virtual void Restore(Base::XMLReader &reader);

    virtual void SaveDocFile (Base::Writer &writer) const;
    virtual void RestoreDocFile(Base::Reader &reader);

    virtual Property *Copy(void) const;
    virtual void Paste(const Property &from);

    virtual unsigned int getMemSize (void) const {
        return _lValueList.size() * sizeof(ShapeHistory);
    }

private:
    std::vector<ShapeHistory> _lValueList;
};

/** A property class to store hash codes and two radii for the fillet algorithm.
 * @author Werner Mayer
 */
struct Standard_EXPORT FilletElement {
    int edgeid;
    double radius1, radius2;
};

class Standard_EXPORT PropertyFilletEdges : public App::PropertyLists
{
    TYPESYSTEM_HEADER();

public:
    PropertyFilletEdges();
    ~PropertyFilletEdges();

    virtual void setSize(int newSize) {
        _lValueList.resize(newSize);
    }
    virtual int getSize(void) const {
        return _lValueList.size();
    }

    /** Sets the property
     */
    void setValue(int id, double r1, double r2);

    void setValues (const std::vector<FilletElement>& values);

    const std::vector<FilletElement> &getValues(void) const {
        return _lValueList;
    }

    virtual PyObject *getPyObject(void);
    virtual void setPyObject(PyObject *);

    virtual void Save (Base::Writer &writer) const;
    virtual void Restore(Base::XMLReader &reader);

    virtual void SaveDocFile (Base::Writer &writer) const;
    virtual void RestoreDocFile(Base::Reader &reader);

    virtual Property *Copy(void) const;
    virtual void Paste(const Property &from);

    virtual unsigned int getMemSize (void) const {
        return _lValueList.size() * sizeof(FilletElement);
    }

private:
    std::vector<FilletElement> _lValueList;
};

} //namespace Part


#endif // PART_PROPERTYTOPOSHAPE_H

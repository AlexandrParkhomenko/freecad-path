/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef APP_PROPERTYGEO_H
#define APP_PROPERTYGEO_H


#include "stdexport.h"
#include "Base/Vector3D.h"
#include "Base/Matrix.h"
#include "Base/BoundBox.h"
#include "Base/Placement.h"

#include "Property.h"
#include "PropertyLinks.h"
#include "ComplexGeoData.h"

namespace Base {
class Writer;
}

namespace Data {
class ComplexGeoData;
}

namespace App
{
class Feature;
class Placement;



/** Vector properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyVector: public Property
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyVector();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    virtual ~PropertyVector();

    /** Sets the property
     */
    void setValue(const Base::Vector3d &vec);
    void setValue(double x, double y, double z);

    /// Get valid paths for this property; used by auto completer
    void getPaths(std::vector<ObjectIdentifier> &paths) const;

    /** This method returns a string representation of the property
     */
    const Base::Vector3d &getValue(void) const;
    const char* getEditorName(void) const {
        return "Gui::PropertyEditor::PropertyVectorItem";
    }

    virtual PyObject *getPyObject(void);
    virtual void setPyObject(PyObject *);

    virtual void Save (Base::Writer &writer) const;
    virtual void Restore(Base::XMLReader &reader);

    virtual Property *Copy(void) const;
    virtual void Paste(const Property &from);

    virtual unsigned int getMemSize (void) const {
        return sizeof(Base::Vector3d);
    }

private:
    Base::Vector3d _cVec;
};


class AppExport PropertyVectorDistance: public PropertyVector
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyVectorDistance();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    virtual ~PropertyVectorDistance();

    const boost::any getPathValue(const ObjectIdentifier &path) const;

    const char* getEditorName(void) const {
        return "Gui::PropertyEditor::PropertyVectorDistanceItem";
    }
};

class AppExport PropertyPosition: public PropertyVector
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyPosition();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    virtual ~PropertyPosition();

    const boost::any getPathValue(const ObjectIdentifier &path) const;

    const char* getEditorName(void) const {
        return "Gui::PropertyEditor::PropertyPositionItem";
    }
};

class AppExport PropertyDirection: public PropertyVector
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyDirection();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    virtual ~PropertyDirection();

    const boost::any getPathValue(const ObjectIdentifier &path) const;

    const char* getEditorName(void) const {
        return "Gui::PropertyEditor::PropertyDirectionItem";
    }
};

class AppExport PropertyVectorList: public PropertyLists
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyVectorList();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    virtual ~PropertyVectorList();

    virtual void setSize(int newSize);
    virtual int getSize(void) const;

    /** Sets the property
     */
    void setValue(const Base::Vector3d&);
    void setValue(double x, double y, double z);

    /// index operator
    const Base::Vector3d& operator[] (const int idx) const {
        return _lValueList.operator[] (idx);
    }

    void set1Value (const int idx, const Base::Vector3d& value) {
        _lValueList.operator[] (idx) = value;
    }

    void setValues (const std::vector<Base::Vector3d>& values);

    void setValue (void){}

    const std::vector<Base::Vector3d> &getValues(void) const {
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

    virtual unsigned int getMemSize (void) const;

private:
    std::vector<Base::Vector3d> _lValueList;
};

/// Property representing a 4x4 matrix
/*!
 * Encapsulates a Base::Matrix4D in a Property
 */
class AppExport PropertyMatrix: public Property
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A constructor.
     * Intitialises to an identity matrix
     */
    PropertyMatrix();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    virtual ~PropertyMatrix();

    /** Sets the property
     */
    void setValue(const Base::Matrix4D &mat);

    /** This method returns a string representation of the property
     */
    const Base::Matrix4D &getValue(void) const;
    const char* getEditorName(void) const {
        return "Gui::PropertyEditor::PropertyMatrixItem";
    }

    virtual PyObject *getPyObject(void);
    virtual void setPyObject(PyObject *);

    virtual void Save (Base::Writer &writer) const;
    virtual void Restore(Base::XMLReader &reader);

    virtual Property *Copy(void) const;
    virtual void Paste(const Property &from);

    virtual unsigned int getMemSize (void) const {
        return sizeof(Base::Matrix4D);
    }

private:
    Base::Matrix4D _cMat;
};

/** Vector properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyPlacement: public Property
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyPlacement();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    virtual ~PropertyPlacement();

    /** Sets the property
     */
    void setValue(const Base::Placement &pos);

    /** This method returns a string representation of the property
     */
    const Base::Placement &getValue(void) const;

    /// Get valid paths for this property; used by auto completer
    void getPaths(std::vector<ObjectIdentifier> &paths) const;

    void setPathValue(const ObjectIdentifier &path, const boost::any &value);

    const boost::any getPathValue(const ObjectIdentifier &path) const;

    const char* getEditorName(void) const {
        return "Gui::PropertyEditor::PropertyPlacementItem";
    }

    virtual PyObject *getPyObject(void);
    virtual void setPyObject(PyObject *);

    virtual void Save (Base::Writer &writer) const;
    virtual void Restore(Base::XMLReader &reader);

    virtual Property *Copy(void) const;
    virtual void Paste(const Property &from);

    virtual unsigned int getMemSize (void) const {
        return sizeof(Base::Placement);
    }

    static const Placement Null;

private:
    Base::Placement _cPos;
};

/** the general Link Property
 *  Main Purpose of this property is to Link Objects and Features in a document.
 */
class AppExport PropertyPlacementLink : public PropertyLink
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyPlacementLink();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    virtual ~PropertyPlacementLink();

    /** This method returns the linked DocumentObject
     */
    App::Placement * getPlacementObject(void) const;

    virtual Property *Copy(void) const;
    virtual void Paste(const Property &from);
};


class AppExport PropertyPlacementList: public PropertyLists
{
    TYPESYSTEM_HEADER();

public:
    /**
     * A property that stores a list of placements
     */
    PropertyPlacementList();

    virtual ~PropertyPlacementList();

    virtual void setSize(int newSize);
    virtual int getSize(void) const;

    /** Sets the property
     */
    void setValue(const Base::Placement&);

    /// index operator
    const Base::Placement& operator[] (const int idx) const {
        return _lValueList.operator[] (idx);
    }

    void set1Value (const int idx, const Base::Placement& value) {
        _lValueList.operator[] (idx) = value;
    }

    void setValues (const std::vector<Base::Placement>& values);

    void setValue (void){}

    const std::vector<Base::Placement> &getValues(void) const {
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

    virtual unsigned int getMemSize (void) const;

private:
    std::vector<Base::Placement> _lValueList;
};


/** The base class for all basic geometry properties.
 * @author Werner Mayer
 */
class AppExport PropertyGeometry : public App::Property
{
    TYPESYSTEM_HEADER();

public:
    PropertyGeometry();
    virtual ~PropertyGeometry();

    /** @name Modification */
    //@{
    /// Applies a transformation on the real geometric data type
    virtual void transformGeometry(const Base::Matrix4D &rclMat) = 0;
    /// Retrieve bounding box information
    virtual Base::BoundBox3d getBoundingBox() const = 0;
    //@}
};

/** The base class for all complex data properties.
 * @author Werner Mayer
 */
class AppExport PropertyComplexGeoData : public App::PropertyGeometry
{
    TYPESYSTEM_HEADER();

public:
    PropertyComplexGeoData();
    virtual ~PropertyComplexGeoData();

    /** @name Modification */
    //@{
    /// Applies a transformation on the real geometric data type
    virtual void transformGeometry(const Base::Matrix4D &rclMat) = 0;
    //@}

    /** @name Getting basic geometric entities */
    //@{
    virtual const Data::ComplexGeoData* getComplexData() const = 0;
    virtual Base::BoundBox3d getBoundingBox() const = 0;
    //@}
};

} // namespace App


#endif // APP_PROPERTYGEO_H

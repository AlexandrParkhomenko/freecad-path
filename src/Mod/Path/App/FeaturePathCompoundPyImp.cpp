/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/



#include "FeaturePathCompound.h"
#include <CXX/Objects.hxx>

// inclusion of the generated files (generated out of FeaturePathCompoundPy.xml)
#include "FeaturePathCompoundPy.h"
#include "FeaturePathCompoundPy.cpp"

using namespace Path;


// returns a string which represents the object e.g. when printed in python
std::string FeaturePathCompoundPy::representation(void) const
{
    return std::string("<Path::FeatureCompound>");
}


PyObject*  FeaturePathCompoundPy::addObject(PyObject *args)
{
    PyObject *object;
    if (!PyArg_ParseTuple(args, "O!", &(DocumentObjectPy::Type), &object))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    DocumentObjectPy* docObj = static_cast<DocumentObjectPy*>(object);
    if (!docObj->getDocumentObjectPtr() || !docObj->getDocumentObjectPtr()->getNameInDocument()) {
        PyErr_SetString(Base::BaseExceptionFreeCADError, "Cannot add an invalid object");
        return NULL;
    }
    if (docObj->getDocumentObjectPtr()->getDocument() != getFeaturePathCompoundPtr()->getDocument()) {
        PyErr_SetString(Base::BaseExceptionFreeCADError, "Cannot add an object from another document to this group");
        return NULL;
    }
    if (docObj->getDocumentObjectPtr() == this->getFeaturePathCompoundPtr()) {
        PyErr_SetString(Base::BaseExceptionFreeCADError, "Cannot add a group object to itself");
        return NULL;
    }

    FeatureCompound* comp = getFeaturePathCompoundPtr();

    if (comp->getTypeId().isDerivedFrom(Path::FeatureCompoundPython::getClassTypeId())) {
        FeatureCompoundPython* comppy = static_cast<FeatureCompoundPython*>(comp);
        App::Property* proxy = comppy->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("addObject"))) {
                Py::Callable method(vp.getAttr(std::string("addObject")));
                // check to which method this belongs to avoid an infinite recursion
                if (method.getAttr(std::string("__self__")) != Py::Object(this)) {
                    Py::Tuple args(1);
                    args[0] = Py::Object(object);
                    method.apply(args);
                    Py_Return;
                }
            }
        }
    }

    comp->addObject(docObj->getDocumentObjectPtr());
    Py_Return;
}


PyObject*  FeaturePathCompoundPy::removeObject(PyObject *args)
{
    PyObject *object;
    if (!PyArg_ParseTuple(args, "O!", &(DocumentObjectPy::Type), &object))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    DocumentObjectPy* docObj = static_cast<DocumentObjectPy*>(object);
    if (!docObj->getDocumentObjectPtr() || !docObj->getDocumentObjectPtr()->getNameInDocument()) {
        PyErr_SetString(Base::BaseExceptionFreeCADError, "Cannot remove an invalid object");
        return NULL;
    }
    if (docObj->getDocumentObjectPtr()->getDocument() != getFeaturePathCompoundPtr()->getDocument()) {
        PyErr_SetString(Base::BaseExceptionFreeCADError, "Cannot remove an object from another document from this group");
        return NULL;
    }

    FeatureCompound* comp = getFeaturePathCompoundPtr();

    if (comp->getTypeId().isDerivedFrom(Path::FeatureCompoundPython::getClassTypeId())) {
        FeatureCompoundPython* comppy = static_cast<FeatureCompoundPython*>(comp);
        App::Property* proxy = comppy->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == App::PropertyPythonObject::getClassTypeId()) {
            Py::Object vp = static_cast<App::PropertyPythonObject*>(proxy)->getValue();
            if (vp.hasAttr(std::string("removeObject"))) {
                Py::Callable method(vp.getAttr(std::string("removeObject")));
                // check to which method this belongs to avoid an infinite recursion
                if (method.getAttr(std::string("__self__")) != Py::Object(this)) {
                    Py::Tuple args(1);
                    args[0] = Py::Object(object);
                    method.apply(args);
                    Py_Return;
                }
            }
        }
    }

    comp->removeObject(docObj->getDocumentObjectPtr());
    Py_Return;
}



PyObject *FeaturePathCompoundPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}


int FeaturePathCompoundPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}

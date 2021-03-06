/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#include "stdexport.h"
# include <sstream>


#include "Base/Console.h"
#include "Base/Interpreter.h"
#include "Base/Reader.h"
#include "Base/Tools.h"

#include "App/DocumentObjectPy.h"
#include "FeaturePython.h"
#include "FeaturePythonPyImp.h"

using namespace App;

FeaturePythonImp::FeaturePythonImp(App::DocumentObject* o) : object(o)
{
}

FeaturePythonImp::~FeaturePythonImp()
{
}

/*!
 Calls the execute() method of the Python feature class. If the Python feature class doesn't have an execute()
 method or if it returns False this method also return false and true otherwise.
 */
bool FeaturePythonImp::execute()
{
    // avoid recursive calls of execute()
    if (object->testStatus(App::PythonCall))
        return false;

    // Run the execute method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == PropertyPythonObject::getClassTypeId()) {
            Py::Object feature = static_cast<PropertyPythonObject*>(proxy)->getValue();
            if (feature.hasAttr(std::string("execute"))) {
                if (feature.hasAttr("__object__")) {
                    Base::ObjectStatusLocker<ObjectStatus, DocumentObject> exe(App::PythonCall, object);
                    Py::Callable method(feature.getAttr(std::string("execute")));
                    Py::Tuple args;
                    Py::Object res = method.apply(args);
                    if (res.isBoolean() && !res.isTrue())
                        return false;
                    return true;
                }
                else {
                    Base::ObjectStatusLocker<ObjectStatus, DocumentObject> exe(App::PythonCall, object);
                    Py::Callable method(feature.getAttr(std::string("execute")));
                    Py::Tuple args(1);
                    args.setItem(0, Py::Object(object->getPyObject(), true));
                    Py::Object res = method.apply(args);
                    if (res.isBoolean() && !res.isTrue())
                        return false;
                    return true;
                }
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        e.ReportException();
        std::stringstream str;
        str << object->Label.getValue() << ": " << e.what();
        throw Base::RuntimeError(str.str());
    }

    return false;
}

void FeaturePythonImp::onBeforeChange(const Property* prop)
{
    // Run the execute method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == PropertyPythonObject::getClassTypeId()) {
            Py::Object feature = static_cast<PropertyPythonObject*>(proxy)->getValue();
            if (feature.hasAttr(std::string("onBeforeChange"))) {
                if (feature.hasAttr("__object__")) {
                    Py::Callable method(feature.getAttr(std::string("onBeforeChange")));
                    Py::Tuple args(1);
                    const char* prop_name = object->getPropertyName(prop);
                    if (prop_name) {
                        args.setItem(0, Py::String(prop_name));
                        method.apply(args);
                    }
                }
                else {
                    Py::Callable method(feature.getAttr(std::string("onBeforeChange")));
                    Py::Tuple args(2);
                    args.setItem(0, Py::Object(object->getPyObject(), true));
                    const char* prop_name = object->getPropertyName(prop);
                    if (prop_name) {
                        args.setItem(1, Py::String(prop_name));
                        method.apply(args);
                    }
                }
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        e.ReportException();
    }
}

void FeaturePythonImp::onChanged(const Property* prop)
{
    // Run the execute method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == PropertyPythonObject::getClassTypeId()) {
            Py::Object feature = static_cast<PropertyPythonObject*>(proxy)->getValue();
            if (feature.hasAttr(std::string("onChanged"))) {
                if (feature.hasAttr("__object__")) {
                    Py::Callable method(feature.getAttr(std::string("onChanged")));
                    Py::Tuple args(1);
                    const char* prop_name = object->getPropertyName(prop);
                    if (prop_name) {
                        args.setItem(0, Py::String(prop_name));
                        method.apply(args);
                    }
                }
                else {
                    Py::Callable method(feature.getAttr(std::string("onChanged")));
                    Py::Tuple args(2);
                    args.setItem(0, Py::Object(object->getPyObject(), true));
                    const char* prop_name = object->getPropertyName(prop);
                    if (prop_name) {
                        args.setItem(1, Py::String(prop_name));
                        method.apply(args);
                    }
                }
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        e.ReportException();
    }
}

void FeaturePythonImp::onDocumentRestored()
{
    // Run the execute method of the proxy object.
    Base::PyGILStateLocker lock;
    try {
        Property* proxy = object->getPropertyByName("Proxy");
        if (proxy && proxy->getTypeId() == PropertyPythonObject::getClassTypeId()) {
            Py::Object feature = static_cast<PropertyPythonObject*>(proxy)->getValue();
            if (feature.hasAttr(std::string("onDocumentRestored"))) {
                if (feature.hasAttr("__object__")) {
                    Py::Callable method(feature.getAttr(std::string("onDocumentRestored")));
                    Py::Tuple args;
                    method.apply(args);
                }
                else {
                    Py::Callable method(feature.getAttr(std::string("onDocumentRestored")));
                    Py::Tuple args(1);
                    args.setItem(0, Py::Object(object->getPyObject(), true));
                    method.apply(args);
                }
            }
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        e.ReportException();
    }
}

PyObject *FeaturePythonImp::getPyObject(void)
{
    // ref counter is set to 1
    return new FeaturePythonPyT<DocumentObjectPy>(object);
}

// ---------------------------------------------------------

namespace App {
PROPERTY_SOURCE_TEMPLATE(App::FeaturePython, App::DocumentObject)
template<> const char* App::FeaturePython::getViewProviderName(void) const {
    return "Gui::ViewProviderPythonFeature";
}
template<> PyObject* App::FeaturePython::getPyObject(void) {
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new FeaturePythonPyT<DocumentObjectPy>(this),true);
    }
    return Py::new_reference_to(PythonObject);
}
// explicit template instantiation
template class AppExport FeaturePythonT<DocumentObject>;
}

// ---------------------------------------------------------

namespace App {
PROPERTY_SOURCE_TEMPLATE(App::GeometryPython, App::GeoFeature)
template<> const char* App::GeometryPython::getViewProviderName(void) const {
    return "Gui::ViewProviderPythonGeometry";
}
// explicit template instantiation
template class AppExport FeaturePythonT<GeoFeature>;
}

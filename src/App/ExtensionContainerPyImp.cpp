/***************************************************************************
 *   Copyright (c) Stefan Tröger          (stefantroeger@gmx.net) 2016     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/



# include <sstream>

#include "Application.h"
#include "DocumentObject.h"

// inclution of the generated files (generated out of PropertyContainerPy.xml)
#include "App/ExtensionContainerPy.h"
#include <App/ExtensionContainerPy.cpp>

using namespace App;

// returns a string which represent the object e.g. when printed in python
std::string ExtensionContainerPy::representation(void) const
{
    return std::string("<extension>");
}

int  ExtensionContainerPy::initialization() {

    if (this->ob_type->tp_dict == NULL) {
        if (PyType_Ready(this->ob_type) < 0)
            return 0;
    }

    ExtensionContainer::ExtensionIterator it = this->getExtensionContainerPtr()->extensionBegin();
    for(; it != this->getExtensionContainerPtr()->extensionEnd(); ++it) {

        // The PyTypeObject is shared by all instances of this type and therefore
        // we have to add new methods only once.
        PyObject* obj = (*it).second->getExtensionPyObject();
        PyMethodDef* meth = reinterpret_cast<PyMethodDef*>(obj->ob_type->tp_methods);
        PyTypeObject *type = this->ob_type;
        PyObject *dict = type->tp_dict;

        // make sure to do the initialization only once
        if (meth->ml_name) {
            PyObject* item = PyDict_GetItemString(dict, meth->ml_name);
            if (item == NULL) {
                // Note: this adds the methods to the type object to make sure
                // it appears in the call tips. The function will not be bound
                // to an instance
                Py_INCREF(dict);
                while (meth->ml_name) {
                    PyObject *func;
                    func = PyCFunction_New(meth, 0);
                    if (func == NULL)
                        break;
                    if (PyDict_SetItemString(dict, meth->ml_name, func) < 0)
                        break;
                    Py_DECREF(func);
                    ++meth;
                }

                Py_DECREF(dict);
            }
        }

        Py_DECREF(obj);
    }
    return 1;
}

int  ExtensionContainerPy::finalization() {
/*
    //we need to delete all added python extensions, as we are the owner! 
    ExtensionContainer::ExtensionIterator it = this->getExtensionContainerPtr()->extensionBegin();
    for(; it != this->getExtensionContainerPtr()->extensionEnd(); ++it) {
        if((*it).second->isPythonExtension())
            delete (*it).second;
    }*/
    return 1;
}

PyObject* ExtensionContainerPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of @self.export.Name@ and the Twin object 
    return 0;
}

// constructor method
int ExtensionContainerPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject *ExtensionContainerPy::getCustomAttributes(const char* attr) const
{
    if (Base::streq(attr, "__dict__")) {
        PyObject* dict = PyDict_New();
        PyObject* props = PropertyContainerPy::getCustomAttributes("__dict__");
        if (props && PyDict_Check(props)) {
            PyDict_Merge(dict, props, 0);
            Py_DECREF(props);
        }

        ExtensionContainer::ExtensionIterator it = this->getExtensionContainerPtr()->extensionBegin();
        for (; it != this->getExtensionContainerPtr()->extensionEnd(); ++it) {
            // The PyTypeObject is shared by all instances of this type and therefore
            // we have to add new methods only once.
            PyObject* obj = (*it).second->getExtensionPyObject();
            PyTypeObject *tp = Py_TYPE(obj);
            if (tp && tp->tp_dict) {
                Py_XINCREF(tp->tp_dict);
                PyDict_Merge(dict, tp->tp_dict, 0);
                Py_XDECREF(tp->tp_dict);
            }
            Py_DECREF(obj);
        }

        return dict;
    }
    // Search for the method called 'attr' in the extensions. If the search with
    // Py_FindMethod is successful then a PyCFunction_New instance is returned
    // with the PyObject pointer of the extension to make sure the method will
    // be called for the correct instance.
    PyObject *func = 0;
    ExtensionContainer::ExtensionIterator it = this->getExtensionContainerPtr()->extensionBegin();
    for (; it != this->getExtensionContainerPtr()->extensionEnd(); ++it) {
        // The PyTypeObject is shared by all instances of this type and therefore
        // we have to add new methods only once.
        PyObject* obj = (*it).second->getExtensionPyObject();
        PyObject *nameobj = PyUnicode_FromString(attr);
        func = PyObject_GenericGetAttr(obj, nameobj);
        Py_DECREF(nameobj);
        Py_DECREF(obj);
        if (func && PyCFunction_Check(func)) {
            PyCFunctionObject* cfunc = reinterpret_cast<PyCFunctionObject*>(func);

            // OK, that's what we wanted
            if (cfunc->m_self == obj)
                break;
            // otherwise cleanup the result again
            Py_DECREF(func);
            func = 0;
        }
        PyErr_Clear(); // clear the error set inside Py_FindMethod
    }

    return func;
}

int ExtensionContainerPy::setCustomAttributes(const char* /*attr*/, PyObject * /*obj*/)
{
    return 0;
}

PyObject* ExtensionContainerPy::hasExtension(PyObject *args) {

    char *type;
    if (!PyArg_ParseTuple(args, "s", &type)) 
        return NULL;                                         // NULL triggers exception 

    //get the extension type asked for
    Base::Type extension =  Base::Type::fromName(type);
    if(extension.isBad() || !extension.isDerivedFrom(App::Extension::getExtensionClassTypeId())) {
        std::stringstream str;
        str << "No extension found of type '" << type << "'" << std::ends;
        throw Py::Exception(Base::BaseExceptionFreeCADError,str.str());
    }

    bool val = false;
    if (getExtensionContainerPtr()->hasExtension(extension)) {
        val = true;
    }

    return PyBool_FromLong(val ? 1 : 0);
}

PyObject* ExtensionContainerPy::addExtension(PyObject *args) {

    char *typeId;
    PyObject* proxy;
    if (!PyArg_ParseTuple(args, "sO", &typeId, &proxy))
        return NULL;

    //get the extension type asked for
    Base::Type extension =  Base::Type::fromName(typeId);
    if (extension.isBad() || !extension.isDerivedFrom(App::Extension::getExtensionClassTypeId())) {
        std::stringstream str;
        str << "No extension found of type '" << typeId << "'" << std::ends;
        throw Py::Exception(Base::BaseExceptionFreeCADError,str.str());
    }

    //register the extension
    App::Extension* ext = static_cast<App::Extension*>(extension.createInstance());
    //check if this really is a python extension!
    if (!ext->isPythonExtension()) {
        delete ext;
        std::stringstream str;
        str << "Extension is not a python addable version: '" << typeId << "'" << std::ends;
        throw Py::Exception(Base::BaseExceptionFreeCADError,str.str());
    }

    ext->initExtension(getExtensionContainerPtr());

    //set the proxy to allow python overrides
    App::Property* pp = ext->extensionGetPropertyByName("ExtensionProxy");
    if (!pp) {
        std::stringstream str;
        str << "Accessing the proxy property failed!" << std::ends;
        throw Py::Exception(Base::BaseExceptionFreeCADError,str.str());
    }
    static_cast<PropertyPythonObject*>(pp)->setPyObject(proxy);

    // The PyTypeObject is shared by all instances of this type and therefore
    // we have to add new methods only once.
    PyObject* obj = ext->getExtensionPyObject();
    PyMethodDef* meth = reinterpret_cast<PyMethodDef*>(obj->ob_type->tp_methods);
    PyTypeObject *type = this->ob_type;
    PyObject *dict = type->tp_dict;

    // make sure to do the initialization only once
    if (meth->ml_name) {
        PyObject* item = PyDict_GetItemString(dict, meth->ml_name);
        if (item == NULL) {
            // Note: this adds the methods to the type object to make sure
            // it appears in the call tips. The function will not be bound
            // to an instance
            Py_INCREF(dict);
            while (meth->ml_name) {
                PyObject *func;
                func = PyCFunction_New(meth, 0);
                if (func == NULL)
                    break;
                if (PyDict_SetItemString(dict, meth->ml_name, func) < 0)
                    break;
                Py_DECREF(func);
                ++meth;
            }

            Py_DECREF(dict);
        }
    }

    Py_DECREF(obj);

    Py_Return;
}

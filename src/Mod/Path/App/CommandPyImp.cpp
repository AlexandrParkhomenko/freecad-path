/***************************************************************************
 *   Copyright (c) Yorik van Havre (yorik@uncreated.net) 2014              *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#include <boost/algorithm/string.hpp>

#include "Base/Exception.h"
#include "Base/Vector3D.h"
#include "Base/VectorPy.h"
#include "Base/PlacementPy.h"
#include "Mod/Path/App/Command.h"

// files generated out of CommandPy.xml
#include "CommandPy.h"
#include "CommandPy.cpp"

using namespace Path;

// returns a string which represents the object e.g. when printed in python
std::string CommandPy::representation(void) const
{
    std::stringstream str;
    str.precision(5);
    str << "Command ";
    str << getCommandPtr()->Name;
    str << " [";
    for(std::map<std::string,double>::iterator i = getCommandPtr()->Parameters.begin(); i != getCommandPtr()->Parameters.end(); ++i) {
        std::string k = i->first;
        double v = i->second;
        str << " " << k << ":" << v;
    }
    str << " ]";
    return str.str();
}


PyObject *CommandPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of CommandPy and the Twin object
    return new CommandPy(new Command);
}

// constructor method
int CommandPy::PyInit(PyObject* args, PyObject* kwd)
{
    PyObject *parameters = NULL;
    char *name = "";
    static char *kwlist[] = {"name", "parameters", NULL};
    if ( PyArg_ParseTupleAndKeywords(args, kwd, "|sO!", kwlist, &name, &PyDict_Type, &parameters) ) {
        std::string sname(name);
        boost::to_upper(sname);
        try {
            if (!sname.empty())
                getCommandPtr()->setFromGCode(name);
        }
        catch (const Base::Exception& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
            return -1;
        }

        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (parameters && PyDict_Next(parameters, &pos, &key, &value)) {
            std::string ckey;
            if (PyUnicode_Check(key)) {
                ckey = PyUnicode_AsUTF8(key);

            }
            else {
                PyErr_SetString(PyExc_TypeError, "The dictionary can only contain string keys");
                return -1;
            }

            boost::to_upper(ckey);
            double cvalue;
            if (PyObject_TypeCheck(value,&(PyLong_Type))) {
                cvalue = (double)PyLong_AsLong(value);

            }
            else if (PyObject_TypeCheck(value, &(PyFloat_Type))) {
                cvalue = PyFloat_AsDouble(value);
            }
            else {
                PyErr_SetString(PyExc_TypeError, "The dictionary can only contain number values");
                return -1;
            }
            getCommandPtr()->Parameters[ckey]=cvalue;
        }
        return 0;
    }
    PyErr_Clear(); // set by PyArg_ParseTuple()

    if ( PyArg_ParseTupleAndKeywords(args, kwd, "|sO!", kwlist, &name, &(Base::PlacementPy::Type), &parameters) ) {
        std::string sname(name);
        boost::to_upper(sname);
        try {
            if (!sname.empty())
                getCommandPtr()->setFromGCode(name);
        }
        catch (const Base::Exception& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
            return -1;
        }
        Base::PlacementPy *p = static_cast<Base::PlacementPy*>(parameters);
        getCommandPtr()->setFromPlacement( *p->getPlacementPtr() );
        return 0;
    }
    return -1;
}

// Name attribute

Py::String CommandPy::getName(void) const
{
    return Py::String(getCommandPtr()->Name.c_str());
}

void CommandPy::setName(Py::String arg)
{
    std::string cmd = arg.as_std_string();
    boost::to_upper(cmd);
    getCommandPtr()->Name = cmd;
}

// Parameters attribute get/set

Py::Dict CommandPy::getParameters(void) const
{
    PyObject *dict = PyDict_New();
    for(std::map<std::string,double>::iterator i = getCommandPtr()->Parameters.begin(); i != getCommandPtr()->Parameters.end(); ++i) {
        PyDict_SetItem(dict,PyUnicode_FromString(i->first.c_str()),PyFloat_FromDouble(i->second));

    }
    return Py::Dict(dict);
}

void CommandPy::setParameters(Py::Dict arg)
{
    PyObject* dict_copy = PyDict_Copy(arg.ptr());
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(dict_copy, &pos, &key, &value)) {
        std::string ckey;
        if (PyUnicode_Check(key)) {
            ckey = PyUnicode_AsUTF8(key);

        }
        else {
            throw Py::TypeError("The dictionary can only contain string keys");
        }

        boost::to_upper(ckey);
        double cvalue;
        if (PyObject_TypeCheck(value,&(PyLong_Type))) {
            cvalue = (double)PyLong_AsLong(value);

        }
        else if (PyObject_TypeCheck(value, &(PyFloat_Type))) {
            cvalue = PyFloat_AsDouble(value);
        }
        else {
            throw Py::TypeError("The dictionary can only contain number values");
        }
        getCommandPtr()->Parameters[ckey]=cvalue;
    }
}

// GCode methods

PyObject* CommandPy::toGCode(PyObject *args)
{
    if (PyArg_ParseTuple(args, "")) {
        return PyUnicode_FromString(getCommandPtr()->toGCode().c_str());

    }
    throw Py::TypeError("This method accepts no argument");
}

PyObject* CommandPy::setFromGCode(PyObject *args)
{
    char *pstr=0;
    if (PyArg_ParseTuple(args, "s", &pstr)) {
        std::string gcode(pstr);
        try {
            getCommandPtr()->setFromGCode(gcode);
        }
        catch (const Base::Exception& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
            return nullptr;
        }

        Py_INCREF(Py_None);
        return Py_None;
    }
    throw Py::TypeError("Argument must be a string");
}

// Placement attribute get/set

Py::Object CommandPy::getPlacement(void) const
{
    return Py::Object(new Base::PlacementPy(new Base::Placement(getCommandPtr()->getPlacement())));
}

void CommandPy::setPlacement(Py::Object arg)
{
    union PyType_Object pyType = {&(Base::PlacementPy::Type)};
    Py::Type PlacementType(pyType.o);
    if(arg.isType(PlacementType)) {
        getCommandPtr()->setFromPlacement( *static_cast<Base::PlacementPy*>((*arg))->getPlacementPtr() );
    } else
    throw Py::TypeError("Argument must be a placement");
}

PyObject* CommandPy::transform(PyObject *args)
{
    PyObject *placement;
    if ( PyArg_ParseTuple(args, "O!", &(Base::PlacementPy::Type), &placement) ) {
        Base::PlacementPy *p = static_cast<Base::PlacementPy*>(placement);
        Path::Command trCmd = getCommandPtr()->transform( *p->getPlacementPtr() );
        return new CommandPy(new Path::Command(trCmd));
    } else
    throw Py::TypeError("Argument must be a placement");
}

// custom attributes get/set

PyObject *CommandPy::getCustomAttributes(const char* attr) const
{
    std::string satt(attr);
    if (satt.length() == 1) {
        if (isalpha(satt[0])) {
            boost::to_upper(satt);
            if (getCommandPtr()->Parameters.count(satt)) {
                return PyFloat_FromDouble(getCommandPtr()->Parameters[satt]);
            }
            Py_INCREF(Py_None);
            return Py_None;
        }
    }
    return 0;
}

int CommandPy::setCustomAttributes(const char* attr, PyObject* obj)
{
    std::string satt(attr);
    if (satt.length() == 1) {
        if (isalpha(satt[0])) {
            boost::to_upper(satt);
            double cvalue;
            if (PyObject_TypeCheck(obj,&(PyLong_Type))) {
                cvalue = (double)PyLong_AsLong(obj);

            } else if (PyObject_TypeCheck(obj,&(PyFloat_Type))) {
                cvalue = PyFloat_AsDouble(obj);
            } else {
                return 0;
            }
            getCommandPtr()->Parameters[satt]=cvalue;
            return 1;
        }
    }
    return 0;
}





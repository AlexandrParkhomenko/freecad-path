/***************************************************************************
 *   Copyright (c) Yorik van Havre (yorik@uncreated.net) 2014              *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/



#include "Mod/Path/App/Path.h"

// inclusion of the generated files (generated out of PathPy.xml)
#include "PathPy.h"
#include "PathPy.cpp"

#include "Base/GeometryPyCXX.h"
#include "CommandPy.h"

using namespace Path;

// returns a string which represents the object e.g. when printed in python
std::string PathPy::representation(void) const
{
    std::stringstream str;
    str.precision(5);
    str << "Path [ ";
    str << "size:" << getToolpathPtr()->getSize() << " ";
    str << "length:" << getToolpathPtr()->getLength();
    str << " ]";

    return str.str();
}

PyObject *PathPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of PathPy and the Twin object 
    return new PathPy(new Toolpath);
}

// constructor method
int PathPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *pcObj=0;
    char *gcode;
    if (PyArg_ParseTuple(args, "|O!", &(PyList_Type), &pcObj)) {
        if (pcObj) {
            Py::List list(pcObj);
            for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
                if (PyObject_TypeCheck((*it).ptr(), &(Path::CommandPy::Type))) {
                    Path::Command &cmd = *static_cast<Path::CommandPy*>((*it).ptr())->getCommandPtr();
                    getToolpathPtr()->addCommand(cmd);
                } else {
                    PyErr_SetString(PyExc_TypeError, "The list must contain only Path Commands");
                    return -1; 
                }
            }
        }
        return 0;
    }
    PyErr_Clear(); // set by PyArg_ParseTuple()
    if (PyArg_ParseTuple(args, "|s", &gcode)) {
        getToolpathPtr()->setFromGCode(gcode);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Argument must be a list of commands or a gcode string");
    return -1; 
}


// Commands get/set

Py::List PathPy::getCommands(void) const
{
    Py::List list;
    for(unsigned int i = 0; i < getToolpathPtr()->getSize(); i++)
        list.append(Py::Object(new Path::CommandPy(new Path::Command(getToolpathPtr()->getCommand(i)))));
    return list;
}

void PathPy::setCommands(Py::List list)
{
    getToolpathPtr()->clear();
    for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
        if (PyObject_TypeCheck((*it).ptr(), &(Path::CommandPy::Type))) {
            Path::Command &cmd = *static_cast<Path::CommandPy*>((*it).ptr())->getCommandPtr();
            getToolpathPtr()->addCommand(cmd);
        } else {
            throw Py::TypeError("The list can only contain Path Commands");
        }
    }
}

Py::Object PathPy::getCenter(void) const
{
    return Py::Vector(getToolpathPtr()->getCenter());
}

void PathPy::setCenter(Py::Object obj)
{
    getToolpathPtr()->setCenter(Py::Vector(obj).toVector());
}

// read-only attributes

Py::Float PathPy::getLength(void) const
{
    return Py::Float(getToolpathPtr()->getLength());
}

Py::Long PathPy::getSize(void) const
{
    return Py::Long((long)getToolpathPtr()->getSize());
}

// specific methods

PyObject* PathPy::copy(PyObject * args)
{
    if (PyArg_ParseTuple(args, "")) {
        return new PathPy(new Path::Toolpath(*getToolpathPtr()));
    }
    throw Py::TypeError("This method accepts no argument");
}

PyObject* PathPy::addCommands(PyObject * args)
{
    PyObject* o;
    if (PyArg_ParseTuple(args, "O!", &(Path::CommandPy::Type), &o)) {
        Path::Command &cmd = *static_cast<Path::CommandPy*>(o)->getCommandPtr();
        getToolpathPtr()->addCommand(cmd);
        return new PathPy(new Path::Toolpath(*getToolpathPtr()));
        //Py_Return;
    }
    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &(PyList_Type), &o)) {
        Py::List list(o);
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            if (PyObject_TypeCheck((*it).ptr(), &(Path::CommandPy::Type))) {
                Path::Command &cmd = *static_cast<Path::CommandPy*>((*it).ptr())->getCommandPtr();
                getToolpathPtr()->addCommand(cmd);
            }
        }
        return new PathPy(new Path::Toolpath(*getToolpathPtr()));
    }
    Py_Error(Base::BaseExceptionFreeCADError, "Wrong parameters - command or list of commands expected");
}

PyObject* PathPy::insertCommand(PyObject * args)
{
    PyObject* o;
    int pos = -1;
    if (PyArg_ParseTuple(args, "O!|i", &(Path::CommandPy::Type), &o, &pos)) {
        Path::Command &cmd = *static_cast<Path::CommandPy*>(o)->getCommandPtr();
        getToolpathPtr()->insertCommand(cmd,pos);
        return new PathPy(new Path::Toolpath(*getToolpathPtr()));
    }
    Py_Error(Base::BaseExceptionFreeCADError, "Wrong parameters - expected command and optional integer");
}

PyObject* PathPy::deleteCommand(PyObject * args)
{
    int pos = -1;
    if (PyArg_ParseTuple(args, "|i", &pos)) {
        getToolpathPtr()->deleteCommand(pos);
        return new PathPy(new Path::Toolpath(*getToolpathPtr()));
    }
    Py_Error(Base::BaseExceptionFreeCADError, "Wrong parameters - expected an integer (optional)");
}

// GCode methods

PyObject* PathPy::toGCode(PyObject * args)
{
    if (PyArg_ParseTuple(args, "")) {
        std::string result = getToolpathPtr()->toGCode();
        return PyUnicode_FromString(result.c_str());

    }
    throw Py::TypeError("This method accepts no argument");
}

PyObject* PathPy::setFromGCode(PyObject * args)
{
    char *pstr=0;
    if (PyArg_ParseTuple(args, "s", &pstr)) {
        std::string gcode(pstr);
        getToolpathPtr()->setFromGCode(gcode);
        Py_INCREF(Py_None);
        return Py_None;
    }
    throw Py::TypeError("Argument must be a string");
}

// custom attributes get/set

PyObject *PathPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int PathPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}



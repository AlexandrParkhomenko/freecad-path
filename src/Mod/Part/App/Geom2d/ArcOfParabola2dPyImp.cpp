/***************************************************************************
 *   Copyright (c) 2016 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


# include <gp_Parab2d.hxx>
# include <Geom2d_Parabola.hxx>
# include <GCE2d_MakeArcOfParabola.hxx>
# include <Geom2d_TrimmedCurve.hxx>

#include "Mod/Part/App/Geometry2d.h"
#include "Mod/Part/App/Geom2d/ArcOfParabola2dPy.h"
#include <Mod/Part/App/Geom2d/ArcOfParabola2dPy.cpp>
#include "Mod/Part/App/Geom2d/Parabola2dPy.h"
#include "Mod/Part/App/OCCError.h"

#include "Base/GeometryPyCXX.h"

using namespace Part;

extern const char* gce_ErrorStatusText(gce_ErrorType et);

// returns a string which represents the object e.g. when printed in python
std::string ArcOfParabola2dPy::representation(void) const
{
    return "<ArcOfParabola2d object>";
}

PyObject *ArcOfParabola2dPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of ArcOfParabola2dPy and the Twin object
    return new ArcOfParabola2dPy(new Geom2dArcOfParabola);
}

// constructor method
int ArcOfParabola2dPy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    PyObject* o;
    double u1, u2;
    PyObject *sense=Py_True;
    if (PyArg_ParseTuple(args, "O!dd|O!", &(Part::Parabola2dPy::Type), &o, &u1, &u2, &PyBool_Type, &sense)) {
        try {
            Handle(Geom2d_Parabola) parabola = Handle(Geom2d_Parabola)::DownCast
                (static_cast<Parabola2dPy*>(o)->getGeom2dParabolaPtr()->handle());
            GCE2d_MakeArcOfParabola arc(parabola->Parab2d(), u1, u2, PyObject_IsTrue(sense) ? Standard_True : Standard_False);
            if (!arc.IsDone()) {
                PyErr_SetString(PartExceptionOCCError, gce_ErrorStatusText(arc.Status()));
                return -1;
            }

            getGeom2dArcOfParabolaPtr()->setHandle(arc.Value());
            return 0;
        }
        catch (Standard_Failure& e) {
    
            PyErr_SetString(PartExceptionOCCError, e.GetMessageString());
            return -1;
        }
        catch (...) {
            PyErr_SetString(PartExceptionOCCError, "creation of arc failed");
            return -1;
        }
    }
    
    // All checks failed
    PyErr_SetString(PyExc_TypeError,
        "ArcOfParabola2d constructor expects an parabola curve and a parameter range");
    return -1;
}

Py::Float ArcOfParabola2dPy::getFocal(void) const
{
    return Py::Float(getGeom2dArcOfParabolaPtr()->getFocal());
}

void  ArcOfParabola2dPy::setFocal(Py::Float arg)
{
    getGeom2dArcOfParabolaPtr()->setFocal((double)arg);
}

Py::Object ArcOfParabola2dPy::getParabola(void) const
{
    Handle(Geom2d_TrimmedCurve) trim = Handle(Geom2d_TrimmedCurve)::DownCast
        (getGeom2dArcOfParabolaPtr()->handle());
    Handle(Geom2d_Parabola) parabola = Handle(Geom2d_Parabola)::DownCast(trim->BasisCurve());
    return Py::asObject(new Parabola2dPy(new Geom2dParabola(parabola)));
}

PyObject *ArcOfParabola2dPy::getCustomAttributes(const char* ) const
{
    return 0;
}

int ArcOfParabola2dPy::setCustomAttributes(const char* , PyObject *)
{
    return 0; 
}

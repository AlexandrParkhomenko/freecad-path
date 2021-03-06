/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2008     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/



#include "TopoShape.h"
#include <BRep_Builder.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

#include "OCCError.h"

// inclusion of the generated files (generated out of TopoShapeCompoundPy.xml)
#include "TopoShapeCompoundPy.h"
#include "TopoShapeCompoundPy.cpp"

using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string TopoShapeCompoundPy::representation(void) const
{
    std::stringstream str;
    str << "<Compound object at " << getTopoShapePtr() << ">";
    return str.str();
}

PyObject *TopoShapeCompoundPy::PyMake(struct _typeobject *, PyObject *, PyObject *)
{
    return new TopoShapeCompoundPy(new TopoShape);
}

// constructor method
int TopoShapeCompoundPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O", &pcObj))
        return -1;

    BRep_Builder builder;
    TopoDS_Compound Comp;
    builder.MakeCompound(Comp);
    
    try {
        Py::Sequence list(pcObj);
        for (Py::Sequence::iterator it = list.begin(); it != list.end(); ++it) {
            if (PyObject_TypeCheck((*it).ptr(), &(Part::TopoShapePy::Type))) {
                const TopoDS_Shape& sh = static_cast<TopoShapePy*>((*it).ptr())->
                    getTopoShapePtr()->getShape();
                if (!sh.IsNull())
                    builder.Add(Comp, sh);
            }
        }
    }
    catch (Standard_Failure& e) {

        PyErr_SetString(PartExceptionOCCError, e.GetMessageString());
        return -1;
    }

    getTopoShapePtr()->setShape(Comp);
    return 0;
}

PyObject*  TopoShapeCompoundPy::add(PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(Part::TopoShapePy::Type), &obj))
        return NULL;

    BRep_Builder builder;
    TopoDS_Shape comp = getTopoShapePtr()->getShape();
    
    try {
        const TopoDS_Shape& sh = static_cast<TopoShapePy*>(obj)->
            getTopoShapePtr()->getShape();
        if (!sh.IsNull())
            builder.Add(comp, sh);
    }
    catch (Standard_Failure& e) {

        PyErr_SetString(PartExceptionOCCError, e.GetMessageString());
        return 0;
    }

    getTopoShapePtr()->setShape(comp);

    Py_Return;
}

PyObject* TopoShapeCompoundPy::connectEdgesToWires(PyObject *args)
{
    PyObject *shared=Py_True;
    double tol = Precision::Confusion();
    if (!PyArg_ParseTuple(args, "|O!d",&PyBool_Type,&shared,&tol))
        return 0;

    try {
        const TopoDS_Shape& s = getTopoShapePtr()->getShape();

        Handle(TopTools_HSequenceOfShape) hEdges = new TopTools_HSequenceOfShape();
        Handle(TopTools_HSequenceOfShape) hWires = new TopTools_HSequenceOfShape();
        for (TopExp_Explorer xp(s, TopAbs_EDGE); xp.More(); xp.Next())
            hEdges->Append(xp.Current());

        ShapeAnalysis_FreeBounds::ConnectEdgesToWires(hEdges, tol, PyObject_IsTrue(shared) ? Standard_True : Standard_False, hWires);
     
        TopoDS_Compound comp;
        BRep_Builder builder;
        builder.MakeCompound(comp);

        int len = hWires->Length();
        for(int i=1;i<=len;i++) {
            builder.Add(comp, hWires->Value(i));
        }

        getTopoShapePtr()->setShape(comp);
        return new TopoShapeCompoundPy(new TopoShape(comp));
    }
    catch (Standard_Failure& e) {

        PyErr_SetString(PartExceptionOCCError, e.GetMessageString());
        return 0;
    }
}

PyObject *TopoShapeCompoundPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TopoShapeCompoundPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}

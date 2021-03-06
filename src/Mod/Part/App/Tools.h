/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef PART_TOOLS_H
#define PART_TOOLS_H

#include "stdexport.h"
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>
#include <Geom_Surface.hxx>
#include <TColStd_ListOfTransient.hxx>

class gp_Lin;
class gp_Pln;

namespace Base {
// Specialization for gp_Pnt
template <>
struct vec_traits<gp_Pnt> {
    typedef gp_Pnt vec_type;
    typedef double float_type;
    vec_traits(const vec_type& v) : v(v){}
    inline float_type x() { return v.X(); }
    inline float_type y() { return v.Y(); }
    inline float_type z() { return v.Z(); }
private:
    const vec_type& v;
};
// Specialization for gp_Vec
template <>
struct vec_traits<gp_Vec> {
    typedef gp_Vec vec_type;
    typedef double float_type;
    vec_traits(const vec_type& v) : v(v){}
    inline float_type x() { return v.X(); }
    inline float_type y() { return v.Y(); }
    inline float_type z() { return v.Z(); }
private:
    const vec_type& v;
};
// Specialization for gp_Dir
template <>
struct vec_traits<gp_Dir> {
    typedef gp_Dir vec_type;
    typedef double float_type;
    vec_traits(const vec_type& v) : v(v){}
    inline float_type x() { return v.X(); }
    inline float_type y() { return v.Y(); }
    inline float_type z() { return v.Z(); }
private:
    const vec_type& v;
};
// Specialization for gp_XYZ
template <>
struct vec_traits<gp_XYZ> {
    typedef gp_XYZ vec_type;
    typedef double float_type;
    vec_traits(const vec_type& v) : v(v){}
    inline float_type x() { return v.X(); }
    inline float_type y() { return v.Y(); }
    inline float_type z() { return v.Z(); }
private:
    const vec_type& v;
};
}

namespace Part
{

Standard_EXPORT
void closestPointsOnLines(const gp_Lin& lin1, const gp_Lin& lin2, gp_Pnt &p1, gp_Pnt &p2);
Standard_EXPORT
bool intersect(const gp_Pln& pln1, const gp_Pln& pln2, gp_Lin& lin);
Standard_EXPORT
bool tangentialArc(const gp_Pnt& p0, const gp_Vec& v0, const gp_Pnt& p1, gp_Pnt& c, gp_Dir& a);

class Standard_EXPORT Tools
{
public:
    Handle(Geom_Surface) makeSurface (const TColStd_ListOfTransient& theBoundaries,
                                     const Standard_Real theTol,
                                     const Standard_Integer theNbPnts,
                                     const Standard_Integer theNbIter,
                                     const Standard_Integer theMaxDeg);

};

} //namespace Part


#endif // PART_TOOLS_H

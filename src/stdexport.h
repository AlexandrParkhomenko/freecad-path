/*
  Copyright (c) 2019 Alexandr Parkhomenko
  GPLv2
*/

#ifndef STDEXPORT_H
#define STDEXPORT_H
#define Standard_EXPORT  //#OSDEPENDENT in WINDOWS __declspec(dllexport)
#define BaseExport
#define GuiExport
#define AppExport
#define DataExport

#define ImportExport
#define MeshExport
#define MeshPartExport
#define PartExport
  #define AttacherExport
#define PathExport
#define PointsExport
#define SketcherExport

#define ImportGuiExport
#define MeshGuiExport
#define MeshPartGuiExport
#define PartGuiExport
#define PathGuiExport
#define PointsGuiExport
#define SketcherGuiExport
#endif //STDEXPORT_H

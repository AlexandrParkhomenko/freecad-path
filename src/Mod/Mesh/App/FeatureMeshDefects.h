/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef MESH_FEATURE_MESH_DEFECTS_H
#define MESH_FEATURE_MESH_DEFECTS_H

#include "stdexport.h"
#include "App/PropertyStandard.h"
#include "App/PropertyLinks.h"
#include "MeshFeature.h"

namespace Mesh
{

/**
 * The FixDefects class fixes defects in the attached mesh kernel.
 * @author Werner Mayer
 */
class Standard_EXPORT FixDefects : public Mesh::Feature
{
  PROPERTY_HEADER(Mesh::FixDefects);

public:
  /// Constructor
  FixDefects(void);
  virtual ~FixDefects();

  /** @name Properties */
  //@{
  App::PropertyLink   Source;
  App::PropertyFloat  Epsilon;
  //@}

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  short mustExecute() const;
  //@}

  /// returns the type name of the ViewProvider
//  virtual const char* getViewProviderName(void) const {return "MeshGui::ViewProviderDefects";}
};

/**
 * The HarmonizeNormals class harmonizes the normals of the faces.
 * @author Werner Mayer
 */
class Standard_EXPORT HarmonizeNormals : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::HarmonizeNormals);

public:
  /// Constructor
  HarmonizeNormals(void);
  virtual ~HarmonizeNormals();

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The FlipNormals class flips the normals of all faces.
 * @author Werner Mayer
 */
class Standard_EXPORT FlipNormals : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FlipNormals);

public:
  /// Constructor
  FlipNormals(void);
  virtual ~FlipNormals();

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The FixNonManifolds class tries to fix edges with more than two faces attached.
 * @author Werner Mayer
 */
class Standard_EXPORT FixNonManifolds : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixNonManifolds);

public:
  /// Constructor
  FixNonManifolds(void);
  virtual ~FixNonManifolds();

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The FixDuplicatedFaces class removes duplicated faces from the mesh.
 * @author Werner Mayer
 */
class Standard_EXPORT FixDuplicatedFaces : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixDuplicatedFaces);

public:
  /// Constructor
  FixDuplicatedFaces(void);
  virtual ~FixDuplicatedFaces();

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The FixDuplicatedPoints class removes duplicated points from the mesh.
 * @author Werner Mayer
 */
class Standard_EXPORT FixDuplicatedPoints : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixDuplicatedPoints);

public:
  /// Constructor
  FixDuplicatedPoints(void);
  virtual ~FixDuplicatedPoints();

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The FixDegenerations class removes degenerated faces from the mesh.
 * @author Werner Mayer
 */
class Standard_EXPORT FixDegenerations : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixDegenerations);

public:
  /// Constructor
  FixDegenerations(void);
  virtual ~FixDegenerations();

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The FixDeformations class tries to repair deformed faces by swapping edge operations.
 * @author Werner Mayer
 */
class Standard_EXPORT FixDeformations : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixDeformations);

public:
  /// Constructor
  FixDeformations(void);
  virtual ~FixDeformations();

  /** @name Properties */
  //@{
  App::PropertyFloat MaxAngle; /**< Maximum allowed angle */
  //@}
  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The FixIndices class tries to validate invalid indices of the internal mesh data structure.
 * @author Werner Mayer
 */
class Standard_EXPORT FixIndices : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FixIndices);

public:
  /// Constructor
  FixIndices(void);
  virtual ~FixIndices();

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The FillHoles class tries to fill up holes in the internal mesh data structure.
 * @author Werner Mayer
 */
class Standard_EXPORT FillHoles : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::FillHoles);

public:
  /// Constructor
  FillHoles(void);
  virtual ~FillHoles();
  App::PropertyInteger FillupHolesOfLength;
  App::PropertyFloat MaxArea;

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

/**
 * The RemoveComponents class removes topologic independent components up to a certain size from the mesh data structure.
 * @author Werner Mayer
 */
class Standard_EXPORT RemoveComponents : public Mesh::FixDefects
{
  PROPERTY_HEADER(Mesh::RemoveComponents);

public:
  /// Constructor
  RemoveComponents(void);
  virtual ~RemoveComponents();
  App::PropertyInteger RemoveCompOfSize;

  /** @name methods override Feature */
  //@{
  /// recalculate the Feature
  virtual App::DocumentObjectExecReturn *execute(void);
  //@}
};

} //namespace Mesh


#endif // MESH_FEATURE_MESH_DEFECTS_H 

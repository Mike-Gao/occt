// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#ifndef Convert_Tools_H
#define Convert_Tools_H

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_OBB.hxx>
#include <NCollection_List.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Standard_Dump.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx> 
#include <Standard_SStream.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QString>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class Geom_Transformation;

//! \class Convert_Tools
//! \brief The tool that gives auxiliary methods for qt elements manipulation
class Convert_Tools
{
public:
  //! Creates shape presentations on the stream if possible. Tries to init some OCCT base for a new presentation
  //! \param theStream source of presentation
  //! \param thePresentations container to add new presentation/s
  Standard_EXPORT static void ConvertStreamToPresentations (const Standard_SStream& theSStream,
                                                            const Standard_Integer theStartPos,
                                                            const Standard_Integer theLastPos,
                                                            NCollection_List<Handle(Standard_Transient)>& thePresentations);

  //! Creates box shape
  //! \param theBoundingBox box shape parameters
  //! \return created shape
  Standard_EXPORT static TopoDS_Shape CreateShape (const Bnd_Box& theBoundingBox);

  //! Creates box shape
  //! \param theBoundingBox box shape parameters
  //! \return created shape
  Standard_EXPORT static TopoDS_Shape CreateShape (const Bnd_OBB& theBoundingBox);

  //! Creates box shape
  //! \param thePntMin minimum point on the bounding box
  //! \param thePntMax maximum point on the bounding box
  //! \return created shape
  Standard_EXPORT static TopoDS_Shape CreateBoxShape (const gp_Pnt& thePntMin, const gp_Pnt& thePntMax);

};

#endif

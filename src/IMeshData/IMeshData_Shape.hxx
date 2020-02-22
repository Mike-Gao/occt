// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _IMeshData_Shape_HeaderFile
#define _IMeshData_Shape_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_Dump.hxx>
#include <TopoDS_Shape.hxx>

//! Interface class representing model with associated TopoDS_Shape.
//! Intended for inheritance by structures and algorithms keeping 
//! reference TopoDS_Shape.
class IMeshData_Shape : public Standard_Transient
{
public:

  //! Destructor.
  Standard_EXPORT virtual ~IMeshData_Shape()
  {
  }

  //! Assigns shape to discrete shape.
  inline void SetShape (const TopoDS_Shape& theShape)
  {
    myShape = theShape;
  }

  //! Returns shape assigned to discrete shape.
  const TopoDS_Shape& GetShape () const
  {
    return myShape;
  }

  //! Dumps the content of me into the stream
  virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const
  {
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myShape)
  }

  DEFINE_STANDARD_RTTI_INLINE(IMeshData_Shape, Standard_Transient)

protected:

  //! Constructor.
  Standard_EXPORT IMeshData_Shape()
  {
  }

  //! Constructor.
  Standard_EXPORT IMeshData_Shape (const TopoDS_Shape& theShape)
    : myShape(theShape)
  {
  }

private:

  TopoDS_Shape myShape;
};

#endif
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

#ifndef _IMeshData_TessellatedShape_HeaderFile
#define _IMeshData_TessellatedShape_HeaderFile

#include <IMeshData_Shape.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

//! Interface class representing shaped model with deflection.
class IMeshData_TessellatedShape : public IMeshData_Shape
{
public:

  //! Destructor.
  Standard_EXPORT virtual ~IMeshData_TessellatedShape()
  {
  }

  //! Gets deflection value for the discrete model.
  inline Standard_Real GetDeflection () const
  {
    return myDeflection;
  }

  //! Sets deflection value for the discrete model.
  inline void SetDeflection (const Standard_Real theValue)
  {
    myDeflection = theValue;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE
  {
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
    OCCT_DUMP_BASE_CLASS (theOStream, theDepth, IMeshData_Shape)

    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDeflection)
  }


  DEFINE_STANDARD_RTTI_INLINE(IMeshData_TessellatedShape, IMeshData_Shape)

protected:

  //! Constructor.
  Standard_EXPORT IMeshData_TessellatedShape ()
    : myDeflection(RealLast())
  {
  }

  //! Constructor.
  Standard_EXPORT IMeshData_TessellatedShape (const TopoDS_Shape& theShape)
    : IMeshData_Shape(theShape),
      myDeflection(RealLast())
  {
  }

private:

  Standard_Real myDeflection;
};

#endif
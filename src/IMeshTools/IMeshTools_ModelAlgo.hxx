// Created on: 2016-04-19
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

#ifndef _IMeshTools_ModelAlgo_HeaderFile
#define _IMeshTools_ModelAlgo_HeaderFile

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

class IMeshData_Model;
struct IMeshTools_Parameters;

//! Interface class providing API for algorithms intended to update or modify discrete model.
class IMeshTools_ModelAlgo : public Standard_Transient
{
public:

  //! Destructor.
  Standard_EXPORT virtual ~IMeshTools_ModelAlgo()
  {
  }

  //! Exceptions protected processing of the given model.
  Standard_Boolean Perform (
    const Handle (IMeshData_Model)& theModel,
    const IMeshTools_Parameters&    theParameters)
  {
    try
    {
      OCC_CATCH_SIGNALS

      return performInternal (theModel, theParameters);
    }
    catch (Standard_Failure const&)
    {
      return Standard_False;
    }
  }

  //! Dumps the content of me into the stream
  virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const
  {
    (void)theDepth;
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  }

  DEFINE_STANDARD_RTTI_INLINE(IMeshTools_ModelAlgo, Standard_Transient)

protected:

  //! Constructor.
  Standard_EXPORT IMeshTools_ModelAlgo()
  {
  }

  //! Performs processing of the given model.
  Standard_EXPORT virtual Standard_Boolean performInternal (
    const Handle (IMeshData_Model)& theModel,
    const IMeshTools_Parameters&    theParameters) = 0;
};

#endif
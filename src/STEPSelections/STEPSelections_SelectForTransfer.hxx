// Created on: 2003-06-02
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _STEPSelections_SelectForTransfer_HeaderFile
#define _STEPSelections_SelectForTransfer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XSSelect_SelectForTransfer.hxx>
class Interface_EntityIterator;
class Interface_Graph;


class STEPSelections_SelectForTransfer;
DEFINE_STANDARD_HANDLE(STEPSelections_SelectForTransfer, XSSelect_SelectForTransfer)


class STEPSelections_SelectForTransfer : public XSSelect_SelectForTransfer
{
 public:

  STEPSelections_SelectForTransfer(const Handle(Transfer_TransientProcess)& TP) : XSSelect_SelectForTransfer(TP) {}
  
  Standard_EXPORT virtual Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(STEPSelections_SelectForTransfer,XSSelect_SelectForTransfer)
};

#endif // _STEPSelections_SelectForTransfer_HeaderFile

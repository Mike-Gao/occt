// Created on: 2020-03-06
// Created by: Andrey SURAVENKOV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#include <Draw_SaveAndRestore.hxx>
#include <Draw_Number.hxx>
#include <DBRep_DrawableShape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <Draw_ProgressIndicator.hxx>

Draw_SaveAndRestoreBase* Draw_SaveAndRestoreBase::Draw_FirstSaveAndRestore = nullptr;

Draw_SaveAndRestoreBase::Draw_SaveAndRestoreBase(Standard_CString thename)
  :myName(thename) 
  ,myNext(Draw_FirstSaveAndRestore)
{
  Draw_FirstSaveAndRestore = this;
}


//=======================================================================
// Draw_SaveAndRestoreNumber
//=======================================================================

Standard_Boolean Draw_SaveAndRestoreNumber::Test(const Handle(Draw_Drawable3D)& theDrawable3D) const
{
  return theDrawable3D->IsInstance(STANDARD_TYPE(Draw_Number));
}

void Draw_SaveAndRestoreNumber::Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& theStream, TopTools_FormatVersion theVersion) const
{
  theVersion; // to suppress a warning
  Handle(Draw_Number) aNum = Handle(Draw_Number)::DownCast(theDrawable3D);
  std::ios::fmtflags aFlags = theStream.flags();
  theStream.setf(std::ios::scientific);
  theStream.precision(15);
  theStream.width(30);
  theStream << aNum->Value() << "\n";
  theStream.setf(aFlags);
}

Handle(Draw_Drawable3D) Draw_SaveAndRestoreNumber::Restore(std::istream& is) const
{
  Standard_Real val;
  is >> val;
  Handle(Draw_Number) N = new Draw_Number(val);
  return N;
}


static Draw_SaveAndRestoreNumber saveAndRestoreNumber;
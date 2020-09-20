// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef Draw_SaveAndRestore_HeaderFile
#define Draw_SaveAndRestore_HeaderFile

#include <Draw.hxx>

enum TopTools_FormatVersion;


class Draw_SaveAndRestoreBase
{
public:
  Standard_EXPORT Draw_SaveAndRestoreBase(Standard_CString thename);

public:
  virtual void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const = 0;
  virtual Handle(Draw_Drawable3D) Restore(std::istream& is) const = 0;
  virtual Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const = 0;

  Standard_CString Name() const
  {
    return myName;
  } 

  const Draw_SaveAndRestoreBase*  Next() const
  {
    return myNext;
  } 
  static  Draw_SaveAndRestoreBase* const GetFirst()   
  {
    return Draw_FirstSaveAndRestore;
  } 
  Standard_Boolean Disp() const { return myDisplay; }

private:
  Draw_SaveAndRestoreBase* myNext;
  Standard_CString         myName;
  Standard_Boolean         myDisplay;

  static Draw_SaveAndRestoreBase* Draw_FirstSaveAndRestore;
};


class Draw_SaveAndRestoreNumber : public Draw_SaveAndRestoreBase
{
public:
  Draw_SaveAndRestoreNumber()
    :Draw_SaveAndRestoreBase("Draw_Number") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};


#endif
// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "TOcafFunction_CylDriver.h"

#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_Real.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================

const Standard_GUID& TOcafFunction_CylDriver::GetID() 
{
  static Standard_GUID anID("22D22E53-D69A-11d4-8F1A-0060B0EE18E8");
  return anID;
}


//=======================================================================
//function : TPartStd_CylDriver
//purpose  : Creation of an instance of the driver. It's possible (and recommended)
//         : to have only one instance of a driver for the whole session.
//=======================================================================

TOcafFunction_CylDriver::TOcafFunction_CylDriver()
{
}

//=======================================================================
//function : Validate
//purpose  : Validation of the object label, its arguments and its results.
//=======================================================================

void TOcafFunction_CylDriver::Validate(Handle(TFunction_Logbook)& log) const
{
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log->SetValid(Label(), Standard_True);
}

//=======================================================================
//function : MustExecute
//purpose  : We call this method to check if the object was modified to
//         : be invoked. If the object label or an argument is modified,
//         : we must recompute the object - to call the method Execute().
//=======================================================================
Standard_Boolean TOcafFunction_CylDriver::MustExecute(const Handle(TFunction_Logbook)& log) const
{
  // If the object's label is modified:
  if (log->IsModified(Label())) return Standard_True;

  // Cylinder (in our simple case) has 5 arguments: 
  // 
  // Let's check them:
  if (log->IsModified(Label().FindChild(1)))
  {
    return Standard_True; // radius.
  }
  if (log->IsModified(Label().FindChild(2)))
  {
    return Standard_True; // height,
  }
  if (log->IsModified(Label().FindChild(3)))
  {
    return Standard_True; // x.
  }
  if (log->IsModified(Label().FindChild(4)))
  {
    return Standard_True; // y,
  }
  if (log->IsModified(Label().FindChild(5)))
  {
    return Standard_True; // z.
  }
 // if there are no any modifications concerned the Cyl,
  // it's not necessary to recompute (to call the method Execute()):
  return Standard_False;
}

//=======================================================================
//function : Execute
//purpose  : 
//         : We compute the object and topologically name it.
//         : If during the execution we found something wrong,
//         : we return the number of the failure. For example:
//         : 1 - an attribute hasn't been found,
//         : 2 - algorithm failed,
//         : if there are no any mistakes occurred we return 0:
//         : 0 - no mistakes were found.
//=======================================================================
Standard_Integer TOcafFunction_CylDriver::Execute(Handle(TFunction_Logbook)& /*log*/) const
{
  // Get the values of dimension and position attributes 
  Handle(TDataStd_Real) TSR;
  Standard_Real x, y, z, r, h;
  if (!Label().FindChild(1).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  r = TSR->Get();

  if (!Label().FindChild(2).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  h = TSR->Get();

  if (!Label().FindChild(3).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  x = TSR->Get();

  if (!Label().FindChild(4).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  y = TSR->Get();

  if (!Label().FindChild(5).FindAttribute(TDataStd_Real::GetID(), TSR))
  {
    return 1;
  }
  z = TSR->Get();

  // Build a Cyl using the dimension and position attributes 
  BRepPrimAPI_MakeCylinder mkCyl(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(0, 0, 1)), r, h);
  TopoDS_Shape ResultShape = mkCyl.Shape();


  // Build a TNaming_NamedShape using built Cyl
  TNaming_Builder B(Label());
  B.Generated(ResultShape);
  // That's all:
    // If there are no any mistakes we return 0:
  return 0;
}

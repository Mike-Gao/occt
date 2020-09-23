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

#include "TOcafFunction_CutDriver.h"

#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>

#include <BRepAlgoAPI_Cut.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <Standard_GUID.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_Reference.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QMessageBox>
#include <QApplication>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================

const Standard_GUID& TOcafFunction_CutDriver::GetID() {
  static Standard_GUID anID("22D22E52-D69A-11d4-8F1A-0060B0EE18E8");
  return anID;
}


//=======================================================================
//function : TPartStd_CutDriver
//purpose  : Creation of an instance of the driver. It's possible (and recommended)
//         : to have only one instance of a driver for the whole session.
//=======================================================================

TOcafFunction_CutDriver::TOcafFunction_CutDriver()
{}

//=======================================================================
//function : Validate
//purpose  : Validation of the object label, its arguments and its results.
//=======================================================================

void TOcafFunction_CutDriver::Validate(Handle(TFunction_Logbook)& log) const
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

Standard_Boolean TOcafFunction_CutDriver::MustExecute(const Handle(TFunction_Logbook)& log) const
{
  // If the object's label is modified:
  if (log->IsModified(Label())) return Standard_True;

  // Cut (in our simple case) has two arguments: The original shape, and the tool shape.
  // They are on the child labels of the cut's label:
  // So, OriginalNShape  - is attached to the first  child label
  //     ToolNShape - is attached to the second child label,
  //     .
  // Let's check them:
  Handle(TDF_Reference) OriginalRef;
  //TDF_Label aLabel = Label().FindChild(1);
/*
  BOOL f = Label().IsNull();
  int a = Label().NbChildren();
*/
  TCollection_AsciiString aEntry;
  TDF_Tool::Entry(Label(), aEntry);
  std::cout << "Entry: " << aEntry.ToCString() << std::endl;
  Label().FindChild(1).FindAttribute(TDF_Reference::GetID(), OriginalRef);
  if (log->IsModified(OriginalRef->Get()))   return Standard_True; // Original shape.

  Handle(TDF_Reference) ToolRef;
  Label().FindChild(2).FindAttribute(TDF_Reference::GetID(), ToolRef);
  if (log->IsModified(ToolRef->Get()))   return Standard_True; // Tool shape.

  // if there are no any modifications concerned the cut,
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

Standard_Integer TOcafFunction_CutDriver::Execute(Handle(TFunction_Logbook)& /*log*/) const
{
  // Let's get the arguments (OriginalNShape, ToolNShape of the object):

  // First, we have to retrieve the TDF_Reference attributes to obtain
  // the root labels of the OriginalNShape and the ToolNShape:
  Handle(TDF_Reference)  OriginalRef, ToolRef;
  if (!Label().FindChild(1).FindAttribute(TDF_Reference::GetID(), OriginalRef)) return 1;
  TDF_Label OriginalLab = OriginalRef->Get();
  if (!Label().FindChild(2).FindAttribute(TDF_Reference::GetID(), ToolRef)) return 1;
  TDF_Label ToolLab = ToolRef->Get();

  // Get the TNaming_NamedShape attributes of these labels
  Handle(TNaming_NamedShape) OriginalNShape, ToolNShape;
  if (!(OriginalLab.FindAttribute(TNaming_NamedShape::GetID(), OriginalNShape)))
    throw Standard_Failure("TOcaf_Commands::CutObjects");
  if (!(ToolLab.FindAttribute(TNaming_NamedShape::GetID(), ToolNShape)))
    throw Standard_Failure("TOcaf_Commands::CutObjects");

  // Now, let's get the TopoDS_Shape of these TNaming_NamedShape:
  TopoDS_Shape OriginalShape = OriginalNShape->Get();
  TopoDS_Shape ToolShape = ToolNShape->Get();

  // STEP 2:
    // Let's call for algorithm computing a cut operation:
  BRepAlgoAPI_Cut mkCut(OriginalShape, ToolShape);
  // Let's check if the Cut has been successfull:
  if (!mkCut.IsDone()) {
    QMessageBox::critical(qApp->activeWindow(),
      QObject::tr("Cut Function Driver"),
      QObject::tr("Cut not done."));
    return 2;
  }
  TopoDS_Shape ResultShape = mkCut.Shape();

  // Build a TNaming_NamedShape using built cut
  TNaming_Builder B(Label());
  B.Modify(OriginalShape, ResultShape);
  // That's all:
    // If there are no any mistakes we return 0:
  return 0;
}

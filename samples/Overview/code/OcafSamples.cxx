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

#include "OcafSamples.h"
#include "TOcaf_Application.h"
#include "TOcafFunction_BoxDriver.h"
#include "TOcafFunction_CylDriver.h"

#include <TPrsStd_AISViewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_DisplayMode.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_DriverTable.hxx>
#include <TDF_ChildIterator.hxx>
#include <PCDM_StoreStatus.hxx>
#include <BinDrivers.hxx>
#include <XmlDrivers.hxx>

OcafSamples::OcafSamples(TCollection_AsciiString theSampleSourcePath,
                         Handle(V3d_Viewer) theViewer,
                         Handle(AIS_InteractiveContext) theContext):
  BaseSample(theSampleSourcePath),
  myViewer(theViewer),
  myContext(theContext)
{

}

void OcafSamples::Process(TCollection_AsciiString theSampleName)
{
  if (IsImportSample(theSampleName))
    myObject3d.Clear();
  myObject2d.Clear();
  myCode.Clear();
  myIsProcessed = Standard_False;
  try 
  {
    ExecuteSample(theSampleName);
  }
  catch (...) 
  {
    TraceError(TCollection_AsciiString("Error in sample: ") + theSampleName);
  }
}

void OcafSamples::ExecuteSample(TCollection_AsciiString theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "CreateOcafDocument")
    CreateOcafDocument();
  else if (theSampleName == "CreateBoxOcafSample")
    CreateBoxOcafSample();
  else if (theSampleName == "CreateCylinderOcafSample")
    CreateCylinderOcafSample();
  else if (theSampleName == "ModifyBoxOcafSample")
    ModifyBoxOcafSample();
  else if (theSampleName == "ModifyCylinderOcafSample")
    ModifyCylinderOcafSample();
  else if (theSampleName == "UndoOcafSample")
    UndoOcafSample();
  else if (theSampleName == "RedoOcafSample")
    RedoOcafSample();
  else if (theSampleName == "DialogOpenOcafSample")
    DialogOpenOcafSample();
  else if (theSampleName == "DialogSaveBinOcafSample")
    DialogSaveBinOcafSample();
  else if (theSampleName == "DialogSaveXmlOcafSample")
    DialogSaveXmlOcafSample();
  else 
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}

Standard_Boolean OcafSamples::IsExportSample(TCollection_AsciiString theSampleName)
{
  if (theSampleName == "DialogSaveBinOcafSample" || theSampleName == "DialogSaveXmlOcafSample")
  {
    return Standard_True;
  }
  else
  {
    return Standard_False;
  }
}

Standard_Boolean OcafSamples::IsImportSample(TCollection_AsciiString theSampleName)
{
  if (theSampleName == "DialogOpenOcafSample")
  {
    return Standard_True;
  }
  else
  {
    return Standard_False;
  }
}

Standard_Boolean OcafSamples::IsBinarySample(TCollection_AsciiString theSampleName)
{
  if (theSampleName == "DialogOpenOcafSample" || theSampleName == "DialogSaveBinOcafSample")
  {
    return Standard_True;
  }
  else
  {
    return Standard_False;
  }
}

Standard_Boolean OcafSamples::IsXmlSample(TCollection_AsciiString theSampleName)
{
  if (theSampleName == "DialogOpenOcafSample" || theSampleName == "DialogSaveXmlOcafSample")
  {
    return Standard_True;
  }
  else
  {
    return Standard_False;
  }
}

void OcafSamples::CreateOcafDocument()
{
  Handle(TOcaf_Application) anOcaf_Application = new TOcaf_Application;
  anOcaf_Application->NewDocument("BinOcaf", myOcafDoc);
  TPrsStd_AISViewer::New(myOcafDoc->Main(), myViewer);

  Handle(AIS_InteractiveContext) anAisContext;
  TPrsStd_AISViewer::Find(myOcafDoc->Main(), anAisContext);
  anAisContext->SetDisplayMode(AIS_Shaded, Standard_True);
  myContext = anAisContext;

  // Set the maximum number of available "undo" actions
  myOcafDoc->SetUndoLimit(10);
}

void OcafSamples::CreateBoxOcafSample()
{
  // Open a new command (for undo)
  myOcafDoc->NewCommand();

  // A data structure for our box:
  // the box itself is attached to the BoxLabel label (as his name and his function attribute) 
  // its arguments (dimensions: width, length and height; and position: x, y, z) 
  // are attached to the child labels of the box:
  // 0:1 Box Label ---> Name --->  Named shape ---> Function
  //     0:1:1 -- Width Label
  //     0:1:2 -- Length Label
  //     0:1:3 -- Height Label
  //     0:1:4 -- X Label
  //     0:1:5 -- Y Label
  //     0:1:6 -- Z Label

  // Create a new label in the data structure for the box
  TDF_Label aLabel = TDF_TagSource::NewChild(myOcafDoc->Main());

  Standard_Real aBoxWidth(30.0), aBoxLenght(20.0), aBoxHight(10.0);
  Standard_Real aBoxX(0.0), aBoxY(0.0), aBoxZ(0.0);
  Standard_CString aBoxName("OcafBox");
  // Create the data structure : Set the dimensions, position and name attributes
  TDataStd_Real::Set(aLabel.FindChild(1), aBoxWidth);
  TDataStd_Real::Set(aLabel.FindChild(2), aBoxLenght);
  TDataStd_Real::Set(aLabel.FindChild(3), aBoxHight);
  TDataStd_Real::Set(aLabel.FindChild(4), aBoxX);
  TDataStd_Real::Set(aLabel.FindChild(5), aBoxY);
  TDataStd_Real::Set(aLabel.FindChild(6), aBoxZ);
  TDataStd_Name::Set(aLabel, aBoxName); // Name


  // Instanciate a TFunction_Function attribute connected to the current box driver
  // and attach it to the data structure as an attribute of the Box Label
  Handle(TFunction_Function) myFunction = TFunction_Function::Set(aLabel, TOcafFunction_BoxDriver::GetID());

  // Initialize and execute the box driver (look at the "Execute()" code)
  Handle(TFunction_Logbook) aLogBook = TFunction_Logbook::Set(aLabel);

  Handle(TFunction_Driver) myBoxDriver;
  // Find the TOcafFunction_BoxDriver in the TFunction_DriverTable using its GUID
  if (!TFunction_DriverTable::Get()->FindDriver(TOcafFunction_BoxDriver::GetID(), myBoxDriver))
    myResult << "Ocaf Box driver not found" << std::endl;

  myBoxDriver->Init(aLabel);
  if (myBoxDriver->Execute(aLogBook))
    myResult << "Create Box function execute failed" << std::endl;

  // Get the TPrsStd_AISPresentation of the new box TNaming_NamedShape
  Handle(TPrsStd_AISPresentation) anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
  // Display it
  anAisPresentation->Display(1);
  // Attach an integer attribute to aLabel to memorize it's displayed
  TDataStd_Integer::Set(aLabel, 1);
  myContext->UpdateCurrentViewer();

  // Close the command (for undo)
  myOcafDoc->CommitCommand();

  myResult << "Created a box with name: " << aBoxName << std::endl;
  myResult << "base coord X: " << aBoxX << " Y: " << aBoxY << " Z: " << aBoxZ << std::endl;
  myResult << "width: " << aBoxWidth << " lenght: " << aBoxLenght << " hight: " << aBoxHight << std::endl;
}

void OcafSamples::CreateCylinderOcafSample()
{
  // Open a new command (for undo)
  myOcafDoc->NewCommand();

  // A data structure for our cylinder:
  // the cylinder itself is attached to the CylinderLabel label (as his name and his function attribute) 
  // its arguments (dimensions: radius and height; and position: x, y, z) 
  // are attached to the child labels of the cylinder:
  // 0:1 Cylinder Label ---> Name --->  Named shape ---> Function
  //     0:1:1 -- Radius Label
  //     0:1:2 -- Height Label
  //     0:1:3 -- X Label
  //     0:1:4 -- Y Label
  //     0:1:5 -- Z Label

  // Create a new label in the data structure for the cylinder
  TDF_Label aLabel = TDF_TagSource::NewChild(myOcafDoc->Main());

  Standard_Real aCylRadius(10.0), aCylHight(20.0);
  Standard_Real aCylX(60.0), aCylY(40.0), aCylZ(0.0);
  Standard_CString aCylName("OcafCylinder");
  // Create the data structure : Set the dimensions, position and name attributes
  TDataStd_Real::Set(aLabel.FindChild(1), aCylRadius);
  TDataStd_Real::Set(aLabel.FindChild(2), aCylHight);
  TDataStd_Real::Set(aLabel.FindChild(3), aCylX);
  TDataStd_Real::Set(aLabel.FindChild(4), aCylY);
  TDataStd_Real::Set(aLabel.FindChild(5), aCylZ);
  TDataStd_Name::Set(aLabel, aCylName);

  // Instanciate a TFunction_Function attribute connected to the current cylinder driver
  // and attach it to the data structure as an attribute of the Cylinder Label
  Handle(TFunction_Function) myFunction = TFunction_Function::Set(aLabel, TOcafFunction_CylDriver::GetID());

  // Initialize and execute the cylinder driver (look at the "Execute()" code)
  Handle(TFunction_Logbook) aLogBook = TFunction_Logbook::Set(aLabel);

  Handle(TFunction_Driver) myCylDriver;
  // Find the TOcafFunction_CylDriver in the TFunction_DriverTable using its GUID
  if (!TFunction_DriverTable::Get()->FindDriver(TOcafFunction_CylDriver::GetID(), myCylDriver))
  {
    myResult << "Ocaf Cylinder driver not found";
  }
  myCylDriver->Init(aLabel);
  if (myCylDriver->Execute(aLogBook))
  {
    myResult << "Create Cylinder function execute failed";
  }
  // Get the TPrsStd_AISPresentation of the new box TNaming_NamedShape
  Handle(TPrsStd_AISPresentation) anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
  // Display it
  anAisPresentation->Display(1);
  // Attach an integer attribute to aLabel to memorize it's displayed
  TDataStd_Integer::Set(aLabel, 1);
  myContext->UpdateCurrentViewer();

  // Close the command (for undo)
  myOcafDoc->CommitCommand();

  myResult << "Created a cylinder with name: " << aCylName << std::endl;
  myResult << "radius: " << aCylRadius << " hight: " << aCylHight << std::endl;
  myResult << "base coord X: " << aCylX << " Y: " << aCylY << " Z: " << aCylZ << std::endl;
}

void OcafSamples::ModifyBoxOcafSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  Standard_Integer aBoxCount(0);
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList) 
  {
    // Get the main label of the selected object
    Handle(TPrsStd_AISPresentation) anAisPresentation = Handle(TPrsStd_AISPresentation)::DownCast(anAisObject->GetOwner());
    TDF_Label aLabel = anAisPresentation->Label();

    // Get the TFunction_Function attribute of the selected object
    Handle(TFunction_Function) aFunction;
    if (!aLabel.FindAttribute(TFunction_Function::GetID(), aFunction)) 
    {
      myResult << "Object cannot be modify.";
      return;
    }
    // Get the Standard_GUID of the TFunction_FunctionDriver of the selected object TFunction_Function attribute
    Standard_GUID aDriverID = aFunction->GetDriverGUID();

    // Case of a box created with the box function driver
    if (aDriverID == TOcafFunction_BoxDriver::GetID()) 
    {
      aBoxCount++;
      Standard_Real aBoxX, aBoxY, aBoxZ, aBoxWidth, aBoxLenght, aBoxHight;

      // Get the attributes values of the current box
      Handle(TDataStd_Real) aCurrentReal;
      aLabel.FindChild(1).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aBoxWidth = aCurrentReal->Get();
      aLabel.FindChild(2).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aBoxLenght = aCurrentReal->Get();
      aLabel.FindChild(3).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aBoxHight = aCurrentReal->Get();
      aLabel.FindChild(4).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aBoxX = aCurrentReal->Get();
      aLabel.FindChild(5).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aBoxY = aCurrentReal->Get();
      aLabel.FindChild(6).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aBoxZ = aCurrentReal->Get();
      Handle(TDataStd_Name) aBoxName;
      aLabel.FindAttribute(TDataStd_Name::GetID(), aBoxName);

      myResult << "Current parameters of box with name: " << aBoxName->Get() << std::endl;
      myResult << "width: " << aBoxWidth << " lenght: " << aBoxLenght << " hight: " << aBoxHight << std::endl;
      myResult << "base coord X: " << aBoxX << " Y: " << aBoxY << " Z: " << aBoxZ << std::endl;

      // Open a new command (for undo)
      myOcafDoc->NewCommand();
      // Modify the box  - 1.5 times increase
      aBoxWidth *= 1.5; aBoxLenght *= 1.5; aBoxHight *= 1.5;

      TDataStd_Real::Set(aLabel.FindChild(1), aBoxWidth);
      TDataStd_Real::Set(aLabel.FindChild(2), aBoxLenght);
      TDataStd_Real::Set(aLabel.FindChild(3), aBoxHight);
      TDataStd_Real::Set(aLabel.FindChild(4), aBoxX);
      TDataStd_Real::Set(aLabel.FindChild(5), aBoxY);
      TDataStd_Real::Set(aLabel.FindChild(6), aBoxZ);

      // Get the TFunction_FunctionDriver GUID used with the TFunction_Function
      aDriverID = aFunction->GetDriverGUID();
      Handle(TFunction_Logbook) aLogBook = TFunction_Logbook::Set(aLabel);
      Handle(TFunction_Driver) aBoxDriver;
      // Find the TOcafFunction_BoxDriver in the TFunction_DriverTable using its GUID 
      TFunction_DriverTable::Get()->FindDriver(aDriverID, aBoxDriver);
      // Execute the cut if it must be (if an attribute changes)
      aBoxDriver->Init(aLabel);

      // Set the box touched, it will be usefull to recompute an object which used this box as attribute
      aLogBook->SetTouched(aLabel);
      if (aBoxDriver->Execute(aLogBook))
        myResult << "Recompute failed" << std::endl;

      // Get the presentation of the box, display it and set it selected
      anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
      TDataStd_Integer::Set(aLabel, 1);
      anAisPresentation->Display(1);
      myContext->UpdateCurrentViewer();
      // Close the command (for undo)
      myOcafDoc->CommitCommand();

      myResult << std::endl;
      myResult << "New box parameters: " << std::endl;
      myResult << "base coord X: " << aBoxX << " Y: " << aBoxY << " Z: " << aBoxZ << std::endl;
      myResult << "width: " << aBoxWidth << " lenght: " << aBoxLenght << " hight: " << aBoxHight << std::endl;
    }
  }
  if (aBoxCount)
  {
    myResult << "Number of modified boxes: " << aBoxCount << std::endl;
  }
  else
  {
    myResult << "No boxes to modyfy" << std::endl;
  }
}

void OcafSamples::ModifyCylinderOcafSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  Standard_Integer aCylCount(0);
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList) 
  {
    // Get the main label of the selected object
    Handle(TPrsStd_AISPresentation) anAisPresentation = Handle(TPrsStd_AISPresentation)::DownCast(anAisObject->GetOwner());
    TDF_Label aLabel = anAisPresentation->Label();

    // Get the TFunction_Function attribute of the selected object
    Handle(TFunction_Function) aFunction;
    if (!aLabel.FindAttribute(TFunction_Function::GetID(), aFunction)) 
    {
      myResult << "Object cannot be modify.";
      return;
    }
    // Get the Standard_GUID of the TFunction_FunctionDriver of the selected object TFunction_Function attribute
    Standard_GUID aDriverID = aFunction->GetDriverGUID();

    // Case of a box created with the box function driver
    if (aDriverID == TOcafFunction_CylDriver::GetID()) 
    {
      aCylCount++;
      Standard_Real aCylRadius, aCylHight, aCylX, aCylY, aCylZ;

      // Get the attributes values of the current box
      Handle(TDataStd_Real) aCurrentReal;
      aLabel.FindChild(1).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aCylRadius = aCurrentReal->Get();
      aLabel.FindChild(2).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aCylHight = aCurrentReal->Get();
      aLabel.FindChild(3).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aCylX = aCurrentReal->Get();
      aLabel.FindChild(4).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aCylY = aCurrentReal->Get();
      aLabel.FindChild(5).FindAttribute(TDataStd_Real::GetID(), aCurrentReal);
      aCylZ = aCurrentReal->Get();

      Handle(TDataStd_Name) aCylName;
      aLabel.FindAttribute(TDataStd_Name::GetID(), aCylName);

      myResult << "Current parameters of box with name: " << aCylName->Get() << std::endl;
      myResult << "radius: " << aCylRadius << " hight: " << aCylHight << std::endl;
      myResult << "base coord X: " << aCylX << " Y: " << aCylY << " Z: " << aCylZ << std::endl;

      // Open a new command (for undo)
      myOcafDoc->NewCommand();
      // Modify the cylinder - 2x  increase
      aCylRadius *= 2.0; aCylHight *= 2.0;
      // and move base point in XY plane
      aCylX *= 2.0; aCylY *= 2.0;

      TDataStd_Real::Set(aLabel.FindChild(1), aCylRadius);
      TDataStd_Real::Set(aLabel.FindChild(2), aCylHight);
      TDataStd_Real::Set(aLabel.FindChild(3), aCylX);
      TDataStd_Real::Set(aLabel.FindChild(4), aCylY);
      TDataStd_Real::Set(aLabel.FindChild(5), aCylZ);

      // Get the TFunction_FunctionDriver GUID used with the TFunction_Function
      aDriverID = aFunction->GetDriverGUID();
      Handle(TFunction_Logbook) aLogBook = TFunction_Logbook::Set(aLabel);
      Handle(TFunction_Driver) aCylDriver;
      // Find the TOcafFunction_CylDriver in the TFunction_DriverTable using its GUID 
      TFunction_DriverTable::Get()->FindDriver(aDriverID, aCylDriver);
      // Execute the cut if it must be (if an attribute changes)
      aCylDriver->Init(aLabel);

      // Set the cylinder touched, it will be usefull to recompute an object which used this box as attribute
      aLogBook->SetTouched(aLabel);
      if (aCylDriver->Execute(aLogBook))
      {
        myResult << "Recompute failed" << std::endl;
      }
      // Get the presentation of the box, display it and set it selected
      anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
      TDataStd_Integer::Set(aLabel, 1);
      anAisPresentation->Display(1);
      myContext->UpdateCurrentViewer();
      // Close the command (for undo)
      myOcafDoc->CommitCommand();

      myResult << std::endl;
      myResult << "New cylinder parameters: " << std::endl;
      myResult << "radius: " << aCylRadius << " hight: " << aCylHight << std::endl;
      myResult << "base coord X: " << aCylX << " Y: " << aCylY << " Z: " << aCylZ << std::endl;
    }
  }
  if (aCylCount)
  {
    myResult << "Number of modified boxes: " << aCylCount << std::endl;
  }
  else
  {
    myResult << "No boxes to modyfy" << std::endl;
  }
}

void OcafSamples::UndoOcafSample()
{
  if (myOcafDoc->Undo()) 
  {
    myOcafDoc->CommitCommand();
    myContext->UpdateCurrentViewer();
    myResult << "Undo was done successfully" << std::endl;
  }
  else
  {
    myResult << "Nothing to undo" << std::endl;
  }
}

void OcafSamples::RedoOcafSample()
{
  if (myOcafDoc->Redo()) 
  {
    myOcafDoc->CommitCommand();
    myContext->UpdateCurrentViewer();
    myResult << "Redo was done successfully" << std::endl;
  }
  else
  {
    myResult << "Nothing to redo" << std::endl;
  }
}

void OcafSamples::DialogOpenOcafSample()
{
  Handle(TOcaf_Application) anOcaf_Application = new TOcaf_Application;
  // load persistence
  BinDrivers::DefineFormat(anOcaf_Application);
  XmlDrivers::DefineFormat(anOcaf_Application);
  // Look for already opened
  if (anOcaf_Application->IsInSession(myFileName)) 
  {
    myResult << "Document: " << myFileName << " is already in session" << std::endl;
    return;
  }
  // Open the document in the current application
  PCDM_ReaderStatus aReaderStatus = anOcaf_Application->Open(myFileName, myOcafDoc);
  if (aReaderStatus == PCDM_ReaderStatus::PCDM_RS_OK) 
  {
    // Connect the document CAF (myDoc) with the AISContext (myAISContext)
    TPrsStd_AISViewer::New(myOcafDoc->Main(), myViewer);
    myOcafDoc->SetUndoLimit(10);

    myContext->RemoveAll(Standard_False);
    Handle(AIS_InteractiveContext) aContext;
    TPrsStd_AISViewer::Find(myOcafDoc->Main(), aContext);
    aContext->SetDisplayMode(AIS_Shaded, Standard_True);
    myContext = aContext;

    // Display the presentations (which was not stored in the document)
    DisplayPresentation();
    myResult << "Open a document" << std::endl;
  }
  else
  {
    myResult << "Error! The file wasn't opened. PCDM_ReaderStatus: " << aReaderStatus << std::endl;
  }
}

void OcafSamples::DialogSaveBinOcafSample()
{
  Handle(TOcaf_Application) anOcaf_Application = new TOcaf_Application;
  BinDrivers::DefineFormat(anOcaf_Application);
  myOcafDoc->ChangeStorageFormat("BinOcaf");
  // Saves the document in the current application
  PCDM_StoreStatus aStoreStatus = anOcaf_Application->SaveAs(myOcafDoc, myFileName);
  if (aStoreStatus == PCDM_StoreStatus::PCDM_SS_OK)
  {
    myResult << "The file was saved successfully" << std::endl;
  }
  else
  {
    myResult << "Error! The file wasn't saved. PCDM_StoreStatus: " << aStoreStatus << std::endl;
  }
}

void OcafSamples::DialogSaveXmlOcafSample()
{
  Handle(TOcaf_Application) anOcaf_Application = new TOcaf_Application;
  XmlDrivers::DefineFormat(anOcaf_Application);
  myOcafDoc->ChangeStorageFormat("XmlOcaf");
  // Saves the document in the current application
  PCDM_StoreStatus aStoreStatus = anOcaf_Application->SaveAs(myOcafDoc, myFileName);
  if (aStoreStatus == PCDM_StoreStatus::PCDM_SS_OK)
  {
    myResult << "The file was saved successfully" << std::endl;
  }
  else
  {
    myResult << "Error! The file wasn't saved. PCDM_StoreStatus: " << aStoreStatus << std::endl;
  }
}

void OcafSamples::DisplayPresentation()
{
  TDF_Label aRootlabel = myOcafDoc->Main();

  for (TDF_ChildIterator it(aRootlabel); it.More(); it.Next()) 
  {
    TDF_Label aLabel = it.Value();
    Handle(TNaming_NamedShape) aNamedShape;
    if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
    {
      continue;
    }
    Handle(TDataStd_Integer) aDataInteger;

    // To know if the object was displayed
    if (aLabel.FindAttribute(TDataStd_Integer::GetID(), aDataInteger))
    {
      if (!aDataInteger->Get())
      {
        continue;
      }
    }
    Handle(TPrsStd_AISPresentation) anAisPresentation;
    if (!aLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), anAisPresentation))
    {
      anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
    }
    anAisPresentation->SetColor(Quantity_NOC_ORANGE);
    anAisPresentation->Display(1);
  }
  myContext->UpdateCurrentViewer();
}

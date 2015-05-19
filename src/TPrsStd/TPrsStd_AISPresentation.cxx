// Created on: 1998-09-30
// Created by: Sergey RUIN
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DefaultDeltaOnRemoval.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TDataXtd_Presentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TPrsStd_Driver.hxx>
#include <TPrsStd_DriverTable.hxx>


//=======================================================================
//function : TPrsStd_AISPresentation
//purpose  : Constructor
//=======================================================================
TPrsStd_AISPresentation::TPrsStd_AISPresentation ()
: myDriverGUID("00000000-0000-0000-0000-000000000000")
{
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TPrsStd_AISPresentation) TPrsStd_AISPresentation::Set
             ( const TDF_Label& theLabel, const Standard_GUID& theDriver )
{
  Handle(TPrsStd_AISPresentation) aPresentation;

  if ( !theLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), aPresentation) )
  {
    aPresentation = new TPrsStd_AISPresentation();
    theLabel.AddAttribute(aPresentation);
  }

  aPresentation->SetDriverGUID(theDriver);
  return aPresentation;
}


//=======================================================================
//function : Unset
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::Unset (const TDF_Label& theLabel)
{
  Handle(TPrsStd_AISPresentation) aPresentation;

  if ( theLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), aPresentation) )
    theLabel.ForgetAttribute(aPresentation);
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TPrsStd_AISPresentation) TPrsStd_AISPresentation::Set(const Handle(TDF_Attribute)& theMaster)
{
  Handle(TPrsStd_AISPresentation) aPresentation;
  if ( !theMaster->Label().FindAttribute(TPrsStd_AISPresentation::GetID(), aPresentation) )
  { 
    aPresentation = new TPrsStd_AISPresentation ();
    theMaster->Label().AddAttribute(aPresentation);
  }

  aPresentation->SetDriverGUID( theMaster->ID() );
  return aPresentation;
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TPrsStd_AISPresentation::GetID()
{
  static Standard_GUID TPrsStd_AISPresentationID("3680ac6c-47ae-4366-bb94-26abb6e07341");
  return TPrsStd_AISPresentationID;
}


//=======================================================================
//function : Display
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::Display(const Standard_Boolean theIsUpdate)
{
  if ( theIsUpdate || myAIS.IsNull() )
    AISUpdate();

  AISDisplay();
}


//=======================================================================
//function : Erase
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::Erase(const Standard_Boolean theIsRemove)
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
  {
    if ( aPresentation.IsNull() == Standard_False && aPresentation->IsDisplayed() )
      AISErase(theIsRemove);
  }
}


//=======================================================================
//function : Update
//purpose  : 
//=======================================================================
void  TPrsStd_AISPresentation::Update()
{
  AISUpdate();
}


//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::IsDisplayed() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->IsDisplayed();

  return Standard_False;
}


//=======================================================================
//function : SetDisplayed
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::SetDisplayed(const Standard_Boolean theIsDisplayed)
{
  Handle(TDataXtd_Presentation) aPresentation = getData();
  return aPresentation->SetDisplayed(theIsDisplayed);
}


//=======================================================================
//function :SetDriverGUID
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::SetDriverGUID(const Standard_GUID& theGUID)
{
  if ( myDriverGUID == theGUID )
    return;

  Backup();
  myDriverGUID = theGUID;
}


//=======================================================================
//function :GetDriverGUID
//purpose  : 
//=======================================================================
Standard_GUID TPrsStd_AISPresentation::GetDriverGUID() const
{
  return myDriverGUID;
}


//=======================================================================
//function :Material
//purpose  : 
//=======================================================================
Graphic3d_NameOfMaterial TPrsStd_AISPresentation::Material() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return static_cast<Graphic3d_NameOfMaterial>( aPresentation->Material() );

  return Graphic3d_NOM_BRASS;
}


//=======================================================================
//function :HasMaterial
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::HasOwnMaterial() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->IsHasOwnMaterial();

  return Standard_False;
}


//=======================================================================
//function : UnsetMaterial
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::UnsetMaterial()
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( !FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return;

  if ( !aPresentation->IsHasOwnMaterial() )
    if( !myAIS.IsNull() && !myAIS->HasMaterial() )
      return;

  aPresentation->UnsetMaterial();

  if ( myAIS.IsNull() )
    AISUpdate();

  if ( !myAIS.IsNull() && myAIS->HasMaterial() )
  {
    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if ( !aContext.IsNull() )
      aContext->UnsetMaterial(myAIS, Standard_False);
    else
      myAIS->UnsetMaterial();
  }
}


//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::SetMaterial(const Graphic3d_NameOfMaterial theName)
{
  const Standard_Integer aName = static_cast<Standard_Integer>(theName);

  Handle(TDataXtd_Presentation) aPresentation = getData();

  if ( aPresentation->IsHasOwnMode() && aPresentation->Material() == aName )
    if ( !myAIS.IsNull() && myAIS->HasMaterial() && myAIS->Material() == theName )
      return;

  aPresentation->SetMaterial(aName);

  if ( myAIS.IsNull() )
    AISUpdate();

  if ( !myAIS.IsNull() )
  {
    if ( myAIS->HasMaterial() && myAIS->Material() == theName )
      return;   // AIS has already had that material

    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if ( !aContext.IsNull() )
      aContext->SetMaterial(myAIS, theName,  Standard_False);
    else
      myAIS->SetMaterial(theName);
  }
}


//=======================================================================
//function :Transparency
//purpose  : 
//=======================================================================
Standard_Real TPrsStd_AISPresentation::Transparency() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->Transparency();

  return 0.0;
}


//=======================================================================
//function :SetTransparency
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::SetTransparency(const Standard_Real theValue)
{
  Handle(TDataXtd_Presentation) aPresentation = getData();

  if( aPresentation->IsHasOwnTransparency() && aPresentation->Transparency() == theValue )
    if ( !myAIS.IsNull() && myAIS->Transparency() == theValue )
      return;

  aPresentation->SetTransparency(theValue);

  if ( myAIS.IsNull() )
    AISUpdate();

  if ( !myAIS.IsNull() )
  {
    if ( myAIS->Transparency() == theValue )
      return;   // AIS has already had that transparency 

    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if ( !aContext.IsNull() )
      aContext->SetTransparency(myAIS, theValue, Standard_False);
    else
      myAIS->SetTransparency(theValue);
  }
}


//=======================================================================
//function :UnsetTransparency
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::UnsetTransparency()
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
  {
    if ( !aPresentation->IsHasOwnTransparency() )
      return;

    aPresentation->UnsetTransparency();
  }

  if ( myAIS.IsNull() )
    AISUpdate();

  if ( !myAIS.IsNull() )
  {
    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if( !aContext.IsNull() )
      aContext->UnsetTransparency(myAIS, Standard_False);
    else
      myAIS->UnsetTransparency();
  }
}


//=======================================================================
//function : HasTransparency
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::HasOwnTransparency() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->IsHasOwnTransparency();

  return Standard_False;
}


//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
Quantity_NameOfColor TPrsStd_AISPresentation::Color() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return static_cast<Quantity_NameOfColor>(aPresentation->Color());

  return Quantity_NOC_WHITE;
}

//=======================================================================
//function : HasOwnColor
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::HasOwnColor() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->IsHasOwnColor();

  return Standard_False;
}


//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::UnsetColor()
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
  {
    if ( aPresentation->IsHasOwnColor() )
      if ( !myAIS.IsNull() && !myAIS->HasColor() )
        return;

    aPresentation->UnsetColor();
  }

  if ( myAIS.IsNull() )
    AISUpdate();

  if ( !myAIS.IsNull() && myAIS->HasColor() )
  {
    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if ( !aContext.IsNull() )
      aContext->UnsetColor(myAIS, Standard_False);
    else
      myAIS->UnsetColor();
  }
}


//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::SetColor(const Quantity_NameOfColor theColor)
{
  Handle(TDataXtd_Presentation) aPresentation = getData();

  const Standard_Integer aColor = static_cast<Standard_Integer>(theColor);
  if ( aPresentation->IsHasOwnColor() && aPresentation->Color() == aColor )
    if ( !myAIS.IsNull() && myAIS->HasColor() && myAIS->Color() == theColor )
      return;

  aPresentation->SetColor(aColor);

  if ( myAIS.IsNull() )
    AISUpdate();

  if ( !myAIS.IsNull() )
  {
    if ( myAIS->HasColor() && myAIS->Color() == theColor )
      return;   // AIS has already had that color

    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if( !aContext.IsNull() )
      aContext->SetColor(myAIS, theColor, Standard_False);
    else 
      myAIS->SetColor(theColor);
  }
}


//=======================================================================
//function :Width
//purpose  : 
//=======================================================================
Standard_Real TPrsStd_AISPresentation::Width() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->Width();

  return 0.0;
}


//=======================================================================
//function : HasWidth
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::HasOwnWidth() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->IsHasOwnWidth();

  return Standard_False;
}


//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::SetWidth(const Standard_Real theWidth)
{
  Handle(TDataXtd_Presentation) aPresentation = getData();

  if ( aPresentation->IsHasOwnWidth() && aPresentation->Width() == theWidth )
    if ( !myAIS.IsNull() && myAIS->HasWidth() && myAIS->Width() == theWidth )
      return;

  aPresentation->SetWidth(theWidth);

  if( !myAIS.IsNull() )
  {
    if ( myAIS->HasWidth() && myAIS->Width() == theWidth )
      return;   // AIS has already had that width

    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if( !aContext.IsNull() )
      aContext->SetWidth(myAIS, theWidth, Standard_False);
    else 
      myAIS->SetWidth(theWidth);
  }
}


//=======================================================================
//function : UnsetWidth
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::UnsetWidth()
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( !FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
  {
    // create new
    return;
  }

  if ( !aPresentation->IsHasOwnWidth() )
    if ( !myAIS.IsNull() && !myAIS->HasWidth() )
      return;

  aPresentation->UnsetWidth();

  if( myAIS.IsNull() )
    AISUpdate();

  if ( !myAIS.IsNull() && myAIS->HasWidth() )
  {
    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if ( !aContext.IsNull() )
      aContext->UnsetWidth(myAIS, Standard_False);
    else
      myAIS->UnsetWidth();
  }
}


//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================
Standard_Integer TPrsStd_AISPresentation::Mode() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->Mode();

  return 0;
}


//=======================================================================
//function : HasOwnMode
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::HasOwnMode() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->IsHasOwnMode();

  return Standard_False;
}


//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::SetMode(const Standard_Integer theMode)
{
  Handle(TDataXtd_Presentation) aPresentation = getData();

  if ( aPresentation->IsHasOwnMode() && aPresentation->Mode() == theMode )
    if ( !myAIS.IsNull() && myAIS->DisplayMode() == theMode )
      return;

  aPresentation->SetMode(theMode);

  if ( myAIS.IsNull() )
    AISUpdate();

  if ( !myAIS.IsNull() )
  {
    if (  myAIS->DisplayMode() == theMode )
      return;   // AIS has already had that mode

    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if( !aContext.IsNull() )
      aContext->SetDisplayMode(myAIS, theMode, Standard_False);
    else 
      myAIS->SetDisplayMode(theMode);
  }
}


//=======================================================================
//function : UnsetMode
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::UnsetMode()
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( !FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return;

  if ( !aPresentation->IsHasOwnMode() )
    if ( !myAIS.IsNull() && !myAIS->HasDisplayMode() )
      return;

  aPresentation->UnsetMode();

  if ( myAIS.IsNull() )
    AISUpdate();

  if( !myAIS.IsNull() &&  myAIS->HasDisplayMode() )
  {
    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if ( !aContext.IsNull() )
      aContext->UnsetDisplayMode(myAIS, Standard_False);
    else
      myAIS->UnsetDisplayMode();
  }
}


//=======================================================================
//function : SelectionMode
//purpose  : 
//=======================================================================
Standard_Integer TPrsStd_AISPresentation::SelectionMode() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->SelectionMode();

  return 0;
}


//=======================================================================
//function : HasOwnSelectionMode
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::HasOwnSelectionMode() const
{
  Handle(TDataXtd_Presentation) aPresentation;
  if ( FindAttribute(TDataXtd_Presentation::GetID(), aPresentation) )
    return aPresentation->IsHasOwnSelectionMode();

  return Standard_False;
}


//=======================================================================
//function : SetSelectionMode
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::SetSelectionMode(const Standard_Integer theSelectionMode)
{
  // OCC2932 correction
  if(hasOwnSelectionMode && mySelectionMode == theSelectionMode && !myAIS.IsNull())
      return;
  }

  Backup();
  mySelectionMode = theSelectionMode;
  hasOwnSelectionMode = Standard_True;
  if( myAIS.IsNull() ) AISUpdate();
}


//=======================================================================
//function : UnsetSelectionMode
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::UnsetSelectionMode()
{
  if(!hasOwnSelectionMode && !myAIS.IsNull())
        return;

  Backup();
  hasOwnSelectionMode = Standard_False;
  if( myAIS.IsNull() ) AISUpdate();
  mySelectionMode = myAIS->GlobalSelectionMode();
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TPrsStd_AISPresentation::ID() const
{
  return GetID();
}


//=======================================================================
//function : BackupCopy
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TPrsStd_AISPresentation::BackupCopy() const 
{
  return new TPrsStd_AISPresentation;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TPrsStd_AISPresentation::NewEmpty() const
{   
  return new TPrsStd_AISPresentation();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::Restore(const Handle(TDF_Attribute)& theWith)
{
  Handle(TPrsStd_AISPresentation) aWith =
    Handle(TPrsStd_AISPresentation)::DownCast(theWith);

  myAIS.Nullify();
  myDriverGUID  = aWith->GetDriverGUID();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::Paste (const Handle(TDF_Attribute)& theInto,
                                     const Handle(TDF_RelocationTable)& ) const
{
  Handle(TPrsStd_AISPresentation) anInto =
    Handle(TPrsStd_AISPresentation)::DownCast(theInto);

  anInto->Backup();
  anInto->myAIS.Nullify();
}


//=======================================================================
//function : AfterAddition
//purpose  : erase if displayed
//=======================================================================
void TPrsStd_AISPresentation::AfterAddition() 
{
  AfterResume();
}


//=======================================================================
//function : BeforeRemoval
//purpose  : erase if displayed
//=======================================================================
void TPrsStd_AISPresentation::BeforeRemoval() 
{
  BeforeForget();
}


//=======================================================================
//function : BeforeForget
//purpose  : erase if displayed
//=======================================================================
void TPrsStd_AISPresentation::BeforeForget() 
{ 
  if ( !myAIS.IsNull() )
  { // Remove AISObject from context.
    AISErase(Standard_True);
    myAIS.Nullify();
  }
}


//=======================================================================
//function : AfterResume
//purpose  : display if displayed
//=======================================================================
void TPrsStd_AISPresentation::AfterResume()
{
  AISUpdate();

  if ( IsDisplayed() )
    AISDisplay();
  else
    AISErase();
}


//=======================================================================
//function : BeforeUndo
//purpose  : le NamedShape associe doit etre present
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::BeforeUndo (const Handle(TDF_AttributeDelta)& AD,
                                                      const Standard_Boolean ) 
{
  Handle(TPrsStd_AISPresentation) P;
  AD->Label().FindAttribute(TPrsStd_AISPresentation::GetID(), P);

  if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
  {
    if ( !P.IsNull() )
      P->BeforeForget();
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnRemoval))) {
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnModification)))
  {
    if ( !P.IsNull() )
      P->BeforeForget();
  }

  return Standard_True;
}


//=======================================================================
//function : AfterUndo
//purpose  : le NamedShape associe doit etre present
//=======================================================================
Standard_Boolean TPrsStd_AISPresentation::AfterUndo (const Handle(TDF_AttributeDelta)& AD,
                                                     const Standard_Boolean ) 
{ 
  Handle(TPrsStd_AISPresentation) P;
  AD->Label().FindAttribute(TPrsStd_AISPresentation::GetID(), P);

  if (AD->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
  {}
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnRemoval)))
  {
    if ( !P.IsNull() )
      P->AfterAddition();
  }
  else if (AD->IsKind(STANDARD_TYPE(TDF_DefaultDeltaOnModification)))
  {
    if ( !P.IsNull() )
      P->AfterResume();
  }

  return Standard_True;
}


//=======================================================================
//function : AISUpdate
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::AISUpdate()
{
  Backup();

  Handle(AIS_InteractiveContext) aContext;
  if ( !Label().IsNull() )
  {
    aContext = getAISContext();

    Handle(TPrsStd_Driver) aDriver;
    if ( TPrsStd_DriverTable::Get()->FindDriver(GetDriverGUID(), aDriver) )
    {
      // Build a new  AIS.
      if ( myAIS.IsNull() )
      {
        Handle(AIS_InteractiveObject) aNewObj;
        if ( aDriver->Update(Label(), aNewObj) )
        {
          myAIS = aNewObj;
          aNewObj->SetOwner(this);
        }
      }
      else
      {
        Handle(AIS_InteractiveObject) anObj = myAIS;
        if ( aDriver->Update(Label(), anObj) )
          if ( !(anObj ==  myAIS) )
          {
            if ( !aContext.IsNull() )
              aContext->Remove(myAIS);

            // Driver has built new AIS.
            myAIS = anObj;
            anObj->SetOwner(this);
          }
      }
    }
  }
  else return;

  // Apply the visualization settings.
  if ( !myAIS.IsNull() )
  {
    if ( HasOwnColor() )
    {
      Quantity_NameOfColor aColor = Color();
      if ( !(myAIS->HasColor()) || (myAIS->HasColor() && myAIS->Color() != aColor) )
        if ( !aContext.IsNull() )
          aContext->SetColor(myAIS, aColor, Standard_False);
        else
          myAIS->SetColor(aColor);
    }

    if ( HasOwnMaterial() )
    {
      Graphic3d_NameOfMaterial aMaterial = Material();
      if ( !(myAIS->HasMaterial()) || (myAIS->HasMaterial() && myAIS->Material() != aMaterial) )
        if ( !aContext.IsNull() )
          aContext->SetMaterial(myAIS, aMaterial, Standard_False );
        else
          myAIS->SetMaterial(aMaterial);
    }

    if ( HasOwnTransparency() )
    {
      const Standard_Real aTransparency = Transparency();
      if ( myAIS->Transparency() != aTransparency )
        if( !aContext.IsNull() )
          aContext->SetTransparency(myAIS, aTransparency, Standard_False);
        else
          myAIS->SetTransparency(aTransparency);
    }

    if ( HasOwnWidth() )
    {
      const Standard_Real aWidth = Width();
      if ( !(myAIS->HasWidth()) || (myAIS->HasWidth() && myAIS->Width() != aWidth) )
        if ( !aContext.IsNull() )
          aContext->SetWidth(myAIS, aWidth, Standard_False);
        else
          myAIS->SetWidth(aWidth);
    }

    if ( HasOwnMode() )
    {
      const Standard_Integer aMode = Mode();
      if ( myAIS->DisplayMode() != aMode )
        myAIS->SetDisplayMode(aMode);
    }

    if ( HasOwnSelectionMode() )
    {
      const Standard_Integer aMode = SelectionMode();
      if (myAIS->SelectionMode() != aMode )
        myAIS->SetSelectionMode(aMode);
    }
  }

  if ( !aContext.IsNull() && IsDisplayed() )
    aContext->Redisplay(myAIS, Standard_False);

    if (hasOwnSelectionMode) {
      const Handle(AIS_InteractiveContext) aContext =
        ctx.IsNull() ? myAIS->GetContext() : ctx;
      if (!aContext.IsNull())
      {
        TColStd_ListOfInteger anActivatedModes;
        aContext->ActivatedModes (myAIS, anActivatedModes);
        Standard_Boolean isActivated = Standard_False;
        for (TColStd_ListIteratorOfListOfInteger aModeIter (anActivatedModes); aModeIter.More(); aModeIter.Next())
        {
          if (aModeIter.Value() == mySelectionMode)
          {
            isActivated = Standard_True;
            break;
          }
        }

        if (!isActivated)
          aContext->Activate (myAIS, mySelectionMode, Standard_False);
      } 
    }

  }
  
  if( isDisplayed && !ctx.IsNull() ) ctx->Redisplay(myAIS, Standard_False);
 
  return;
}

//=======================================================================
//function : AISDisplay
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::AISDisplay()
{
  if ( !Label().IsNull() )
  {
    Handle(AIS_InteractiveContext) aContext = getAISContext();

    if ( aContext.IsNull() )
      return;

    if ( !myAIS.IsNull() )
    {
      if ( !(myAIS->GetContext()).IsNull() && (myAIS->GetContext()) != aContext )
        myAIS->GetContext()->Remove(myAIS);

      if ( IsDisplayed() && aContext->IsDisplayed(myAIS) )
        return;

      aContext->Display(myAIS, Standard_False);

      if ( aContext->IsDisplayed(myAIS) )
        SetDisplayed(Standard_True);
    }
  }
}


//=======================================================================
//function : AISErase
//purpose  : 
//=======================================================================
void TPrsStd_AISPresentation::AISErase(const Standard_Boolean theIsRemove)
{
  SetDisplayed(Standard_False);
  Handle(AIS_InteractiveContext) aContext, anOwnContext;

  if ( !myAIS.IsNull() )
  {
    if ( !Label().IsNull() )
    {
      Handle(TPrsStd_AISViewer) viewer;
      if( !TPrsStd_AISViewer::Find(Label(), viewer) )
        return;
      anOwnContext = myAIS->GetContext();
      aContext = viewer->GetInteractiveContext();

      if ( theIsRemove )
      {
        if ( !aContext.IsNull() )
          aContext->Remove(myAIS, Standard_False);
        if ( !anOwnContext.IsNull() && anOwnContext != aContext )
          anOwnContext->Remove(myAIS, Standard_False);

        myAIS->SetToUpdate();
      }
      else
      {
        if ( !aContext.IsNull() )
          aContext->Erase(myAIS, Standard_False);
        if ( !anOwnContext.IsNull() && anOwnContext != aContext )
          anOwnContext->Erase(myAIS, Standard_False);
      }
    }
    else
    {
      if ( theIsRemove )
      {
        if ( !anOwnContext.IsNull() )
        {
          anOwnContext->Remove(myAIS, Standard_False);
          myAIS->SetToUpdate();
        }
      }
      else
        if( !anOwnContext.IsNull() )
          anOwnContext->Erase(myAIS, Standard_False);
    }
  }
}


//=======================================================================
//function :GetAIS
//purpose  : 
//=======================================================================
Handle(AIS_InteractiveObject) TPrsStd_AISPresentation::GetAIS() const
{
  return myAIS;
}


//=======================================================================
//function : getAISContext
//purpose  : 
//=======================================================================
Handle(AIS_InteractiveContext) TPrsStd_AISPresentation::getAISContext() const
{
  Handle(TPrsStd_AISViewer) aViewer;
  if ( TPrsStd_AISViewer::Find(Label(), aViewer) )
    return aViewer->GetInteractiveContext();

  return Handle_AIS_InteractiveContext();
}


//=======================================================================
//function : getData
//purpose  : 
//=======================================================================
Handle(TDataXtd_Presentation) TPrsStd_AISPresentation::getData() const
{
  return TDataXtd_Presentation::Set(this);
}

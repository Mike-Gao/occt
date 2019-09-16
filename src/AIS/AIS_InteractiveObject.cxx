// Created on: 1996-12-18
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#include <AIS_InteractiveObject.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectFillCapping.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <PrsMgr_PresentationManager.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_InteractiveObject,SelectMgr_SelectableObject)

//=======================================================================
//function : AIS_InteractiveObject
//purpose  : 
//=======================================================================
AIS_InteractiveObject::AIS_InteractiveObject (const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
: SelectMgr_SelectableObject (aTypeOfPresentation3d),
  myCTXPtr (NULL)
{
  //
}

//=======================================================================
//function : Redisplay
//purpose  :
//=======================================================================
void AIS_InteractiveObject::Redisplay (const Standard_Boolean AllModes)
{
  if (myCTXPtr == NULL)
    return;

  myCTXPtr->Redisplay (this, Standard_False, AllModes);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Handle(AIS_InteractiveContext) AIS_InteractiveObject::GetContext() const 
{
  return myCTXPtr;
}

//=======================================================================
//function : SetContext
//purpose  :
//=======================================================================
void AIS_InteractiveObject::SetContext (const Handle(AIS_InteractiveContext)& theCtx)
{
  if (myCTXPtr == theCtx.get())
  {
    return;
  }

  myCTXPtr = theCtx.get();
  if (!theCtx.IsNull())
  {
    myDrawer->Link (theCtx->DefaultDrawer());
  }
}

//=======================================================================
//function : HasPresentation
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveObject::HasPresentation() const
{
  return HasInteractiveContext()
      && myCTXPtr->MainPrsMgr()->HasPresentation (this, myDrawer->DisplayMode());
}

//=======================================================================
//function : Presentation
//purpose  :
//=======================================================================
Handle(Prs3d_Presentation) AIS_InteractiveObject::Presentation() const
{
  if (!HasInteractiveContext())
  {
    return Handle(Prs3d_Presentation)();
  }

  Handle(PrsMgr_Presentation) aPrs = myCTXPtr->MainPrsMgr()->Presentation (this, myDrawer->DisplayMode(), false);
  return aPrs;
}

//=======================================================================
//function : SetAspect 
//purpose  : 
//=======================================================================
void AIS_InteractiveObject::SetAspect(const Handle(Prs3d_BasicAspect)& theAspect)
{

  if (!HasPresentation())
  {
    return;
  }

  Handle(Prs3d_Presentation) aPrs = Presentation();
  if (aPrs->Groups().IsEmpty())
  {
    return;
  }
  const Handle(Graphic3d_Group)& aGroup = aPrs->Groups().Last();
  if (Handle(Prs3d_ShadingAspect) aShadingAspect = Handle(Prs3d_ShadingAspect)::DownCast(theAspect))
  {
    aGroup->SetGroupPrimitivesAspect (aShadingAspect->Aspect());
  }
  else if (Handle(Prs3d_LineAspect) aLineAspect = Handle(Prs3d_LineAspect)::DownCast(theAspect))
  {
    aGroup->SetGroupPrimitivesAspect (aLineAspect->Aspect());
  }
  else if (Handle(Prs3d_PointAspect) aPointAspect = Handle(Prs3d_PointAspect)::DownCast(theAspect))
  {
    aGroup->SetGroupPrimitivesAspect (aPointAspect->Aspect());
  }
  else if (Handle(Prs3d_TextAspect) aTextAspect = Handle(Prs3d_TextAspect)::DownCast(theAspect))
  {
    aGroup->SetGroupPrimitivesAspect (aTextAspect->Aspect());
  }
}

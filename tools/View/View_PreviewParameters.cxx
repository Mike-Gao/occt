// Created on: 2019-05-03
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <inspector/View_PreviewParameters.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
View_PreviewParameters::View_PreviewParameters()
{
  myDrawer = new Prs3d_Drawer();

  Quantity_Color aColor(Quantity_NOC_TOMATO);//Quantity_NOC_GREENYELLOW));//Quantity_NOC_BLUE1));
  Standard_ShortReal aTransparency = 0.8;

  // point parameters
  myDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_O_PLUS, aColor, 3.0));

  // shading parameters
  Graphic3d_MaterialAspect aShadingMaterial;
  aShadingMaterial.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
  aShadingMaterial.SetMaterialType (Graphic3d_MATERIAL_ASPECT);

  myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
  myDrawer->ShadingAspect()->Aspect()->SetInteriorStyle (Aspect_IS_SOLID);
  myDrawer->ShadingAspect()->SetColor (aColor);
  myDrawer->ShadingAspect()->SetMaterial (aShadingMaterial);

  //myDrawer->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency (aTransparency);
  //myDrawer->ShadingAspect()->Aspect()->ChangeBackMaterial() .SetTransparency (aTransparency);
  //myDrawer->SetTransparency (aTransparency);

  // common parameters
  myDrawer->SetZLayer (Graphic3d_ZLayerId_Topmost);
}

// =======================================================================
// function : SaveState
// purpose :
// =======================================================================
void View_PreviewParameters::SaveState (View_PreviewParameters* theParameters,
                                              QMap<QString, QString>& theItems,
                                              const QString& thePrefix)
{
  Handle(Prs3d_Drawer) aDrawer = theParameters->GetDrawer();

  //Quantity_Color aColor = aDrawer->Color();
  //Standard_ShortReal aTransparency = aDrawer->Transparency();

  //// point parameters
  //{
  //  Standard_Boolean anOwnPointAspect = aDrawer->HasOwnPointAspect();
  //  Standard_SStream OS;
  //  if (anOwnPointAspect)
  //    myDrawer->PointAspect()->Dump (OS);
  //  TCollection_AsciiString aStream (OS.str().c_str());
  //  theItems[thePrefix + "has_point_aspect"] = anOwnPointAspect;
  //  theItems[thePrefix + "point_aspect"] = aStream.ToCString();
  //}
  //// shading parameters
  //{
  //  Standard_Boolean anOwnShadingAspect = aDrawer->HasOwnShadingAspect();
  //  Standard_SStream OS;
  //  if (anOwnShadingAspect)
  //    myDrawer->ShadingAspect()->Dump (OS);
  //  TCollection_AsciiString aStream (OS.str().c_str());
  //  theItems[thePrefix + "has_shading_aspect"] = anOwnShadingAspect;
  //  theItems[thePrefix + "shading_aspect"] = aStream.ToCString();
  //}
}

// =======================================================================
// function : RestoreState
// purpose :
// =======================================================================
bool  View_PreviewParameters::RestoreState (View_PreviewParameters* theParameters,
                                                  const QString& theKey, const QString& theValue,
                                                  const QString& thePrefix)
{
  //if (theKey == thePrefix + "has_point_aspect") // point parameters
  //{
  //  myDrawer->SetOwnPointAspect (theValue.toBool());
  //}
  //else if (theKey == thePrefix + "point_aspect") // point parameters
  //{
  //  Standard_SStream aStream;
  //  aStream << theValue.ToString().ToStdString();
  //  myDrawer->PointAspect()->Init (aStream);
  //}
  //else if (theKey == thePrefix + "has_shading_aspect") // shading parameters
  //{
  //  myDrawer->SetOwnShadingAspect (theValue.toBool());
  //}
  //else if (theKey == thePrefix + "shading_aspect") // shading parameters
  //{
  //  Standard_SStream aStream;
  //  aStream << theValue.ToString().ToStdString();
  //  myDrawer->ShadingAspect()->Init (aStream);
  //}
  //else
  //  return false;
  return true;
}

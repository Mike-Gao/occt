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

#include <Graphic3d_Aspects.hxx>
#include <TCollection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Aspects, Standard_Transient)

// =======================================================================
// function : Graphic3d_Aspects
// purpose  :
// =======================================================================
Graphic3d_Aspects::Graphic3d_Aspects()
: myInteriorColor       (Quantity_NOC_CYAN1),
  myBackInteriorColor   (Quantity_NOC_CYAN1),
  myEdgeColor           (Quantity_NOC_WHITE),
  myInteriorStyle       (Aspect_IS_SOLID),
  myShadingModel        (Graphic3d_TOSM_DEFAULT),
  myAlphaMode           (Graphic3d_AlphaMode_BlendAuto),
  myAlphaCutoff         (0.5f),
  myLineType            (Aspect_TOL_SOLID),
  myLineWidth           (1.0f),
  myMarkerType          (Aspect_TOM_POINT),
  myMarkerScale         (1.0f),
  myTextStyle           (Aspect_TOST_NORMAL),
  myTextDisplayType     (Aspect_TODT_NORMAL),
  myTextFontAspect      (Font_FontAspect_Regular),
  myTextAngle           (0.0f),
  myToSkipFirstEdge     (false),
  myToDistinguishMaterials (false),
  myToDrawEdges         (false),
  myToDrawSilhouette    (false),
  myToSuppressBackFaces (true),
  myToMapTexture        (false),
  myIsTextZoomable      (false)
{
  //
}

// =======================================================================
// function : SetTextureMap
// purpose  :
// =======================================================================
void Graphic3d_Aspects::SetTextureMap (const Handle(Graphic3d_TextureMap)& theTexture)
{
  if (theTexture.IsNull())
  {
    myTextureSet.Nullify();
    return;
  }

  myTextureSet = new Graphic3d_TextureSet (theTexture);
}

const TCollection_AsciiString Graphic3d_Aspects_ClassName = "Graphic3d_Aspects";

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Graphic3d_Aspects::Dump (Standard_OStream& OS) const
{
  DUMP_START_KEY (OS, Graphic3d_Aspects_ClassName);
  
  /*Handle(Graphic3d_ShaderProgram)  myProgram;
  Handle(Graphic3d_TextureSet)     myTextureSet;
  Handle(Graphic3d_MarkerImage)    myMarkerImage;
  Handle(Graphic3d_HatchStyle)     myHatchStyle;
  Handle(TCollection_HAsciiString) myTextFont;
  Graphic3d_MaterialAspect         myFrontMaterial;
  Graphic3d_MaterialAspect         myBackMaterial;
  */

  Standard_SStream aTmpStream;
  myInteriorColor.Dump (aTmpStream);
  DUMP_VALUES (OS, "InteriorColor", TCollection::ToDumpString (aTmpStream));

  aTmpStream.str ("");
  myBackInteriorColor.Dump (aTmpStream);
  DUMP_VALUES (OS, "BackInteriorColor", TCollection::ToDumpString (aTmpStream));

  aTmpStream.str ("");
  myEdgeColor.Dump (aTmpStream);
  DUMP_VALUES (OS, "EdgeColor", TCollection::ToDumpString (aTmpStream));

  aTmpStream.str ("");
  myPolygonOffset.Dump (aTmpStream);
  DUMP_VALUES (OS, "PolygonOffset", TCollection::ToDumpString (aTmpStream));


  /*Aspect_InteriorStyle         myInteriorStyle;
  Graphic3d_TypeOfShadingModel myShadingModel;
  Graphic3d_AlphaMode          myAlphaMode;
  Standard_ShortReal           myAlphaCutoff;

  Aspect_TypeOfLine            myLineType;
  Standard_ShortReal           myLineWidth;

  Aspect_TypeOfMarker          myMarkerType;
  Standard_ShortReal           myMarkerScale;

  Aspect_TypeOfStyleText   myTextStyle;
  Aspect_TypeOfDisplayText myTextDisplayType;
  Font_FontAspect          myTextFontAspect;
  Standard_ShortReal       myTextAngle;
  */

  DUMP_VALUES (OS, "ToSkipFirstEdge", myToSkipFirstEdge);
  DUMP_VALUES (OS, "ToDistinguishMaterials", myToDistinguishMaterials);
  DUMP_VALUES (OS, "ToDrawEdges", myToDrawEdges);
  DUMP_VALUES (OS, "ToDrawSilhouette", myToDrawSilhouette);
  DUMP_VALUES (OS, "ToSuppressBackFaces", myToSuppressBackFaces);
  DUMP_VALUES (OS, "ToMapTexture", myToMapTexture);
  DUMP_VALUES (OS, "IsTextZoomable", myIsTextZoomable);


  DUMP_STOP_KEY (OS, Graphic3d_Aspects_ClassName);
}

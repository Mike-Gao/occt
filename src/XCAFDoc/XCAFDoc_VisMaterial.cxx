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

#include <XCAFDoc_VisMaterial.hxx>

#include <Graphic3d_MaterialAspect.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_VisMaterial, TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& XCAFDoc_VisMaterial::GetID()
{
  static Standard_GUID THE_VIS_MAT_ID ("EBB00255-03A0-4845-BD3B-A70EEDEEFA78");
  return THE_VIS_MAT_ID;
}

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
XCAFDoc_VisMaterial::XCAFDoc_VisMaterial()
{
  //
}

 //=======================================================================
 //function : Set
 //purpose  :
 //=======================================================================
Handle(XCAFDoc_VisMaterial) XCAFDoc_VisMaterial::Set (const TDF_Label& theLabel,
                                                      const XCAFDoc_VisMaterialMetallicRoughness& theMetRoughnessMat,
                                                      const XCAFDoc_VisMaterialCommon& theCommonMat)
{
  Handle(XCAFDoc_VisMaterial) anAttrib;
  if (!theLabel.FindAttribute (XCAFDoc_VisMaterial::GetID(), anAttrib))
  {
    anAttrib = new XCAFDoc_VisMaterial();
    theLabel.AddAttribute (anAttrib);
  }

  anAttrib->Backup();
  anAttrib->myMetalRoughMat = theMetRoughnessMat;
  anAttrib->myCommonMat = theCommonMat;
  return anAttrib;
}

//=======================================================================
//function : SetMetalRougnessMaterial
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::SetMetalRougnessMaterial (const XCAFDoc_VisMaterialMetallicRoughness& theMaterial)
{
  Backup();
  myMetalRoughMat = theMaterial;
}

//=======================================================================
//function : SetCommonMaterial
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::SetCommonMaterial (const XCAFDoc_VisMaterialCommon& theMaterial)
{
  Backup();
  myCommonMat = theMaterial;
}

//=======================================================================
//function : Restore
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::Restore (const Handle(TDF_Attribute)& theWith)
{
  XCAFDoc_VisMaterial* anOther = dynamic_cast<XCAFDoc_VisMaterial* >(theWith.get());
  myMetalRoughMat = anOther->myMetalRoughMat;
  myCommonMat     = anOther->myCommonMat;
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================
Handle(TDF_Attribute) XCAFDoc_VisMaterial::NewEmpty() const
{
  return new XCAFDoc_VisMaterial();
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::Paste (const Handle(TDF_Attribute)& theInto,
                                 const Handle(TDF_RelocationTable)& ) const
{
  XCAFDoc_VisMaterial* anOther = dynamic_cast<XCAFDoc_VisMaterial* >(theInto.get());
  anOther->Backup();
  anOther->myMetalRoughMat = myMetalRoughMat;
  anOther->myCommonMat     = myCommonMat;
}

//=======================================================================
//function : FillMaterialAspect
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterial::FillMaterialAspect (Graphic3d_MaterialAspect& theAspect) const
{
  if (myCommonMat.IsDefined)
  {
    theAspect = Graphic3d_MaterialAspect (Graphic3d_NOM_UserDefined);
    theAspect.SetAmbientColor (myCommonMat.AmbientColor);
    theAspect.SetDiffuseColor (myCommonMat.DiffuseColor);
    theAspect.SetSpecularColor(myCommonMat.SpecularColor);
    theAspect.SetEmissiveColor(myCommonMat.EmissiveColor);
    theAspect.SetTransparency (myCommonMat.Transparency);
    theAspect.SetShininess    (myCommonMat.Shininess);
  }
  if (myMetalRoughMat.IsDefined)
  {
    if (myCommonMat.IsDefined)
    {
      return;
    }

    theAspect = Graphic3d_MaterialAspect (Graphic3d_NOM_UserDefined);
    theAspect.SetDiffuseColor (myMetalRoughMat.BaseColor.GetRGB());
    theAspect.SetAlpha        (myMetalRoughMat.BaseColor.Alpha());
    theAspect.SetSpecularColor(Quantity_Color (Graphic3d_Vec3 (myMetalRoughMat.Metallic)));
    theAspect.SetShininess    (1.0f - myMetalRoughMat.Roughness);
  }
}

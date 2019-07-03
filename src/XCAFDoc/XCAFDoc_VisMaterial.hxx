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

#ifndef _XCAFDoc_VisMaterial_HeaderFile
#define _XCAFDoc_VisMaterial_HeaderFile

#include <Graphic3d_Vec.hxx>
#include <Image_Texture.hxx>
#include <TDF_Attribute.hxx>
#include <Quantity_ColorRGBA.hxx>

class Graphic3d_MaterialAspect;

//! Common (obsolete) material definition.
struct XCAFDoc_VisMaterialCommon
{
  Handle(Image_Texture)   AmbientTexture;  //!< image defining ambient color
  Handle(Image_Texture)   DiffuseTexture;  //!< image defining diffuse color
  Handle(Image_Texture)   SpecularTexture; //!< image defining specular color
  Quantity_Color          AmbientColor;    //!< ambient  color
  Quantity_Color          DiffuseColor;    //!< diffuse  color
  Quantity_Color          SpecularColor;   //!< specular color
  Quantity_Color          EmissiveColor;   //!< emission color
  Standard_ShortReal      Shininess;       //!< shininess value
  Standard_ShortReal      Transparency;    //!< transparency value within [0, 1] range with 0 meaning opaque
  Standard_Boolean        IsDefined;       //!< defined flag; FALSE by default

  //! Empty constructor.
  XCAFDoc_VisMaterialCommon()
  : AmbientColor (0.1, 0.1, 0.1, Quantity_TOC_RGB),
    DiffuseColor (0.8, 0.8, 0.8, Quantity_TOC_RGB),
    SpecularColor(0.2, 0.2, 0.2, Quantity_TOC_RGB),
    EmissiveColor(0.0, 0.0, 0.0, Quantity_TOC_RGB),
    Shininess (1.0f),
    Transparency (0.0f),
    IsDefined (Standard_False) {}

  //! Compare two materials.
  Standard_Boolean IsEqual (const XCAFDoc_VisMaterialCommon& theOther) const
  {
    if (&theOther == this)
    {
      return true;
    }
    else if (theOther.IsDefined != IsDefined)
    {
      return false;
    }
    else if (!IsDefined)
    {
      return true;
    }

    return theOther.AmbientTexture  == AmbientTexture
        && theOther.DiffuseTexture  == DiffuseTexture
        && theOther.SpecularTexture == SpecularTexture
        && theOther.AmbientColor    == AmbientColor
        && theOther.DiffuseColor    == DiffuseColor
        && theOther.SpecularColor   == SpecularColor
        && theOther.EmissiveColor   == EmissiveColor
        && theOther.Shininess       == Shininess
        && theOther.Transparency    == Transparency;
  }
};

//! Metallic-roughness PBR material definition.
struct XCAFDoc_VisMaterialMetallicRoughness
{
  Handle(Image_Texture)   BaseColorTexture;         //!< RGB texture for the base color
  Handle(Image_Texture)   MetallicRoughnessTexture; //!< RG texture packing the metallic and roughness properties together
  Handle(Image_Texture)   EmissiveTexture;          //!< RGB emissive map controls the color and intensity of the light being emitted by the material
  Handle(Image_Texture)   OcclusionTexture;         //!< R occlusion map indicating areas of indirect lighting
  Handle(Image_Texture)   NormalTexture;            //!< normal map
  Quantity_ColorRGBA      BaseColor;                //!< base color (or scale factor to the texture); [1.0, 1.0, 1.0, 1.0] by default
  Graphic3d_Vec3          EmissiveFactor;           //!< emissive color; [0.0, 0.0, 0.0] by default
  Standard_ShortReal      Metallic;                 //!< metalness  (or scale factor to the texture) within range [0.0, 1.0]; 1.0 by default
  Standard_ShortReal      Roughness;                //!< roughness  (or scale factor to the texture) within range [0.0, 1.0]; 1.0 by default
  Standard_Boolean        IsDefined;                //!< defined flag; FALSE by default

  //! Empty constructor.
  XCAFDoc_VisMaterialMetallicRoughness()
  : BaseColor (1.0f, 1.0f, 1.0f, 1.0f),
    EmissiveFactor (0.0f, 0.0f, 0.0f),
    Metallic  (0.0f),
    Roughness (0.0f),
    IsDefined (Standard_False) {}

  //! Compare two materials.
  Standard_Boolean IsEqual (const XCAFDoc_VisMaterialMetallicRoughness& theOther) const
  {
    if (&theOther == this)
    {
      return true;
    }
    else if (theOther.IsDefined != IsDefined)
    {
      return false;
    }
    else if (!IsDefined)
    {
      return true;
    }

    return theOther.BaseColorTexture == BaseColorTexture
        && theOther.MetallicRoughnessTexture == MetallicRoughnessTexture
        && theOther.EmissiveTexture == EmissiveTexture
        && theOther.OcclusionTexture == OcclusionTexture
        && theOther.NormalTexture == NormalTexture
        && theOther.BaseColor == BaseColor
        && theOther.EmissiveFactor == EmissiveFactor
        && theOther.Metallic == Metallic
        && theOther.Roughness == Roughness;
  }
};

//! Attribute storing Material definition for visualization purposes.
class XCAFDoc_VisMaterial : public TDF_Attribute
{
  DEFINE_STANDARD_RTTIEXT(XCAFDoc_VisMaterial, TDF_Attribute)
public:

  //! Return attribute GUID.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Find, or create, the Material attribute.
  Standard_EXPORT static Handle(XCAFDoc_VisMaterial) Set (const TDF_Label& theLabel,
                                                          const XCAFDoc_VisMaterialMetallicRoughness& theMetRoughnessMat,
                                                          const XCAFDoc_VisMaterialCommon& theCommonMat);

  //! Find, or create, the Material attribute.
  static Handle(XCAFDoc_VisMaterial) Set (const TDF_Label& theLabel,
                                          const XCAFDoc_VisMaterialMetallicRoughness& theMetRoughnessMat)
  {
    return Set (theLabel, theMetRoughnessMat, XCAFDoc_VisMaterialCommon());
  }

  //! Find, or create, the Material attribute.
  static Handle(XCAFDoc_VisMaterial) Set (const TDF_Label& theLabel,
                                          const XCAFDoc_VisMaterialCommon& theCommonMat)
  {
    return Set (theLabel, XCAFDoc_VisMaterialMetallicRoughness(), theCommonMat);
  }

public:

  Standard_EXPORT XCAFDoc_VisMaterial();

  //! Return TRUE if material definition is empty.
  bool IsEmpty() const { return !myMetalRoughMat.IsDefined && !myCommonMat.IsDefined; }

  //! Fill in material aspect.
  Standard_EXPORT void FillMaterialAspect (Graphic3d_MaterialAspect& theAspect) const;

  //! Return TRUE if metal-roughness PBR material is defined.
  Standard_Boolean HasMetalRougnessMaterial() const { return myMetalRoughMat.IsDefined; }

  //! Return metal-roughness PBR material.
  const XCAFDoc_VisMaterialMetallicRoughness& MetalRougnessMaterial() const { return myMetalRoughMat; }

  //! Setup metal-roughness PBR material.
  Standard_EXPORT void SetMetalRougnessMaterial (const XCAFDoc_VisMaterialMetallicRoughness& theMaterial);

  //! Setup undefined metal-roughness PBR material.
  void UnsetMetalRougnessMaterial() { SetMetalRougnessMaterial (XCAFDoc_VisMaterialMetallicRoughness()); }

  //! Return TRUE if common material is defined.
  Standard_Boolean HasCommonMaterial() const { return myCommonMat.IsDefined; }

  //! Return common material.
  const XCAFDoc_VisMaterialCommon& CommonMaterial() const { return myCommonMat; }

  //! Setup common material.
  Standard_EXPORT void SetCommonMaterial (const XCAFDoc_VisMaterialCommon& theMaterial);

  //! Setup undefined common material.
  void UnsetCommonMaterial() { SetCommonMaterial (XCAFDoc_VisMaterialCommon()); }

  //! Compare two materials.
 /** Standard_Boolean IsEqual (const Handle(XCAFDoc_VisMaterial)& theOther) const
  {
    return theOther == this
       || (theOther->myColor == myColor);
  }*/

public: //! @name interface implementation

  //! Return GUID of this attribute type.
  virtual const Standard_GUID& ID() const Standard_OVERRIDE { return GetID(); }

  //! Restore attribute from specified state.
  //! @param theWith [in] attribute state to restore (copy into this)
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& theWith) Standard_OVERRIDE;

  //! Create a new empty attribute.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Paste this attribute into another one.
  //! @param theInto [in/out] target attribute to copy this into
  //! @param theRelTable [in] relocation table
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& theInto,
                                      const Handle(TDF_RelocationTable)& theRelTable) const Standard_OVERRIDE;

private:

  XCAFDoc_VisMaterialMetallicRoughness myMetalRoughMat; //!< metal-roughness material definition
  XCAFDoc_VisMaterialCommon            myCommonMat;     //!< common material definition
  TCollection_AsciiString              myId;            //!< material identifier

};

DEFINE_STANDARD_HANDLE(XCAFDoc_VisMaterial, TDF_Attribute)

#endif // _XCAFDoc_VisMaterial_HeaderFile

// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <RWGltf_GltfMaterialMap.hxx>

#include <RWGltf_GltfRootElement.hxx>

// =======================================================================
// function : baseColorTexture
// purpose  :
// =======================================================================
const Handle(Image_Texture)& RWGltf_GltfMaterialMap::baseColorTexture (const Handle(XCAFDoc_VisMaterial)& theMat)
{
  static const Handle(Image_Texture) THE_NULL_TEXTURE;
  if (theMat.IsNull())
  {
    return THE_NULL_TEXTURE;
  }
  else if (theMat->HasPbrMaterial()
       && !theMat->PbrMaterial().BaseColorTexture.IsNull())
  {
    return theMat->PbrMaterial().BaseColorTexture;
  }
  else if (theMat->HasCommonMaterial()
       && !theMat->CommonMaterial().DiffuseTexture.IsNull())
  {
    return theMat->CommonMaterial().DiffuseTexture;
  }
  return THE_NULL_TEXTURE;
}

// =======================================================================
// function : RWGltf_GltfMaterialMap
// purpose  :
// =======================================================================
RWGltf_GltfMaterialMap::RWGltf_GltfMaterialMap (const TCollection_AsciiString& theFile,
                                                const Standard_Integer theDefSamplerId)
: RWMesh_MaterialMap (theFile),
  myWriter (NULL),
  myDefSamplerId (theDefSamplerId),
  myNbImages (0)
{
  myMatNameAsKey = false;
}

// =======================================================================
// function : AddImages
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddImages (rapidjson::Writer<rapidjson::OStreamWrapper>* theWriter,
                                        const XCAFPrs_Style&                          theStyle,
                                    bool&                                         theIsStarted)
{
  if (theWriter == NULL
   || theStyle.Material().IsNull()
   || theStyle.Material()->IsEmpty())
  {
    return;
  }

  const Handle(Image_Texture)& aTexture = baseColorTexture (theStyle.Material());
  if (aTexture.IsNull()
   || myImageMap.IsBound1 (aTexture)
   || myImageFailMap.Contains (aTexture))
  {
    return;
  }

  TCollection_AsciiString aGltfImgKey = myNbImages;
  ++myNbImages;
  for (; myImageMap.IsBound2 (aGltfImgKey); ++myNbImages)
  {
    aGltfImgKey = myNbImages;
  }

  TCollection_AsciiString aTextureUri;
  if (!CopyTexture (aTextureUri, aTexture, aGltfImgKey))
  {
    myImageFailMap.Add (aTexture);
    return;
  }

  myImageMap.Bind (aTexture, aGltfImgKey);

  if (!theIsStarted)
  {
    theWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Images));
    theWriter->StartArray();
    theIsStarted = true;
  }

  theWriter->StartObject();
  {
    theWriter->Key ("uri");
    theWriter->String (aTextureUri.ToCString());
  }
  theWriter->EndObject();
}

// =======================================================================
// function : AddMaterial
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddMaterial (rapidjson::Writer<rapidjson::OStreamWrapper>* theWriter,
                                          const XCAFPrs_Style&                          theStyle,
                                          bool&                                         theIsStarted)
{
  if (theWriter == nullptr
   || ((theStyle.Material().IsNull() || theStyle.Material()->IsEmpty())
    && !theStyle.IsSetColorSurf()))
  {
    return;
  }

  if (!theIsStarted)
  {
    theWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Materials));
    theWriter->StartArray();
    theIsStarted = true;
  }
  myWriter = theWriter;
  AddMaterial (theStyle);
  myWriter = nullptr;
}

// =======================================================================
// function : AddTextures
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::AddTextures (rapidjson::Writer<rapidjson::OStreamWrapper>* theWriter,
                                          const XCAFPrs_Style&                          theStyle,
                                          bool&                                         theIsStarted)
{
  if (theWriter == NULL
   || theStyle.Material().IsNull()
   || theStyle.Material()->IsEmpty())
  {
    return;
  }

  const Handle(Image_Texture)& aTexture = baseColorTexture (theStyle.Material());
  if (aTexture.IsNull()
  ||  myTextureMap.Contains (aTexture)
  || !myImageMap  .IsBound1 (aTexture))
  {
    return;
  }

  const TCollection_AsciiString anImgKey = myImageMap.Find1 (aTexture);
  myTextureMap.Add (aTexture);
  if (anImgKey.IsEmpty())
  {
    return;
  }

  if (!theIsStarted)
  {
    theWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Textures));
    theWriter->StartArray();
    theIsStarted = true;
  }

  theWriter->StartObject();
  {
    theWriter->Key ("sampler");
    theWriter->Int (myDefSamplerId); // mandatory field by specs
    theWriter->Key ("source");
    theWriter->Int (anImgKey.IntegerValue());
  }
  theWriter->EndObject();
}

// =======================================================================
// function : AddTextures
// purpose  :
// =======================================================================
TCollection_AsciiString RWGltf_GltfMaterialMap::AddMaterial (const XCAFPrs_Style& theStyle)
{
  return RWMesh_MaterialMap::AddMaterial (theStyle);
}

// =======================================================================
// function : DefineMaterial
// purpose  :
// =======================================================================
void RWGltf_GltfMaterialMap::DefineMaterial (const XCAFPrs_Style& theStyle,
                                             const TCollection_AsciiString& /*theKey*/,
                                             const TCollection_AsciiString& theName)
{
  if (myWriter == NULL)
  {
    Standard_ProgramError::Raise ("RWGltf_GltfMaterialMap::DefineMaterial() should be called with JSON Writer");
    return;
  }

  XCAFDoc_VisMaterialPBR aPbrMat;
  if (!theStyle.Material().IsNull()
   && !theStyle.Material()->IsEmpty())
  {
    aPbrMat = theStyle.Material()->ConvertToPbrMaterial();
  }
  else if (!myDefaultStyle.Material().IsNull()
         && myDefaultStyle.Material()->HasPbrMaterial())
  {
    aPbrMat = myDefaultStyle.Material()->PbrMaterial();
  }
  if (theStyle.IsSetColorSurf())
  {
    aPbrMat.BaseColor.SetRGB (theStyle.GetColorSurf());
    if (theStyle.GetColorSurfRGBA().Alpha() < 1.0f)
    {
      aPbrMat.BaseColor.SetAlpha (theStyle.GetColorSurfRGBA().Alpha());
    }
  }
  myWriter->StartObject();
  {
    myWriter->Key ("name");
    myWriter->String (theName.ToCString());

    myWriter->Key ("pbrMetallicRoughness");
    myWriter->StartObject();
    {
      myWriter->Key ("baseColorFactor");
      myWriter->StartArray();
      {
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Red());
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Green());
        myWriter->Double (aPbrMat.BaseColor.GetRGB().Blue());
        myWriter->Double (aPbrMat.BaseColor.Alpha());
      }
      myWriter->EndArray();

      if (const Handle(Image_Texture)& aBaseTexture = baseColorTexture (theStyle.Material()))
      {
        if (myImageMap.IsBound1 (aBaseTexture))
        {
          myWriter->Key ("baseColorTexture");
          myWriter->StartObject();
          {
            myWriter->Key ("index");
            const TCollection_AsciiString& anImageIdx = myImageMap.Find1 (aBaseTexture);
            if (!anImageIdx.IsEmpty())
            {
              myWriter->Int (anImageIdx.IntegerValue());
            }
          }
          myWriter->EndObject();
        }
      }

      if (aPbrMat.EmissiveFactor != Graphic3d_Vec3 (0.0f, 0.0f, 0.0f))
      {
        myWriter->Key ("emissiveFactor");
        myWriter->StartArray();
        {
          myWriter->Double (aPbrMat.EmissiveFactor.r());
          myWriter->Double (aPbrMat.EmissiveFactor.g());
          myWriter->Double (aPbrMat.EmissiveFactor.b());
        }
        myWriter->EndArray();
      }

      myWriter->Key ("metallicFactor");
      myWriter->Double (aPbrMat.Metallic);

      myWriter->Key ("roughnessFactor");
      myWriter->Double (aPbrMat.Roughness);
    }
    myWriter->EndObject();
  }
  myWriter->EndObject();
}

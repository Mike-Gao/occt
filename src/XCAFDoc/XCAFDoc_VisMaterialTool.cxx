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

#include <XCAFDoc_VisMaterialTool.hxx>

#include <Standard_GUID.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_VisMaterialTool, TDF_Attribute)

static Standard_Boolean XCAFDoc_VisMaterialTool_AutoNaming = Standard_True;

//=======================================================================
//function : SetAutoNaming
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterialTool::SetAutoNaming (Standard_Boolean theIsAutoNaming)
{
  XCAFDoc_VisMaterialTool_AutoNaming = theIsAutoNaming;
}

//=======================================================================
//function : AutoNaming
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_VisMaterialTool::AutoNaming()
{
  return XCAFDoc_VisMaterialTool_AutoNaming;
}

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& XCAFDoc_VisMaterialTool::GetID()
{
  static Standard_GUID THE_VIS_MAT_TOOL_ID ("87B511CE-DA15-4A5E-98AF-E3F46AB5B6E8");
  return THE_VIS_MAT_TOOL_ID;
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
Handle(XCAFDoc_VisMaterialTool) XCAFDoc_VisMaterialTool::Set (const TDF_Label& theLabel)
{
  Handle(XCAFDoc_VisMaterialTool) aTool;
  if (!theLabel.FindAttribute (XCAFDoc_VisMaterialTool::GetID(), aTool))
  {
    aTool = new XCAFDoc_VisMaterialTool();
    theLabel.AddAttribute (aTool);
    aTool->myShapeTool = XCAFDoc_DocumentTool::ShapeTool (theLabel);
  }
  return aTool;
}


//=======================================================================
//function : XCAFDoc_VisMaterialTool
//purpose  :
//=======================================================================
XCAFDoc_VisMaterialTool::XCAFDoc_VisMaterialTool()
{
  //
}

//=======================================================================
//function : ShapeTool
//purpose  :
//=======================================================================
const Handle(XCAFDoc_ShapeTool)& XCAFDoc_VisMaterialTool::ShapeTool()
{
  if (myShapeTool.IsNull())
  {
    myShapeTool = XCAFDoc_DocumentTool::ShapeTool (Label());
  }
  return myShapeTool;
}

//=======================================================================
//function : GetMaterial
//purpose  :
//=======================================================================
Handle(XCAFDoc_VisMaterial) XCAFDoc_VisMaterialTool::GetMaterial (const TDF_Label& theMatLabel) const
{
  Handle(XCAFDoc_VisMaterial) aMatAttrib;
  if (theMatLabel.Father() == Label())
  {
    theMatLabel.FindAttribute (XCAFDoc_VisMaterial::GetID(), aMatAttrib);
  }
  return aMatAttrib;
}

//=======================================================================
//function : FindMaterial
//purpose  :
//=======================================================================
/**Standard_Boolean XCAFDoc_VisMaterialTool::FindMaterial (const Handle(XCAFDoc_VisMaterial)& theMaterial,
                                                        TDF_Label& theLabel) const
{
  for (TDF_ChildIDIterator aChildIter (Label(), XCAFDoc_VisMaterial::GetID()); aChildIter.More(); aChildIter.Next())
  {
    const TDF_Label aLabel = aChildIter.Value()->Label();
    if (Handle(XCAFDoc_VisMaterial) aMat = GetMaterial (aLabel))
    {
      if (aMat->IsEqual (theMaterial))
      {
        theLabel = aLabel;
        return Standard_True;
      }
    }
  }
  return Standard_False;
}*/

//=======================================================================
//function : AddMaterial
//purpose  :
//=======================================================================
TDF_Label XCAFDoc_VisMaterialTool::AddMaterial (const XCAFDoc_VisMaterialMetallicRoughness& theMetRoughnessMat,
                                                const XCAFDoc_VisMaterialCommon& theCommonMat) const
{
  TDF_TagSource aTag;
  TDF_Label aLab = aTag.NewChild (Label());
  XCAFDoc_VisMaterial::Set (aLab, theMetRoughnessMat, theCommonMat);
  if (!XCAFDoc_VisMaterialTool_AutoNaming)
  {
    return aLab;
  }

  // set name according to color value
  /**const NCollection_Vec4<float>& anRgbaF = theColor;
  const NCollection_Vec4<unsigned int> anRgba (anRgbaF * 255.0f);
  char aColorHex[32];
  Sprintf (aColorHex, "%02X%02X%02X%02X", anRgba.r(), anRgba.g(), anRgba.b(), anRgba.a());
  const TCollection_AsciiString aName = TCollection_AsciiString (Quantity_Color::StringName (theColor.GetRGB().Name()))
                                      + " (#" + aColorHex + ")";
  TDataStd_Name::Set (aLab, aName);*/

  return aLab;
}

//=======================================================================
//function : RemoveMaterial
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterialTool::RemoveMaterial (const TDF_Label& theLabel) const
{
  theLabel.ForgetAllAttributes (true);
}

//=======================================================================
//function : GetMaterials
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterialTool::GetMaterials (TDF_LabelSequence& theLabels) const
{
  theLabels.Clear();
  for (TDF_ChildIDIterator aChildIDIterator (Label(), XCAFDoc_VisMaterial::GetID()); aChildIDIterator.More(); aChildIDIterator.Next())
  {
    const TDF_Label aLabel = aChildIDIterator.Value()->Label();
    if (IsMaterial (aLabel))
    {
      theLabels.Append (aLabel);
    }
  }
}

//=======================================================================
//function : SetShapeMaterial
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterialTool::SetShapeMaterial (const TDF_Label& theShapeLabel,
                                                const TDF_Label& theMaterialLabel) const
{
  // set reference
  Handle(TDataStd_TreeNode) aMainNode = TDataStd_TreeNode::Set (theMaterialLabel, XCAFDoc::VisMaterialRefGUID());
  Handle(TDataStd_TreeNode) aRefNode  = TDataStd_TreeNode::Set (theShapeLabel,    XCAFDoc::VisMaterialRefGUID());
  aRefNode->Remove(); // abv: fix against bug in TreeNode::Append()
  aMainNode->Prepend (aRefNode);
}

//=======================================================================
//function : UnSetShapeMaterial
//purpose  :
//=======================================================================
void XCAFDoc_VisMaterialTool::UnSetShapeMaterial (const TDF_Label& theShapeLabel) const
{
  theShapeLabel.ForgetAttribute (XCAFDoc::VisMaterialRefGUID());
}

//=======================================================================
//function : IsSet
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_VisMaterialTool::IsSet (const TDF_Label& theLabel) const
{
  Handle(TDataStd_TreeNode) aNode;
  return theLabel.FindAttribute (XCAFDoc::VisMaterialRefGUID(), aNode)
      && aNode->HasFather();
}

//=======================================================================
//function : GetShapeMaterial
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_VisMaterialTool::GetShapeMaterial (const TDF_Label& theShapeLabel,
                                                            TDF_Label& theMaterialLabel)
{
  Handle(TDataStd_TreeNode) aNode;
  if (!theShapeLabel.FindAttribute (XCAFDoc::VisMaterialRefGUID(), aNode)
   || !aNode->HasFather())
  {
    return Standard_False;
  }

  theMaterialLabel = aNode->Father()->Label();
  return Standard_True;
}

//=======================================================================
//function : GetShapeMaterial
//purpose  :
//=======================================================================
Handle(XCAFDoc_VisMaterial) XCAFDoc_VisMaterialTool::GetShapeMaterial (const TDF_Label& theShapeLabel)
{
  TDF_Label aMatLabel;
  return GetShapeMaterial (theShapeLabel, aMatLabel)
       ? GetMaterial (aMatLabel)
       : Handle(XCAFDoc_VisMaterial)();
}

//=======================================================================
//function : SetShapeMaterial
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_VisMaterialTool::SetShapeMaterial (const TopoDS_Shape& theShape,
                                                            const TDF_Label& theMaterialLabel)
{
  TDF_Label aShapeLabel;
  if (!ShapeTool()->Search (theShape, aShapeLabel))
  {
    return Standard_False;
  }

  SetShapeMaterial (aShapeLabel, theMaterialLabel);
  return Standard_True;
}

//=======================================================================
//function : UnSetShapeMaterial
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_VisMaterialTool::UnSetShapeMaterial (const TopoDS_Shape& theShape)
{
  TDF_Label aShapeLabel;
  if (!ShapeTool()->Search (theShape, aShapeLabel))
  {
    return Standard_False;
  }

  UnSetShapeMaterial (aShapeLabel);
  return Standard_True;
}

//=======================================================================
//function : IsSet
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_VisMaterialTool::IsSet (const TopoDS_Shape& theShape)
{
  TDF_Label aShapeLabel;
  return ShapeTool()->Search (theShape, aShapeLabel)
      && IsSet (aShapeLabel);
}

//=======================================================================
//function : GetShapeMaterial
//purpose  :
//=======================================================================
Standard_Boolean XCAFDoc_VisMaterialTool::GetShapeMaterial (const TopoDS_Shape& theShape,
                                                            TDF_Label& theMaterialLabel)
{
  TDF_Label aShapeLabel;
  return ShapeTool()->Search (theShape, aShapeLabel)
      && GetShapeMaterial (aShapeLabel, theMaterialLabel);
}

//=======================================================================
//function : GetShapeMaterial
//purpose  :
//=======================================================================
Handle(XCAFDoc_VisMaterial) XCAFDoc_VisMaterialTool::GetShapeMaterial (const TopoDS_Shape& theShape)
{
  TDF_Label aMatLabel;
  return GetShapeMaterial (theShape, aMatLabel)
       ? GetMaterial (aMatLabel)
       : Handle(XCAFDoc_VisMaterial)();
}

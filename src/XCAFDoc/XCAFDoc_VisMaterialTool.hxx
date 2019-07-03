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

#ifndef _XCAFDoc_VisMaterialTool_HeaderFile
#define _XCAFDoc_VisMaterialTool_HeaderFile

#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_LabelSequence.hxx>

class TopoDS_Shape;
class Quantity_ColorRGBA;
class XCAFDoc_ShapeTool;
class XCAFDoc_VisMaterial;
struct XCAFDoc_VisMaterialMetallicRoughness;
struct XCAFDoc_VisMaterialCommon;

//! Provides tools to store and retrieve attributes (visualization materials) of TopoDS_Shape in and from TDocStd_Document.
class XCAFDoc_VisMaterialTool : public TDF_Attribute
{
  DEFINE_STANDARD_RTTIEXT(XCAFDoc_VisMaterialTool, TDF_Attribute)
public:
  //! Returns current auto-naming mode; TRUE by default.
  //! If TRUE then for added colors the TDataStd_Name attribute will be automatically added.
  //! This setting is global.
  Standard_EXPORT static Standard_Boolean AutoNaming();

  //! See also AutoNaming().
  Standard_EXPORT static void SetAutoNaming (Standard_Boolean theIsAutoNaming);

  //! Creates (if not exist) ColorTool.
  Standard_EXPORT static Handle(XCAFDoc_VisMaterialTool) Set (const TDF_Label& L);

  Standard_EXPORT static const Standard_GUID& GetID();

public:

  Standard_EXPORT XCAFDoc_VisMaterialTool();

  //! returns the label under which colors are stored
  Standard_EXPORT TDF_Label BaseLabel() const { return Label(); }

  //! Returns internal XCAFDoc_ShapeTool tool
  Standard_EXPORT const Handle(XCAFDoc_ShapeTool)& ShapeTool();

  //! Returns TRUE if Label belongs to a Material Table.
  Standard_Boolean IsMaterial (const TDF_Label& theLabel) const { return !GetMaterial (theLabel).IsNull(); }

  //! Returns Material defined by specified Label, or NULL if the label is not in Material Table.
  Standard_EXPORT Handle(XCAFDoc_VisMaterial) GetMaterial (const TDF_Label& theMatLabel) const;

  //! Finds Material definition in Material Table and returns its label if found.
  /*Standard_EXPORT Standard_Boolean FindMaterial (const Handle(XCAFDoc_VisMaterial)& theMaterial,
                                                 TDF_Label& theLabel) const;*/

  //! Finds a color definition in a colortable and returns its label if found (or Null label else)
/**  TDF_Label FindColor (const Quantity_ColorRGBA& theColor) const
  {
    TDF_Label aLabel;
    FindColor (theColor, aLabel);
    return aLabel;
  }*/

  //! Adds Material definition to a Material Table and returns its Label (returns existing label if the same material is already defined).
  Standard_EXPORT TDF_Label AddMaterial (const XCAFDoc_VisMaterialMetallicRoughness& theMetRoughnessMat,
                                         const XCAFDoc_VisMaterialCommon& theCommonMat) const;

  //! Adds Material definition to a Material Table and returns its Label (returns existing label if the same material is already defined).
  /*Standard_EXPORT TDF_Label AddUniqueMaterial (const XCAFDoc_VisMaterialMetallicRoughness& theMetRoughnessMat,
                                               const XCAFDoc_VisMaterialCommon& theCommonMat) const;*/

  //! Removes Material from the Material Table
  Standard_EXPORT void RemoveMaterial (const TDF_Label& theLabel) const;

  //! Returns a sequence of Materials currently stored in the Material Table.
  Standard_EXPORT void GetMaterials (TDF_LabelSequence& Labels) const;

  //! Sets a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color
  //! defined by <colorL>. Color of shape is defined following way
  //! in dependance with type of color.
  //! If type of color is XCAFDoc_ColorGen - then this color
  //! defines default color for surfaces and curves.
  //! If for shape color with types XCAFDoc_ColorSurf or XCAFDoc_ColorCurv is specified
  //! then such color overrides generic color.
  Standard_EXPORT void SetShapeMaterial (const TDF_Label& theShapeLabel,
                                         const TDF_Label& theMaterialLabel) const;

  //! Removes a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color
  Standard_EXPORT void UnSetShapeMaterial (const TDF_Label& theShapeLabel) const;

  //! Returns True if label <L> has a color assignment
  //! of the type <type>
  Standard_EXPORT Standard_Boolean IsSet (const TDF_Label& L) const;

  //! Returns label with color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT static Standard_Boolean GetShapeMaterial (const TDF_Label& theShapeLabel, TDF_Label& theMaterialLabel);

  //! Returns color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT Handle(XCAFDoc_VisMaterial) GetShapeMaterial (const TDF_Label& theShapeLabel);

  //! Sets a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color
  //! defined by <colorL>
  //! Returns False if cannot find a label for shape S
  Standard_EXPORT Standard_Boolean SetShapeMaterial (const TopoDS_Shape& theShape,
                                                     const TDF_Label& theMaterialLabel);

  //! Removes a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color
  //! Returns True if such link existed
  Standard_EXPORT Standard_Boolean UnSetShapeMaterial (const TopoDS_Shape& theShape);

  //! Returns True if label <L> has a color assignment
  //! of the type <type>
  Standard_EXPORT Standard_Boolean IsSet (const TopoDS_Shape& theShape);

  //! Returns label with color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT Standard_Boolean GetShapeMaterial (const TopoDS_Shape& theShape, TDF_Label& theMaterialLabel);

  //! Returns color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT Handle(XCAFDoc_VisMaterial) GetShapeMaterial (const TopoDS_Shape& theShape);

  /// TODO
  ///Standard_EXPORT Standard_Boolean ReverseChainsOfTreeNodes();

public:

  //! Returns GUID of this attribute type.
  virtual const Standard_GUID& ID() const Standard_OVERRIDE { return GetID(); }

  //! Does nothing.
  virtual void Restore (const Handle(TDF_Attribute)& ) Standard_OVERRIDE {}

  //! Creates new instance of this tool.
  virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE { return new XCAFDoc_VisMaterialTool(); }

  //! Does nothing.
  virtual void Paste (const Handle(TDF_Attribute)& ,
                      const Handle(TDF_RelocationTable)& ) const Standard_OVERRIDE {}

private:

  Handle(XCAFDoc_ShapeTool) myShapeTool;

};

DEFINE_STANDARD_HANDLE(XCAFDoc_VisMaterialTool, TDF_Attribute)

#endif // _XCAFDoc_VisMaterialTool_HeaderFile

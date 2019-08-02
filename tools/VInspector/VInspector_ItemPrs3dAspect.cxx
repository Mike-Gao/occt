// Created on: 2018-08-26
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/VInspector_ItemPrs3dAspect.hxx>
#include <inspector/VInspector_ItemPrs3dDrawer.hxx>
#include <inspector/ViewControl_ColorSelector.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Aspect.hxx>
#include <Font.hxx>
#include <Graphic3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Prs3d.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

int GetMaterialRows() { return 25; }

// =======================================================================
// function : initValue
// purpose :
// =======================================================================

QVariant VInspector_ItemPrs3dAspect::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole == Qt::DisplayRole || theItemRole == Qt::ToolTipRole)
  {
    Handle(Prs3d_BasicAspect) anAspect = GetAspect();
    bool aNullAspect = anAspect.IsNull();
    switch (Column())
    {
      case 0:
      {
        return theItemRole == Qt::ToolTipRole
          ? (aNullAspect ? QVariant("Prs3d_BasicAspect is empty") : QVariant (anAspect->DynamicType()->Name()))
          : QVariant (myName.ToCString());
      }
      default: break;
    }
  }
  if (theItemRole == Qt::ForegroundRole)
  {
    Handle(Prs3d_BasicAspect) anAspect = GetAspect();
    bool aNullAspect = anAspect.IsNull();
    if (aNullAspect) return QColor (Qt::lightGray);

    VInspector_ItemPrs3dDrawerPtr aParentItem = itemDynamicCast<VInspector_ItemPrs3dDrawer>(Parent());
    if (aParentItem)
    {
      Handle(Prs3d_Drawer) aDrawer = aParentItem->GetDrawer();
      if (aDrawer->Link().IsNull())
        return QVariant(); // default color

      TCollection_AsciiString aName;
      Standard_Boolean isOwnAspect;
      aParentItem->GetPrs3dAspect(Row(), aName, isOwnAspect);
    if (!isOwnAspect)
      return QColor (Qt::lightGray);
    }
    else
    {
      VInspector_ItemPrs3dAspectPtr aParentAspectItem = itemDynamicCast<VInspector_ItemPrs3dAspect>(Parent());
      if (aParentAspectItem)
        return aParentAspectItem->initValue (theItemRole);
    }
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================

int VInspector_ItemPrs3dAspect::initRowCount() const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return 0;

  Standard_CString anAspectKind = anAspect->DynamicType()->Name();
  if (anAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
    return 3 + 2 * (int) Prs3d_DP_None;
  else if (anAspectKind == STANDARD_TYPE (Prs3d_PlaneAspect)->Name())
    return 3;
  else if (anAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
    return 3;

  return 0;
}

// =======================================================================
// function : GetStream
// purpose :
// =======================================================================
void VInspector_ItemPrs3dAspect::GetStream (Standard_OStream& OS) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return;

  anAspect->Dump (OS);
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================

TreeModel_ItemBasePtr VInspector_ItemPrs3dAspect::createChild (int theRow, int theColumn)
{
  return VInspector_ItemPrs3dAspect::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemPrs3dAspect::Init()
{
  TCollection_AsciiString aName;
  Handle(Prs3d_BasicAspect) anAspect;
  VInspector_ItemPrs3dDrawerPtr aParentItem = itemDynamicCast<VInspector_ItemPrs3dDrawer>(Parent());

  if (aParentItem)
  {
    Standard_Boolean isOwnAspect;
    anAspect = aParentItem->GetPrs3dAspect(Row(), aName, isOwnAspect);
  }
  else
  {
    VInspector_ItemPrs3dAspectPtr aParentAspectItem = itemDynamicCast<VInspector_ItemPrs3dAspect>(Parent());
    Standard_Boolean isOwnAspect;
    anAspect = aParentAspectItem->GetPrs3dAspect(Row(), aName, isOwnAspect);
  }

  setAspect(anAspect, aName);
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemPrs3dAspect::Reset()
{
  VInspector_ItemBase::Reset();

  setAspect (NULL, TCollection_AsciiString());
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemPrs3dAspect::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemPrs3dAspect*>(this)->Init();
}

// =======================================================================
// function : GetPrs3dAspect
// purpose :
// =======================================================================

Handle(Prs3d_BasicAspect) VInspector_ItemPrs3dAspect::GetPrs3dAspect (const int theRow,
                                                                      TCollection_AsciiString& theName,
                                                                      Standard_Boolean& theOwnAspect) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return 0;

  Standard_CString anAspectKind = anAspect->DynamicType()->Name();
  theOwnAspect = true;
  if (anAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
  {
    Handle(Prs3d_DatumAspect) aCustomAspect = Handle(Prs3d_DatumAspect)::DownCast (anAspect);
    switch (theRow) {
      case 0: theName = "TextAspect"; return aCustomAspect->TextAspect();
      case 1: theName = "PointAspect"; return aCustomAspect->PointAspect();
      case 2: theName = "ArrowAspect"; return aCustomAspect->ArrowAspect();
      default:
      {
        if (theRow >= 3 && theRow < 3 + Prs3d_DP_None)
        {
          Prs3d_DatumParts aParts = (Prs3d_DatumParts)(theRow - 3);
          theName = TCollection_AsciiString ("LineAspect: ") + Prs3d::DatumPartsToString (aParts);
          return aCustomAspect->LineAspect (aParts);
        }

        if (theRow >= 3 + Prs3d_DP_None && theRow < 3 + 2 * Prs3d_DP_None)
        {
          Prs3d_DatumParts aParts = (Prs3d_DatumParts)(theRow - (3+ Prs3d_DP_None));
          theName = TCollection_AsciiString ("ShadingAspect: ") + Prs3d::DatumPartsToString (aParts);
          return aCustomAspect->ShadingAspect (aParts);
        }
      }
      break;
    }
  }
  else if (anAspectKind == STANDARD_TYPE (Prs3d_PlaneAspect)->Name())
  {
    Handle(Prs3d_PlaneAspect) aCustomAspect = Handle(Prs3d_PlaneAspect)::DownCast (anAspect);
    switch (theRow)
    {
      case 0: theName = "EdgesAspect"; return aCustomAspect->EdgesAspect();
      case 1: theName = "IsoAspect"; return aCustomAspect->IsoAspect();
      case 2: theName = "ArrowAspect"; return aCustomAspect->ArrowAspect();
    }
  }
  else if (anAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
  {
    Handle(Prs3d_DimensionAspect) aCustomAspect = Handle(Prs3d_DimensionAspect)::DownCast (anAspect);
    switch (theRow)
    {
      case 0: theName = "LineAspect"; return aCustomAspect->LineAspect();
      case 1: theName = "TextAspect"; return aCustomAspect->TextAspect();
      case 2: theName = "ArrowAspect"; return aCustomAspect->ArrowAspect();
    }
  }

  theName = "None";
  return Handle(Prs3d_BasicAspect)();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemPrs3dAspect::GetTableRowCount() const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return 0;

  return getTableRowCount (anAspect->DynamicType()->Name());
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType VInspector_ItemPrs3dAspect::GetTableEditType (const int theRow, const int) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return ViewControl_EditType_None;

  return getTableEditType (theRow, anAspect->DynamicType()->Name(), 0);
}

// =======================================================================
// function : GetTableEnumValues
// purpose :
// =======================================================================
QList<QVariant> VInspector_ItemPrs3dAspect::GetTableEnumValues (const int theRow, const int) const
{
  QList<QVariant> aValues;
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return aValues;

  return getTableEnumValues (theRow, anAspect->DynamicType()->Name(), 0);
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemPrs3dAspect::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return QVariant();

  return getTableData (theRow, theColumn, theRole, anAspect->DynamicType()->Name(), 0);
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool VInspector_ItemPrs3dAspect::SetTableData (const int theRow, const int, const QVariant& theValue)
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return false;

  return setTableData (theRow, anAspect->DynamicType()->Name(), 0, theValue);
}


// =======================================================================
// function : getTableRowCount
// purpose :
// =======================================================================

int VInspector_ItemPrs3dAspect::getTableRowCount (const TCollection_AsciiString& theAspectKind) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return 0;

  if (theAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
    return 3 + (Prs3d_DP_ShadingNumberOfFacettes + 1);
  else if (theAspectKind == STANDARD_TYPE (Prs3d_IsoAspect)->Name())
    return 1 + getTableRowCount (STANDARD_TYPE (Prs3d_LineAspect)->Name());
  else if (theAspectKind == STANDARD_TYPE (Prs3d_LineAspect)->Name())
    return 4; // TODO: add Graphic3d_ShaderProgram
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PointAspect)->Name())
    return 3; // TODO: add Graphic3d_ShaderProgram, Graphic3d_MarkerImage
  else if (theAspectKind == STANDARD_TYPE (Prs3d_TextAspect)->Name())
    return 15; // TODO: add Graphic3d_ShaderProgram
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PlaneAspect)->Name())
    return 10;
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ArrowAspect)->Name())
    return 2 + 3; // TODO: add Graphic3d_ShaderProgram
  else if (theAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
    return 10;
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ShadingAspect)->Name())
    return 17 + 2 * GetMaterialRows(); // TODO: Graphic3d_ShaderProgram, Graphic3d_TextureSet, Graphic3d_MaterialAspect

  return 0;
}

// =======================================================================
// function : getTableEditType
// purpose :
// =======================================================================
ViewControl_EditType VInspector_ItemPrs3dAspect::getTableEditType (const int theRow,
                                                                   const TCollection_AsciiString& theAspectKind,
                                                                   const int theStartRow) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return ViewControl_EditType_None;

  int aRow = theRow - theStartRow;
  if (theAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
  {
    switch (aRow)
    {
      case 0: return ViewControl_EditType_Combo;
      case 1: return ViewControl_EditType_Bool;
      case 2: return ViewControl_EditType_Bool;
      default:
      {
        if (theRow >= 3 && theRow <= 3 + Prs3d_DP_ShadingNumberOfFacettes)
          return ViewControl_EditType_Double;
        break;
      }
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_IsoAspect)->Name())
  {
    switch (aRow)
    {
      case 0:  return ViewControl_EditType_Spin;
      default: return getTableEditType (theRow, STANDARD_TYPE (Prs3d_LineAspect)->Name(), 1);
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_LineAspect)->Name())
  {
    switch (aRow)
    {
      case 0: return ViewControl_EditType_None;
      case 1: return ViewControl_EditType_Color;
      case 2: return ViewControl_EditType_Combo;
      case 3: return ViewControl_EditType_Double;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PointAspect)->Name())
  {
    switch (aRow)
    {
      case 0: return ViewControl_EditType_Color;
      case 1: return ViewControl_EditType_Combo;
      case 2: return ViewControl_EditType_Double;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_TextAspect)->Name())
  {
    switch (aRow)
    {
      case 0: return ViewControl_EditType_Double;
      case 1: return ViewControl_EditType_Double;
      case 2: return ViewControl_EditType_Combo;
      case 3: return ViewControl_EditType_Combo;
      case 4: return ViewControl_EditType_Combo;
      case 5: return ViewControl_EditType_Line;
      case 6: return ViewControl_EditType_Color;
      case 7: return ViewControl_EditType_Double;
      case 8: return ViewControl_EditType_Double;
      case 9: return ViewControl_EditType_Combo;
      case 10: return ViewControl_EditType_Combo;
      case 11: return ViewControl_EditType_Color;
      case 12: return ViewControl_EditType_Bool;
      case 13: return ViewControl_EditType_Double;
      case 14: return ViewControl_EditType_Combo;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PlaneAspect)->Name())
  {
    if (aRow >= 0 && aRow < 6 ) return ViewControl_EditType_Double;
    if (aRow >= 6 && aRow < 16 ) return ViewControl_EditType_Bool;
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ArrowAspect)->Name())
  {
    switch (aRow)
    {
      case 0: return ViewControl_EditType_Double;
      case 1: return ViewControl_EditType_Double;

      case 2: return ViewControl_EditType_Color;
      case 3: return ViewControl_EditType_Combo;
      case 4: return ViewControl_EditType_Double;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
  {
    switch (aRow)
    {
      case 0: return ViewControl_EditType_Line;
      case 1: return ViewControl_EditType_Double;
      case 2: return ViewControl_EditType_Double;

      case 3: return ViewControl_EditType_Combo;
      case 4: return ViewControl_EditType_Combo;
      case 5: return ViewControl_EditType_Combo;

      case 6: return ViewControl_EditType_Bool;
      case 7: return ViewControl_EditType_Bool;
      case 8: return ViewControl_EditType_Bool;
      case 9: return ViewControl_EditType_Bool;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ShadingAspect)->Name())
  {
    switch (aRow)
    {
      case 0: return ViewControl_EditType_Color;
      case 1: return ViewControl_EditType_Color;
      case 2: return ViewControl_EditType_Color;

      case 3: return ViewControl_EditType_Combo;
      case 4: return ViewControl_EditType_Combo;
      case 5: return ViewControl_EditType_Combo;

      case 6: return ViewControl_EditType_Double;
      case 7: return ViewControl_EditType_Combo;
      case 8: return ViewControl_EditType_Double;
      case 9: return ViewControl_EditType_Combo; // Graphic3d_HatchStyle is not modified
      case 10: return ViewControl_EditType_Combo;
      case 11: return ViewControl_EditType_Double;
      case 12: return ViewControl_EditType_Double;

      case 13: return ViewControl_EditType_Bool;
      case 14: return ViewControl_EditType_Bool;
      case 15: return ViewControl_EditType_Bool;
      case 16: return ViewControl_EditType_Bool;
      default: break;
    }

    Handle(Prs3d_ShadingAspect) aCustomAspect = Handle(Prs3d_ShadingAspect)::DownCast (anAspect);
    Handle(Graphic3d_AspectFillArea3d) aFillAreaAspect = aCustomAspect->Aspect();

    ViewControl_EditType aType = ViewControl_EditType_None;
    // front material
    aRow = aRow - 17;
    if (getTableEditTypeMaterial (aRow, aType))
      return aType;

    // back material
    aRow = aRow - GetMaterialRows();
    if (getTableEditTypeMaterial (aRow, aType))
      return aType;
  }

  return ViewControl_EditType_None;
}

// =======================================================================
// function : getTableEnumValues
// purpose :
// =======================================================================
QList<QVariant> VInspector_ItemPrs3dAspect::getTableEnumValues (const int theRow,
                                                                const TCollection_AsciiString& theAspectKind,
                                                                const int theStartRow) const
{
  QList<QVariant> aValues;
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return aValues;

  int aRow = theRow - theStartRow;
  if (theAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
  {
    Handle(Prs3d_DatumAspect) aCustomAspect = Handle(Prs3d_DatumAspect)::DownCast (anAspect);
    if (aRow == 0)
    {
      for (int i = 0; i <= Prs3d_DA_XYZAxis; i++)
        aValues.append (Prs3d::DatumAxesToString ((Prs3d_DatumAxes)i));
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_IsoAspect)->Name())
  {
    Handle(Prs3d_IsoAspect) aCustomAspect = Handle(Prs3d_IsoAspect)::DownCast (anAspect);
    if (aRow > 0)
      return getTableEnumValues (theRow, STANDARD_TYPE (Prs3d_LineAspect)->Name(), 1);
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_LineAspect)->Name())
  {
    if (aRow == 2)
    {
      for (int i = 0; i <= Aspect_TOL_USERDEFINED; i++)
        aValues.append (Aspect::TypeOfLineToString ((Aspect_TypeOfLine)i));
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PointAspect)->Name())
  {
    if (aRow == 1)
    {
      for (int i = Aspect_TOM_EMPTY; i <= Aspect_TOM_USERDEFINED; i++)
        aValues.append (Aspect::TypeOfMarkerToString ((Aspect_TypeOfMarker)i));
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_TextAspect)->Name())
  {
    if (theRow == 2)
    {
      for (int i = Graphic3d_HTA_LEFT; i <= Graphic3d_HTA_RIGHT; i++)
        aValues.append (Graphic3d::HorizontalTextAlignmentToString ((Graphic3d_HorizontalTextAlignment)i));
    }
    else if (theRow == 3)
    {
      for (int i = Graphic3d_VTA_BOTTOM; i <= Graphic3d_VTA_TOPFIRSTLINE; i++)
        aValues.append (Graphic3d::VerticalTextAlignmentToString ((Graphic3d_VerticalTextAlignment)i));
    }
    else if (theRow == 4)
    {
      for (int i = Graphic3d_TP_UP; i <= Graphic3d_TP_RIGHT; i++)
        aValues.append (Graphic3d::TextPathToString ((Graphic3d_TextPath)i));
    }
    else if (theRow == 9)
    {
      for (int i = Aspect_TOST_NORMAL; i <= Aspect_TOST_ANNOTATION; i++)
        aValues.append (Aspect::TypeOfStyleTextToString ((Aspect_TypeOfStyleText)i));
    }
    else if (theRow == 10)
    {
      for (int i = Aspect_TODT_NORMAL; i <= Aspect_TODT_SHADOW; i++)
        aValues.append (Aspect::TypeOfDisplayTextToString ((Aspect_TypeOfDisplayText)i));
    }
    else if (theRow == 14)
    {
      for (int i = Font_FA_Undefined; i <= Font_FA_BoldItalic; i++)
        aValues.append (Font::FontAspectToString ((Font_FontAspect)i));
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ArrowAspect)->Name())
  {
    if  (theRow == 0)
    {
      QList<QVariant> aValues;
      aValues << 0 << M_PI / 2.0 << 6;
      return aValues;
    }
    else if (aRow == 3)
    {
      for (int i = 0; i <= Aspect_TOL_USERDEFINED; i++)
        aValues.append (Aspect::TypeOfLineToString ((Aspect_TypeOfLine)i));
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
  {
    if (aRow == 3)
    {
      for (int i = 0; i <= Prs3d_DAO_Fit; i++)
        aValues.append (Prs3d::DimensionArrowOrientationToString ((Prs3d_DimensionArrowOrientation)i));
    }
    if (aRow == 4)
    {
      for (int i = 0; i <= Prs3d_DTHP_Fit; i++)
        aValues.append (Prs3d::DimensionTextHorizontalPositionToString ((Prs3d_DimensionTextHorizontalPosition)i));
    }
    if (aRow == 5)
    {
      for (int i = 0; i <= Prs3d_DTVP_Center; i++)
        aValues.append (Prs3d::DimensionTextVerticalPositionToString ((Prs3d_DimensionTextVerticalPosition)i));
    }
  }

  else if (theAspectKind == STANDARD_TYPE (Prs3d_ShadingAspect)->Name())
  {
    if (aRow == 3)
    {
      for (int i = 0; i <= Aspect_IS_POINT; i++)
        aValues.append (Aspect::InteriorStyleToString ((Aspect_InteriorStyle)i));
    }
    else if (aRow == 4)
    {
      for (int i = Graphic3d_TOSM_DEFAULT; i <= Graphic3d_TOSM_FRAGMENT; i++)
        aValues.append (Graphic3d::TypeOfShadingModelToString ((Graphic3d_TypeOfShadingModel)i));
    }
    else if (aRow == 5)
    {
      for (int i = Graphic3d_AlphaMode_BlendAuto; i <= Graphic3d_AlphaMode_Blend; i++)
        aValues.append (Graphic3d::AlphaModeToString ((Graphic3d_AlphaMode)i));
    }
    else if (aRow == 7)
    {
      for (int i = 0; i <= Aspect_TOL_USERDEFINED; i++)
        aValues.append (Aspect::TypeOfLineToString ((Aspect_TypeOfLine)i));
    }
    else if (aRow == 9) //Aspect_HatchStyle
    {
      for (int i = 0; i <= Aspect_HS_NB; i++)
        aValues.append (Aspect::HatchStyleToString ((Aspect_HatchStyle)i));
    }
    else if (aRow == 10) //Aspect_PolygonOffsetMode
    {
       aValues.append (Aspect::PolygonOffsetModeToString (Aspect_POM_Off));
       aValues.append (Aspect::PolygonOffsetModeToString (Aspect_POM_Fill));
       aValues.append (Aspect::PolygonOffsetModeToString (Aspect_POM_Line));
       aValues.append (Aspect::PolygonOffsetModeToString (Aspect_POM_Point));
       aValues.append (Aspect::PolygonOffsetModeToString (Aspect_POM_All));
       aValues.append (Aspect::PolygonOffsetModeToString (Aspect_POM_None));
       aValues.append (Aspect::PolygonOffsetModeToString (Aspect_POM_Mask));
    }
  }

  return aValues;
}

// =======================================================================
// function : getTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemPrs3dAspect::getTableData (const int theRow,
                                                   const int theColumn,
                                                   const int theRole,
                                                   const TCollection_AsciiString& theAspectKind,
                                                   const int theStartRow) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return QVariant();

  bool isFirstColumn = theColumn == 0;
  int aRow = theRow - theStartRow;

  if (theAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
  {
    if (theRole != Qt::DisplayRole)
      return QVariant();

    Handle(Prs3d_DatumAspect) aCustomAspect = Handle(Prs3d_DatumAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: return isFirstColumn ? QVariant ("DatumAxes") : QVariant (Prs3d::DatumAxesToString (aCustomAspect->DatumAxes()));
      case 1: return isFirstColumn ? QVariant ("DrawLabels") : QVariant (aCustomAspect->ToDrawLabels());
      case 2: return isFirstColumn ? QVariant ("DrawArrows") : QVariant (aCustomAspect->ToDrawArrows());
      default:
      {
        if (theRow >= 3 && theRow <= 3 + Prs3d_DP_ShadingNumberOfFacettes)
        {
          Prs3d_DatumAttribute aType = (Prs3d_DatumAttribute)(theRow - 3);
          return isFirstColumn ? QVariant (Prs3d::DatumAttributeToString (aType))
                               : ViewControl_Tools::ToVariant (aCustomAspect->Attribute (aType));
        }
        break;
      }
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_IsoAspect)->Name())
  {
    Handle(Prs3d_IsoAspect) aCustomAspect = Handle(Prs3d_IsoAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0:
      {
        if (theRole != Qt::DisplayRole) return QVariant();
        else return isFirstColumn ? QVariant ("Number") : QVariant (aCustomAspect->Number());
      }
      default: return getTableData (theRow, theColumn, theRole, STANDARD_TYPE (Prs3d_LineAspect)->Name(), 1);
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_LineAspect)->Name())
  {
    if (theRole != Qt::DisplayRole && theRole != Qt::BackgroundRole ||
        (theRole == Qt::BackgroundRole && (isFirstColumn || aRow != 1)))
      return QVariant();

    Handle(Prs3d_LineAspect) aCustomAspect = Handle(Prs3d_LineAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: return isFirstColumn ? QVariant ("Aspect") :
                                     QVariant (ViewControl_Tools::GetPointerInfo (aCustomAspect->Aspect()).ToCString());
      case 1: return getColorData("ColorRGBA", aCustomAspect->Aspect()->ColorRGBA(), isFirstColumn, theRole);
      case 2: return isFirstColumn ? QVariant ("Type")
                                   : QVariant (Aspect::TypeOfLineToString (aCustomAspect->Aspect()->Type()));
      case 3: return isFirstColumn ? QVariant ("Width") : ViewControl_Tools::ToVariant (aCustomAspect->Aspect()->Width());
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PointAspect)->Name())
  {
    if (theRole != Qt::DisplayRole && theRole != Qt::BackgroundRole ||
        (theRole == Qt::BackgroundRole && (isFirstColumn || aRow != 0)))
      return QVariant();

    Handle(Prs3d_PointAspect) aCustomAspect = Handle(Prs3d_PointAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: return getColorData("ColorRGBA", aCustomAspect->Aspect()->ColorRGBA(), isFirstColumn, theRole);
      case 1: return isFirstColumn ? QVariant ("Type")
                                  : QVariant (Aspect::TypeOfMarkerToString (aCustomAspect->Aspect()->Type()));
      case 2: return isFirstColumn ? QVariant ("Scale") : ViewControl_Tools::ToVariant (aCustomAspect->Aspect()->Scale());
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_TextAspect)->Name())
  {
    if (theRole != Qt::DisplayRole && theRole != Qt::BackgroundRole ||
       (theRole == Qt::BackgroundRole && (isFirstColumn || (aRow != 6 && theRow != 11))))
      return QVariant();

    Handle(Prs3d_TextAspect) aCustomAspect = Handle(Prs3d_TextAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: return isFirstColumn ? QVariant ("Angle") : ViewControl_Tools::ToVariant (aCustomAspect->Angle());
      case 1: return isFirstColumn ? QVariant ("Height") : ViewControl_Tools::ToVariant (aCustomAspect->Height());
      case 2: return isFirstColumn ? QVariant ("HorizontalJustification")
                                   : QVariant (Graphic3d::HorizontalTextAlignmentToString (aCustomAspect->HorizontalJustification()));
      case 3: return isFirstColumn ? QVariant ("VerticalJustification")
                                   : QVariant (Graphic3d::VerticalTextAlignmentToString (aCustomAspect->VerticalJustification()));
      case 4: return isFirstColumn ? QVariant ("Orientation") : QVariant (Graphic3d::TextPathToString (aCustomAspect->Orientation()));
      // Graphic3d_AspectText3d attributes
      case 5: return isFirstColumn ? QVariant ("Font") : QVariant (aCustomAspect->Aspect()->Font().ToCString());
      case 6: return getColorData("ColorRGBA", aCustomAspect->Aspect()->ColorRGBA(), isFirstColumn, theRole);
      //case 7: return isFirstColumn ? QVariant ("ExpansionFactor")
      //                             : ViewControl_Tools::ToVariant (aCustomAspect->Aspect()->ExpansionFactor());
      //case 8: return isFirstColumn ? QVariant ("Space")
      //                             : ViewControl_Tools::ToVariant (aCustomAspect->Aspect()->Space());
      case 9: return isFirstColumn ? QVariant ("Style")
                                   : QVariant (Aspect::TypeOfStyleTextToString (aCustomAspect->Aspect()->Style()));
      case 10: return isFirstColumn ? QVariant ("DisplayType")
                                    : QVariant (Aspect::TypeOfDisplayTextToString (aCustomAspect->Aspect()->DisplayType()));
      case 11: return getColorData("ColorSubTitleRGBA", aCustomAspect->Aspect()->ColorSubTitleRGBA(), isFirstColumn, theRole);
      case 12: return isFirstColumn ? QVariant ("TextZoomable") : QVariant (aCustomAspect->Aspect()->GetTextZoomable());
      case 13: return isFirstColumn ? QVariant ("TextAngle")
                                    : ViewControl_Tools::ToVariant (aCustomAspect->Aspect()->GetTextAngle());
      case 14: return isFirstColumn ? QVariant ("TextFontAspect")
                                    : QVariant (Font::FontAspectToString (aCustomAspect->Aspect()->GetTextFontAspect()));
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PlaneAspect)->Name())
  {
    if (theRole != Qt::DisplayRole)
      return QVariant();

    Handle(Prs3d_PlaneAspect) aCustomAspect = Handle(Prs3d_PlaneAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: return isFirstColumn ? QVariant ("ArrowsLength") : ViewControl_Tools::ToVariant (aCustomAspect->ArrowsLength());
      case 1: return isFirstColumn ? QVariant ("ArrowsSize") : ViewControl_Tools::ToVariant (aCustomAspect->ArrowsSize());
      case 2: return isFirstColumn ? QVariant ("ArrowsAngle") : ViewControl_Tools::ToVariant (aCustomAspect->ArrowsAngle());
      case 3: return isFirstColumn ? QVariant ("PlaneXLength") : ViewControl_Tools::ToVariant (aCustomAspect->PlaneXLength());
      case 4: return isFirstColumn ? QVariant ("PlaneYLength") : ViewControl_Tools::ToVariant (aCustomAspect->PlaneYLength());
      case 5: return isFirstColumn ? QVariant ("IsoDistance") : ViewControl_Tools::ToVariant (aCustomAspect->IsoDistance());

      case 6: return isFirstColumn ? QVariant ("DisplayCenterArrow") : aCustomAspect->DisplayCenterArrow();
      case 7: return isFirstColumn ? QVariant ("DisplayEdgesArrows") : aCustomAspect->DisplayEdgesArrows();
      case 8: return isFirstColumn ? QVariant ("DisplayEdges") : aCustomAspect->DisplayEdges();
      case 9: return isFirstColumn ? QVariant ("DisplayIso") : aCustomAspect->DisplayIso();
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ArrowAspect)->Name())
  {
    if (theRole != Qt::DisplayRole && theRole != Qt::BackgroundRole ||
        (theRole == Qt::BackgroundRole && (isFirstColumn || aRow != 2)))
      return QVariant();

    Handle(Prs3d_ArrowAspect) aCustomAspect = Handle(Prs3d_ArrowAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: return isFirstColumn ? QVariant ("Angle") : ViewControl_Tools::ToVariant (aCustomAspect->Angle());
      case 1: return isFirstColumn ? QVariant ("Length") : ViewControl_Tools::ToVariant (aCustomAspect->Length());
      case 2: return getColorData("ColorRGBA", aCustomAspect->Aspect()->ColorRGBA(), isFirstColumn, theRole);
      case 3: return isFirstColumn ? QVariant ("Type")
                                  : QVariant (Aspect::TypeOfLineToString (aCustomAspect->Aspect()->Type()));
      case 4: return isFirstColumn ? QVariant ("Width") : ViewControl_Tools::ToVariant (aCustomAspect->Aspect()->Width());
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
  {
    if (theRole != Qt::DisplayRole)
      return QVariant();

    Handle(Prs3d_DimensionAspect) aCustomAspect = Handle(Prs3d_DimensionAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: return isFirstColumn ? QVariant ("ValueStringFormat") : QVariant (aCustomAspect->ValueStringFormat().ToCString());
      case 1: return isFirstColumn ? QVariant ("ExtensionSize") : ViewControl_Tools::ToVariant (aCustomAspect->ExtensionSize());
      case 2: return isFirstColumn ? QVariant ("ArrowTailSize") : ViewControl_Tools::ToVariant (aCustomAspect->ArrowTailSize());

      case 3: return isFirstColumn ? QVariant ("ArrowOrientation")
                                  : QVariant (Prs3d::DimensionArrowOrientationToString (aCustomAspect->ArrowOrientation()));
      case 4: return isFirstColumn ? QVariant ("TextHorizontalPosition")
                                  : QVariant (Prs3d::DimensionTextHorizontalPositionToString (aCustomAspect->TextHorizontalPosition()));
      case 5: return isFirstColumn ? QVariant ("TextVerticalPosition")
                                  : QVariant (Prs3d::DimensionTextVerticalPositionToString (aCustomAspect->TextVerticalPosition()));

      case 6: return isFirstColumn ? QVariant ("IsUnitsDisplayed") : aCustomAspect->IsUnitsDisplayed();
      case 7: return isFirstColumn ? QVariant ("IsText3d") : aCustomAspect->IsText3d();
      case 8: return isFirstColumn ? QVariant ("IsTextShaded") : aCustomAspect->IsTextShaded();
      case 9: return isFirstColumn ? QVariant ("IsArrows3d") : aCustomAspect->IsArrows3d();

      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ShadingAspect)->Name())
  {
    if (theRole != Qt::DisplayRole && theRole != Qt::BackgroundRole ||
       (theRole == Qt::BackgroundRole && (isFirstColumn || (aRow != 0 && theRow != 1 && theRow != 2
                                                            && !isColorMaterialRow (theRow)))))
      return QVariant();

    Handle(Prs3d_ShadingAspect) aCustomAspect = Handle(Prs3d_ShadingAspect)::DownCast (anAspect);
    Handle(Graphic3d_AspectFillArea3d) anAspect = aCustomAspect->Aspect();
    switch (aRow)
    {
      case 0: return getColorData("InteriorColorRGBA", anAspect->InteriorColorRGBA(), isFirstColumn, theRole);
      case 1: return getColorData("BackInteriorColorRGBA", anAspect->BackInteriorColorRGBA(), isFirstColumn, theRole);
      case 2: return getColorData("EdgeColorRGBA", anAspect->EdgeColorRGBA(), isFirstColumn, theRole);
      case 3: return isFirstColumn ? QVariant ("InteriorStyle")
                                   : QVariant (Aspect::InteriorStyleToString (anAspect->InteriorStyle()));
      case 4: return isFirstColumn ? QVariant ("ShadingModel")
                                   : QVariant (Graphic3d::TypeOfShadingModelToString (anAspect->ShadingModel()));
      case 5: return isFirstColumn ? QVariant ("AlphaMode")
                                   : QVariant (Graphic3d::AlphaModeToString (anAspect->AlphaMode()));
      case 6: return isFirstColumn ? QVariant ("AlphaCutoff") : ViewControl_Tools::ToVariant (anAspect->AlphaCutoff());
      case 7: return isFirstColumn ? QVariant ("EdgeLineType")
                                   : QVariant (Aspect::TypeOfLineToString (anAspect->EdgeLineType()));
      case 8:  return isFirstColumn ? QVariant ("EdgeWidth") : ViewControl_Tools::ToVariant (anAspect->EdgeWidth());
      case 9: return isFirstColumn ? QVariant ("HatchStyle: Type")
                                   : (!anAspect->HatchStyle().IsNull() ? QVariant (Aspect::HatchStyleToString
                                                   ((Aspect_HatchStyle)anAspect->HatchStyle()->HatchType())) : QVariant());
      case 10: return isFirstColumn ? QVariant ("PolygonOffset: Mode")
                                   : QVariant (Aspect::PolygonOffsetModeToString (anAspect->PolygonOffset().Mode));
      case 11: return isFirstColumn ? QVariant ("PolygonOffset: Factor")
                                    : ViewControl_Tools::ToVariant (anAspect->PolygonOffset().Factor);
      case 12: return isFirstColumn ? QVariant ("PolygonOffset: Units")
                                    : ViewControl_Tools::ToVariant (anAspect->PolygonOffset().Units);
      case 13: return isFirstColumn ? QVariant ("Distinguish") : anAspect->Distinguish();
      case 14: return isFirstColumn ? QVariant ("ToDrawEdges") : anAspect->ToDrawEdges();
      case 15: return isFirstColumn ? QVariant ("ToSuppressBackFaces") : anAspect->ToSuppressBackFaces();
      case 16: return isFirstColumn ? QVariant ("ToMapTexture") : anAspect->ToMapTexture();
    default: break;
    }
    // front material
    aRow = aRow - 17;
    const Graphic3d_MaterialAspect& aFrontMaterial = anAspect->FrontMaterial();
    QVariant aValue = getMaterialValue (aRow, theColumn, theRole, aFrontMaterial, "FrontMaterial");
    if (aValue.isValid())
      return aValue;

    // back material
    aRow = aRow - GetMaterialRows();
    const Graphic3d_MaterialAspect& aBackMaterial = anAspect->BackMaterial();
    aValue = getMaterialValue (aRow, theColumn, theRole, aBackMaterial, "BackMaterial");
    if (aValue.isValid())
      return aValue;
  }
  return QVariant();
}

// =======================================================================
// function : setTableData
// purpose :
// =======================================================================
bool VInspector_ItemPrs3dAspect::setTableData (const int theRow,
                                               const TCollection_AsciiString& theAspectKind,
                                               const int theStartRow,
                                               const QVariant& theValue) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return false;

  int aRow = theRow - theStartRow;
  if (theAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
  {
    Handle(Prs3d_DatumAspect) aCustomAspect = Handle(Prs3d_DatumAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: aCustomAspect->SetDrawDatumAxes (Prs3d::DatumAxesFromString (theValue.toString().toStdString().c_str())); break;
      case 1: aCustomAspect->SetDrawLabels (theValue.toBool()); break;
      case 2: aCustomAspect->SetDrawArrows (theValue.toBool()); break;
      default:
      {
        if (theRow >= 3 && theRow <= 3 + Prs3d_DP_ShadingNumberOfFacettes)
        {
          Prs3d_DatumAttribute aType = (Prs3d_DatumAttribute)(theRow - 3);
          aCustomAspect->SetAttribute (aType, ViewControl_Tools::ToRealValue (theValue));
        }
        break;
      }
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_IsoAspect)->Name())
  {
    Handle(Prs3d_IsoAspect) aCustomAspect = Handle(Prs3d_IsoAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: aCustomAspect->SetNumber(theValue.toInt()); break;
      default: setTableData (theRow, STANDARD_TYPE (Prs3d_LineAspect)->Name(), 1, theValue); break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_LineAspect)->Name())
  {
    Handle(Prs3d_LineAspect) aCustomAspect = Handle(Prs3d_LineAspect)::DownCast (anAspect);
    Handle(Graphic3d_AspectLine3d) anAspect = aCustomAspect->Aspect();
    switch (aRow)
    {
      case 0: break;
      case 1: anAspect->SetColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
      case 2: anAspect->SetType (Aspect::TypeOfLineFromString (theValue.toString().toStdString().c_str())); break;
      case 3: anAspect->SetWidth (ViewControl_Tools::ToShortRealValue (theValue)); break;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PointAspect)->Name())
  {
    Handle(Prs3d_PointAspect) aCustomAspect = Handle(Prs3d_PointAspect)::DownCast (anAspect);
    Handle(Graphic3d_AspectMarker3d) anAspect = aCustomAspect->Aspect();
    switch (aRow)
    {
      case 0: anAspect->SetColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
      case 1: anAspect->SetType (Aspect::TypeOfMarkerFromString (theValue.toString().toStdString().c_str())); break;
      case 2: anAspect->SetScale (ViewControl_Tools::ToShortRealValue (theValue)); break;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_TextAspect)->Name())
  {
    Handle(Prs3d_TextAspect) aCustomAspect = Handle(Prs3d_TextAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: aCustomAspect->SetAngle (ViewControl_Tools::ToRealValue (theValue)); break;
      case 1: aCustomAspect->SetHeight (ViewControl_Tools::ToRealValue (theValue)); break;
      case 2: aCustomAspect->SetHorizontalJustification (Graphic3d::HorizontalTextAlignmentFromString
        (theValue.toString().toStdString().c_str())); break;
      case 3: aCustomAspect->SetVerticalJustification (Graphic3d::VerticalTextAlignmentFromString
        (theValue.toString().toStdString().c_str())); break;
      case 4: aCustomAspect->SetOrientation (Graphic3d::TextPathFromString (theValue.toString().toStdString().c_str())); break;
      // Graphic3d_AspectText3d attributes
      case 5: aCustomAspect->Aspect()->SetFont (theValue.toString().toStdString().c_str()); break;
      case 6: aCustomAspect->Aspect()->SetColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
      //case 7: aCustomAspect->Aspect()->SetExpansionFactor (ViewControl_Tools::ToRealValue (theValue)); break;
      //case 8: aCustomAspect->Aspect()->SetSpace (ViewControl_Tools::ToRealValue (theValue)); break;
      case 9: aCustomAspect->Aspect()->SetStyle (Aspect::TypeOfStyleTextFromString (theValue.toString().toStdString().c_str())); break;
      case 10: aCustomAspect->Aspect()->SetDisplayType (Aspect::TypeOfDisplayTextFromString (theValue.toString().toStdString().c_str())); break;
      case 11: aCustomAspect->Aspect()->SetColorSubTitle (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
      case 12: aCustomAspect->Aspect()->SetTextZoomable (theValue.toBool()); break;
      case 13: aCustomAspect->Aspect()->SetTextAngle (ViewControl_Tools::ToRealValue (theValue)); break;
      case 14: aCustomAspect->Aspect()->SetTextFontAspect (Font::FontAspectFromString (theValue.toString().toStdString().c_str())); break;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_PlaneAspect)->Name())
  {
    Handle(Prs3d_PlaneAspect) aCustomAspect = Handle(Prs3d_PlaneAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: aCustomAspect->SetArrowsLength (ViewControl_Tools::ToRealValue (theValue)); break;
      case 1: aCustomAspect->SetArrowsSize (ViewControl_Tools::ToRealValue (theValue)); break;
      case 2: aCustomAspect->SetArrowsAngle (ViewControl_Tools::ToRealValue (theValue)); break;
      case 3: aCustomAspect->SetPlaneLength (ViewControl_Tools::ToRealValue (theValue),
                                             aCustomAspect->PlaneYLength()); break;
      case 4: aCustomAspect->SetPlaneLength (aCustomAspect->PlaneXLength(),
                                             ViewControl_Tools::ToRealValue (theValue)); break;
      case 5: aCustomAspect->SetIsoDistance (ViewControl_Tools::ToRealValue (theValue)); break;
      case 6: aCustomAspect->SetDisplayCenterArrow (theValue.toBool()); break;
      case 7: aCustomAspect->SetDisplayEdgesArrows (theValue.toBool()); break;
      case 8: aCustomAspect->SetDisplayEdges (theValue.toBool()); break;
      case 9: aCustomAspect->SetDisplayIso (theValue.toBool()); break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ArrowAspect)->Name())
  {
    Handle(Prs3d_ArrowAspect) aCustomAspect = Handle(Prs3d_ArrowAspect)::DownCast (anAspect);
    Handle(Graphic3d_AspectLine3d) anAspect = aCustomAspect->Aspect();
    switch (aRow)
    {
      case 0: aCustomAspect->SetAngle (ViewControl_Tools::ToRealValue (theValue)); break;
      case 1: aCustomAspect->SetLength (ViewControl_Tools::ToRealValue (theValue)); break;

      case 2: anAspect->SetColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
      case 3: anAspect->SetType (Aspect::TypeOfLineFromString (theValue.toString().toStdString().c_str())); break;
      case 4: anAspect->SetWidth (ViewControl_Tools::ToShortRealValue (theValue)); break;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
  {
    Handle(Prs3d_DimensionAspect) aCustomAspect = Handle(Prs3d_DimensionAspect)::DownCast (anAspect);
    switch (aRow)
    {
      case 0: aCustomAspect->SetValueStringFormat (theValue.toString().toStdString().c_str()); break;
      case 1: aCustomAspect->SetExtensionSize (ViewControl_Tools::ToRealValue (theValue)); break;
      case 2: aCustomAspect->SetArrowTailSize (ViewControl_Tools::ToRealValue (theValue)); break;

      case 3: aCustomAspect->SetArrowOrientation (Prs3d::DimensionArrowOrientationFromString (
                                                  theValue.toString().toStdString().c_str())); break;
      case 4: aCustomAspect->SetTextHorizontalPosition (Prs3d::DimensionTextHorizontalPositionFromString (
                                                        theValue.toString().toStdString().c_str())); break;
      case 5: aCustomAspect->SetTextVerticalPosition (Prs3d::DimensionTextVerticalPositionFromString (
                                                      theValue.toString().toStdString().c_str())); break;
      case 6: aCustomAspect->MakeUnitsDisplayed (theValue.toBool()); break;
      case 7: aCustomAspect->MakeText3d (theValue.toBool()); break;
      case 8: aCustomAspect->MakeTextShaded (theValue.toBool()); break;
      case 9: aCustomAspect->MakeArrows3d (theValue.toBool()); break;
      default: break;
    }
  }
  else if (theAspectKind == STANDARD_TYPE (Prs3d_ShadingAspect)->Name())
  {
    Handle(Prs3d_ShadingAspect) aCustomAspect = Handle(Prs3d_ShadingAspect)::DownCast (anAspect);
    Handle(Graphic3d_AspectFillArea3d) anAspect = aCustomAspect->Aspect();
    switch (aRow)
    {
      case 0: anAspect->SetInteriorColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
      case 1: anAspect->SetBackInteriorColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
      case 2: anAspect->SetEdgeColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
      case 3: anAspect->SetInteriorStyle (Aspect::InteriorStyleFromString (
                                          theValue.toString().toStdString().c_str())); break;
      case 4: anAspect->SetShadingModel (Graphic3d::TypeOfShadingModelFromString (
                                         theValue.toString().toStdString().c_str())); break;
      case 5:
      case 6:
      {
        Graphic3d_AlphaMode aMode = anAspect->AlphaMode();
        Standard_ShortReal aCutoff = anAspect->AlphaCutoff();
        if (aRow == 5)
          aMode = Graphic3d::AlphaModeFromString (theValue.toString().toStdString().c_str());
        else
          aCutoff = ViewControl_Tools::ToShortRealValue (theValue);
        anAspect->SetAlphaMode (aMode, aCutoff);
        break;
      }
      case 7: anAspect->SetEdgeLineType (Aspect::TypeOfLineFromString (
                                         theValue.toString().toStdString().c_str())); break;
      case 8: anAspect->SetEdgeWidth (ViewControl_Tools::ToShortRealValue (theValue)); break;
      case 9: break; // Graphic3d_HatchStyle is not modified

      case 10:
      case 11:
      case 12:
      {
        Standard_Integer aMode;
        Standard_ShortReal aFactor, aUnits;
        anAspect->PolygonOffsets (aMode, aFactor, aUnits);
        if (aRow == 10)
          aMode = Aspect::PolygonOffsetModeFromString (theValue.toString().toStdString().c_str());
        else if (aRow == 11)
          aFactor = ViewControl_Tools::ToShortRealValue (theValue);
        else if (aRow == 12)
          aUnits = ViewControl_Tools::ToShortRealValue (theValue);

        anAspect->SetPolygonOffsets (aMode, aFactor, aUnits);
        break;
      }
      case 13: anAspect->SetDistinguish (theValue.toBool()); break;
      case 14: anAspect->SetDrawEdges (theValue.toBool()); break;
      case 15: anAspect->SetSuppressBackFaces (theValue.toBool()); break;
      case 16: anAspect->SetTextureMapOn (theValue.toBool()); break;
      default: break;
    }

    // front material
    aRow = aRow - 17;
    Graphic3d_MaterialAspect& aFrontMaterial = anAspect->ChangeFrontMaterial();
    if (setMaterialValue (aRow, aFrontMaterial, theValue))
      return true;

    // back material
    aRow = aRow - GetMaterialRows();
    Graphic3d_MaterialAspect& aBackMaterial = anAspect->ChangeBackMaterial();
    if (setMaterialValue (aRow, aBackMaterial, theValue))
      return true;
  }
  return true;
}

// =======================================================================
// function : isColorMaterialRow
// purpose :
// =======================================================================
Standard_Boolean VInspector_ItemPrs3dAspect::isColorMaterialRow (const int theRow) const
{
  // front material
  int aRow = theRow - 17;
  if (aRow >= 8 && aRow <= 12)
    return Standard_True;

  // back material
  aRow = aRow - GetMaterialRows();
  if (aRow >= 8 && aRow <= 12)
    return Standard_True;

  return Standard_False;
}

// =======================================================================
// function : getTableEditTypeMaterial
// purpose :
// =======================================================================
Standard_Boolean VInspector_ItemPrs3dAspect::getTableEditTypeMaterial (const int theRow,
                                                                       ViewControl_EditType& theType) const
{
  theType = ViewControl_EditType_None;

  switch (theRow)
  {
    case 6:
    case 7: theType = ViewControl_EditType_Line; break;

    case 8:
    case 9:
    case 10:
    case 11:
    case 12: theType = ViewControl_EditType_Color; break;

    case 13:
    case 14:
    case 15:
    case 16:
    case 17: theType = ViewControl_EditType_Line; break;

    case 18:
    case 19:
    case 20:
    case 21: theType = ViewControl_EditType_Bool; break;

    default: break;
  }

  return theRow < GetMaterialRows();
}

// =======================================================================
// function : getMaterialValue
// purpose :
// =======================================================================
QVariant VInspector_ItemPrs3dAspect::getMaterialValue (const int theRow,
                                                       const int theColumn,
                                                       const int theRole,
                                                       const Graphic3d_MaterialAspect& theMaterial,
                                                       const TCollection_AsciiString& theInfo) const
{
  bool isFirstColumn = theColumn == 0;

  if ((theRole != Qt::DisplayRole && theRole != Qt::BackgroundRole) ||
      (theRole == Qt::BackgroundRole && (isFirstColumn || (theRow < 8 || theRow > 12))))
    return QVariant();

  switch (theRow)
  {
    case 0: return ViewControl_Tools::TableSeparator();
    case 1: return isFirstColumn ? QVariant (theInfo.ToCString()) : QVariant ("");
    case 2: return ViewControl_Tools::TableSeparator();
    
    case 3: return isFirstColumn ? QVariant ("Name") : QVariant (theMaterial.Name()); //TODO
    case 4: return isFirstColumn ? QVariant ("RequestedName") : QVariant (theMaterial.RequestedName()); // TODO
    case 5: return isFirstColumn ? QVariant ("MaterialName") : QVariant (theMaterial.MaterialName());

    case 6: return isFirstColumn ? QVariant ("Transparency") : QVariant (theMaterial.Transparency());
    case 7: return isFirstColumn ? QVariant ("Alpha") : QVariant (theMaterial.Alpha());

    case 8: return getColorData("Color", Quantity_ColorRGBA (theMaterial.Color()), isFirstColumn, theRole);
    case 9: return getColorData("AmbientColor", Quantity_ColorRGBA (theMaterial.AmbientColor()), isFirstColumn, theRole);
    case 10: return getColorData("DiffuseColor", Quantity_ColorRGBA (theMaterial.DiffuseColor()), isFirstColumn, theRole);
    case 11: return getColorData("SpecularColor", Quantity_ColorRGBA (theMaterial.SpecularColor()), isFirstColumn, theRole);
    case 12: return getColorData("EmissiveColor", Quantity_ColorRGBA (theMaterial.EmissiveColor()), isFirstColumn, theRole);

    case 13: return isFirstColumn ? QVariant ("Ambient") : QVariant (theMaterial.Ambient());
    case 14: return isFirstColumn ? QVariant ("Diffuse") : QVariant (theMaterial.Diffuse());
    case 15: return isFirstColumn ? QVariant ("Specular") : QVariant (theMaterial.Specular());
    case 16: return isFirstColumn ? QVariant ("Emissive") : QVariant (theMaterial.Emissive());

    case 17: return isFirstColumn ? QVariant ("Shininess") : QVariant (theMaterial.Shininess());

    case 18: return isFirstColumn ? QVariant ("ReflectionMode (Ambient)") : QVariant (theMaterial.ReflectionMode (Graphic3d_TOR_AMBIENT));
    case 19: return isFirstColumn ? QVariant ("ReflectionMode (Diffuse)") : QVariant (theMaterial.ReflectionMode (Graphic3d_TOR_DIFFUSE));
    case 20: return isFirstColumn ? QVariant ("ReflectionMode (Specular)") : QVariant (theMaterial.ReflectionMode (Graphic3d_TOR_SPECULAR));
    case 21: return isFirstColumn ? QVariant ("ReflectionMode (Emissive)") : QVariant (theMaterial.ReflectionMode (Graphic3d_TOR_EMISSION));

    case 22: return isFirstColumn ? QVariant ("RefractionIndex") : QVariant (theMaterial.RefractionIndex());
    case 23: return isFirstColumn ? QVariant ("BSDF.FresnelCoat.FresnelType()") : QVariant (theMaterial.BSDF().FresnelCoat.FresnelType()); // TODO
    case 24: return isFirstColumn ? QVariant ("BSDF.FresnelBase.FresnelType()") : QVariant (theMaterial.BSDF().FresnelBase.FresnelType()); // TODO
  }
  return QVariant();
}

// =======================================================================
// function : setMaterialValue
// purpose :
// =======================================================================
Standard_Boolean VInspector_ItemPrs3dAspect::setMaterialValue (const int theRow,
                                                               Graphic3d_MaterialAspect& theMaterial,
                                                               const QVariant& theValue) const
{
  switch (theRow)
  {
    case 6: theMaterial.SetTransparency (ViewControl_Tools::ToShortRealValue (theValue)); break;
    case 7: theMaterial.SetAlpha (ViewControl_Tools::ToShortRealValue (theValue)); break;

    case 8: theMaterial.SetColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
    case 9: theMaterial.SetAmbientColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
    case 10: theMaterial.SetDiffuseColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
    case 11: theMaterial.SetSpecularColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;
    case 12: theMaterial.SetEmissiveColor (ViewControl_ColorSelector::StringToColor (theValue.toString())); break;

    case 13: theMaterial.SetAmbient (ViewControl_Tools::ToShortRealValue (theValue)); break;
    case 14: theMaterial.SetDiffuse (ViewControl_Tools::ToShortRealValue (theValue)); break;
    case 15: theMaterial.SetSpecular (ViewControl_Tools::ToShortRealValue (theValue)); break;
    case 16: theMaterial.SetEmissive (ViewControl_Tools::ToShortRealValue (theValue)); break;
    case 17: theMaterial.SetShininess (ViewControl_Tools::ToShortRealValue (theValue)); break;

    case 18: theMaterial.SetReflectionMode (Graphic3d_TOR_AMBIENT, theValue.toBool()); break;
    case 19: theMaterial.SetReflectionMode (Graphic3d_TOR_DIFFUSE, theValue.toBool()); break;
    case 20: theMaterial.SetReflectionMode (Graphic3d_TOR_SPECULAR, theValue.toBool()); break;
    case 21: theMaterial.SetReflectionMode (Graphic3d_TOR_EMISSION, theValue.toBool()); break;
  }
  return theRow < GetMaterialRows();
}

// =======================================================================
// function : getColorData
// purpose :
// =======================================================================
QVariant VInspector_ItemPrs3dAspect::getColorData(const TCollection_AsciiString& theInfo,
                                                  const Quantity_ColorRGBA& theColor,
                                                  const Standard_Boolean isFirstColumn,
                                                  const int theRole) const
{
  if (isFirstColumn)
    return theInfo.ToCString();

  if (theRole == Qt::BackgroundRole)
    return ViewControl_ColorSelector::ColorToQColor (theColor);

  else if (theRole == Qt::DisplayRole)
    return ViewControl_ColorSelector::ColorToString (theColor);

  return QVariant();
}

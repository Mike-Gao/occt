// Created on: 2017-06-16
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

#include <inspector/VInspector_ItemPresentableObject.hxx>

#include <AIS.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Aspect.hxx>

#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemSelectBasicsEntityOwner.hxx>
#include <inspector/VInspector_ItemFolderObject.hxx>
#include <inspector/VInspector_ItemPresentations.hxx>
#include <inspector/VInspector_ItemSelectMgrSelection.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>

#include <inspector/ViewControl_ColorSelector.hxx>
#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Graphic3d.hxx>
#include <NCollection_List.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <PrsMgr.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <Standard_Version.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <QItemSelectionModel>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemPresentableObject::initValue (int theItemRole) const
{
  if (theItemRole == Qt::DisplayRole || theItemRole == Qt::ToolTipRole)
  {
    Handle(AIS_InteractiveObject) anIO = GetInteractiveObject();
    bool aNullIO = anIO.IsNull();
    switch (Column())
    {
      case 0:
      {
        if (aNullIO)
          return theItemRole == Qt::ToolTipRole ? QVariant ("Owners where Selectable is empty")
                                                : QVariant ("Free Owners");
        else
          return theItemRole == Qt::ToolTipRole ? QVariant ("")
                                                : QVariant (anIO->DynamicType()->Name());
      }
      case 1:
        return rowCount();
      case 2:
      {
        if (!aNullIO)
          return VInspector_Tools::GetPointerInfo (anIO, true).ToCString();
        break;
      }
      case 4:
      {
        int aNbSelected = VInspector_Tools::SelectedOwners (GetContext(), anIO, false);
        return aNbSelected > 0 ? QString::number (aNbSelected) : "";
      }
      case 6:
      {
        double aDeviationCoefficient = 0;
        Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast (anIO);
        if (!anAISShape.IsNull())
        {
          Standard_Real aPreviousCoefficient;
          anAISShape->OwnDeviationCoefficient(aDeviationCoefficient, aPreviousCoefficient);
        }
        return QString::number(aDeviationCoefficient);
      }
      case 8:
      {
        double aDeviationCoefficient = 0;
        Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast (anIO);
        if (!anAISShape.IsNull())
        {
          Standard_Real aPreviousCoefficient;
          anAISShape->OwnDeviationCoefficient(aDeviationCoefficient, aPreviousCoefficient);
        }
        Handle(AIS_Shape) aShapeIO = Handle(AIS_Shape)::DownCast (anIO);
        bool anIsAutoTriangulation = aNullIO ? false : anIO->Attributes()->IsAutoTriangulation();
        return anIsAutoTriangulation ? QString ("true") : QString ("false");
      }
      default: break;
    }
  }
  if (theItemRole == Qt::BackgroundRole || theItemRole == Qt::ForegroundRole)
  {
    Handle(AIS_InteractiveContext) aContext = GetContext();
    if (Column() == 2 && VInspector_Tools::SelectedOwners(aContext, GetInteractiveObject(), false) > 0)
    {
      return (theItemRole == Qt::BackgroundRole) ? QColor(Qt::darkBlue) : QColor(Qt::white);
    }
    else if (theItemRole == Qt::ForegroundRole)
    {
      Handle(AIS_InteractiveObject) anIO = GetInteractiveObject();
      if (anIO.IsNull())
        return QVariant();

      AIS_ListOfInteractive aListOfIO;
      GetContext()->ErasedObjects(aListOfIO);
      for (AIS_ListIteratorOfListOfInteractive anIOIt(aListOfIO); anIOIt.More(); anIOIt.Next())
      {
        if (anIO == anIOIt.Value())
          return QColor(Qt::darkGray);
      }
      return QColor(Qt::black);
    }
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemPresentableObject::initRowCount() const
{
  if (Column() != 0)
    return 0;

  int aNbProperties = 2; // "Properties", "Presentations"

  Handle(AIS_InteractiveObject) anIO = GetInteractiveObject();
#if OCC_VERSION_HEX < 0x070201
  int aRows = 0;
  if (anIO.IsNull())
    return aRows;
  // iteration through sensitive privitives
  for (anIO->Init(); anIO->More(); anIO->Next())
    aRows++;
  int aNbSelected = aRows;
#else
  int aNbSelected = !anIO.IsNull() ? anIO->Selections().Size() : 0;
#endif

  return aNbProperties + aNbSelected;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemPresentableObject::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemFolderObject::CreateItem (currentItem(), theRow, theColumn);
  if (theRow == 1)
    return VInspector_ItemPresentations::CreateItem (currentItem(), theRow, theColumn);
  else
    return VInspector_ItemSelectMgrSelection::CreateItem(currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::Init()
{
  VInspector_ItemContextPtr aParentItem = itemDynamicCast<VInspector_ItemContext>(Parent());
  Handle(AIS_InteractiveContext) aContext = aParentItem->GetContext();
  SetContext (aContext);

  Handle(AIS_InteractiveObject) anIO;
  if (!GetContext().IsNull())
  {
    int aRowId = Row();
    AIS_ListOfInteractive aListOfIO;
    GetContext()->DisplayedObjects (aListOfIO); // the presentation is in displayed objects of Context
    GetContext()->ErasedObjects (aListOfIO); // the presentation is in erased objects of Context
    int aDeltaIndex = 1; // properties item
    int aCurrentIndex = 0;
    for (AIS_ListIteratorOfListOfInteractive anIOIt (aListOfIO); anIOIt.More(); anIOIt.Next(), aCurrentIndex++)
    {
      if (aCurrentIndex != aRowId - aDeltaIndex)
        continue;
      anIO = anIOIt.Value();
      break;
    }
  }

  setInteractiveObject (anIO);
  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::Reset()
{
  VInspector_ItemBase::Reset();

  SetContext (NULL);
  setInteractiveObject (NULL);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemPresentableObject*>(this)->Init();
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemPresentableObject::buildPresentationShape()
{
  Handle(AIS_InteractiveObject) aPrs = myIO;
  if (aPrs.IsNull())
    return TopoDS_Shape();

  Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast (aPrs);
  if (!aShapePrs.IsNull())
    return aShapePrs->Shape();

  return TopoDS_Shape();
}

// =======================================================================
// function : PointerInfo
// purpose :
// =======================================================================
QString VInspector_ItemPresentableObject::PointerInfo() const
{
  return VInspector_Tools::GetPointerInfo (GetInteractiveObject(), true).ToCString();
}

// =======================================================================
// function : GetPresentations
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::GetPresentations (NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  if (Column() != 0)
    return;

  thePresentations.Append (GetInteractiveObject());
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemPresentableObject::GetTableRowCount() const
{
  return 36;
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType VInspector_ItemPresentableObject::GetTableEditType (const int theRow, const int) const
{
  switch (theRow)
  {
    case 3: return ViewControl_EditType_Combo;
    case 6: return ViewControl_EditType_Combo;
    case 8: return ViewControl_EditType_Bool;
    case 13: return ViewControl_EditType_Color;
    case 15: return ViewControl_EditType_Line;
    case 17: return ViewControl_EditType_Combo;
    case 18: return ViewControl_EditType_Line;

    case 29: return ViewControl_EditType_Combo;
    case 30: return ViewControl_EditType_Bool;
    case 32: return ViewControl_EditType_Combo;
    case 35: return ViewControl_EditType_Bool;
    default: return ViewControl_EditType_None;
  }
}

// =======================================================================
// function : GetTableEnumValues
// purpose :
// =======================================================================
QList<QVariant> VInspector_ItemPresentableObject::GetTableEnumValues (const int theRow, const int) const
{
  QList<QVariant> aValues;
  switch (theRow)
  {
    case 3:
    {
      for (int i = 0; i <= AIS_KOI_Dimension; i++)
        aValues.append (AIS::KindOfInteractiveToString((AIS_KindOfInteractive)i));
    }
    break;
    case 6:
    {
      for (int i = 0; i <= Aspect_TOFM_FRONT_SIDE; i++)
        aValues.append (Aspect::TypeOfFacingModelToString((Aspect_TypeOfFacingModel)i));
    }
    break;
    case 17:
    {
      for (int i = 0; i <= Graphic3d_NOM_UserDefined; i++)
        aValues.append (Graphic3d::NameOfMaterialToString ((Graphic3d_NameOfMaterial)i));
    }
    break;
    case 29:
    {
      for (int i = 0; i <= PrsMgr_TOP_ProjectorDependant; i++)
        aValues.append (PrsMgr::TypeOfPresentation3dToString ((PrsMgr_TypeOfPresentation3d)i));
    }
    break;
    case 32:
    {
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_UNKNOWN));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Default));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Top));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Topmost));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_TopOSD));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_BotOSD));
    }
    break;
    default: break;
  }
  return aValues;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemPresentableObject::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  bool isFirstColumn = theColumn == 0;
  if (theRole != Qt::DisplayRole && theRole != Qt::BackgroundRole ||
      (theRole == Qt::BackgroundRole && (isFirstColumn || theRow != 13)))
    return QVariant();

  Handle(AIS_InteractiveObject) aPrs = GetInteractiveObject();
  switch (theRow)
  {
    case 0: return ViewControl_Table::SeparatorData();
    case 1: return isFirstColumn ? QVariant (STANDARD_TYPE (AIS_InteractiveObject)->Name())
                                 : ViewControl_Tools::GetPointerInfo (aPrs).ToCString();
    case 2: return ViewControl_Table::SeparatorData();
    case 3: return isFirstColumn ? QVariant("Type")
                                 : QVariant (AIS::KindOfInteractiveToString (aPrs->Type()));
    case 4: return isFirstColumn ? QVariant("Signature") : QVariant (aPrs->Signature());

    case 5: return isFirstColumn ? QVariant("AcceptShapeDecomposition") : QVariant (aPrs->AcceptShapeDecomposition());
    case 6: return isFirstColumn ? QVariant ("CurrentFacingModel")
                                 : QVariant (Aspect::TypeOfFacingModelToString (aPrs->CurrentFacingModel()));
    case 7: return isFirstColumn ? QVariant ("DefaultDisplayMode") : QVariant (aPrs->DefaultDisplayMode());
    case 8: return isFirstColumn ? QVariant ("IsInfinite") : QVariant (aPrs->IsInfinite());
    case 9: return isFirstColumn ? QVariant ("Owner")
      : (aPrs->GetOwner().IsNull() ? QVariant("") : ViewControl_Tools::GetPointerInfo (aPrs).ToCString());
    case 10: return isFirstColumn ? QVariant ("DisplayMode") : QVariant (aPrs->DisplayMode());
    case 11: return isFirstColumn ? QVariant ("HilightMode") : QVariant (aPrs->HilightMode());

    case 12: return isFirstColumn ? QVariant ("HasColor") : QVariant (aPrs->HasColor());
    case 13:
    {
      if (isFirstColumn)
        return QVariant ("Color");
      Quantity_Color aColor;
      aPrs->Color(aColor);
      return ViewControl_ColorSelector::ColorToQColor (aColor);
    }
    case 14: return isFirstColumn ? QVariant ("HasWidth") : QVariant (aPrs->HasWidth());
    case 15: return isFirstColumn ? QVariant ("Width") : QVariant (aPrs->Width());
    case 16: return isFirstColumn ? QVariant ("HasMaterial") : QVariant (aPrs->HasMaterial());
    case 17: return isFirstColumn ? QVariant ("Material")
      : QVariant (Graphic3d::NameOfMaterialToString (aPrs->Material()));
    case 18: return isFirstColumn ? QVariant ("Transparency") : QVariant (aPrs->Transparency());
    case 19:
    {
      if (isFirstColumn)
        return QVariant ("PolygonOffsets");
      Standard_Integer aMode;
      Standard_ShortReal aFactor, aUnits;
      aPrs->PolygonOffsets (aMode, aFactor, aUnits);
      return QString("Mode: %1, Factor: %2, Units: %3").arg (aMode).arg (aFactor).arg (aUnits);
    }
    case 20: return ViewControl_Table::SeparatorData();
    case 21: return isFirstColumn ? QVariant (STANDARD_TYPE (SelectMgr_SelectableObject)->Name())
                                 : ViewControl_Tools::GetPointerInfo (aPrs).ToCString();
    case 22: return ViewControl_Table::SeparatorData();
    case 23: return isFirstColumn ? QVariant ("IsAutoHilight") : QVariant (aPrs->IsAutoHilight());
    case 24: return isFirstColumn ? QVariant ("GlobalSelectionMode") : QVariant (aPrs->GlobalSelectionMode());
    case 25:
    {
      if (isFirstColumn)
        return QVariant ("BoundingBox");
      Bnd_Box aBndBox;
      aPrs->BoundingBox (aBndBox);
      return ViewControl_Tools::ToString (aBndBox).ToCString();
    }
    case 26: return ViewControl_Table::SeparatorData();
    case 27: return isFirstColumn ? QVariant (STANDARD_TYPE (PrsMgr_PresentableObject)->Name())
                                 : ViewControl_Tools::GetPointerInfo (aPrs).ToCString();
    case 28: return ViewControl_Table::SeparatorData();
    case 29: return isFirstColumn ? QVariant ("TypeOfPresentation3d")
                                  : QVariant (PrsMgr::TypeOfPresentation3dToString (aPrs->TypeOfPresentation3d()));
    case 30: return isFirstColumn ? QVariant ("IsMutable") : QVariant (aPrs->IsMutable());
    case 31: return isFirstColumn ? QVariant ("HasOwnPresentations") : QVariant (aPrs->HasOwnPresentations());
    case 32: return isFirstColumn ? QVariant ("ZLayer") : QVariant (Graphic3d::ZLayerIdToString (aPrs->ZLayer()));
    case 33: return isFirstColumn ? QVariant ("TransformationGeom")
      : (!aPrs->TransformationGeom().IsNull()
        ? QVariant (ViewControl_Tools::ToString(aPrs->TransformationGeom()->Trsf()).ToCString()) : QVariant());
    case 34: return isFirstColumn ? QVariant ("LocalTransformationGeom")
      : (!aPrs->LocalTransformationGeom().IsNull()
        ? QVariant (ViewControl_Tools::ToString(aPrs->LocalTransformationGeom()->Trsf()).ToCString()) : QVariant());
    case 35: return isFirstColumn ? QVariant ("ResetTransformation") : (!aPrs->LocalTransformationGeom().IsNull());
    default: return QVariant();
  }
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool VInspector_ItemPresentableObject::SetTableData (const int theRow, const int, const QVariant& theValue)
{
  Handle(AIS_InteractiveObject) aPrs = GetInteractiveObject();
  switch (theRow)
  {
    case 6: aPrs->SetCurrentFacingModel (Aspect::TypeOfFacingModelFromString (theValue.toString().toStdString().c_str())); break;
    case 8: aPrs->SetInfiniteState (theValue.toBool()); break;
    case 15:
    {
      double aValue = theValue.toDouble();
      if (aValue > 0) aPrs->SetWidth (aValue);
      else aPrs->UnsetWidth();
    }
    break;
    case 13:
    {
      float anAlpha;
      aPrs->SetColor (ViewControl_ColorSelector::StringToColor (theValue.toString(), anAlpha));
    }
    break;
    case 17: aPrs->SetMaterial (Graphic3d::NameOfMaterialFromString (theValue.toString().toStdString().c_str())); break;
    case 18:
    {
      double aValue = theValue.toDouble();
      if (aValue > 0 && aValue < 1)
        aPrs->SetTransparency(theValue.toDouble());
    }
    break;
    case 23: aPrs->SetAutoHilight(theValue.toBool()); break;
    case 29: aPrs->SetTypeOfPresentation (PrsMgr::TypeOfPresentation3dFromString (theValue.toString().toStdString().c_str())); break;
    case 30: aPrs->SetMutable (theValue.toBool()); break;
    case 32: aPrs->SetZLayer (Graphic3d::ZLayerIdFromString (theValue.toString().toStdString().c_str())); break;
    case 35: if (!theValue.toBool()) aPrs->ResetTransformation(); break;
    default: return false;
  }
  return true;
}

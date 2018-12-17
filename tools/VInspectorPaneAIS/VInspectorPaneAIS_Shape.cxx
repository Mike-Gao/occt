// Created on: 2018-12-08
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

#include <inspector/VInspectorPaneAIS_Shape.hxx>

//#include <AIS_Shape.hxx>
//#include <AIS_ListOfInteractive.hxx>
//#include <AIS_ListIteratorOfListOfInteractive.hxx>
//#include <Aspect.hxx>
//
//#include <inspector/VInspector_ItemContext.hxx>
//#include <inspector/VInspector_ItemSelectBasicsEntityOwner.hxx>
//#include <inspector/VInspector_ItemFolderObject.hxx>
//#include <inspector/VInspector_ItemPresentations.hxx>
//#include <inspector/VInspector_ItemSelectMgrSelection.hxx>
#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>
#include <inspector/VInspector_Tools.hxx>
//#include <inspector/VInspector_ViewModel.hxx>

#include <inspector/ViewControl_PaneItem.hxx>

#include <Prs3d.hxx>
#include <TopAbs.hxx>
//
//#include <Graphic3d.hxx>
//#include <NCollection_List.hxx>
//#include <Prs3d.hxx>
//#include <Prs3d_Drawer.hxx>
//#include <PrsMgr.hxx>
//#include <SelectBasics_EntityOwner.hxx>
//#include <StdSelect_BRepOwner.hxx>
//#include <Standard_Version.hxx>
//
//#include <Standard_WarningsDisable.hxx>
//#include <QColor>
//#include <QItemSelectionModel>
//#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspectorPaneAIS_Shape::GetTableRowCount(const Handle(Standard_Transient)& theObject) const
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return 0;

  return 8;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspectorPaneAIS_Shape::GetTableData (const Handle(Standard_Transient)& theObject,
                                                const int theRow, const int theColumn, const int theRole) const
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return QVariant();

  if (theRole != Qt::DisplayRole)
    return QVariant();

  bool isFirstColumn = theColumn == 0;
  switch (theRow)
  {
    case 0: return ViewControl_Table::SeparatorData();
    case 1: return isFirstColumn ? QVariant (STANDARD_TYPE (AIS_Shape)->Name())
                                 : ViewControl_Tools::GetPointerInfo (aPrs).ToCString();
    case 2: return ViewControl_Table::SeparatorData();
    case 3: return isFirstColumn ? QVariant ("TShape")
      : (aPrs->Shape().IsNull() ? QVariant ("") : ViewControl_Tools::GetPointerInfo (aPrs->Shape().TShape()).ToCString());
    case 4: return isFirstColumn ? QVariant ("ShapeType")
      : (aPrs->Shape().IsNull() ? QVariant ("") : TopAbs::ShapeTypeToString (aPrs->Shape().ShapeType()));
    case 5: return isFirstColumn ? QVariant ("Orientation")
      : (aPrs->Shape().IsNull() ? QVariant ("") : VInspector_Tools::OrientationToName (aPrs->Shape().Orientation()).ToCString());
    case 6: return isFirstColumn ? QVariant ("Location")
      : (aPrs->Shape().IsNull() ? QVariant ("") : ViewControl_Tools::ToString (aPrs->Shape().Location()).ToCString());
    case 7: return isFirstColumn ? QVariant ("Deflection")
      : (aPrs->Shape().IsNull() ? QVariant ("") : QVariant (Prs3d::GetDeflection(aPrs->Shape(), aPrs->Attributes())));

    default: return QVariant();
  }
  return QVariant();
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType VInspectorPaneAIS_Shape::GetTableEditType (const Handle(Standard_Transient)& theObject,
                                                                const int theRow, const int) const
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return ViewControl_EditType_None;

  switch (theRow)
  {
    //case 4: return ViewControl_EditType_Line;
    //case 5: return ViewControl_EditType_Combo;
    //case 6: return ViewControl_EditType_Bool;
    //case 12: return ViewControl_EditType_Bool;
    //case 17: return ViewControl_EditType_Combo;
    //case 18: return ViewControl_EditType_Bool;
    //case 20: return ViewControl_EditType_Combo;
    //case 23: return ViewControl_EditType_Bool;
    default: return ViewControl_EditType_None;
  }
}

// =======================================================================
// function : GetTableEnumValues
// purpose :
// =======================================================================
QList<QVariant> VInspectorPaneAIS_Shape::GetTableEnumValues (const Handle(Standard_Transient)& theObject,
                                                             const int theRow, const int) const
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return QList<QVariant>();

  QList<QVariant> aValues;
  switch (theRow)
  {
    //case 5:
    //{
    //  for (int i = 0; i <= Aspect_TOFM_FRONT_SIDE; i++)
    //    aValues.append (Aspect::TypeOfFacingModelToString((Aspect_TypeOfFacingModel)i));
    //}
    //break;
    //case 17:
    //{
    //  for (int i = 0; i <= PrsMgr_TOP_ProjectorDependant; i++)
    //    aValues.append (PrsMgr::TypeOfPresentation3dToString ((PrsMgr_TypeOfPresentation3d)i));
    //}
    //break;
    //case 20:
    //{
    //  aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_UNKNOWN));
    //  aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Default));
    //  aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Top));
    //  aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Topmost));
    //  aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_TopOSD));
    //  aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_BotOSD));
    //}
    //break;
    default: break;
  }
  return aValues;
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool VInspectorPaneAIS_Shape::SetTableData (const Handle(Standard_Transient)& theObject,
                                            const int theRow, const int, const QVariant& theValue)
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return false;

  //Handle(AIS_InteractiveObject) aPrs = GetInteractiveObject();
  switch (theRow)
  {
    //case 4:
    //{
    //  double aValue = theValue.toDouble();
    //  if (aValue > 0) aPrs->SetWidth (aValue);
    //  else aPrs->UnsetWidth();
    //}
    //break;
    //case 5: aPrs->SetCurrentFacingModel (Aspect::TypeOfFacingModelFromString (theValue.toString().toStdString().c_str()));
    //case 6: aPrs->SetInfiniteState (theValue.toBool());
    //case 12: aPrs->SetAutoHilight(theValue.toBool());
    //case 17: aPrs->SetTypeOfPresentation (PrsMgr::TypeOfPresentation3dFromString (theValue.toString().toStdString().c_str()));
    //case 18: aPrs->SetMutable (theValue.toBool());
    //case 20: aPrs->SetZLayer (Graphic3d::ZLayerIdFromString (theValue.toString().toStdString().c_str()));
    //case 23: if (!theValue.toBool()) aPrs->ResetTransformation();
    default: return false;
  }
  return true;
}

// =======================================================================
// function : GetSelected
// purpose :
// =======================================================================
ViewControl_PaneItem* VInspectorPaneAIS_Shape::GetSelected (const Handle(Standard_Transient)& theObject,
                                                            const int theRow, const int theColumn)
{
  Handle(AIS_Shape) aPrs = GetPresentation (theObject);
  if (aPrs.IsNull())
    return NULL;

  ViewControl_PaneItem* aPaneItem = new ViewControl_PaneItem();
  aPaneItem->SetShape (aPrs->Shape());

  return aPaneItem;
}

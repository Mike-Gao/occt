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


#include <inspector/VInspector_ItemSelect3DSensitiveSetItem.hxx>

//#include <AIS_ListOfInteractive.hxx>
//#include <AIS_Shape.hxx>
//#include <BRep_Builder.hxx>
//#include <BRepBuilderAPI_MakeEdge.hxx>
//#include <BRepBuilderAPI_MakeVertex.hxx>
//#include <SelectBasics_SensitiveEntity.hxx>
//#include <Select3D_SensitiveBox.hxx>
//#include <Select3D_SensitiveCircle.hxx>
//#include <Select3D_SensitiveCurve.hxx>
//#include <Select3D_SensitiveFace.hxx>
//#include <Select3D_SensitivePoint.hxx>
//#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveSet.hxx>
//#include <Select3D_SensitiveTriangle.hxx>
//#include <Select3D_InteriorSensitivePointSet.hxx> // child of Select3D_SensitiveSet
//#include <Select3D_SensitiveGroup.hxx>
//#include <Select3D_SensitivePoly.hxx>
//#include <Select3D_SensitivePrimitiveArray.hxx>
//#include <Select3D_SensitiveTriangulation.hxx>
//#include <Select3D_SensitiveWire.hxx>
//#include <SelectMgr_EntityOwner.hxx>
//#include <SelectMgr_Selection.hxx>
//#include <SelectMgr_SensitiveEntity.hxx>
//#include <Standard_Version.hxx>
//#include <StdSelect_BRepOwner.hxx>
//#include <TColgp_HArray1OfPnt.hxx>
//#include <TopoDS_Edge.hxx>
//#include <TopoDS_Face.hxx>
//#include <TopoDS_Shape.hxx>
//#include <TopoDS_Vertex.hxx>
//
//#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemSelectBasicsSensitiveEntity.hxx>
//#include <inspector/VInspector_ItemSelectMgrSensitiveEntity.hxx>
#include <inspector/VInspector_Tools.hxx>
//#include <inspector/ViewControl_Table.hxx>
//#include <inspector/ViewControl_Tools.hxx>
//

#include <inspector/Convert_Tools.hxx>
#include <Standard_WarningsDisable.hxx>
//#include <QStringList>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : GetSensitiveEntity
// purpose :
// =======================================================================
Handle(Standard_Transient) VInspector_ItemSelect3DSensitiveSetItem::GetObject() const
{
  //initItem(); // to update presentation shapes
  VInspector_ItemSelectBasicsSensitiveEntityPtr aParentItem = itemDynamicCast<VInspector_ItemSelectBasicsSensitiveEntity>(Parent());
  if (aParentItem)
    return aParentItem->GetSensitiveEntity();
  return Handle(Standard_Transient)();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelect3DSensitiveSetItem::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  Handle(SelectBasics_SensitiveEntity) anEntity = GetSensitiveEntity();
  if (anEntity.IsNull())
    return QVariant();

  switch (theItemRole)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      switch (Column())
      {
        case 0: return QString ("Index = %1").arg (Row());
        default:
          break;
      }
      break;
    }
    case Qt::BackgroundRole:
    case Qt::ForegroundRole:
    {
      VInspector_ItemSelectBasicsSensitiveEntityPtr aParentItem = itemDynamicCast<VInspector_ItemSelectBasicsSensitiveEntity>(Parent());
      if (aParentItem)
        return aParentItem->data(QModelIndex(), theItemRole);
      break;
    }
    default:
    break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelect3DSensitiveSetItem::Init()
{
  UpdatePresentationShape();
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelect3DSensitiveSetItem::Reset()
{
  // an empty method to don't clear the main label, otherwise the model will be empty
  TreeModel_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelect3DSensitiveSetItem::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelect3DSensitiveSetItem*>(this)->Init();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelect3DSensitiveSetItem::GetTableRowCount() const
{
  return 1;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemSelect3DSensitiveSetItem::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  bool isFirstColumn = theColumn == 0;

  Handle(Select3D_SensitiveSet) aSensitiveSet = Handle(Select3D_SensitiveSet)::DownCast (GetSensitiveEntity());
  if (aSensitiveSet.IsNull() || aSensitiveSet->Size() <= Row())
    return "Empty Select3D_SensitiveSet item";

  Handle(SelectBasics_SensitiveEntity) anEntity = GetSensitiveEntity();
  switch (theRow)
  {
    case 0: return isFirstColumn ? QVariant ("Box") : QVariant (VInspector_Tools::ToVariant (aSensitiveSet->Box (Row())));
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemSelect3DSensitiveSetItem::buildPresentationShape()
{
  Handle(SelectBasics_SensitiveEntity) anEntity = GetSensitiveEntity();
  if (anEntity.IsNull())
    return TopoDS_Shape();

  Handle(Select3D_SensitiveSet) aSensitiveSet = Handle(Select3D_SensitiveSet)::DownCast (GetSensitiveEntity());
  if (aSensitiveSet.IsNull() || aSensitiveSet->Size() < Row())
    return TopoDS_Shape();

  Select3D_BndBox3d aBndBox = aSensitiveSet->Box (Row());
  return VInspector_Tools::CreateShape (aBndBox);
}

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


#include <inspector/VInspector_ItemSelectBasicsSensitiveEntity.hxx>
#include <inspector/VInspector_ItemSelect3DSensitiveSetItem.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveSet.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_InteriorSensitivePointSet.hxx> // child of Select3D_SensitiveSet
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitivePoly.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SensitiveEntity.hxx>
#include <Standard_Version.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <inspector/Convert_Tools.hxx>

#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemSelectBasicsEntityOwner.hxx>
#include <inspector/VInspector_ItemSelectMgrSensitiveEntity.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
int VInspector_ItemSelectBasicsSensitiveEntity::initRowCount() const
{
  Handle(Select3D_SensitiveSet) aSensitiveSet = Handle(Select3D_SensitiveSet)::DownCast (GetSensitiveEntity());
  if (!aSensitiveSet.IsNull())
    return aSensitiveSet->Size();

  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectBasicsSensitiveEntity::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  Handle(SelectBasics_SensitiveEntity) anEntity = GetSensitiveEntity();
  if (anEntity.IsNull())
    return QVariant();

  Handle(SelectBasics_EntityOwner) anOwner = anEntity->OwnerId();

  switch (theItemRole)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
    {
      switch (Column())
      {
        case 0: return anEntity->DynamicType()->Name();
        default:
          break;
      }
      break;
    }
    case Qt::BackgroundRole:
    case Qt::ForegroundRole:
    {
      if (Column() == 2)
      {
        Handle(AIS_InteractiveContext) aContext = GetContext();
        if (!aContext.IsNull())
        {
          if (VInspector_Tools::IsOwnerSelected(aContext, getEntityOwner()))
            return QVariant ((theItemRole == Qt::BackgroundRole) ? QColor (Qt::darkBlue) : QColor (Qt::white));
        }
      }
      VInspector_ItemSelectMgrSensitiveEntityPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrSensitiveEntity>(Parent());
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
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectBasicsSensitiveEntity::createChild (int theRow, int theColumn)
{
  return VInspector_ItemSelect3DSensitiveSetItem::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelectBasicsSensitiveEntity::Init()
{
  VInspector_ItemSelectMgrSensitiveEntityPtr aParentItem = itemDynamicCast<VInspector_ItemSelectMgrSensitiveEntity>(Parent());

  Handle(SelectMgr_SensitiveEntity) anEntity = aParentItem->GetSensitiveEntity();

  myEntity = anEntity->BaseSensitive();

  UpdatePresentationShape();
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelectBasicsSensitiveEntity::Reset()
{
  // an empty method to don't clear the main label, otherwise the model will be empty
  TreeModel_ItemBase::Reset();
  myEntity = NULL;
  //if (!myPresentation.IsNull() && !myPresentation->GetContext().IsNull())
  //{
  //  myPresentation->GetContext()->Erase (myPresentation, Standard_False);
  //  myPresentation = NULL;
  //}
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelectBasicsSensitiveEntity::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectBasicsSensitiveEntity*>(this)->Init();
}

// =======================================================================
// function : getEntityOwner
// purpose :
// =======================================================================
Handle(SelectBasics_EntityOwner) VInspector_ItemSelectBasicsSensitiveEntity::getEntityOwner() const
{
  initItem();

  Handle(SelectBasics_EntityOwner) anOwner;
  const Handle(SelectBasics_SensitiveEntity)& aBase = GetSensitiveEntity();
  if (aBase.IsNull())
    return anOwner;
  return aBase->OwnerId();
}

// =======================================================================
// function : GetPresentations
// purpose :
// =======================================================================
void VInspector_ItemSelectBasicsSensitiveEntity::GetPresentations(NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  if (Column() != 0)
    return;

  if (!myPresentation.IsNull())
    return;

  Handle(Select3D_SensitiveEntity) aBaseEntity = GetSensitiveEntity();
  if (aBaseEntity.IsNull())
    return;

  TopoDS_Shape aShape = GetPresentationShape();
  if (aShape.IsNull())
    return;

  myPresentation = new AIS_Shape (aShape);
  myPresentation->SetColor (Quantity_Color (Quantity_NOC_BLUE1));
  thePresentations.Append (myPresentation);
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemSelectBasicsSensitiveEntity::buildPresentationShape
  (const Handle(SelectBasics_SensitiveEntity)& theEntity)
{
  Handle(Select3D_SensitiveEntity) aBaseEntity = theEntity;
  if (aBaseEntity.IsNull())
    return TopoDS_Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (aBaseEntity->CenterOfGeometry()));
  
  Select3D_BndBox3d aBoundingBox = aBaseEntity->BoundingBox();
  if (aBoundingBox.IsValid())
    aBuilder.Add (aCompound, VInspector_Tools::CreateShape (aBoundingBox));

  Standard_CString aTypeName = aBaseEntity->DynamicType()->Name();
  if (aTypeName == STANDARD_TYPE (Select3D_SensitiveBox)->Name())
  {
    Handle(Select3D_SensitiveBox) anEntity = Handle(Select3D_SensitiveBox)::DownCast (aBaseEntity);
    TopoDS_Shape aShape = Convert_Tools::CreateShape(anEntity->Box());
    aBuilder.Add (aCompound, aShape);
  }
  else if (aTypeName == STANDARD_TYPE (Select3D_SensitiveFace)->Name())
  {
    Handle(Select3D_SensitiveFace) anEntity = Handle(Select3D_SensitiveFace)::DownCast (aBaseEntity);
    Handle(TColgp_HArray1OfPnt) aPoints;
    anEntity->GetPoints (aPoints);
    for (Standard_Integer aPntIter = aPoints->Lower(); aPntIter <= aPoints->Upper(); ++aPntIter)
      aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex(aPoints->Value (aPntIter)));
  }
  else if (aTypeName == STANDARD_TYPE (Select3D_SensitivePoint)->Name())
  {
    Handle(Select3D_SensitivePoint) anEntity = Handle(Select3D_SensitivePoint)::DownCast (aBaseEntity);
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex(anEntity->Point()));
  }
  else if (aTypeName == STANDARD_TYPE (Select3D_SensitiveSegment)->Name())
  {
    Handle(Select3D_SensitiveSegment) anEntity = Handle(Select3D_SensitiveSegment)::DownCast (aBaseEntity);
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge(anEntity->StartPoint(), anEntity->EndPoint()));
  }
  else if (aTypeName == STANDARD_TYPE (Select3D_SensitiveTriangle)->Name())
  {
    Handle(Select3D_SensitiveTriangle) anEntity = Handle(Select3D_SensitiveTriangle)::DownCast (aBaseEntity);
    gp_Pnt aPnt0, aPnt1, aPnt2;
    anEntity->Points3D (aPnt0, aPnt1, aPnt2);
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex(aPnt0));
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex(aPnt1));
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex(aPnt2));
  }
  else if (aTypeName == STANDARD_TYPE (Select3D_SensitivePoly)->Name() ||
           aTypeName == STANDARD_TYPE (Select3D_SensitiveCircle)->Name() ||
           aTypeName == STANDARD_TYPE (Select3D_SensitiveCurve)->Name())
  {
    Handle(Select3D_SensitivePoly) anEntity = Handle(Select3D_SensitivePoly)::DownCast (aBaseEntity);

    Handle(TColgp_HArray1OfPnt) aPoints;
    anEntity->Points3D (aPoints);
    for (Standard_Integer aPntIter = aPoints->Lower(); aPntIter <= aPoints->Upper(); ++aPntIter)
      aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex(aPoints->Value (aPntIter)));
  }
  else if (aTypeName == STANDARD_TYPE (Select3D_SensitivePrimitiveArray)->Name())
  {
  }
  else if (aTypeName == STANDARD_TYPE (Select3D_SensitiveTriangulation)->Name())
  {
    Handle(Select3D_SensitiveTriangulation) anEntity = Handle(Select3D_SensitiveTriangulation)::DownCast (aBaseEntity);
    const Handle(Poly_Triangulation)& aPolyTriangulation = anEntity->Triangulation();
    if (!aPolyTriangulation.IsNull())
    {
      TopoDS_Face aFace;
      aBuilder.MakeFace (aFace, aPolyTriangulation);
      aBuilder.Add (aCompound, aFace);
    }
  }

  return aCompound;
}

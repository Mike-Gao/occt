// Created on: 2018-08-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <inspector/VInspector_ItemGraphic3dCamera.hxx>

#include <inspector/VInspector_ItemGraphic3dCView.hxx>
#include <inspector/VInspector_ItemV3dView.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Graphic3d.hxx>
#include <Graphic3d_Camera.hxx>
#include <TopoDS_Compound.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCamera::Init()
{
  VInspector_ItemV3dViewPtr aParentItem = itemDynamicCast<VInspector_ItemV3dView>(Parent());
  if (aParentItem)
    myCamera = aParentItem->GetView()->DefaultCamera();
  else
  {
    VInspector_ItemGraphic3dCViewPtr aParentItem = itemDynamicCast<VInspector_ItemGraphic3dCView>(Parent());
    if (aParentItem)
      myCamera = aParentItem->GetCView()->Camera();
  }

  UpdatePresentationShape();
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCamera::Reset()
{
  VInspector_ItemBase::Reset();
  myCamera = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCamera::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemGraphic3dCamera*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemGraphic3dCamera::initRowCount() const
{
  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemGraphic3dCamera::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(Graphic3d_Camera) aCamera = GetCamera();
  if (aCamera.IsNull())
    return Column() == 0 ? "Empty Graphic3d_Camera" : "";

  switch (Column())
  {
    case 0: return theItemRole == Qt::DisplayRole ? aCamera->DynamicType()->Name()
                                                  : STANDARD_TYPE (Graphic3d_Camera)->Name();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemGraphic3dCamera::GetTableRowCount() const
{
  return 40;
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType VInspector_ItemGraphic3dCamera::GetTableEditType (const int theRow, const int) const
{
  switch (theRow)
  {
    //case 0: return ViewControl_EditType_DoubleVec3;
    //case 1: return ViewControl_EditType_DoubleVec3;
    //case 2: return ViewControl_EditType_DoubleVec3;
    //case 3: return ViewControl_EditType_DoubleVec3;
    case 4: return ViewControl_EditType_Double;
    case 5: return ViewControl_EditType_Combo;
    case 6: return ViewControl_EditType_Double;
    case 7: return ViewControl_EditType_Double;
    case 8: return ViewControl_EditType_Double;
    case 9: return ViewControl_EditType_Double;
    case 10: return ViewControl_EditType_Combo;
    case 11: return ViewControl_EditType_Combo;
    case 12: return ViewControl_EditType_Double;
    // calculated
    //case 13: return ViewControl_EditType_DoubleVec3;
    //case 14: return ViewControl_EditType_DoubleVec3;
    //case 15: return ViewControl_EditType_DoubleVec3;
    default: return ViewControl_EditType_None;
  }
}

// =======================================================================
// function : GetTableEnumValues
// purpose :
// =======================================================================
QList<QVariant> VInspector_ItemGraphic3dCamera::GetTableEnumValues (const int theRow, const int) const
{
  QList<QVariant> aValues;
  switch (theRow)
  {
    case 5:
    {
      for (int i = 0; i <= Graphic3d_Camera::Projection_MonoRightEye; i++)
        aValues.append (Graphic3d::CameraProjectionToString((Graphic3d_Camera::Projection)i));
    }
    break;
    case 10:
    {
      for (int i = 0; i <= Graphic3d_Camera::FocusType_Relative; i++)
        aValues.append (Graphic3d::CameraFocusTypeToString((Graphic3d_Camera::FocusType)i));
    }
    break;
    case 11:
    {
      for (int i = 0; i <= Graphic3d_Camera::IODType_Relative; i++)
        aValues.append (Graphic3d::CameraIODTypeToString((Graphic3d_Camera::IODType)i));
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
QVariant VInspector_ItemGraphic3dCamera::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  Handle(Graphic3d_Camera) aCamera = GetCamera();
  if (aCamera.IsNull())
    return QVariant();

  bool isFirstColumn = theColumn == 0;
  bool isToolTip = theRole == Qt::ToolTipRole;
  switch (theRow)
  {
    case 0: return isFirstColumn ? (!isToolTip ? QVariant("Eye") : QVariant ("Camera Eye position"))
                                 : QVariant (ViewControl_Tools::ToString (aCamera->Eye()).ToCString());
    case 1: return isFirstColumn ? (!isToolTip ? QVariant("Center") : QVariant ("Center of the camera"))
                                 : QVariant (ViewControl_Tools::ToString (aCamera->Center()).ToCString());
    case 2: return isFirstColumn ? (!isToolTip ? QVariant("Up") : QVariant ("Camera Up direction vector, orthogonal to camera direction"))
                                 : QVariant (ViewControl_Tools::ToString (aCamera->Up()).ToCString());

    case 3: return isFirstColumn ? (!isToolTip ? QVariant ("AxialScale") : QVariant ("Camera axial scale vector"))
                                 : QVariant (ViewControl_Tools::ToString (aCamera->AxialScale()).ToCString());
    case 4: return isFirstColumn ? (!isToolTip ? QVariant ("Scale") : QVariant ("Camera scale (depend on Projection, see sources doc)"))
                                 : ViewControl_Tools::ToVariant (aCamera->Scale());
    case 5: return isFirstColumn ? (!isToolTip ? QVariant ("ProjectionType") : QVariant ("Camera projection type"))
                                 : QVariant (Graphic3d::CameraProjectionToString (aCamera->ProjectionType()));

    case 6: return isFirstColumn ? (!isToolTip ? QVariant ("FOVy") : QVariant ("Field Of View (FOV) in y axis for perspective projection"))
                                 : ViewControl_Tools::ToVariant (aCamera->FOVy());
    case 7: return isFirstColumn ? (!isToolTip ? QVariant ("ZNear") : QVariant ("Near Z-clipping plane position: distance of the plane from the Eye"))
                                 : ViewControl_Tools::ToVariant (aCamera->ZNear());
    case 8: return isFirstColumn ? (!isToolTip ? QVariant ("ZFar") : QVariant ("Far Z-clipping plane position: distance of the plane from the Eye"))
                                  : ViewControl_Tools::ToVariant (aCamera->ZFar());
    case 9: return isFirstColumn ? (!isToolTip ? QVariant ("Aspect") : QVariant ("Camera width / height display ratio"))
                                  : ViewControl_Tools::ToVariant (aCamera->Aspect());

    case 10: return isFirstColumn ? (!isToolTip ? QVariant ("ZFocusType") : QVariant ("Type used for stereographic focus"))
                                  : QVariant (Graphic3d::CameraFocusTypeToString (aCamera->ZFocusType()));
    case 11: return isFirstColumn ? (!isToolTip ? QVariant ("ZFocus") : QVariant ("Stereographic focus value"))
                                  : ViewControl_Tools::ToVariant (aCamera->ZFocus());

    case 12: return isFirstColumn ? (!isToolTip ? QVariant ("GetIODType") : QVariant ("Intraocular distance definition type"))
                                  : QVariant (Graphic3d::CameraIODTypeToString (aCamera->GetIODType()));
    case 13: return isFirstColumn ? (!isToolTip ? QVariant ("IOD") : QVariant ("Intraocular distance value"))
                                  : ViewControl_Tools::ToVariant (aCamera->IOD());

    case 14: return ViewControl_Table::SeparatorData();
    case 15: return isFirstColumn ? QVariant ("Calculated values:") : QVariant();
    case 16: return ViewControl_Table::SeparatorData();

    case 17: return isFirstColumn ? (!isToolTip ? QVariant ("Distance") : QVariant ("Distance of Eye from camera Center"))
                                  : ViewControl_Tools::ToVariant (aCamera->Distance());
    case 18: return isFirstColumn ? (!isToolTip ? QVariant ("Direction") : QVariant ("Camera look direction"))
                                  : QVariant (ViewControl_Tools::ToString (aCamera->Direction()).ToCString());
    case 19: return isFirstColumn ? (!isToolTip ? QVariant ("ViewDimensions") : QVariant ("View plane size at center (target) point and distance between ZFar and ZNear planes"))
                                  : ViewControl_Tools::ToString (aCamera->ViewDimensions()).ToCString();

    case 20: return ViewControl_Table::SeparatorData();
    case 21: return isFirstColumn ? QVariant ("TransformMatrices (D):") : QVariant();
    case 22: return ViewControl_Table::SeparatorData();

    case 23: return isFirstColumn ? QVariant ("OrientationMatrix") : VInspector_Tools::ToVariant (aCamera->OrientationMatrix());
    case 24: return isFirstColumn ? QVariant ("ProjectionMatrix")  : VInspector_Tools::ToVariant (aCamera->ProjectionMatrix());
    case 25: return isFirstColumn ? QVariant ("ProjectionStereoLeft") : VInspector_Tools::ToVariant (aCamera->ProjectionStereoLeft());
    case 26: return isFirstColumn ? QVariant ("ProjectionStereoRight") : VInspector_Tools::ToVariant (aCamera->ProjectionStereoRight());

    case 27: return ViewControl_Table::SeparatorData();
    case 28: return isFirstColumn ? QVariant ("TransformMatrices (F):") : QVariant();
    case 29: return ViewControl_Table::SeparatorData();

    case 30: return isFirstColumn ? QVariant ("OrientationMatrixF") : VInspector_Tools::ToVariant (aCamera->OrientationMatrixF());
    case 31: return isFirstColumn ? QVariant ("ProjectionMatrixF")  : VInspector_Tools::ToVariant (aCamera->ProjectionMatrixF());
    case 32: return isFirstColumn ? QVariant ("ProjectionStereoLeftF") : VInspector_Tools::ToVariant (aCamera->ProjectionStereoLeftF());
    case 33: return isFirstColumn ? QVariant ("ProjectionStereoRightF") : VInspector_Tools::ToVariant (aCamera->ProjectionStereoRightF());
  }
  return QVariant();
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool VInspector_ItemGraphic3dCamera::SetTableData (const int theRow, const int, const QVariant& theValue)
{
  Handle(Graphic3d_Camera) aCamera = GetCamera();
  if (aCamera.IsNull())
    return false;

  switch (theRow)
  {
    //case 0: return isFirstColumn ? (!isToolTip ? QVariant("Eye") : QVariant ("Camera Eye position"))
    //                             : QVariant (ViewControl_Tools::ToString (aCamera->Eye()).ToCString());
    //case 1: return isFirstColumn ? (!isToolTip ? QVariant("Center") : QVariant ("Center of the camera"))
    //                             : QVariant (ViewControl_Tools::ToString (aCamera->Center()).ToCString());
    //case 2: return isFirstColumn ? (!isToolTip ? QVariant("Up") : QVariant ("Camera Up direction vector, orthogonal to camera direction"))
    //                             : QVariant (ViewControl_Tools::ToString (aCamera->Up()).ToCString());
    //case 3: return isFirstColumn ? (!isToolTip ? QVariant ("AxialScale") : QVariant ("Camera axial scale vector"))
    //                             : QVariant (ViewControl_Tools::ToString (aCamera->AxialScale()).ToCString());
    case 4: aCamera->SetScale (ViewControl_Tools::ToRealValue (theValue)); break;
    case 5: aCamera->SetProjectionType (Graphic3d::CameraProjectionFromString(theValue.toString().toStdString().c_str())); break;

    case 6: aCamera->SetFOVy (ViewControl_Tools::ToRealValue (theValue)); break;
    case 7: aCamera->SetZRange (ViewControl_Tools::ToRealValue (theValue), aCamera->ZFar()); break;
    case 8: aCamera->SetZRange (aCamera->ZNear(), ViewControl_Tools::ToRealValue (theValue)); break;
    case 9: aCamera->SetAspect (ViewControl_Tools::ToRealValue (theValue)); break;

    case 10: aCamera->SetZFocus (Graphic3d::CameraFocusTypeFromString(theValue.toString().toStdString().c_str()),
                                 aCamera->ZFocus()); break;
    case 11: aCamera->SetZFocus (aCamera->ZFocusType(), ViewControl_Tools::ToRealValue (theValue)); break;

    case 12: aCamera->SetIOD (Graphic3d::CameraIODTypeFromString(theValue.toString().toStdString().c_str()),
                              aCamera->IOD()); break;
    case 13: aCamera->SetIOD (aCamera->GetIODType(),
                              ViewControl_Tools::ToRealValue (theValue)); break;

    //case 14: return ViewControl_Table::SeparatorData();
    //case 15: return isFirstColumn ? QVariant ("Calculated values:") : QVariant();
    //case 16: return ViewControl_Table::SeparatorData();

    case 17: aCamera->SetDistance (ViewControl_Tools::ToRealValue (theValue)); break;
    //case 18: return isFirstColumn ? (!isToolTip ? QVariant ("Direction") : QVariant ("Camera look direction"))
    //                              : QVariant (ViewControl_Tools::ToString (aCamera->Direction()).ToCString());
    default: return false;
  }
  return true;
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemGraphic3dCamera::buildPresentationShape (const Handle(Graphic3d_Camera)& theCamera)
{
  if (theCamera.IsNull())
    return TopoDS_Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  const gp_Pnt& aCenter = theCamera->Center ();
  const gp_Pnt& anEye = theCamera->Eye ();
  const gp_Dir& anUp = theCamera->Up();
  const gp_Dir& aCameraDirection = theCamera->Direction();

  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (aCenter));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (anEye));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Lin (anEye, anUp)));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Lin (anEye, aCameraDirection)));

  return aCompound;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemGraphic3dCamera::createChild (int, int)
{
  return TreeModel_ItemBasePtr();
}

// Created on: 2018-12-14
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


#include <inspector/VInspector_ItemAspectWindow.hxx>

#include <Select3D_SensitiveSet.hxx>
#include <Aspect_Window.hxx>
#include <inspector/VInspector_ItemV3dView.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <V3d_View.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemAspectWindow::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(Aspect_Window) aWindow = GetAspectWindow();
  if (aWindow.IsNull())
    return Column() == 0 ? "Empty Aspect_Window" : "";

  switch (Column())
  {
    case 0: return theItemRole == Qt::DisplayRole ? aWindow->DynamicType()->Name()
                                                  : STANDARD_TYPE (Aspect_Window)->Name();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemAspectWindow::Init()
{
  VInspector_ItemV3dViewPtr aParentItem = itemDynamicCast<VInspector_ItemV3dView>(Parent());
  if (aParentItem)
    myWindow = aParentItem->GetView()->Window();

  UpdatePresentationShape();
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemAspectWindow::Reset()
{
  myWindow = NULL;
  // an empty method to don't clear the main label, otherwise the model will be empty
  TreeModel_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemAspectWindow::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemAspectWindow*>(this)->Init();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemAspectWindow::GetTableRowCount() const
{
  return 30;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemAspectWindow::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  bool isFirstColumn = theColumn == 0;

  Handle(Aspect_Window) aWindow = GetAspectWindow();
  switch (theRow)
  {
    case 0: return isFirstColumn ? QVariant ("Background") : QVariant ();
    case 1: return isFirstColumn ? QVariant ("BackgroundFillMethod") : QVariant ();
    case 2: return isFirstColumn ? QVariant ("GradientBackground") : QVariant ();
    case 3: return isFirstColumn ? QVariant ("IsMapped") : QVariant (aWindow->IsMapped());
    case 4: return isFirstColumn ? QVariant ("IsVirtual") : QVariant (aWindow->IsVirtual());
    case 5: return isFirstColumn ? QVariant ("Ratio") : QVariant (aWindow->Ratio());
    case 6:
    {
      if (isFirstColumn)
        return QVariant ("Position");
      Standard_Integer aX1, aY1, aX2, aY2;
      aWindow->Position (aX1, aY1, aX2, aY2);
      return QString ("X1 = %1, Y1 = %2, X2 = %3, Y2 = %4").arg (aX1).arg (aY1).arg (aX2).arg (aY2);
    }
    case 7:
    {
      if (isFirstColumn)
        return QVariant ("Size");
      Standard_Integer aWidth, aHeight;
      aWindow->Size (aWidth, aHeight);
      return QString ("width = %1, height = %2").arg (aWidth).arg (aHeight);
    }
    case 8: return isFirstColumn ? QVariant ("NativeHandle") : QVariant ();
    case 9: return isFirstColumn ? QVariant ("NativeParentHandle") : QVariant ();
    case 10: return isFirstColumn ? QVariant ("NativeFBConfig") : QVariant ();
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemAspectWindow::buildPresentationShape()
{
  return TopoDS_Shape();
}

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

#include <inspector/ViewControl_Tools.hxx>
#include <inspector/ViewControl_TableModel.hxx>

#include <Geom_Transformation.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QHeaderView>
#include <QLocale>
#include <QObject>
#include <QPalette>
#include <QTableView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : CreateAction
// purpose :
// =======================================================================
QAction* ViewControl_Tools::CreateAction (const QString& theText, const char* theSlot, QObject* theParent, QObject* theContext)
{
  QAction* anAction = new QAction (theText, theParent);
  QObject::connect (anAction, SIGNAL (triggered (bool)), theContext, theSlot);
  return anAction;
}

// =======================================================================
// function : SetWhiteBackground
// purpose :
// =======================================================================
void ViewControl_Tools::SetWhiteBackground (QWidget* theControl)
{
  QPalette aPalette = theControl->palette();
  aPalette.setColor (QPalette::All, QPalette::Foreground, Qt::white);
  theControl->setPalette (aPalette);
}

// =======================================================================
// function : SetDefaultHeaderSections
// purpose :
// =======================================================================
void ViewControl_Tools::SetDefaultHeaderSections(QTableView* theTableView, const Qt::Orientation theOrientation)
{
  ViewControl_TableModel * aTableModel = dynamic_cast<ViewControl_TableModel*> (theTableView->model());
  ViewControl_TableModelValues* aModelValues = aTableModel->GetModelValues();
  if (!aModelValues)
    return;

  int aSectionSize;
  if (aModelValues->GetDefaultSectionSize (Qt::Horizontal, aSectionSize) )
    theTableView->horizontalHeader()->setDefaultSectionSize (aSectionSize);
  else {
    bool isStretchLastSection = true;
    for (int aColumnId = 0, aNbColumns = aTableModel->columnCount(); aColumnId < aNbColumns; aColumnId++)
    {
      TreeModel_HeaderSection aSection = aModelValues->GetHeaderItem (theOrientation, aColumnId);
      if (aSection.IsEmpty())
        continue;

      int aColumnWidth = aSection.GetWidth();
      if (aColumnWidth > 0)
      {
        theTableView->setColumnWidth (aColumnId, aColumnWidth);
        if (aColumnId == aNbColumns - 1)
          isStretchLastSection = false;
      }
      theTableView->setColumnHidden (aColumnId, aSection.IsHidden());
    }
    if (isStretchLastSection != theTableView->horizontalHeader()->stretchLastSection())
      theTableView->horizontalHeader()->setStretchLastSection (isStretchLastSection);

  }
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::GetPointerInfo (const Handle(Standard_Transient)& thePointer, const bool isShortInfo)
{
  if (thePointer.IsNull())
    return TCollection_AsciiString();

  return GetPointerInfo(thePointer.operator->(), isShortInfo);
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::GetPointerInfo (const void* thePointer, const bool isShortInfo)
{
  std::ostringstream aPtrStr;
  aPtrStr << thePointer;
  if (!isShortInfo)
    return aPtrStr.str().c_str();

  TCollection_AsciiString anInfoPtr (aPtrStr.str().c_str());
  for (int aSymbolId = 1; aSymbolId < anInfoPtr.Length(); aSymbolId++)
  {
    if (anInfoPtr.Value(aSymbolId) != '0')
    {
      anInfoPtr = anInfoPtr.SubString(aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend(GetPointerPrefix());
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// =======================================================================
// function : ToVariant
// purpose :
// =======================================================================
QVariant ViewControl_Tools::ToVariant (const Standard_ShortReal theValue)
{
  return QVariant (QLocale().toString (theValue));
}

// =======================================================================
// function : ToVariant
// purpose :
// =======================================================================
QVariant ViewControl_Tools::ToVariant (const Standard_Real theValue)
{
  return QVariant (QLocale().toString (theValue));
}

// =======================================================================
// function : ToRealValue
// purpose :
// =======================================================================
Standard_ShortReal ViewControl_Tools::ToShortRealValue (const QVariant& theValue)
{
  return QLocale().toFloat (theValue.toString());
}

// =======================================================================
// function : ToRealValue
// purpose :
// =======================================================================
Standard_Real ViewControl_Tools::ToRealValue (const QVariant& theValue)
{
  return QLocale().toDouble (theValue.toString());
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::ToString (const Handle(Geom_Transformation)& theValue)
{
  if (theValue.IsNull())
    return TCollection_AsciiString();

  return ViewControl_Tools::ToString (theValue->Trsf());
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::ToString (const gp_Trsf& theValue)
{
  TCollection_AsciiString aValues;
  for (int aRowId = 1; aRowId <= 3; aRowId++)
  {
    for (int aColId = 1; aColId <= 4; aColId++) {
      aValues += TCollection_AsciiString (theValue.Value(aRowId, aColId));
      if (aColId != 4)
        aValues += ",";
    }
    if (aRowId != 3)
      aValues += "  ";
  }
  return aValues;
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::ToString (const gp_Pnt& thePoint)
{
  return TCollection_AsciiString (thePoint.X()) + ","
       + TCollection_AsciiString (thePoint.Y()) + ","
       + TCollection_AsciiString (thePoint.Z());
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::ToString (const gp_Dir& theDir)
{
  return TCollection_AsciiString (theDir.X()) + ","
       + TCollection_AsciiString (theDir.Y()) + ","
       + TCollection_AsciiString (theDir.Z());
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::ToString (const gp_XYZ& theValue)
{
  return TCollection_AsciiString (theValue.X()) + ","
       + TCollection_AsciiString (theValue.Y()) + ","
       + TCollection_AsciiString (theValue.Z());
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::ToString (const Bnd_Box& theValue)
{
  if (theValue.IsVoid())
    return "Bnd_Box is void";

  return QString ("(%1, %2, %3), (%4, %5, %6)")
    .arg (theValue.CornerMin().X()).arg (theValue.CornerMin().Y()).arg (theValue.CornerMin().Z())
    .arg (theValue.CornerMax().X()).arg (theValue.CornerMax().Y()).arg (theValue.CornerMax().Z()).toStdString().c_str();

}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::ToString (const Handle(TColgp_HArray1OfPnt)& thePoints)
{
  QStringList aPointList;
  for (Standard_Integer aPntIter = thePoints->Lower(); aPntIter <= thePoints->Upper(); ++aPntIter)
    aPointList.append(ViewControl_Tools::ToString (thePoints->Value (aPntIter)).ToCString());

  return aPointList.join(" ,").toStdString().c_str();
}

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
TCollection_AsciiString ViewControl_Tools::ToString (const TopLoc_Location& theLocation)
{
  return ToString (theLocation.Transformation());
}

//=======================================================================
//function : CreateShape
//purpose  :
//=======================================================================
TopoDS_Shape ViewControl_Tools::CreateShape (const Bnd_Box& theBoundingBox)
{
  if (theBoundingBox.IsVoid() || theBoundingBox.IsWhole())
    return TopoDS_Shape();

  Standard_Real aXmin, anYmin, aZmin, aXmax, anYmax, aZmax;
  theBoundingBox.Get (aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);

  gp_Pnt aPntMin = gp_Pnt (aXmin, anYmin, aZmin);
  gp_Pnt aPntMax = gp_Pnt (aXmax, anYmax, aZmax);

  return CreateBoxShape (aPntMin, aPntMax);
}

//=======================================================================
//function : CreateShape
//purpose  :
//=======================================================================
TopoDS_Shape ViewControl_Tools::CreateShape (const Bnd_OBB& theBoundingBox)
{
  if (theBoundingBox.IsVoid())
    return TopoDS_Shape();

  TColgp_Array1OfPnt anArrPnts(0, 8);
  theBoundingBox.GetVertex(&anArrPnts(0));

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(0)), gp_Pnt (anArrPnts.Value(1))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(0)), gp_Pnt (anArrPnts.Value(2))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(1)), gp_Pnt (anArrPnts.Value(3))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(2)), gp_Pnt (anArrPnts.Value(3))));

  return aCompound;
}

//=======================================================================
//function : CreateBoxShape
//purpose  :
//=======================================================================
TopoDS_Shape ViewControl_Tools::CreateBoxShape (const gp_Pnt& thePntMin, const gp_Pnt& thePntMax)
{
  Standard_Boolean aThinOnX = fabs (thePntMin.X() - thePntMax.X()) < Precision::Confusion();
  Standard_Boolean aThinOnY = fabs (thePntMin.Y() - thePntMax.Y()) < Precision::Confusion();
  Standard_Boolean aThinOnZ = fabs (thePntMin.Z() - thePntMax.Z()) < Precision::Confusion();

  if (((int)aThinOnX + (int)aThinOnY + (int)aThinOnZ) > 1) // thin box in several directions is a point
  {
    BRep_Builder aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound (aCompound);
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (thePntMin));
    return aCompound;
  }

  if (aThinOnX || aThinOnY || aThinOnZ)
  {
    gp_Pnt aPnt1, aPnt2, aPnt3, aPnt4 ;
    if (aThinOnX)
    {
      aPnt1 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMin.Z());
      aPnt2 = gp_Pnt(thePntMin.X(), thePntMax.Y(), thePntMin.Z());
      aPnt3 = gp_Pnt(thePntMin.X(), thePntMax.Y(), thePntMax.Z());
      aPnt4 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMax.Z());
    }
    else if (aThinOnY)
    {
      aPnt1 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMin.Z());
      aPnt2 = gp_Pnt(thePntMax.X(), thePntMin.Y(), thePntMin.Z());
      aPnt3 = gp_Pnt(thePntMax.X(), thePntMin.Y(), thePntMax.Z());
      aPnt4 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMax.Z());
    }
    else if (aThinOnZ)
    {
      aPnt1 = gp_Pnt(thePntMin.X(), thePntMin.Y(), thePntMin.Z());
      aPnt2 = gp_Pnt(thePntMax.X(), thePntMin.Y(), thePntMin.Z());
      aPnt3 = gp_Pnt(thePntMax.X(), thePntMax.Y(), thePntMin.Z());
      aPnt4 = gp_Pnt(thePntMin.X(), thePntMax.Y(), thePntMin.Z());
    }
    BRep_Builder aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound (aCompound);
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (aPnt1, aPnt2));
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (aPnt2, aPnt3));
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (aPnt3, aPnt4));
    aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (aPnt4, aPnt1));

    return aCompound;
  }
  else
  {
    BRepPrimAPI_MakeBox aBoxBuilder (thePntMin, thePntMax);
    return aBoxBuilder.Shape();
  }
}

// Created on: 2019-02-25
// Created by: Natalia ERMOLAEVA
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

#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_ItemStream.hxx>
#include <inspector/Convert_Tools.hxx>
#include <inspector/Convert_TransientShape.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TreeModel_ItemProperties, Standard_Transient)

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void TreeModel_ItemProperties::Init ()
{
  myRowValues.Clear();

  const Standard_SStream& aStream = Item()->Stream();

  NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue> aValues;
  TCollection_AsciiString aStreamText = Standard_Dump::Text (aStream);
  Standard_Dump::SplitJson (aStreamText, aValues);

  TreeModel_ItemStreamPtr aStreamParent = itemDynamicCast<TreeModel_ItemStream>(Item());
  TCollection_AsciiString aKey;
  Standard_DumpValue aKeyValue;
  if (!aStreamParent)
  {
    Handle(Standard_Transient) anItemObject = Item()->GetObject();
    aKey = anItemObject.IsNull() ? "Dump" : anItemObject->DynamicType()->Name();
    aKeyValue = Standard_DumpValue (aStreamText, 1);

    myKey = aKey;
    myStreamValue = aKeyValue;
  }
  else
  {
    TCollection_AsciiString aValue;
    Handle(TreeModel_ItemProperties) aParentProperties = Item()->Parent() ? Item()->Parent()->Properties() : 0;
    if (aParentProperties)
      aParentProperties->GetChildStream (Item()->Row(), aKey, aKeyValue);

    myKey = aKey;
    myStreamValue = aKeyValue;

    aValues.Clear();
    Standard_Dump::SplitJson (myStreamValue.myValue, aValues);
  }

  for (Standard_Integer anIndex = 1; anIndex <= aValues.Size(); anIndex++)
  {
    Standard_DumpValue aValue = aValues.FindFromIndex (anIndex);
    if (Standard_Dump::HasChildKey (aValue.myValue))
      myChildren.Add (aValues.FindKey (anIndex), aValue);
    else
    {
      TreeModel_RowValue aRowValue (aValue.myStartPosition, aValues.FindKey (anIndex).ToCString(), aValue.myValue.ToCString());
      myRowValues.Add (myRowValues.Size() + 1, aRowValue);
    }
  }
  if (myRowValues.Size() == 1)
  {
    Quantity_Color aColor;
    if (Convert_Tools::ConvertStreamToColor (aStream, aColor))
    {
      Standard_Real aRed, aGreen, aBlue;
      aColor.Values (aRed, aGreen, aBlue, Quantity_TOC_RGB);

      int aDelta = 255;
      myRowValues.ChangeFromIndex (1).myCustomValues.insert ((int)Qt::BackgroundRole, QColor((int)(aRed * aDelta),
        (int)(aGreen * aDelta), (int)(aBlue * aDelta)));
    }
  }
}

// =======================================================================
// function :  Reset
// purpose :
// =======================================================================
void TreeModel_ItemProperties::Reset()
{
  myKey = "";
  myStreamValue = Standard_DumpValue();

  myChildren.Clear();
  myRowValues.Clear();
}

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================

int TreeModel_ItemProperties::RowCount() const
{
  return RowValues().Size();
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant TreeModel_ItemProperties::Data (const int theRow, const int theColumn, int theRole) const
{
  if (theColumn == 1 && theRole == Qt::BackgroundRole)
  {
    const QMap<int, QVariant>& aCachedValues = RowValues().FindFromIndex (theRow + 1).myCustomValues;
    if (aCachedValues.contains ((int)theRole))
      return aCachedValues[(int)theRole];
  }

  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  if (theColumn == 0) return RowValues().FindFromIndex (theRow + 1).myKey;
  else if (theColumn == 1) return RowValues().FindFromIndex (theRow + 1).myValue;

  return QVariant();
}

// =======================================================================
// function : GetEditType
// purpose :
// =======================================================================

ViewControl_EditType TreeModel_ItemProperties::GetEditType (const int, const int theColumn) const
{
  if (theColumn == 0)
    return ViewControl_EditType_None;

  return ViewControl_EditType_Line;
}

// =======================================================================
// function : SetData
// purpose :
// =======================================================================

bool TreeModel_ItemProperties::SetData (const int /*theRow*/, const int theColumn, const QVariant& /*theValue*/, int)
{
  if (theColumn == 0)
    return false;

  TreeModel_ItemBasePtr aParent = Item()->Parent();
  while (aParent && itemDynamicCast<TreeModel_ItemStream>(aParent))
  {
    aParent = aParent->Parent();
  }
  if (!aParent)
    return false;

  //const Standard_SStream& aStream = Item()->Stream();

  //TCollection_AsciiString aStreamValue = Standard_Dump::Text (aStream);

  //Standard_DumpValue aValue = Values().FindFromIndex (theRow + 1);
  //Standard_Integer aStartPos = aValue.myStartPosition;
  //Standard_Integer aLastPos = aStartPos + aValue.myValue.Length() - 1;

  //aStream.str ("");
  ////aStream << aStreamValue.SubString (1, aStartPos - 1);
  //aStream << theValue.toString().toStdString().c_str();
  ////if (aLastPos + 1 <= aStreamValue.Length())
  ////  aStream << aStreamValue.SubString (aLastPos + 1, aStreamValue.Length());

  ////TCollection_AsciiString aStreamValue_debug = Standard_Dump::Text (aStream);

  //Item()->SetStream (aStream, aStartPos, aLastPos);
  //Item()->Reset();

  //return true;
  return false;
}

// =======================================================================
// function : GetPresentations
// purpose :
// =======================================================================

void TreeModel_ItemProperties::GetPresentations (const int theRow, const int theColumn,
                                                 NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  (void)thePresentations;

  if (theRow < 0) // full presentation
  {
    const Standard_SStream& aStream = Item()->Stream();
    Convert_Tools::ConvertStreamToPresentations (aStream, 1, -1, thePresentations);
    return;
  }


  if (theColumn == 0)
    return;

  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aValues;
  aValues.Add (Data (theRow, 0).toString().toStdString().c_str(), Data (theRow, theColumn).toString().toStdString().c_str());

  //Standard_SStream aStreamOnSelected;
  //Standard_Dump::JoinJson (aStreamOnSelected, aValues);

  //if (theRow < 0) // full presentation
  //{
    //Bnd_Box aBox;
    //if (aBox.Init (myStream))
    //  thePresentations.Append (new Convert_TransientShape (Convert_Tools::CreateShape (aBox)));
  //}

  //gp_XYZ aPoint;
  //Standard_Integer aPosition = 1;
  //if (aPoint.InitJson (aStreamOnSelected, aPosition))
  //{
  //  thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPoint)));
  //}


  //QVariant aValue = GetTableData (theRow, theColumn, Qt::DisplayRole);
  //TCollection_AsciiString aStrValue = aValue.toString().toStdString().c_str();

  //gp_XYZ aPoint;
  //if (!aPoint.Init (Standard_SStream (aStrValue.ToCString())))
  //  return;

  //thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPoint)));
}

// =======================================================================
// function : GetTableFlags
// purpose :
// =======================================================================

Qt::ItemFlags TreeModel_ItemProperties::GetTableFlags (const int, const int theColumn) const
{
  Qt::ItemFlags aFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (theColumn == 1)
    aFlags = aFlags | Qt::ItemIsEditable;

  return aFlags;
}

// =======================================================================
// function : GetChildStream
// purpose :
// =======================================================================
void TreeModel_ItemProperties::GetChildStream (const int theRowId,
                                               TCollection_AsciiString& theKey,
                                               Standard_DumpValue& theValue) const
{
  if (myChildren.Size() <= theRowId)
    return;

  theKey = myChildren.FindKey (theRowId + 1);
  theValue = myChildren.FindFromIndex (theRowId + 1);
}

// =======================================================================
// function : GetChildStream
// purpose :
// =======================================================================
void TreeModel_ItemProperties::initItem() const
{
  if (!Item())
    return;
  if (Item()->IsInitialized())
    return;
  Item()->Init();
}

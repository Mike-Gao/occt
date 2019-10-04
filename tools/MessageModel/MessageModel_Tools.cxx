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

#include <inspector/MessageModel_Tools.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/ViewControl_TableModelValuesDefault.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepTools.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeStream.hxx>

#include <Precision.hxx>
#include <TopoDS_AlertAttribute.hxx>
#include <TopoDS_Edge.hxx>

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString MessageModel_Tools::GetPointerInfo (const Handle(Standard_Transient)& thePointer, const bool isShortInfo)
{
  if (thePointer.IsNull())
    return TCollection_AsciiString();

  std::ostringstream aPtrStr;
  aPtrStr << thePointer.operator->();
  if (!isShortInfo)
    return aPtrStr.str().c_str();

  TCollection_AsciiString anInfoPtr (aPtrStr.str().c_str());
  for (int aSymbolId = 1; aSymbolId < anInfoPtr.Length(); aSymbolId++)
  {
    if (anInfoPtr.Value(aSymbolId) != '0')
    {
      anInfoPtr = anInfoPtr.SubString (aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend("0x");
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// =======================================================================
// function : GetPointerAlertInfo
// purpose :
// =======================================================================
TCollection_AsciiString MessageModel_Tools::GetPointerAlertInfo (const Handle(Message_Alert)& theAlert)
{
  Handle(Message_AlertExtended) anExtAlert = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anExtAlert.IsNull())
    return TCollection_AsciiString();

  Handle(Message_Attribute) anAttribute = anExtAlert->Attribute();
  if (anAttribute.IsNull())
    return TCollection_AsciiString();

  if (anAttribute->IsKind (STANDARD_TYPE (TopoDS_AlertAttribute)))
    return GetPointerInfo (Handle(TopoDS_AlertAttribute)::DownCast (anAttribute)->GetShape().TShape());
  else if (anAttribute->IsKind (STANDARD_TYPE (Message_AttributeObject)))
    return GetPointerInfo (Handle(Message_AttributeObject)::DownCast (anAttribute)->GetObject());
  return TCollection_AsciiString();
}

// =======================================================================
// function : GetShapeTypeAlertInfo
// purpose :
// =======================================================================
TCollection_AsciiString MessageModel_Tools::GetShapeTypeAlertInfo (const Handle(Message_Alert)& theAlert)
{
  Handle(Message_AlertExtended) anExtAlert = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anExtAlert.IsNull())
    return TCollection_AsciiString();

  Handle(Message_Attribute) anAttribute = anExtAlert->Attribute();
  if (anAttribute.IsNull())
    return TCollection_AsciiString();

  TopoDS_Shape aShape;
  if (anAttribute->IsKind (STANDARD_TYPE (TopoDS_AlertAttribute)))
    aShape = Handle(TopoDS_AlertAttribute)::DownCast (anAttribute)->GetShape();

  if (aShape.IsNull())
    return TCollection_AsciiString();

  TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
  Standard_SStream aSStream;
  TopAbs::Print ((TopAbs_ShapeEnum)aShapeType, aSStream);
  return aSStream.str().c_str();
}

// =======================================================================
// function : GetStandardTypeAlertInfo
// purpose :
// =======================================================================
TCollection_AsciiString MessageModel_Tools::GetStandardTypeAlertInfo (const Handle(Message_Alert)& theAlert)
{
  Handle(Message_AlertExtended) anExtAlert = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anExtAlert.IsNull())
    return TCollection_AsciiString();

  Handle(Message_Attribute) anAttribute = anExtAlert->Attribute();
  if (anAttribute.IsNull())
    return TCollection_AsciiString();

  Handle(Standard_Transient) aPointer;
  if (anAttribute->IsKind (STANDARD_TYPE (TopoDS_AlertAttribute)))
    aPointer = Handle(TopoDS_AlertAttribute)::DownCast (anAttribute)->GetShape().TShape();
  else if (anAttribute->IsKind (STANDARD_TYPE (Message_AttributeObject)))
    aPointer = Handle(Message_AttributeObject)::DownCast (anAttribute)->GetObject();

  if (aPointer.IsNull())
    return TCollection_AsciiString();

  return aPointer->DynamicType()->Name();
}

// =======================================================================
// function : GetPropertyTableValues
// purpose :
// =======================================================================
void MessageModel_Tools::GetPropertyTableValues (const TreeModel_ItemBasePtr& theItem,
                                                 QList<ViewControl_TableModelValues*>& theTableValues)
{
  //Handle(TreeModel_ItemProperties) anItemProperties = theItem->GetProperties();
  //if (!anItemProperties.IsNull())
  //{
  //  ViewControl_TableModelValues* aTableValues = new ViewControl_TableModelValues();
  //  aTableValues->SetProperties (anItemProperties);
  //  theTableValues.append (aTableValues);
  //  return;
  //}

  //MessageModel_ItemAlertPtr anAlertItem = itemDynamicCast<MessageModel_ItemAlert>(theItem);
  //if (!anAlertItem)
  //  return;

  //const Handle(Message_Alert)& anAlert = anAlertItem->GetAlert();
  //Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anAlert);
  //if (anExtendedAlert.IsNull())
  //  return;

  //Handle(Message_Attribute) anAttribute = anExtendedAlert->Attribute();
  //if (anAttribute.IsNull())
  //  return;

  ////if (anAttribute->IsKind (STANDARD_TYPE (Message_AttributeStream)))
  ////{
  //  //int aSectionSize = 200;
  //  //ViewControl_TableModelValues* aTableValues = new MessageModel_TableModelValues (anAttribute, aSectionSize);
  //  //theTableValues.append (aTableValues);
  ////}
  ////else
  ////{
  //if (!anAttribute->GetDescription().IsEmpty())
  //{
  //  ViewControl_TableModelValuesDefault* aTableValues = new ViewControl_TableModelValuesDefault();
  //  QList<TreeModel_HeaderSection> aHeaderValues;
  //  QVector<QVariant> aValues;
  //  aHeaderValues << TreeModel_HeaderSection ("Description", -2);
  //  aValues << anAttribute->GetDescription().ToCString();
  //  aTableValues->SetHeaderValues (aHeaderValues, Qt::Horizontal);

  //  QString aValueStr = anAttribute->GetDescription().ToCString();
  //  QStringList aValueStrList = aValueStr.split ("\n");
  //  int aNbRows = aValueStrList.size();

  //  QFontMetrics aFontMetrics (qApp->font());
  //  int aHeight = aFontMetrics.boundingRect(aValueStr).height();
  //  aHeight = (aHeight + TreeModel_Tools::HeaderSectionMargin()) * aNbRows;
  //  aTableValues->SetValues (aValues);
  //  aTableValues->SetDefaultSectionSize(Qt::Vertical, aHeight);

  //  theTableValues.append (aTableValues);
  //}
  ////}
}

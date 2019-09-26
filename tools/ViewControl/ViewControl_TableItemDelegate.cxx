// Created on: 2018-08-09
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

#include <inspector/ViewControl_TableItemDelegate.hxx>
#include <inspector/ViewControl_ColorSelector.hxx>
#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/ViewControl_EditType.hxx>
#include <inspector/ViewControl_TableDoubleVector.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QFont>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================

ViewControl_TableItemDelegate::ViewControl_TableItemDelegate (QObject* theParent)
 : QItemDelegate (theParent), myModelValues (0)
{
}

// =======================================================================
// function : createEditor
// purpose :
// =======================================================================

QWidget* ViewControl_TableItemDelegate::createEditor (QWidget* theParent,
                                                      const QStyleOptionViewItem&,
                                                      const QModelIndex& theIndex) const
{
  if (!myModelValues)
    return 0;

  int aRow = theIndex.row();
  int aColumn = theIndex.column();
  ViewControl_EditType anEditType = myModelValues->GetEditType (aRow, aColumn);

  QWidget* anEditor = createEditorControl (theParent, anEditType);
  initEditorParameters (anEditor, anEditType, myModelValues, aRow, aColumn);
  return anEditor;
}

// =======================================================================
// function : setEditorData
// purpose :
// =======================================================================

void ViewControl_TableItemDelegate::setEditorData (QWidget* theEditor, const QModelIndex& theIndex) const
{
  if (!myModelValues)
    return;

  int aRow = theIndex.row();
  int aColumn = theIndex.column();
  ViewControl_EditType anEditType = myModelValues->GetEditType (aRow, aColumn);

  setEditorValue (theEditor, anEditType, theIndex.model()->data(theIndex));
}

// =======================================================================
// function : setModelData
// purpose :
// =======================================================================

void ViewControl_TableItemDelegate::setModelData (QWidget* theEditor, QAbstractItemModel* theModel,
                                                  const QModelIndex& theIndex) const
{
  if (!myModelValues)
    return;

  int aRow = theIndex.row();
  int aColumn = theIndex.column();
  ViewControl_EditType anEditType = myModelValues->GetEditType (aRow, aColumn);

  theModel->setData (theIndex, getEditorValue (theEditor, anEditType));
}

// =======================================================================
// function : createEditorControl
// purpose :
// =======================================================================

QWidget* ViewControl_TableItemDelegate::createEditorControl (QWidget* theParent, const ViewControl_EditType theEditType)
{
  switch (theEditType)
  {
    case ViewControl_EditType_None: return 0;
    case ViewControl_EditType_Bool: return new QComboBox (theParent);
    case ViewControl_EditType_Color: return new ViewControl_ColorSelector (theParent);
    case ViewControl_EditType_Double:
    {
      QLineEdit* aLineEdit = new QLineEdit (theParent);
      aLineEdit->setValidator (new QDoubleValidator (theParent));
      return aLineEdit;
    }
    case ViewControl_EditType_Line: return new QLineEdit (theParent);
    case ViewControl_EditType_Spin:
    {
      QSpinBox* aSpinBox = new QSpinBox (theParent);
      aSpinBox->setRange (IntegerFirst(), IntegerLast());
      return aSpinBox;
    }
    case ViewControl_EditType_DoAction: return new QPushButton (theParent);
    case ViewControl_EditType_DoubleVector: return new ViewControl_TableDoubleVector(theParent);

    default: return 0;
  }
}

// =======================================================================
// function : initEditorParameters
// purpose :
// =======================================================================

void ViewControl_TableItemDelegate::initEditorParameters (QWidget* theEditor,
                                                          const ViewControl_EditType theEditType,
                                                          ViewControl_TableModelValues* theModelValues,
                                                          const int theRow, const int theColumn)
{
  switch (theEditType)
  {
    case ViewControl_EditType_Bool:
    {
      (qobject_cast<QComboBox*> (theEditor))->insertItem(0, "true");
      (qobject_cast<QComboBox*> (theEditor))->insertItem(1, "false");
      break;
    }
    case ViewControl_EditType_Double:
    {
    }
    default: break;
  }
}

// =======================================================================
// function : setEditorValue
// purpose :
// =======================================================================

void ViewControl_TableItemDelegate::setEditorValue (QWidget* theEditor, const ViewControl_EditType theEditType,
                                                    const QVariant& theValue)
{
  switch (theEditType)
  {
    case ViewControl_EditType_None: break;
    case ViewControl_EditType_Bool: (qobject_cast<QComboBox*>(theEditor))->setCurrentIndex (theValue.toBool() ? 0 : 1); break;
    case ViewControl_EditType_Color: (qobject_cast<ViewControl_ColorSelector*>(theEditor))->SetColor (theValue.toString()); break;
    case ViewControl_EditType_Double:
    case ViewControl_EditType_Line: (qobject_cast<QLineEdit*>(theEditor))->setText (theValue.toString()); break;
    case ViewControl_EditType_Spin: (qobject_cast<QSpinBox*>(theEditor))->setValue (theValue.toInt()); break;
    case ViewControl_EditType_DoAction: (qobject_cast<QPushButton*>(theEditor))->setText ("UnSelect"); break;
    case ViewControl_EditType_DoubleVector: (qobject_cast<ViewControl_TableDoubleVector*>(theEditor))->SetVectorValue(theValue.toString()); break;

    default: break;
  }
}

// =======================================================================
// function : getEditorValue
// purpose :
// =======================================================================

QVariant ViewControl_TableItemDelegate::getEditorValue (QWidget* theEditor, const ViewControl_EditType theEditType)
{
  switch (theEditType)
  {
    case ViewControl_EditType_None: return QVariant();
    case ViewControl_EditType_Bool: return (qobject_cast<QComboBox*>(theEditor))->currentIndex() == 0 ? true : false;
    case ViewControl_EditType_Color: return (qobject_cast<ViewControl_ColorSelector*>(theEditor))->GetColor();
    case ViewControl_EditType_Double:
    case ViewControl_EditType_Line: return (qobject_cast<QLineEdit*>(theEditor))->text();
    case ViewControl_EditType_Spin: return (qobject_cast<QSpinBox*>(theEditor))->value();
    case ViewControl_EditType_DoAction: return QVariant ("Clicked");
    case ViewControl_EditType_DoubleVector: return (qobject_cast<ViewControl_TableDoubleVector*>(theEditor))->GetVector();

    default: return QVariant();
  }
}

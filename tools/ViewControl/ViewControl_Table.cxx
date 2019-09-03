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

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_TableItemDelegate.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QGridLayout>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QTableView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

#include <limits>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================

ViewControl_Table::ViewControl_Table (QWidget* theParent)
: QObject (theParent), myIsUseProperty (false)
{
  myMainWidget = new QWidget (theParent);
  QGridLayout* aLayout = new QGridLayout (myMainWidget);
  aLayout->setContentsMargins (0, 0, 0, 0);

  myTableView = new QTableView (myMainWidget);
  myTableView->setVerticalScrollMode (QAbstractItemView::ScrollPerPixel);

  myTableView->setItemDelegate (new ViewControl_TableItemDelegate (theParent));

  QHeaderView* aVHeader = myTableView->verticalHeader();
  int aDefCellSize = aVHeader->minimumSectionSize();
  aVHeader->setDefaultSectionSize (aDefCellSize);

  aLayout->addWidget (myTableView);
}

// =======================================================================
// function : SetModel
// purpose :
// =======================================================================

void ViewControl_Table::SetModel (QAbstractTableModel* theModel)
{
  myTableView->setModel (theModel);

  myTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QItemSelectionModel* aSelectionModel = new QItemSelectionModel(theModel);
  myTableView->setSelectionModel (aSelectionModel);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void ViewControl_Table::Init (ViewControl_TableModelValues* theModelValues)
{
  myTableView->selectionModel()->clearSelection();

  ViewControl_TableModel* aModel = dynamic_cast<ViewControl_TableModel*> (myTableView->model());
  aModel->SetModelValues (theModelValues);

  ViewControl_TableItemDelegate* aDelegate = dynamic_cast<ViewControl_TableItemDelegate*>(myTableView->itemDelegate());
  aDelegate->SetModelValues (theModelValues);

  int aSectionSize;
  myTableView->horizontalHeader()->setVisible (theModelValues->IsHeaderVisible (Qt::Horizontal));
  if (theModelValues->GetDefaultSectionSize (Qt::Horizontal, aSectionSize) )
    myTableView->horizontalHeader()->setDefaultSectionSize (aSectionSize);

  myTableView->verticalHeader()->setVisible (theModelValues->IsHeaderVisible (Qt::Vertical));
  if (theModelValues->GetDefaultSectionSize (Qt::Vertical, aSectionSize) )
  {
    myTableView->verticalHeader()->setDefaultSectionSize (aSectionSize);
  }
  else
    myTableView->verticalHeader()->setDefaultSectionSize (myTableView->verticalHeader()->minimumSectionSize());

  aModel->EmitLayoutChanged();
}

// =======================================================================
// function : GetSelectedIndices
// purpose :
// =======================================================================

void ViewControl_Table::GetSelectedIndices (QMap<int, QList<int>>& theSelectedIndices) const
{
  QModelIndexList aSelected = myTableView->selectionModel()->selectedIndexes();

  int aRow, aColumn;
  for (QModelIndexList::const_iterator anIt = aSelected.begin(); anIt != aSelected.end(); anIt++)
  {
    QModelIndex anIndex = *anIt;
    aRow = anIndex.row();
    aColumn = anIndex.column();
    if (!theSelectedIndices.contains (aRow))
      theSelectedIndices.insert (aRow, QList<int>());
    theSelectedIndices[aRow].append (aColumn);
  }
}

// =======================================================================
// function : SeparatorData
// purpose :
// =======================================================================

QString ViewControl_Table::SeparatorData()
{
  return ViewControl_Tools::TableSeparator();
}

// =======================================================================
// function : GetSelectedPresentations
// purpose :
// =======================================================================

void ViewControl_Table::GetSelectedPresentations (const TreeModel_ItemBasePtr& theTreeItem,
                                                  NCollection_List<Handle(Standard_Transient)>& theSelPresentations) const
{
  Handle(TreeModel_ItemProperties) anItemProperties = theTreeItem->GetProperties();
  if (anItemProperties.IsNull())
    return;

  QMap<int, QList<int>> aSelectedIndices;
  GetSelectedIndices (aSelectedIndices);

  for (QMap<int, QList<int>>::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    int aRowId = aSelIt.key();
    QList<int> aColIds = aSelIt.value();
    for (int aColId = 0; aColId < aColIds.size(); aColId++)
    {
      int aSelectedColId = aColIds[aColId];
      if (aSelectedColId != 1)
        continue;

      if (anItemProperties)
        anItemProperties->GetPresentations (aRowId, aSelectedColId, theSelPresentations);
    }
  }
}

// =======================================================================
// function : GetSelectedPointers
// purpose :
// =======================================================================

void ViewControl_Table::GetSelectedPointers (QStringList& thePointers) const
{
  QMap<int, QList<int>> aSelectedIndices;
  GetSelectedIndices (aSelectedIndices);

  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*>(GetTableView()->model());
  ViewControl_TableModelValues* aTableValues = aTableModel->GetModelValues();

  for (QMap<int, QList<int>>::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    int aRowId = aSelIt.key();
    QList<int> aColIds = aSelIt.value();
    for (int aColId = 0; aColId < aColIds.size(); aColId++)
    {
      int aSelectedColId = aColIds[aColId];
      if (aSelectedColId != 1)
        continue;

      QString aData = aTableValues->Data (aRowId, aSelectedColId, Qt::DisplayRole).toString();
      if (aData.contains (ViewControl_Tools::GetPointerPrefix().ToCString()))
        thePointers.append (aData);
    }
  }
}
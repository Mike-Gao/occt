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

#include <inspector/VInspector_Window.hxx>

#include <AIS_ColoredShape.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_Axis2Placement.hxx>
#include <TopoDS_Compound.hxx>

#include <TopExp_Explorer.hxx>

#include <inspector/TreeModel_ColumnType.hxx>
#include <inspector/TreeModel_ContextMenu.hxx>
#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <inspector/ViewControl_MessageDialog.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_Tools.hxx>
#include <inspector/ViewControl_TransientShape.hxx>

#include <inspector/VInspector_ToolBar.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>
#include <inspector/VInspector_CallBack.hxx>
#include <inspector/VInspector_Communicator.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemSelectBasicsEntityOwner.hxx>
#include <inspector/VInspector_ItemFolderObject.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_PreviewParameters.hxx>
#include <inspector/VInspector_PropertiesCreator.hxx>
#include <inspector/VInspector_TableModelValues.hxx>
#include <inspector/VInspector_ToolBar.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>
#include <inspector/VInspector_ViewModelHistory.hxx>

#include <inspector/VInspectorPaneAIS_PaneCreator.hxx>
#include <inspector/VInspectorPaneAIS_PropertiesCreator.hxx>

#include <inspector/ViewControl_PropertyView.hxx>
#include <inspector/ViewControl_TreeView.hxx>

#include <inspector/View_Displayer.hxx>
#include <inspector/View_Widget.hxx>
#include <inspector/View_Window.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QDockWidget>
#include <QHeaderView>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QTreeView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

//#define DEBUG_TWO_VIEWS
#ifdef DEBUG_TWO_VIEWS
#include <inspector/View_CameraPositionPrs.hxx>
#endif

const int VINSPECTOR_DEFAULT_PROPERTY_VIEW_WIDTH = 300;//600;
const int VINSPECTOR_DEFAULT_PROPERTY_VIEW_HEIGHT = 1000;

const int VINSPECTOR_DEFAULT_WIDTH  = 1250;
const int VINSPECTOR_DEFAULT_HEIGHT = 800;

const int VINSPECTOR_DEFAULT_POSITION_X = 200;
const int VINSPECTOR_DEFAULT_POSITION_Y = 60;

const int VINSPECTOR_DEFAULT_VIEW_WIDTH = 400;
const int VINSPECTOR_DEFAULT_VIEW_HEIGHT = 1000;

const int VINSPECTOR_DEFAULT_HISTORY_VIEW_WIDTH = 400;
const int VINSPECTOR_DEFAULT_HISTORY_VIEW_HEIGHT = 50;

const int VINSPECTOR_DEFAULT_VIEW_POSITION_X = 200 + 900 + 100; // TINSPECTOR_DEFAULT_POSITION_X + TINSPECTOR_DEFAULT_WIDTH + 100
const int VINSPECTOR_DEFAULT_VIEW_POSITION_Y = 60; // TINSPECTOR_DEFAULT_POSITION_Y + 50

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_Window::VInspector_Window()
: myParent (0), myExportToShapeViewDialog (0), myViewWindow (0)
{
  myPreviewParameters = new VInspector_PreviewParameters();

  myMainWindow = new QMainWindow (0);

  QWidget* aCentralWidget = new QWidget (myMainWindow);
  QGridLayout* aParentLay = new QGridLayout (aCentralWidget);
  aParentLay->setContentsMargins (0, 0, 0, 0);
  aParentLay->setSpacing(0);

  // restore state of tool bar: on the bottom of the window
  myToolBar = new VInspector_ToolBar(aCentralWidget);
  connect (myToolBar, SIGNAL (actionClicked (int)), this, SLOT (onToolBarActionClicked (int)));
  aParentLay->addWidget (myToolBar->GetControl(), 0, 0);

  // tree view
  myTreeView = new QTreeView (aCentralWidget);
  myTreeView->setSelectionBehavior (QAbstractItemView::SelectRows);
  myTreeView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  myTreeView->header()->setStretchLastSection (true);
  myTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
  //((ViewControl_TreeView*)myTreeView)->SetPredefinedSize (QSize (VINSPECTOR_DEFAULT_TREE_VIEW_WIDTH,
  //                                                               VINSPECTOR_DEFAULT_TREE_VIEW_HEIGHT));
  VInspector_ViewModel* aTreeModel = new VInspector_ViewModel (myTreeView);
  aTreeModel-> AddPropertiesCreator(new VInspector_PropertiesCreator());
  myTreeView->setModel (aTreeModel);
  // hide Visibility column
  TreeModel_HeaderSection anItem = aTreeModel->GetHeaderItem ((int)TreeModel_ColumnType_Visibility);
  anItem.SetIsHidden (true);
  aTreeModel->SetHeaderItem ((int)TreeModel_ColumnType_Visibility, anItem);

  connect (myTreeView, SIGNAL(customContextMenuRequested(const QPoint&)),
           this, SLOT (onTreeViewContextMenuRequested(const QPoint&)));
  new TreeModel_ContextMenu (myTreeView);

  QItemSelectionModel* aSelModel = new QItemSelectionModel (myTreeView->model(), myTreeView);
  myTreeView->setSelectionModel (aSelModel);
  connect (aSelModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
           this, SLOT (onTreeViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  aParentLay->addWidget(myTreeView, 1, 0);
  aParentLay->setRowStretch (1, 1);
  myMainWindow->setCentralWidget (aCentralWidget);

  // property view
  myPaneCreators.Append (new VInspectorPaneAIS_PaneCreator());
  aTreeModel->AddPropertiesCreator (new VInspectorPaneAIS_PropertiesCreator());

  myPropertyView = new ViewControl_PropertyView (myMainWindow,
    QSize(VINSPECTOR_DEFAULT_PROPERTY_VIEW_WIDTH, VINSPECTOR_DEFAULT_PROPERTY_VIEW_HEIGHT));
  myPropertyPanelWidget = new QDockWidget (tr ("PropertyPanel"), myMainWindow);
  myPropertyPanelWidget->setObjectName (myPropertyPanelWidget->windowTitle());
  myPropertyPanelWidget->setTitleBarWidget (new QWidget(myMainWindow));
  myPropertyPanelWidget->setWidget (myPropertyView->GetControl());
  myMainWindow->addDockWidget (Qt::RightDockWidgetArea, myPropertyPanelWidget);
  connect (myPropertyPanelWidget->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT (onPropertyPanelShown (bool)));
  connect (myPropertyView, SIGNAL (propertyViewSelectionChanged()), this, SLOT (onPropertyViewSelectionChanged ()));

  myHistoryView = new ViewControl_TreeView (myMainWindow);
  myHistoryView->setSelectionBehavior (QAbstractItemView::SelectRows);
  ((ViewControl_TreeView*)myHistoryView)->SetPredefinedSize (QSize (VINSPECTOR_DEFAULT_HISTORY_VIEW_WIDTH,
                                                                    VINSPECTOR_DEFAULT_HISTORY_VIEW_HEIGHT));
  myHistoryView->setContextMenuPolicy (Qt::CustomContextMenu);
  myHistoryView->header()->setStretchLastSection (true);
  new TreeModel_ContextMenu (myHistoryView);

  myHistoryView->setSelectionMode (QAbstractItemView::ExtendedSelection);
  VInspector_ViewModelHistory* aHistoryModel = new VInspector_ViewModelHistory (myHistoryView);
  myHistoryView->setModel (aHistoryModel);

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (aHistoryModel);
  myHistoryView->setSelectionModel (aSelectionModel);
  connect (aSelectionModel, SIGNAL (selectionChanged (const QItemSelection&, const QItemSelection&)),
    this, SLOT (onHistoryViewSelectionChanged (const QItemSelection&, const QItemSelection&)));

  anItem = aHistoryModel->GetHeaderItem (0);
  TreeModel_Tools::UseVisibilityColumn (myHistoryView, false);
  // hide Visibility column
  anItem = aHistoryModel->GetHeaderItem ((int)TreeModel_ColumnType_Visibility);
  anItem.SetIsHidden (true);
  aHistoryModel->SetHeaderItem ((int)TreeModel_ColumnType_Visibility, anItem);

  QModelIndex aParentIndex = myHistoryView->model()->index (0, 0);
  myHistoryView->setExpanded (aParentIndex, true);

  QDockWidget* aHistoryDockWidget = new QDockWidget (tr ("HistoryView"), myMainWindow);
  aHistoryDockWidget->setObjectName (aHistoryDockWidget->windowTitle());
  aHistoryDockWidget->setTitleBarWidget (new QWidget(myMainWindow));
  aHistoryDockWidget->setWidget (myHistoryView);
  myMainWindow->addDockWidget (Qt::BottomDockWidgetArea, aHistoryDockWidget);

  myMainWindow->resize (450, 800);
  myMainWindow->move (60, 20);

  myMainWindow->resize (VINSPECTOR_DEFAULT_WIDTH, VINSPECTOR_DEFAULT_HEIGHT);
  myMainWindow->move (VINSPECTOR_DEFAULT_POSITION_X, VINSPECTOR_DEFAULT_POSITION_Y);
}

// =======================================================================
// function : SetParent
// purpose :
// =======================================================================
void VInspector_Window::SetParent (void* theParent)
{
  myParent = (QWidget*)theParent;
  if (!myParent)
    return;

  QLayout* aLayout = myParent->layout();
  if (aLayout)
    aLayout->addWidget (GetMainWindow());
}

// =======================================================================
// function : FillActionsMenu
// purpose :
// =======================================================================
void VInspector_Window::FillActionsMenu (void* theMenu)
{
  QMenu* aMenu = (QMenu*)theMenu;
  QList<QDockWidget*> aDockwidgets = myMainWindow->findChildren<QDockWidget*>();
  for (QList<QDockWidget*>::iterator it = aDockwidgets.begin(); it != aDockwidgets.end(); ++it)
  {
    QDockWidget* aDockWidget = *it;
    if (aDockWidget->parentWidget() == myMainWindow)
      aMenu->addAction (aDockWidget->toggleViewAction());
  }
}

// =======================================================================
// function : GetPreferences
// purpose :
// =======================================================================
void VInspector_Window::GetPreferences (TInspectorAPI_PreferencesDataMap& theItem)
{
  theItem.Bind ("geometry",  TreeModel_Tools::ToString (myMainWindow->saveState()).toStdString().c_str());

  QMap<QString, QString> anItems;
  TreeModel_Tools::SaveState (myTreeView, anItems);
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
  {
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
  }

  anItems.clear();
  VInspector_PreviewParameters::SaveState (myPreviewParameters, anItems, "preview_parameters_");
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());

  anItems.clear();
  TreeModel_Tools::SaveState (myTreeView, anItems);
  for (QMap<QString, QString>::const_iterator anItemsIt = anItems.begin(); anItemsIt != anItems.end(); anItemsIt++)
  {
    theItem.Bind (anItemsIt.key().toStdString().c_str(), anItemsIt.value().toStdString().c_str());
  }
}

// =======================================================================
// function : SetPreferences
// purpose :
// =======================================================================
void VInspector_Window::SetPreferences (const TInspectorAPI_PreferencesDataMap& theItem)
{
  if (theItem.IsEmpty())
  {
    TreeModel_Tools::SetDefaultHeaderSections (myTreeView);
    TreeModel_Tools::SetDefaultHeaderSections (myHistoryView);
    return;
  }

  for (TInspectorAPI_IteratorOfPreferencesDataMap anItemIt (theItem); anItemIt.More(); anItemIt.Next())
  {
    if (anItemIt.Key().IsEqual ("geometry"))
      myMainWindow->restoreState (TreeModel_Tools::ToByteArray (anItemIt.Value().ToCString()));
    else if (TreeModel_Tools::RestoreState (myTreeView, anItemIt.Key().ToCString(), anItemIt.Value().ToCString()))
      continue;
    else if (TreeModel_Tools::RestoreState (myHistoryView, anItemIt.Key().ToCString(), anItemIt.Value().ToCString(),
                                            "history_view_"))
      continue;
    else if (VInspector_PreviewParameters::RestoreState (myPreviewParameters, anItemIt.Key().ToCString(), anItemIt.Value().ToCString(),
                                            "preview_parameters_"))
      continue;
  }
}

// =======================================================================
// function : UpdateContent
// purpose :
// =======================================================================
void VInspector_Window::UpdateContent()
{
  TCollection_AsciiString aName = "TKVInspector";

  bool isModelUpdated = false;
  if (myParameters->FindParameters (aName))
    isModelUpdated = Init (myParameters->Parameters (aName));
  if (myParameters->FindFileNames (aName))
  {
    for (NCollection_List<TCollection_AsciiString>::Iterator aFileNamesIt (myParameters->FileNames (aName));
         aFileNamesIt.More(); aFileNamesIt.Next())
         isModelUpdated = OpenFile (aFileNamesIt.Value()) || isModelUpdated;

    NCollection_List<TCollection_AsciiString> aNames;
    myParameters->SetFileNames (aName, aNames);
  }
  if (!isModelUpdated)
    UpdateTreeModel();

  // make AIS_InteractiveObject selected selected if exist in select parameters
  NCollection_List<Handle(Standard_Transient)> anObjects;
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*>(myTreeView->model());
  if (aViewModel && myParameters->GetSelectedObjects(aName, anObjects))
  {
    QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
    aSelectionModel->clear();
    for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (anObjects);
         aParamsIt.More(); aParamsIt.Next())
    {
      Handle(Standard_Transient) anObject = aParamsIt.Value();
      Handle(AIS_InteractiveObject) aPresentation = Handle(AIS_InteractiveObject)::DownCast (anObject);
      if (aPresentation.IsNull())
        continue;

      QModelIndex aPresentationIndex = aViewModel->FindIndex (aPresentation);
      if (!aPresentationIndex.isValid())
        continue;
       aSelectionModel->select (aPresentationIndex, QItemSelectionModel::Select);
       myTreeView->scrollTo (aPresentationIndex);
    }
  }

  if (!myCallBack.IsNull())
  {
    VInspector_ViewModelHistory* aHistoryModel = dynamic_cast<VInspector_ViewModelHistory*>
      (myHistoryView->model());
    aHistoryModel->Reset();
    aHistoryModel->EmitLayoutChanged();
  }
}

// =======================================================================
// function : GetSelectedPresentations
// purpose :
// =======================================================================
NCollection_List<Handle(AIS_InteractiveObject)> VInspector_Window::GetSelectedPresentations (QItemSelectionModel* theModel)
{
  NCollection_List<Handle(AIS_InteractiveObject)> aSelectedPresentations;

  QList<TreeModel_ItemBasePtr> anItems = TreeModel_ModelBase::GetSelectedItems (theModel->selectedIndexes());

  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  NCollection_List<Handle(Standard_Transient)> anItemPresentations;
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); anItemIt++)
  {
    TreeModel_ItemBasePtr anItem = *anItemIt;
    VInspector_ItemBasePtr aVItem = itemDynamicCast<VInspector_ItemBase>(anItem);
    if (!aVItem)
      continue;

    anItemPresentations.Clear();
    aVItem->GetPresentations (anItemPresentations);

    for (NCollection_List<Handle(Standard_Transient)>::Iterator anIt (anItemPresentations); anIt.More(); anIt.Next())
    {
      Handle(AIS_InteractiveObject) aPresentation = Handle(AIS_InteractiveObject)::DownCast (anIt.Value());
      if (aSelectedIds.contains ((size_t)aPresentation.operator->()))
        continue;
      aSelectedIds.append ((size_t)aPresentation.operator->());
      if (!aPresentation.IsNull())
        aSelectedPresentations.Append (aPresentation);
    }
  }
  return aSelectedPresentations;
}

// =======================================================================
// function : GetSelectedShapes
// purpose :
// =======================================================================
NCollection_List<TopoDS_Shape> VInspector_Window::GetSelectedShapes (const QModelIndexList& theIndices)
{
  NCollection_List<TopoDS_Shape> aSelectedShapes;

  QList<TreeModel_ItemBasePtr> anItems = TreeModel_ModelBase::GetSelectedItems (theIndices);
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); anItemIt++)
  {
    TreeModel_ItemBasePtr anItem = *anItemIt;
    VInspector_ItemBasePtr aVItem = itemDynamicCast<VInspector_ItemBase>(anItem);
    if (!aVItem /*|| aVItem->Row() == 0*/)
      continue;

    TopoDS_Shape aShape = aVItem->GetPresentationShape();
    if (aShape.IsNull())
      continue;

    aSelectedShapes.Append (aShape);
  }

  // obtain selection from the property panel
  {
    QList<ViewControl_Table*> aPropertyTables;
    myPropertyView->GetActiveTables (aPropertyTables);
    if (!aPropertyTables.isEmpty())
    {
      ViewControl_Table* aFirstTable = aPropertyTables[0]; // TODO: implement for several tables

      Handle(Graphic3d_TransformPers) aSelectedPersistent = GetSelectedTransformPers();
      QModelIndexList aTreeViewSelected = myTreeView->selectionModel()->selectedIndexes();
      GetSelectedPropertyPanelShapes(aTreeViewSelected,
                                     aFirstTable->GetTableView()->selectionModel()->selectedIndexes(),
                                     aSelectedShapes);
    }
  }

  return aSelectedShapes;
}

// =======================================================================
// function : GetSelectedPropertyPanelShapes
// purpose :
// =======================================================================
void VInspector_Window::GetSelectedPropertyPanelShapes (const QModelIndexList& theTreeViewIndices,
                                                        const QModelIndexList& thePropertyPanelIndices,
                                                        NCollection_List<TopoDS_Shape>& theShapes)
{
  QList<TreeModel_ItemBasePtr> anItems = TreeModel_ModelBase::GetSelectedItems (theTreeViewIndices);
  NCollection_List<Handle(Standard_Transient)> aPropertyPresentations;
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); anItemIt++)
  {
    TreeModel_ItemBasePtr anItem = *anItemIt;
    if (!anItem || anItem->Column() != 0)
      continue;

    QList<ViewControl_TableModelValues*> aTableValues;
    VInspector_Tools::GetPropertyTableValues (anItem, myPaneCreators, aTableValues);
    if (aTableValues.isEmpty())
      continue;

    Handle(TreeModel_ItemProperties) anItemProperties = anItem->GetProperties();
    for (int aTableIt = 0; aTableIt < aTableValues.size(); aTableIt++)
    {
      VInspector_TableModelValues* aTableVals = dynamic_cast<VInspector_TableModelValues*>(aTableValues[aTableIt]);
      if (!aTableVals)
        continue;

      // default shape by NULL selection
      aTableVals->GetPaneShapes (-1, -1, theShapes);

      for (QModelIndexList::const_iterator anIndicesIt = thePropertyPanelIndices.begin(); anIndicesIt != thePropertyPanelIndices.end(); anIndicesIt++)
      {
        QModelIndex anIndex = *anIndicesIt;
        aTableVals->GetPaneShapes (anIndex.row(), anIndex.column(), theShapes);

        if (!anItemProperties.IsNull())
        {
          anItemProperties->GetPresentations (anIndex.row(), anIndex.column(), aPropertyPresentations);
        }
      }
    }
  }

  if (!aPropertyPresentations.IsEmpty())
  {
    for (NCollection_List<Handle(Standard_Transient)>::Iterator aPrsIterator (aPropertyPresentations); aPrsIterator.More(); aPrsIterator.Next())
    {
      Handle(ViewControl_TransientShape) aShapePrs = Handle(ViewControl_TransientShape)::DownCast (aPrsIterator.Value());
      if (!aShapePrs.IsNull())
        theShapes.Append (aShapePrs->GetShape());
    }
  }
}

// =======================================================================
// function : GetSelectedTransformPers
// purpose :
// =======================================================================
Handle(Graphic3d_TransformPers) VInspector_Window::GetSelectedTransformPers()
{
  QList<TreeModel_ItemBasePtr> anItems = TreeModel_ModelBase::GetSelectedItems (myTreeView->selectionModel()->selectedIndexes());
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); anItemIt++)
  {
    TreeModel_ItemBasePtr anItem = *anItemIt;
    VInspector_ItemBasePtr aVItem = itemDynamicCast<VInspector_ItemBase>(anItem);
    if (!aVItem)
      continue;

    while (aVItem)
    {
      Handle(Graphic3d_TransformPers) aPers = aVItem->TransformPersistence();
      if (!aPers.IsNull())
        return aPers;

      aVItem = itemDynamicCast<VInspector_ItemBase>(aVItem->Parent());
    }
  }
  return Handle(Graphic3d_TransformPers)();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
bool VInspector_Window::Init (const NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return Standard_False;

  Handle(AIS_InteractiveContext) aContext;
  Handle(VInspector_CallBack) aCallBack;
  Standard_Boolean isModelUpdated = Standard_False;

  for (NCollection_List<Handle(Standard_Transient)>::Iterator aParamsIt (theParameters); aParamsIt.More(); aParamsIt.Next())
  {
    Handle(Standard_Transient) anObject = aParamsIt.Value();
    if (aContext.IsNull())
      aContext = Handle(AIS_InteractiveContext)::DownCast (anObject);

    if (aCallBack.IsNull())
      aCallBack = Handle(VInspector_CallBack)::DownCast (anObject);

    if (!Handle(ViewControl_PaneCreator)::DownCast (anObject).IsNull())
    {
      Handle(ViewControl_PaneCreator) aPaneCreator = Handle(ViewControl_PaneCreator)::DownCast (anObject);
      if (!myPaneCreators.Contains (aPaneCreator))
        myPaneCreators.Append (aPaneCreator);
      isModelUpdated = Standard_True;
    }
    if (!Handle(TreeModel_ItemPropertiesCreator)::DownCast (anObject).IsNull())
    {
      Handle(TreeModel_ItemPropertiesCreator) aPropCreator = Handle(TreeModel_ItemPropertiesCreator)::DownCast (anObject);
      VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*>(myTreeView->model());
      aViewModel->AddPropertiesCreator (aPropCreator);
      isModelUpdated = Standard_True;
    }
  }
  if (aViewModel->GetContext() != aContext)
    SetContext(aContext);
  else
    isModelUpdated = Standard_True;

  if (!aCallBack.IsNull() && aCallBack != myCallBack)
  {
    myCallBack = aCallBack;
    VInspector_ViewModelHistory* aHistoryModel = dynamic_cast<VInspector_ViewModelHistory*>
      (myHistoryView->model());
    myCallBack->SetContext(aContext);
    myCallBack->SetHistoryModel(aHistoryModel);
  }

  if (isModelUpdated)
    UpdateTreeModel();

  return true;
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void VInspector_Window::SetContext (const Handle(AIS_InteractiveContext)& theContext)
{
  if (theContext.IsNull())
    return;

  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  bool isFirst = aViewModel->GetContext().IsNull();

  aViewModel->SetContext (theContext);
  myTreeView->setExpanded (aViewModel->index (0, 0), true);

  if (!myCallBack.IsNull())
    myCallBack->SetContext (theContext);

  myPreviewParameters->GetDrawer()->Link (theContext->DefaultDrawer());

  if (isFirst)
    onExportToMessageView();
}

// =======================================================================
// function : OpenFile
// purpose :
// =======================================================================
bool VInspector_Window::OpenFile(const TCollection_AsciiString& theFileName)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return false;

  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
  bool isModelUpdated = false;
  if (aContext.IsNull())
  {
    aContext = createView();
    SetContext (aContext);
    isModelUpdated = true;
  }

  TopoDS_Shape aShape = VInspector_Tools::ReadShape (theFileName);
  if (aShape.IsNull())
    return isModelUpdated;

#ifndef DEBUG_COLORED_SHAPE
  Handle(AIS_Shape) aPresentation = new AIS_Shape (aShape);
#else
  Handle(AIS_ColoredShape) aPresentation = new AIS_ColoredShape (aShape);

  TopExp_Explorer expS(aShape, TopAbs_EDGE);
  for (; expS.More(); expS.Next())
  {
    aPresentation->SetCustomColor (expS.Current(), Quantity_Color (Quantity_NOC_GREEN));
  }
#endif
  View_Displayer* aDisplayer = myViewWindow->GetDisplayer();
  aDisplayer->DisplayPresentation (aPresentation);

  //aContext->Display (aPresentation, false);
  //aContext->Load (aPresentation, -1/*selection mode*/);
  aContext->UpdateCurrentViewer();

  UpdateTreeModel();
  myTreeView->setExpanded (aViewModel->index (0, 0), true);
  return true;
}

// =======================================================================
// function : onTreeViewContextMenuRequested
// purpose :
// =======================================================================
void VInspector_Window::onTreeViewContextMenuRequested(const QPoint& thePosition)
{
  QMenu* aMenu = new QMenu (GetMainWindow());
  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Export to ShapeView"), SLOT (onExportToShapeView()), GetMainWindow(), this));
  aMenu->addSeparator();

  QModelIndex anIndex = TreeModel_ModelBase::SingleSelected (myTreeView->selectionModel()->selectedIndexes(), 0);
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (anItemBase)
  {
    if (itemDynamicCast<VInspector_ItemContext> (anItemBase))
    {
      aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Export to MessageView"), SLOT (onExportToMessageView()), GetMainWindow(), this));
      aMenu->addSeparator();

      aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Default preview"), SLOT (onDefaultPreview()), GetMainWindow(), this));
    }
  }

  aMenu->addSeparator();
  for (int aTypeId = (int)VInspector_DisplayActionType_DisplayId; aTypeId <= (int)VInspector_DisplayActionType_RemoveId; aTypeId++)
    aMenu->addAction (ViewControl_Tools::CreateAction (VInspector_Tools::DisplayActionTypeToString ((VInspector_DisplayActionType) aTypeId),
                      SLOT (onDisplayActionTypeClicked()), GetMainWindow(), this));
  aMenu->addSeparator();

  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Expand"), SLOT (onExpand()), GetMainWindow(), this));
  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Expand All"), SLOT (onExpandAll()), GetMainWindow(), this));
  aMenu->addAction (ViewControl_Tools::CreateAction (tr ("Collapse All"), SLOT (onCollapseAll()), GetMainWindow(), this));

  QPoint aPoint = myTreeView->mapToGlobal (thePosition);
  aMenu->exec(aPoint);
}

// =======================================================================
// function : onToolBarActionClicked
// purpose :
// =======================================================================
void VInspector_Window::onToolBarActionClicked (const int theActionId)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;

  switch (theActionId)
  {
    case VInspector_ToolActionType_UpdateId:
    {
      UpdateTreeModel();
      break;
    }
    case VInspector_ToolActionType_SelectPresentationsId:
    {
      bool isChecked = myToolBar->GetToolButton ((VInspector_ToolActionType)theActionId)->isChecked();
      NCollection_List<Handle(AIS_InteractiveObject)> aPresentationsForViewer;
      if (isChecked)
        aPresentationsForViewer = GetSelectedPresentations (myTreeView->selectionModel());
      Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
      VInspector_Tools::AddOrRemovePresentations (aContext, aPresentationsForViewer);
      UpdateTreeModel();
      break;
    }
    case VInspector_ToolActionType_SelectOwnersId:
    {
      NCollection_List<Handle(SelectBasics_EntityOwner)> anOwnersForViewer;
      if (myToolBar->GetToolButton((VInspector_ToolActionType)theActionId)->isChecked())
        VInspector_ViewModel::GetSelectedOwners(myTreeView->selectionModel(), anOwnersForViewer);
      VInspector_Tools::AddOrRemoveSelectedShapes(aViewModel->GetContext(), anOwnersForViewer);
      UpdateTreeModel();
      break;
    }
    default:
      break;
  }
}

// =======================================================================
// function : onPropertyPanelShown
// purpose :
// =======================================================================
void VInspector_Window::onPropertyPanelShown (bool isToggled)
{
  if (!isToggled)
    return;

  updatePropertyPanelBySelection();
}

// =======================================================================
// function : onPropertyViewSelectionChanged
// purpose :
// =======================================================================
void VInspector_Window::onPropertyViewSelectionChanged()
{
  QList<ViewControl_Table*> aPropertyTables;
  myPropertyView->GetActiveTables (aPropertyTables);
  if (aPropertyTables.isEmpty())
    return;

  ViewControl_Table* aFirstTable = aPropertyTables[0]; // TODO: implement for several tables

  QMap<int, QList<int>> aSelectedIndices;
  aFirstTable->GetSelectedIndices (aSelectedIndices);

  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*>(aFirstTable->GetTableView()->model());
  ViewControl_TableModelValues* aTableValues = aTableModel->GetModelValues();

  QStringList aPointers;
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
        aPointers.append (aData);
    }
  }
  highlightTreeViewItems (aPointers);

  Handle(Graphic3d_TransformPers) aSelectedPersistent = GetSelectedTransformPers();
  QModelIndexList aTreeViewSelected = myTreeView->selectionModel()->selectedIndexes();
  NCollection_List<TopoDS_Shape> aSelectedShapes = GetSelectedShapes (aTreeViewSelected);

  GetSelectedPropertyPanelShapes(aTreeViewSelected,
                                 aFirstTable->GetTableView()->selectionModel()->selectedIndexes(),
                                 aSelectedShapes);

  updatePreviewPresentation(aSelectedShapes, aSelectedPersistent);
}

// =======================================================================
// function : onTreeViewSelectionChanged
// purpose :
// =======================================================================
void VInspector_Window::onTreeViewSelectionChanged (const QItemSelection&,
                                                    const QItemSelection&)
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  if (myToolBar->GetToolButton(VInspector_ToolActionType_SelectPresentationsId)->isChecked())
    onToolBarActionClicked(VInspector_ToolActionType_SelectPresentationsId);
  else if (myToolBar->GetToolButton(VInspector_ToolActionType_SelectOwnersId)->isChecked())
    onToolBarActionClicked(VInspector_ToolActionType_SelectOwnersId);

  if (myPropertyPanelWidget->toggleViewAction()->isChecked())
    updatePropertyPanelBySelection();

  Handle(Graphic3d_TransformPers) aSelectedPersistent = GetSelectedTransformPers();

  NCollection_List<TopoDS_Shape> aSelectedShapes = GetSelectedShapes (myTreeView->selectionModel()->selectedIndexes());
  updatePreviewPresentation(aSelectedShapes, aSelectedPersistent);

  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onHistoryViewSelectionChanged
// purpose :
// =======================================================================
void VInspector_Window::onHistoryViewSelectionChanged (const QItemSelection& theSelected,
                                                       const QItemSelection&)
{
  VInspector_ViewModelHistory* aHistoryModel = dynamic_cast<VInspector_ViewModelHistory*> (myHistoryView->model());
  if (!aHistoryModel)
    return;

  if (theSelected.size() == 0)
    return;

  QModelIndexList aSelectedIndices = theSelected.indexes();
  QStringList aPointers = aHistoryModel->GetSelectedPointers(aSelectedIndices.first());
  selectTreeViewItems (aPointers);
}

// =======================================================================
// function : onExportToShapeView
// purpose :
// =======================================================================
void VInspector_Window::onExportToMessageView()
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;
  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();

  TCollection_AsciiString aPluginName ("TKMessageView");
  NCollection_List<Handle(Standard_Transient)> aParameters;
  if (myParameters->FindParameters (aPluginName))
    aParameters = myParameters->Parameters (aPluginName);

  QStringList anExportedPointers;
  anExportedPointers.append (VInspector_Tools::GetPointerInfo (aContext, true).ToCString());
  aParameters.Append (aContext);

  myParameters->SetParameters (aPluginName, aParameters, false);//myExportToShapeViewDialog->IsAccepted());
}

// =======================================================================
// function : onExportToShapeView
// purpose :
// =======================================================================
void VInspector_Window::onExportToShapeView()
{
  const QModelIndexList anIndices;
  NCollection_List<TopoDS_Shape> aSelectedShapes = GetSelectedShapes (myTreeView->selectionModel()->selectedIndexes());

  TCollection_AsciiString aPluginName ("TKShapeView");
  NCollection_List<Handle(Standard_Transient)> aParameters;
  if (myParameters->FindParameters (aPluginName))
    aParameters = myParameters->Parameters (aPluginName);

  NCollection_List<TCollection_AsciiString> anItemNames;
  if (myParameters->FindSelectedNames (aPluginName))
    anItemNames = myParameters->GetSelectedNames (aPluginName);

  QStringList anExportedPointers;
  if (aSelectedShapes.Extent() > 0)
  {
    for (NCollection_List<TopoDS_Shape>::Iterator anIOIt (aSelectedShapes); anIOIt.More(); anIOIt.Next())
    {
      const TopoDS_Shape& aShape = anIOIt.Value();
      if (aShape.IsNull())
        continue;
      aParameters.Append (aShape.TShape());
      anItemNames.Append (TInspectorAPI_PluginParameters::ParametersToString(aShape));
      anExportedPointers.append (VInspector_Tools::GetPointerInfo (aShape.TShape(), true).ToCString());
    }
  }

  // seach for objects to be exported
  QList<TreeModel_ItemBasePtr> anItems = TreeModel_ModelBase::GetSelectedItems (myTreeView->selectionModel()->selectedIndexes());
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); anItemIt++)
  {
    TreeModel_ItemBasePtr anItem = *anItemIt;
    VInspector_ItemBasePtr aVItem = itemDynamicCast<VInspector_ItemBase>(anItem);
    if (!aVItem)
    continue;

    Handle(Standard_Transient) anObject = aVItem->GetObject();
    if (anObject.IsNull())
      continue;

    aParameters.Append (anObject);
    anItemNames.Append (anObject->DynamicType()->Name());
    anExportedPointers.append (VInspector_Tools::GetPointerInfo (anObject, true).ToCString());
  }

  if (anExportedPointers.isEmpty())
    return;

  TCollection_AsciiString aPluginShortName = aPluginName.SubString (3, aPluginName.Length());
  QString aMessage = QString ("Objects %1 are sent to %2.")
    .arg (anExportedPointers.join(", "))
    .arg (aPluginShortName.ToCString());
  QString aQuestion = QString ("Would you like to activate %1 immediately?\n")
    .arg (aPluginShortName.ToCString()).toStdString().c_str();
  if (!myExportToShapeViewDialog)
    myExportToShapeViewDialog = new ViewControl_MessageDialog (myParent, aMessage, aQuestion);
  else
    myExportToShapeViewDialog->SetInformation (aMessage);
  myExportToShapeViewDialog->Start();

  myParameters->SetSelectedNames (aPluginName, anItemNames);
  myParameters->SetParameters (aPluginName, aParameters, myExportToShapeViewDialog->IsAccepted());
}

// =======================================================================
// function : onDefaultPreview
// purpose :
// =======================================================================
void VInspector_Window::onDefaultPreview()
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;

  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
  if (aContext.IsNull())
    return;

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (gp_Pnt(25., 10., 0.)));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt(20., 20., 0.), gp_Pnt(30., 20., 10.)));
  //aBuilder.Add (aCompound, BRepBuilderAPI_MakeFace (gp_Pln (gp_Pnt (20., 30., 0.), gp_Dir (1., 0., 0.))).Face());
  aBuilder.Add (aCompound, VInspector_Tools::CreateBoxShape (gp_Pnt(20., 40., 0.), gp_Pnt(30., 60., 10.)));

  Handle(AIS_Shape) aDefaultPreview = new AIS_Shape (aCompound);
  aDefaultPreview->SetAttributes (myPreviewParameters->GetDrawer());
  if (!aContext.IsNull())
    aContext->Display (aDefaultPreview, AIS_Shaded, -1/*do not participate in selection*/, Standard_True);

  UpdateTreeModel();
}

// =======================================================================
// function : onDisplayActionTypeClicked
// purpose :
// =======================================================================
void VInspector_Window::onDisplayActionTypeClicked()
{
  QAction* anAction = (QAction*)sender();

  displaySelectedPresentations (VInspector_Tools::DisplayActionTypeFromString (anAction->text().toStdString().c_str()));
}

// =======================================================================
// function : onExpand
// purpose :
// =======================================================================
void VInspector_Window::onExpand()
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++)
  {
    int aLevels = 2;
    TreeModel_Tools::SetExpanded (myTreeView, aSelectedIndices[aSelectedId], true, aLevels);
  }
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onExpandAll
// purpose :
// =======================================================================
void VInspector_Window::onExpandAll()
{
  QApplication::setOverrideCursor (Qt::WaitCursor);

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int  aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++)
  {
    int aLevels = -1;
    TreeModel_Tools::SetExpanded (myTreeView, aSelectedIndices[aSelectedId], true, aLevels);
  }
  QApplication::restoreOverrideCursor();
}

// =======================================================================
// function : onCollapseAll
// purpose :
// =======================================================================
void VInspector_Window::onCollapseAll()
{
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  for (int aSelectedId = 0, aSize = aSelectedIndices.size(); aSelectedId < aSize; aSelectedId++)
  {
    int aLevels = -1;
    TreeModel_Tools::SetExpanded (myTreeView, aSelectedIndices[aSelectedId], false, aLevels);
  }
}

#ifdef DEBUG_TWO_VIEWS
// =======================================================================
// function : onViewLeftButtonDown
// purpose :
// =======================================================================
void VInspector_Window::onViewLeftButtonDown (const int theX, const int theY)
{
  myCameraPrs->StartTransformation (theX, theY);
}

// =======================================================================
// function : onViewLeftButtonUp
// purpose :
// =======================================================================
void VInspector_Window::onViewLeftButtonUp (const int theX, const int theY)
{
  myCameraPrs->StopTransformation (theX, theY);
}

// =======================================================================
// function : onViewMoveTo
// purpose :
// =======================================================================
void VInspector_Window::onViewMoveTo (const int theX, const int theY)
{
  View_Widget* aViewWidget = (View_Widget*) sender();

  myCameraPrs->Transform (theX, theY, aViewWidget->GetViewer()->GetView());

  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;
  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
  if (aContext.IsNull())
    return;
  aContext->Redisplay (myCameraPrs, Standard_True);
}

// =======================================================================
// function : onViewLeftButtonUp
// purpose :
// =======================================================================
void VInspector_Window::onViewLeftButtonUp()
{

}
#endif

// =======================================================================
// function : UpdateTreeModel
// purpose :
// =======================================================================
void VInspector_Window::UpdateTreeModel()
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (aViewModel)
    aViewModel->UpdateTreeModel();
}

// =======================================================================
// function : updatePropertyPanelBySelection
// purpose :
// =======================================================================
void VInspector_Window::updatePropertyPanelBySelection()
{
  QItemSelectionModel* aModel = myTreeView->selectionModel();
  if (!aModel)
    return;

  QModelIndexList aSelected = TreeModel_ModelBase::GetSelected (aModel->selectedIndexes(), 0);
  QList<ViewControl_TableModelValues*> aTableValues;

  if (aSelected.size() == 1)
  {
    TreeModel_ItemBasePtr aSelectedItem = TreeModel_ModelBase::GetItemByIndex(aSelected.first());
    VInspector_Tools::GetPropertyTableValues (aSelectedItem, myPaneCreators, aTableValues);
  }
  myPropertyView->Init (aTableValues);
}

// =======================================================================
// function : displaySelectedPresentations
// purpose :
// =======================================================================

void VInspector_Window::displaySelectedPresentations (const VInspector_DisplayActionType theType)
{
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aViewModel)
    return;

  Handle(AIS_InteractiveContext) aContext = aViewModel->GetContext();
  if (aContext.IsNull())
    return;

  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  if (!aSelectionModel)
    return;

  NCollection_List<Handle(AIS_InteractiveObject)> aSelectedPresentations = GetSelectedPresentations (aSelectionModel);
  const QModelIndexList& aSelectedIndices = aSelectionModel->selectedIndexes();

  bool aPreviewPresentationShown = !myPreviewPresentation.IsNull();
  // the order of objects returned by AIS_InteractiveContext is changed because the processed object is moved from
  // Erased to Displayed container or back
  aSelectionModel->clear();

  // redisplay preview presentation if exists
  if (aPreviewPresentationShown && theType == VInspector_DisplayActionType_RedisplayId)
  {
    QList<TreeModel_ItemBasePtr> anItems = TreeModel_ModelBase::GetSelectedItems (aSelectedIndices);
    for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); anItemIt++)
    {
      TreeModel_ItemBasePtr anItem = *anItemIt;
      VInspector_ItemBasePtr aVItem = itemDynamicCast<VInspector_ItemBase>(anItem);
      if (aVItem)
        aVItem->UpdatePresentationShape();
    }
    NCollection_List<TopoDS_Shape> aSelectedShapes = GetSelectedShapes (aSelectedIndices);
    updatePreviewPresentation(aSelectedShapes, GetSelectedTransformPers());
  }

  if (aSelectedPresentations.Extent() == 0)
    return;

  for (NCollection_List<Handle(AIS_InteractiveObject)>::Iterator anIOIt(aSelectedPresentations); anIOIt.More(); anIOIt.Next())
  {
    Handle(AIS_InteractiveObject) aPresentation = anIOIt.Value();
    switch (theType)
    {
      case VInspector_DisplayActionType_DisplayId:
      {
        aContext->Display(aPresentation, false);
        aContext->Load(aPresentation, -1);
      }
      break;

      case VInspector_DisplayActionType_RedisplayId: aContext->Redisplay (aPresentation, false); break;
      case VInspector_DisplayActionType_EraseId: aContext->Erase (aPresentation, false); break;
      case VInspector_DisplayActionType_RemoveId: aContext->Remove (aPresentation, false); break;
      default: break;
    }
  }
  aContext->UpdateCurrentViewer();

  UpdateTreeModel();
}

// =======================================================================
// function : highlightTreeViewItems
// purpose :
// =======================================================================
void VInspector_Window::highlightTreeViewItems (const QStringList& thePointers)
{
  VInspector_ViewModel* aTreeModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aTreeModel)
    return;

  QModelIndexList anIndices;
  aTreeModel->FindPointers (thePointers, QModelIndex(), anIndices);
  for (int anIndicesId = 0, aSize = anIndices.size(); anIndicesId < aSize; anIndicesId++)
  {
    QModelIndex anIndex = anIndices[anIndicesId];
    TreeModel_Tools::SetExpandedTo (myTreeView, anIndex);
  }
  aTreeModel->SetHighlighted (anIndices);

  if (!anIndices.isEmpty())
    myTreeView->scrollTo (anIndices.last());
}

// =======================================================================
// function : selectTreeViewItems
// purpose :
// =======================================================================
void VInspector_Window::selectTreeViewItems (const QStringList& thePointers)
{
  VInspector_ViewModel* aTreeModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (!aTreeModel)
    return;
  
  QModelIndexList anIndices;
  aTreeModel->FindPointers (thePointers, QModelIndex(), anIndices);
  QItemSelectionModel* aSelectionModel = myTreeView->selectionModel();
  aSelectionModel->clear();
  for (int anIndicesId = 0, aSize = anIndices.size(); anIndicesId < aSize; anIndicesId++)
  {
    QModelIndex anIndex = anIndices[anIndicesId];
    TreeModel_Tools::SetExpandedTo (myTreeView, anIndex);
    aSelectionModel->select (anIndex, QItemSelectionModel::Select);
  }
}

// =======================================================================
// function : createView
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) VInspector_Window::createView()
{
  // create two view windows
  Handle(AIS_InteractiveContext) aContext = View_Viewer::CreateStandardViewer();

  Handle(AIS_Trihedron) aTrihedron = new AIS_Trihedron (new Geom_Axis2Placement (gp::XOY()));
  aTrihedron->SetDatumDisplayMode (Prs3d_DM_Shaded);
  aContext->Display (aTrihedron, Standard_True);

  myViewWindow = new View_Window (0, aContext, false /*for opening several BREP files*/, true);
  myViewWindow->GetView()->SetPredefinedSize (VINSPECTOR_DEFAULT_VIEW_WIDTH, VINSPECTOR_DEFAULT_VIEW_HEIGHT);
  myViewWindow->move (VINSPECTOR_DEFAULT_VIEW_POSITION_X, VINSPECTOR_DEFAULT_VIEW_POSITION_Y);
  myViewWindow->show();

#ifdef DEBUG_TWO_VIEWS
  myViewWindow->move (VINSPECTOR_DEFAULT_VIEW_POSITION_X - 240, VINSPECTOR_DEFAULT_VIEW_POSITION_Y);

  View_Window* aSecondViewWindow = new View_Window (0, aContext);
  aSecondViewWindow->GetView()->SetPredefinedSize (VINSPECTOR_DEFAULT_VIEW_WIDTH, VINSPECTOR_DEFAULT_VIEW_HEIGHT);
  aSecondViewWindow->move (VINSPECTOR_DEFAULT_VIEW_POSITION_X + 220, VINSPECTOR_DEFAULT_VIEW_POSITION_Y);
  aSecondViewWindow->show();

  myCameraPrs = new View_CameraPositionPrs (aContext, 0);
  aContext->Display (myCameraPrs, Standard_True);

  View_Widget* aViewWidget = myViewWindow->GetView();
  connect (aViewWidget, SIGNAL (leftButtonDown (const int, const int)),
           this, SLOT (onViewLeftButtonDown (const int, const int)));
  connect (aViewWidget, SIGNAL (leftButtonUp (const int, const int)),
           this, SLOT (onViewLeftButtonUp (const int, const int)));
  connect (aViewWidget, SIGNAL (moveTo (const int, const int)),
           this, SLOT(onViewMoveTo (const int, const int)));
#endif

  return aContext;
}

// =======================================================================
// function : updatePreviewPresentation
// purpose :
// =======================================================================
void VInspector_Window::updatePreviewPresentation (const NCollection_List<TopoDS_Shape>& theShapes,
                                                   const Handle(Graphic3d_TransformPers)& thePersistent)
{
  Handle(AIS_InteractiveContext) aContext;
  VInspector_ViewModel* aViewModel = dynamic_cast<VInspector_ViewModel*> (myTreeView->model());
  if (aViewModel)
    aContext = aViewModel->GetContext();

  if (theShapes.IsEmpty())
  {
    if (!aContext.IsNull())
      aContext->Remove (myPreviewPresentation, Standard_True);
    myPreviewPresentation = NULL;
    return;
  }

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator (theShapes); anIterator.More(); anIterator.Next())
  {
    aBuilder.Add (aCompound, anIterator.Value());
  }

  if (myPreviewPresentation.IsNull())
  {
    myPreviewPresentation = new AIS_Shape (aCompound);
    myPreviewPresentation->SetAttributes (myPreviewParameters->GetDrawer());

    myPreviewPresentation->SetTransformPersistence(thePersistent);
    if (!aContext.IsNull())
      aContext->Display (myPreviewPresentation, AIS_Shaded, -1/*do not participate in selection*/, Standard_True);
  }
  else
  {
    Handle(AIS_Shape)::DownCast (myPreviewPresentation)->Set (aCompound);
    myPreviewPresentation->SetTransformPersistence(thePersistent);
    if (!aContext.IsNull())
      aContext->Redisplay (myPreviewPresentation, Standard_True);
  }
}

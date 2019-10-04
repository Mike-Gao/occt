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

                                                          
#include <inspector/MessageModel_Actions.hxx>

#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_Tools.hxx>
#include <inspector/MessageModel_TreeModel.hxx>
#include <inspector/TInspectorAPI_PluginParameters.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Message_AlertExtended.hxx>

#include <TCollection_AsciiString.hxx>
#include <TopoDS_AlertAttribute.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QMenu>
#include <QMessageBox>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

#define DEBUG_ALERTS

#ifdef DEBUG_ALERTS
#include <Message_Alerts.hxx>
#include <Message_PerfMeter.hxx>
#endif

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
MessageModel_Actions::MessageModel_Actions (QWidget* theParent,
  MessageModel_TreeModel* theTreeModel, QItemSelectionModel* theModel)
: QObject (theParent), myTreeModel (theTreeModel), mySelectionModel (theModel)
{
  myActions.insert (MessageModel_ActionType_Deactivate,
                    ViewControl_Tools::CreateAction ("Deactivate", SLOT (OnDeactivateReport()), parent(), this));
  myActions.insert (MessageModel_ActionType_Activate,
                    ViewControl_Tools::CreateAction ("Activate", SLOT (OnActivateReport()), parent(), this));
  myActions.insert (MessageModel_ActionType_Clear,
                    ViewControl_Tools::CreateAction ("Clear", SLOT (OnClearReport()), parent(), this));
  myActions.insert (MessageModel_ActionType_ExportToShapeView,
                    ViewControl_Tools::CreateAction (tr ("Export to ShapeView"), SLOT (OnExportToShapeView()), parent(), this));
#ifdef DEBUG_ALERTS
  myActions.insert (MessageModel_ActionType_TestMetric,
                    ViewControl_Tools::CreateAction ("Test <metric>", SLOT (OnTestMetric()), parent(), this));
  myActions.insert (MessageModel_ActionType_TestProperties,
                    ViewControl_Tools::CreateAction ("Test <PropertyPanel>", SLOT (OnTestPropertyPanel()), parent(), this));
#endif
}

// =======================================================================
// function : GetAction
// purpose :
// =======================================================================
QAction* MessageModel_Actions::GetAction (const MessageModel_ActionType& theType)
{
  if (myActions.contains (theType))
    return myActions[theType];

  return 0;
}

// =======================================================================
// function : AddMenuActions
// purpose :
// =======================================================================
void MessageModel_Actions::AddMenuActions (const QModelIndexList& theSelectedIndices, QMenu* theMenu)
{
  MessageModel_ItemRootPtr aRootItem;
  MessageModel_ItemReportPtr aReportItem;
  MessageModel_ItemAlertPtr anAlertItem;
  for (QModelIndexList::const_iterator aSelIt = theSelectedIndices.begin(); aSelIt != theSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    aRootItem = itemDynamicCast<MessageModel_ItemRoot> (anItemBase);
    if (aRootItem)
      break;

    aReportItem = itemDynamicCast<MessageModel_ItemReport> (anItemBase);
    if (aReportItem)
      break;

    anAlertItem = itemDynamicCast<MessageModel_ItemAlert> (anItemBase);
    if (anAlertItem)
      break;
  }

  if (aReportItem && !aReportItem->GetReport().IsNull())
  {
    theMenu->addAction (myActions[MessageModel_ActionType_Deactivate]);
    theMenu->addAction (myActions[MessageModel_ActionType_Activate]);
    theMenu->addAction (myActions[MessageModel_ActionType_Clear]);
#ifdef DEBUG_ALERTS
    theMenu->addAction (myActions[MessageModel_ActionType_TestMetric]);
    theMenu->addAction (myActions[MessageModel_ActionType_TestProperties]);
#endif
  }
  else if (anAlertItem)
  {
    theMenu->addAction (myActions[MessageModel_ActionType_ExportToShapeView]);
  }

  theMenu->addSeparator();
}

// =======================================================================
// function : onImportReport
// purpose :
// =======================================================================
Handle(Message_Report) MessageModel_Actions::getSelectedReport (QModelIndex& theReportIndex) const
{
  MessageModel_ItemReportPtr aReportItem;
  QModelIndexList aSelectedIndices = mySelectionModel->selectedIndexes();
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    aReportItem = itemDynamicCast<MessageModel_ItemReport> (anItemBase);
    theReportIndex = anIndex;
    if (aReportItem)
      break;
  }
  if (!aReportItem)
    return NULL;

  return aReportItem->GetReport();
}

// =======================================================================
// function : OnDeactivateReport
// purpose :
// =======================================================================
void MessageModel_Actions::OnDeactivateReport()
{
  QModelIndex aReportIndex;
  Handle(Message_Report) aReport = getSelectedReport (aReportIndex);
  if (aReport.IsNull())
    return;

  aReport->SetActive (Standard_False);
  ((MessageModel_TreeModel*)mySelectionModel->model())->EmitDataChanged (aReportIndex, aReportIndex);
}

// =======================================================================
// function : OnActivateReport
// purpose :
// =======================================================================
void MessageModel_Actions::OnActivateReport()
{
  QModelIndex aReportIndex;
  Handle(Message_Report) aReport = getSelectedReport (aReportIndex);
  if (aReport.IsNull())
    return;

  aReport->SetActive (Standard_True);
  ((MessageModel_TreeModel*)mySelectionModel->model())->EmitDataChanged (aReportIndex, aReportIndex);
}

// =======================================================================
// function : OnClearReport
// purpose :
// =======================================================================
void MessageModel_Actions::OnClearReport()
{
  QModelIndex aReportIndex;
  Handle(Message_Report) aReport = getSelectedReport (aReportIndex);
  if (aReport.IsNull())
    return;

  aReport->Clear();
  ((MessageModel_TreeModel*)mySelectionModel->model())->EmitDataChanged (aReportIndex, aReportIndex);
}

// =======================================================================
// function : OnExportToShapeView
// purpose :
// =======================================================================
void MessageModel_Actions::OnExportToShapeView()
{
  TCollection_AsciiString aPluginName ("TKShapeView");

  NCollection_List<Handle(Standard_Transient)> aPluginParameters;
  if (myParameters->FindParameters (aPluginName))
    aPluginParameters = myParameters->Parameters (aPluginName);
  NCollection_List<TCollection_AsciiString> anItemNames;
  if (myParameters->FindSelectedNames (aPluginName))
    anItemNames = myParameters->GetSelectedNames (aPluginName);

  QModelIndexList aSelectedIndices = mySelectionModel->selectedIndexes();
  QStringList anExportedPointers;
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() != 0)
      continue;

    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    if (!anItemBase)
      continue;

    MessageModel_ItemAlertPtr anAlertItem = itemDynamicCast<MessageModel_ItemAlert> (anItemBase);
    if (!anAlertItem)
      continue;

    Handle(Message_Alert) anAlert = anAlertItem->GetAlert();
    if (anAlert.IsNull())
      continue;

    Handle(Message_AlertExtended) anExtAlert = Handle(Message_AlertExtended)::DownCast (anAlert);
    if (anExtAlert.IsNull())
      continue;

    Handle(Message_Attribute) anAttribute = anExtAlert->Attribute();
    if (anAttribute.IsNull())
      continue;

    if (!anAttribute->IsKind (STANDARD_TYPE (TopoDS_AlertAttribute)))
      continue;

    const TopoDS_Shape aShape = Handle(TopoDS_AlertAttribute)::DownCast (anAttribute)->GetShape();
    if (aShape.IsNull())
      continue;
    aPluginParameters.Append (aShape.TShape());
    anItemNames.Append (TInspectorAPI_PluginParameters::ParametersToString (aShape));

    anExportedPointers.append (MessageModel_Tools::GetPointerInfo (aShape.TShape(), true).ToCString());
  }

  if (anExportedPointers.empty())
      return;
  myParameters->SetSelectedNames (aPluginName, anItemNames);
  myParameters->SetParameters (aPluginName, aPluginParameters);
  QMessageBox::information (0, "Information", QString ("TShapes '%1' are sent to %2 tool.")
    .arg (anExportedPointers.join (", ")).arg (QString (aPluginName.ToCString())));
}

// =======================================================================
// function : OnTestMetric
// purpose :
// =======================================================================
#include <OSD_Chronometer.hxx>
#include <ctime>
void MessageModel_Actions::OnTestMetric()
{
#ifdef DEBUG_ALERTS
  QModelIndex aReportIndex;
  Handle(Message_Report) aReport = getSelectedReport (aReportIndex);
  if (aReport.IsNull())
    return;

  Message_PerfMeter aPerfMeter;
  MESSAGE_INFO ("MessageModel_Actions::OnTestAlerts()", "", &aPerfMeter, NULL);
  unsigned int start_time =  clock();
  //Standard_Real aSystemSeconds, aCurrentSeconds;
  //OSD_Chronometer::GetThreadCPU (aCurrentSeconds, aSystemSeconds);

  Standard_Integer aCounter = 5000;//0;
  Standard_Real aValue = 0., aValue2 = 0.1;

  double* aMemValue;
  for (int aTopIt = 0; aTopIt < 4; aTopIt++)
  {
    for (int j = 0; j < aCounter; j++)
    {
      for (int i = 0; i < aCounter; i++)
      {
        aValue = (aValue * 2. + 3.) * 0.5 - 0.3 * 0.5;

        Standard_Real aValue3 = aValue + aValue2 * 0.2;
        (void)aValue3;

        aMemValue = new double;
      }
    }
    MESSAGE_INFO ("Calculate", "", &aPerfMeter, NULL);
  }

  //((MessageModel_TreeModel*)mySelectionModel->model())->EmitLayoutChanged();

  myTreeModel->UpdateTreeModel();

  //Standard_Real aSystemSeconds1, aCurrentSeconds1;
  //OSD_Chronometer::GetThreadCPU (aCurrentSeconds1, aSystemSeconds1);

  //std::cout << aValue << std::endl;
  //std::cout << "user time = " << aCurrentSeconds1 - aCurrentSeconds
  //          << ",  system time = " << aSystemSeconds1 - aSystemSeconds << std::endl;

  unsigned int end_time = clock();
  std::cout << "clock() = " << end_time - start_time << std::endl;
#endif
}

// =======================================================================
// function : OnTestPropertyPanel
// purpose :
// =======================================================================
void MessageModel_Actions::OnTestPropertyPanel()
{
#ifdef DEBUG_ALERTS
  QModelIndex aReportIndex;
  Handle(Message_Report) aReport = getSelectedReport (aReportIndex);
  if (aReport.IsNull())
    return;

  Message_PerfMeter aPerfMeter;
  MESSAGE_INFO ("MessageModel_Actions::OnTestAlerts()", "", &aPerfMeter, NULL);

  // gp_XYZ
  {
    gp_XYZ aCoords (1.3, 2.3, 3.4);
    Standard_SStream aStream;
    aCoords.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "gp_XYZ", "", &aPerfMeter, NULL);
  }
  // Bnd_Box
  {
    Bnd_Box aBox (gp_Pnt (20., 15., 10.), gp_Pnt (25., 20., 15.));
    Standard_SStream aStream;
    aBox.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "Bnd_Box", "", &aPerfMeter, NULL);
  }
  // Bnd_OBB
  {
    Bnd_OBB anOBB (gp_Pnt (-10., -15., -10.), gp_Dir (1., 0., 0.), gp_Dir (0., 1., 0.), gp_Dir (0., 0., 1.),
                  5., 10., 5.);
    Standard_SStream aStream;
    anOBB.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "Bnd_OBB", "", &aPerfMeter, NULL);
  }
  myTreeModel->UpdateTreeModel();
#endif
}

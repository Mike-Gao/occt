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

#include <inspector/MessageView_ActionsTest.hxx>

#include <inspector/MessageModel_ItemReport.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>
#include <inspector/MessageModel_Tools.hxx>
#include <inspector/MessageModel_TreeModel.hxx>
#include <inspector/TInspectorAPI_PluginParameters.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterToReport.hxx>

#include <Bnd_Box.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <OSD_Chronometer.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
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

//#define DEBUG_ALERTS

#ifdef DEBUG_ALERTS
#include <Message_Alerts.hxx>
#include <Message_Level.hxx>
#endif

#include <ctime>


// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
MessageView_ActionsTest::MessageView_ActionsTest (QWidget* theParent,
  MessageModel_TreeModel* theTreeModel, QItemSelectionModel* theModel)
: QObject (theParent), myTreeModel (theTreeModel), mySelectionModel (theModel)
{
  myActions.insert (MessageModel_ActionType_TestMetric,
                    ViewControl_Tools::CreateAction ("Test <metric>", SLOT (OnTestMetric()), parent(), this));
  myActions.insert (MessageModel_ActionType_TestProperties,
                    ViewControl_Tools::CreateAction ("Test <PropertyPanel>", SLOT (OnTestPropertyPanel()), parent(), this));
  myActions.insert (MessageModel_ActionType_TestMessenger,
                    ViewControl_Tools::CreateAction ("Test <Message_Messenger>", SLOT (OnTestMessenger()), parent(), this));
  myActions.insert (MessageModel_ActionType_TestReportTree,
                    ViewControl_Tools::CreateAction ("Test <Tree of messages>", SLOT (OnTestReportTree()), parent(), this));
}

// =======================================================================
// function : AddMenuActions
// purpose :
// =======================================================================
void MessageView_ActionsTest::AddMenuActions (const QModelIndexList& theSelectedIndices, QMenu* theMenu)
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
    theMenu->addAction (myActions[MessageModel_ActionType_TestMetric]);
    theMenu->addAction (myActions[MessageModel_ActionType_TestProperties]);
    theMenu->addAction (myActions[MessageModel_ActionType_TestMessenger]);
    theMenu->addAction (myActions[MessageModel_ActionType_TestReportTree]);

    bool isReportEnabled = aReportItem->GetReport()->IsActiveInMessenger();

    myActions[MessageModel_ActionType_TestMetric]->setEnabled (isReportEnabled);
    myActions[MessageModel_ActionType_TestProperties]->setEnabled (isReportEnabled);
    myActions[MessageModel_ActionType_TestMessenger]->setEnabled (isReportEnabled);
    myActions[MessageModel_ActionType_TestReportTree]->setEnabled (isReportEnabled);
  }
  theMenu->addSeparator();
}

// =======================================================================
// function : getSelectedReport
// purpose :
// =======================================================================
Handle(Message_Report) MessageView_ActionsTest::getSelectedReport (QModelIndex& theReportIndex) const
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
// function : OnTestMetric
// purpose :
// =======================================================================
void MessageView_ActionsTest::OnTestMetric()
{
#ifdef DEBUG_ALERTS
  QModelIndex aReportIndex;
  Handle(Message_Report) aReport = getSelectedReport (aReportIndex);
  if (aReport.IsNull())
    return;

  MESSAGE_ADD_LEVEL_SENTRY
  MESSAGE_INFO ("MessageModel_Actions::OnTestMetric()");
  unsigned int start_time =  clock();
  //Standard_Real aSystemSeconds, aCurrentSeconds;
  //OSD_Chronometer::GetThreadCPU (aCurrentSeconds, aSystemSeconds);

  Standard_Integer aCounter = 1500;//0;
  Standard_Real aValue = 0., aValue2 = 0.1;

  double* aMemValue;
  for (int aTopIt = 0; aTopIt < 4; aTopIt++)
  {
    MESSAGE_INFO ("Calculate");
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
void MessageView_ActionsTest::OnTestPropertyPanel()
{
#ifdef DEBUG_ALERTS
  QModelIndex aReportIndex;
  Handle(Message_Report) aReport = getSelectedReport (aReportIndex);
  if (aReport.IsNull())
    return;

  MESSAGE_ADD_LEVEL_SENTRY
  MESSAGE_INFO ("MessageModel_Actions::OnTestPropertyPanel()");

  // gp_XYZ
  {
    gp_XYZ aCoords (1.3, 2.3, 3.4);
    Standard_SStream aStream;
    aCoords.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "gp_XYZ");
  }
  // gp_Dir
  {
    gp_Dir aDir (0.3, 0.3, 0.4);
    Standard_SStream aStream;
    aDir.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "gp_Dir");
  }
  // gp_Ax1
  {
    gp_Ax1 aCoords (gp_Pnt (1.3, 2.3, 3.4), gp_Dir (0.3, 0.3, 0.4));
    Standard_SStream aStream;
    aCoords.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "gp_Ax1");
  }
  // gp_Ax2
  {
    gp_Ax2 aCoords (gp_Pnt (10.3, 20.3, 30.4), gp_Dir (0.3, 0.3, 0.4));
    Standard_SStream aStream;
    aCoords.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "gp_Ax2");
  }
  // gp_Ax3
  {
    gp_Ax3 aPln (gp_Pnt (10., 20., 15.), gp_Dir (0., 0., 1.), gp_Dir (1., 0., 0.));
    Standard_SStream aStream;
    aPln.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "gp_Ax3");
  }
  // gp_Trsf
  {
    gp_Trsf aTrsf;
    aTrsf.SetRotation (gp::OZ(), 0.3);
    aTrsf.SetTranslationPart (gp_Vec (15., 15., 15.));
    aTrsf.SetScaleFactor (3.);

    Standard_SStream aStream;
    aTrsf.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "gp_Trsf");
  }
  // Bnd_Box
  {
    Bnd_Box aBox (gp_Pnt (20., 15., 10.), gp_Pnt (25., 20., 15.));
    Standard_SStream aStream;
    aBox.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "Bnd_Box");
  }
  // Bnd_OBB
  {
    Bnd_OBB anOBB (gp_Pnt (-10., -15., -10.), gp_Dir (1., 0., 0.), gp_Dir (0., 1., 0.), gp_Dir (0., 0., 1.),
                  5., 10., 5.);
    Standard_SStream aStream;
    anOBB.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "Bnd_OBB");
  }
  // Quantity_ColorRGBA
  {
    Quantity_ColorRGBA aColor (0.2f, 0.8f, 0.8f, 0.2f);
    Standard_SStream aStream;
    aColor.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "Quantity_ColorRGBA");
  }
  // Quantity_Color
  {
    Quantity_Color aColor (0.8, 0.8, 0.8, Quantity_TOC_RGB);
    Standard_SStream aStream;
    aColor.DumpJson (aStream);
    MESSAGE_INFO_STREAM(aStream, "Quantity_Color");
  }

  // stream of some table values
  {
    Standard_SStream aStream;
    OCCT_DUMP_FIELD_VALUES_NUMERICAL (aStream, "value_1", 1, 100);
    OCCT_DUMP_FIELD_VALUES_STRING (aStream, "value_2", 2, "value_1", "value_2");

    MESSAGE_INFO_STREAM(aStream, "Table: Name to value");
  }

  // SHAPE messages
  {
    BRepBuilderAPI_MakeEdge aBuilder (gp_Pnt (0., 0., 0.), gp_Pnt (20., 10., 20.));
    TopoDS_Shape aShape = aBuilder.Shape();

    MESSAGE_INFO_SHAPE (aShape, "Shape message edge");
  }

  myTreeModel->UpdateTreeModel();
#endif
}

// =======================================================================
// function : OnTestMessenger
// purpose :
// =======================================================================
void MessageView_ActionsTest::OnTestMessenger()
{
  // string messages
  const Handle(Message_Messenger)& aMessenger = Message::DefaultMessenger();

  MESSAGE_ADD_LEVEL_SENTRY
  aMessenger << "MessageModel_Actions::OnTestMessenger()" << Message_EndLine;

  aMessenger << "IGESBasic_Hierarchy" << Message_EndLine;
  aMessenger << "Number of property valueaMessenger : " << 15 << Message_EndLine;
  aMessenger << "Line Font    : " << 1 << Message_EndLine << "View Number  : " << 3 << Message_EndLine;
  aMessenger << "Entity level : " << 1 << Message_EndLine;
  aMessenger << "Blank statuaMessenger : " << 0 << Message_EndLine;
  aMessenger << "Line weight  : " << 14 << Message_EndLine;
  aMessenger << "Color number : " << 5 << Message_EndLine;

  // stream messages
  // gp_XYZ
  {
    gp_XYZ aCoords (1.3, 2.3, 3.4);
    Standard_SStream aStream;
    aCoords.DumpJson (aStream);
    aMessenger << "gp_XYZ" << aStream << Message_EndLine;
  }
  // Bnd_Box
  {
    Bnd_Box aBox (gp_Pnt (20., 15., 10.), gp_Pnt (25., 20., 15.));
    Standard_SStream aStream;
    aBox.DumpJson (aStream);
    aMessenger << "Bnd_Box" << aStream;
  }

  // object messages
  Handle(Standard_Transient) anObject = new Message_AlertExtended();
  aMessenger << "Message_AlertExtended" << anObject;

  // shape messages
  {
    BRepBuilderAPI_MakeEdge aBuilder (gp_Pnt (0., 0., 0.), gp_Pnt (20., 10., 20.));
    TopoDS_Shape aShape = aBuilder.Shape();

    MESSAGE_INFO_SHAPE (aShape, "Shape message edge");

    aMessenger << "Shape message edge" << aShape;
  }
  myTreeModel->UpdateTreeModel();
}

// =======================================================================
// function : levelAlerts
// purpose :
// =======================================================================
void levelAlerts (const int theCurrentLevel, const int theTopLevel)
{
  if (theTopLevel - theCurrentLevel <= 0)
    return;

  MESSAGE_ADD_LEVEL_SENTRY

  const Handle(Message_Messenger)& aMessenger = Message::DefaultMessenger();
  aMessenger << "Level: " << theCurrentLevel << Message_EndLine;
  aMessenger << "Alert: " << 1 << ", " << 2 << Message_EndLine;
  aMessenger << "Alert: " << 3 << ", " << 4 << Message_EndLine;

  for (int i = 0; i < 2; i++)
    levelAlerts (theCurrentLevel + 1, theTopLevel);

  aMessenger << "Alert: " << 4 << ", " << 5 << Message_EndLine;
}

// =======================================================================
// function : levelAlert
// purpose :
// =======================================================================
void levelAlert (const int theCurrentLevel, const int theTopLevel)
{
  if (theTopLevel - theCurrentLevel <= 0)
    return;

  MESSAGE_ADD_LEVEL_SENTRY

  const Handle(Message_Messenger)& aMessenger = Message::DefaultMessenger();
  aMessenger << "Level: " << theCurrentLevel << "(Single, no alerts on the level)" << Message_EndLine;

  for (int i = 0; i < 2; i++)
    levelAlerts (theCurrentLevel + 1, theTopLevel);
}

// =======================================================================
// function : OnTestMessenger
// purpose :
// =======================================================================
void MessageView_ActionsTest::OnTestReportTree()
{
  MESSAGE_ADD_LEVEL_SENTRY
  const Handle(Message_Messenger)& aMessenger = Message::DefaultMessenger();
  aMessenger << "MessageModel_Actions::OnTestReportTree()" << Message_EndLine;

  // string messages
  //aMessenger << "Alert: " << 1 << Message_EndLine;
  //aMessenger << "Alert: " << 2 << Message_EndLine;

  int aTopLevel = 3;
  levelAlerts (1, aTopLevel);

  //aMessenger << "Alert: " << 3 << Message_EndLine;
  //levelAlerts (1, aTopLevel);

  aMessenger << "Alert: " << 4 << Message_EndLine;
  levelAlert (1, aTopLevel);

  myTreeModel->UpdateTreeModel();
}


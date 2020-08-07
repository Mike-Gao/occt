// Copyright (c) 2020 OPEN CASCADE SAS
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

#include "ApplicationCommon.h"
#include "DocumentCommon.h"
#include "View.h"
#include "GeometrySamples.h"
#include "TopologySamples.h"
#include "TriangulationSamples.h"
#include "DataExchangeSamples.h"
#include "OcafSamples.h"
#include "Viewer3dSamples.h"
#include "Viewer2dSamples.h"

#include <QtGlobal>
#include <QApplication>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QStatusBar>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QPair>


#include <Standard_WarningsDisable.hxx>
#include <Standard_WarningsRestore.hxx>


#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>

#include <stdlib.h>
#include <memory>

static QMdiArea* stWs = 0;

ApplicationCommonWindow::ApplicationCommonWindow(ApplicationType theSampleType)
  : QMainWindow(nullptr),
  APP_TYPE(theSampleType),
  mySampleMapper(new QSignalMapper(this)),
  myExchangeMapper(new QSignalMapper(this)),
  myOcafMapper(new QSignalMapper(this)),
  myViewer3dMapper(new QSignalMapper(this)),
  myViewer2dMapper(new QSignalMapper(this)),
  myStdToolBar(nullptr),
  myCasCadeBar(nullptr),
  myViewBar(nullptr),
  myFilePopup(nullptr),
  mySamples(nullptr)
{
  setWindowTitle(GetTitle());

  switch (APP_TYPE)
  {
  case ApplicationType::Geometry:
    mySamples = new GeometrySamples();
    MenuFormJson(":/menus/Geometry.json", mySampleMapper);
    break;
  case ApplicationType::Topology:
    mySamples = new TopologySamples();
    MenuFormJson(":/menus/Topology.json", mySampleMapper);
    break;
  case ApplicationType::Triangulation:
    mySamples = new TriangulationSamples();
    MenuFormJson(":/menus/Triangulation.json", mySampleMapper);
    break;
  case ApplicationType::DataExchange:
    mySamples = new DataExchangeSamples();
    MenuFormJson(":/menus/DataExchange.json", myExchangeMapper);
    break;
  case ApplicationType::Ocaf:
    mySamples = new OcafSamples();
    MenuFormJson(":/menus/Ocaf.json", myOcafMapper);
    break;
  case ApplicationType::Viewer3d:
    mySamples = new Viewer3dSamples();
    MenuFormJson(":/menus/Viewer3d.json", myViewer3dMapper);
    break;
  case ApplicationType::Viewer2d:
    mySamples = new Viewer2dSamples();
    MenuFormJson(":/menus/Viewer2d.json", myViewer2dMapper);
    break;
  default:
    setWindowTitle("Unknown application");
    return;
  }

  connect(mySampleMapper,   static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessSample);
  connect(myExchangeMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessExchange);  
  connect(myOcafMapper,     static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessOcaf);  
  connect(myViewer3dMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessViewer3d);
  connect(myViewer2dMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessViewer2d);
  TCollection_AsciiString aSampleSourcePach = getSampleSourceDir();
  mySamples->SetCodePach(aSampleSourcePach);

  setFocusPolicy(Qt::StrongFocus);

  QFont aCodeViewFont;
  aCodeViewFont.setFamily("Courier");
  aCodeViewFont.setFixedPitch(true);
  aCodeViewFont.setPointSize(10);

  myCodeView = new QTextEdit;
  myCodeView->setDocumentTitle("Code");
  myCodeView->setLineWrapMode(QTextEdit::NoWrap);
  myCodeView->setReadOnly(true);
  myCodeView->setFont(aCodeViewFont);
  myCodeViewHighlighter = new OcctHighlighter(myCodeView->document());

  myResultView = new QTextEdit;
  myResultView->setDocumentTitle("Output");
  myResultView->setReadOnly(true);
  myResultView->setFont(aCodeViewFont);

  QSplitter* aCodeResultSplitter = new QSplitter(Qt::Vertical);
  aCodeResultSplitter->addWidget(myCodeView);
  aCodeResultSplitter->addWidget(myResultView);

  myDocument3d = createNewDocument();
  myDocument2d = createNewDocument();
  myGeomWidget = new GeomWidget(myDocument3d, myDocument2d, this);
  myGeomWidget->setContentsMargins(0, 0, 0, 0);
  QSplitter* aGeomTextSplitter = new QSplitter(Qt::Horizontal);

  aGeomTextSplitter->addWidget(myGeomWidget);
  aGeomTextSplitter->addWidget(aCodeResultSplitter);
  aGeomTextSplitter->setStretchFactor(0, 1);
  aGeomTextSplitter->setStretchFactor(1, 1);
  QList<int> aSizeList{ 640, 640 };
  aGeomTextSplitter->setSizes(aSizeList);
  setCentralWidget(aGeomTextSplitter);

  Q_INIT_RESOURCE(Samples);

  createStandardOperations();

  if (APP_TYPE == ApplicationType::DataExchange)
  {
    Handle(DataExchangeSamples) aDataExchangeSamples = Handle(DataExchangeSamples)::DownCast(mySamples);
    if (aDataExchangeSamples)
    {
      aDataExchangeSamples->AppendBottle();
      aDataExchangeSamples->SetView(myGeomWidget->Get3dView());
      myDocument3d->SetObjects(mySamples->Get3dObjects());
      myGeomWidget->FitAll();
    }
  }
  else if (APP_TYPE == ApplicationType::Ocaf)
  {
    Handle(OcafSamples) aOcafSamples = Handle(OcafSamples)::DownCast(mySamples);
    if (aOcafSamples)
    {
      aOcafSamples->SetContext(myDocument3d->getContext());
      aOcafSamples->SetViewer(myDocument3d->getViewer());
      onProcessOcaf("CreateOcafDocument");
      myGeomWidget->Show3d();
    }
  }
  else if (APP_TYPE == ApplicationType::Viewer3d)
  {
    Handle(Viewer3dSamples) aViewer3dSamples = Handle(Viewer3dSamples)::DownCast(mySamples);
    if (aViewer3dSamples)
    {
      aViewer3dSamples->SetContext(myDocument3d->getContext());
      aViewer3dSamples->AppendBottle();
      aViewer3dSamples->SetView(myGeomWidget->Get3dView());
      myDocument3d->SetObjects(mySamples->Get3dObjects());
      myGeomWidget->FitAll();
    }
  }
  else if (APP_TYPE == ApplicationType::Viewer2d)
  {
    Handle(Viewer2dSamples) aViewer2dSamples = Handle(Viewer2dSamples)::DownCast(mySamples);
    if (aViewer2dSamples)
    {
      aViewer2dSamples->SetContext(myDocument2d->getContext());
      aViewer2dSamples->SetView(myGeomWidget->Get2dView());
      aViewer2dSamples->SetViewer(myDocument2d->getViewer());
      myDocument2d->SetObjects(mySamples->Get2dObjects());
      myGeomWidget->Show2d();
    }
  }

  resize(1280, 560);
}

ApplicationCommonWindow::ApplicationType ApplicationCommonWindow::AppTypeFromString(QString theParameter)
{
  if (theParameter == "Geometry")
    return ApplicationType::Geometry;
  else if (theParameter == "Topology")
    return ApplicationType::Topology;
  else if (theParameter == "Triangulation")
    return ApplicationType::Triangulation;
  else if (theParameter == "DataExchange")
    return ApplicationType::DataExchange;
  else if (theParameter == "Ocaf")
    return ApplicationType::Ocaf;
  else  if (theParameter == "Viewer3d")
    return ApplicationType::Viewer3d;
  else  if (theParameter == "Viewer2d")
    return ApplicationType::Viewer2d;
  else  
    return ApplicationType::Unknokwn;
}

QString ApplicationCommonWindow::GetTitle()
{
  switch (APP_TYPE)
  {
  case ApplicationType::Geometry:
    return "Geometry";
  case ApplicationType::Topology:
    return "Topology";
  case ApplicationType::Triangulation:
    return "Triangulation";
  case ApplicationType::DataExchange:
    return "DataExchange";
  case ApplicationType::Ocaf:
    return "OCAF";
  case ApplicationType::Viewer3d:
    return "3D viewer";
  case ApplicationType::Viewer2d:
    return "2D Viewer";
  default:
    return "Unknown application";
  }
}

void ApplicationCommonWindow::createStandardOperations()
{

  QAction* fileQuitAction = CreateAction(&ApplicationCommonWindow::onCloseAllWindows, "Quit", "CTRL+Q");
  myStdActions.insert(StdActions::FileQuit, fileQuitAction );

  QAction* helpAboutAction = CreateAction(&ApplicationCommonWindow::onAbout, "About", "F1", ":/icons/help.png");
  myStdActions.insert(StdActions::HelpAbout, helpAboutAction);

  // populate a menu with all actions
  myFilePopup = new QMenu( this );
  myFilePopup = menuBar()->addMenu(tr("&File"));
  myFilePopup->addAction( fileQuitAction );

  for(QMenu* aSampleMenu: mySamplePopups)
  {
    menuBar()->addMenu(aSampleMenu);
  }

  // add a help menu
  QMenu * help = new QMenu( this );
  menuBar()->addSeparator();
  help = menuBar()->addMenu(tr("&Help"));
  help->addAction( helpAboutAction );
}

QAction*  ApplicationCommonWindow::getToolAction(ToolActions theAction)
{
    return myToolActions.value(theAction);
}

QList<QAction*> ApplicationCommonWindow::getMaterialActions()
{
    return myMaterialActions.values();
}



DocumentCommon* ApplicationCommonWindow::createNewDocument()
{
  return new DocumentCommon(this);
}

void ApplicationCommonWindow::onAbout()
{
  QMessageBox::information( this, tr("Tutorial"), 
                            tr("Qt based application to study OpenCASCADE Technology"), 
                            tr("Ok" ), QString::null, QString::null, 0, 0 );
}

QString ApplicationCommonWindow::getResourceDir()
{
  static QString aResourceDir =
    QString (OSD_Environment ("CSF_ResourcesDefaults").Value().ToCString());
  if (aResourceDir.isEmpty())
    aResourceDir = QString (OSD_Environment ("CSF_OCCTResourcePath").Value().ToCString()) + "/samples";

  return aResourceDir;
}

TCollection_AsciiString  ApplicationCommonWindow::getSampleSourceDir()
{
  TCollection_AsciiString aSampleSourceDir = OSD_Environment ("CSF_SampleSources").Value();
  return aSampleSourceDir;
}


template <typename PointerToMemberFunction>
QAction* ApplicationCommonWindow::CreateAction(PointerToMemberFunction theHandlerMethod,
                                               QString theActionName, 
                                               QString theShortcut,
                                               QString theIconName)
{
  QAction* aAction(NULL);
  if (!theIconName.isEmpty())
  {
    QPixmap aIcon = QPixmap(theIconName);
    aAction = new QAction(aIcon, theActionName, this);
  }
  else
  {
    aAction = new QAction(theActionName, this);
  }
  aAction->setToolTip(theActionName);
  aAction->setStatusTip(theActionName);
  aAction->setShortcut(theShortcut);
  connect(aAction, &QAction::triggered, this, theHandlerMethod);
  return aAction;
}

template <typename PointerToMemberFunction>
QAction* ApplicationCommonWindow::CreateSample(PointerToMemberFunction theHandlerMethod,
                                                     const char* theActionName)
{
  QAction* aAction = new QAction(QObject::tr(theActionName), this);
  connect(aAction, &QAction::triggered, this, theHandlerMethod);
  return aAction;
}

void ApplicationCommonWindow::resizeEvent( QResizeEvent* e )
{
    QMainWindow::resizeEvent( e );
    statusBar()->setSizeGripEnabled( !isMaximized() );
}

QMenu* ApplicationCommonWindow::getFilePopup()
{
  return myFilePopup;
}

QToolBar* ApplicationCommonWindow::getCasCadeBar()
{
  return myCasCadeBar;
}

void ApplicationCommonWindow::onProcessSample(const QString& theSampleName)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  setWindowTitle(GetTitle() + " - " + theSampleName);
  mySamples->Process(theSampleName.toUtf8().data());
  myDocument3d->SetObjects(mySamples->Get3dObjects());
  myDocument2d->SetObjects(mySamples->Get2dObjects());
  myCodeView->setPlainText(mySamples->GetCode().ToCString());
  myResultView->setPlainText(mySamples->GetResult().ToCString());
  myGeomWidget->FitAll();
  QApplication::restoreOverrideCursor();
}

void ApplicationCommonWindow::onProcessExchange(const QString& theSampleName)
{
  setWindowTitle(GetTitle() + " - " + theSampleName);
  int aMode;
  QString aFileName = selectFileName(theSampleName, getDataExchangeDialog(theSampleName), aMode);
  Handle(DataExchangeSamples) aDataExchangeSamples = Handle(DataExchangeSamples)::DownCast(mySamples);
  if (aDataExchangeSamples)
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    aDataExchangeSamples->SetFileName(aFileName.toUtf8().data());
    aDataExchangeSamples->SetStepType(static_cast<STEPControl_StepModelType>(aMode));
    mySamples->Process(theSampleName.toUtf8().data());
    myDocument3d->SetObjects(mySamples->Get3dObjects());
    myDocument2d->SetObjects(mySamples->Get2dObjects());
    myCodeView->setPlainText(mySamples->GetCode().ToCString());
    myResultView->setPlainText(mySamples->GetResult().ToCString());
    myGeomWidget->FitAll();
    QApplication::restoreOverrideCursor();
  }
}

void ApplicationCommonWindow::onProcessOcaf(const QString& theSampleName)
{
  setWindowTitle(GetTitle() + " - " + theSampleName);
  Handle(OcafSamples) aOcafSamples = Handle(OcafSamples)::DownCast(mySamples);
  if (aOcafSamples)
  {
    if (theSampleName.indexOf("Dialog") == 0)
    {
      int aMode; // not used
      QString aFileName = selectFileName(theSampleName, getOcafDialog(theSampleName), aMode);
      aOcafSamples->SetFileName(aFileName.toUtf8().data());
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    mySamples->Process(theSampleName.toUtf8().data());
    myDocument2d->SetObjects(mySamples->Get2dObjects());
    myCodeView->setPlainText(mySamples->GetCode().ToCString());
    myResultView->setPlainText(mySamples->GetResult().ToCString());
    QApplication::restoreOverrideCursor();
  }
}

void ApplicationCommonWindow::onProcessViewer3d(const QString& theSampleName)
{
  setWindowTitle(GetTitle() + " - " + theSampleName);
  Handle(Viewer3dSamples) aViewer3dSamples = Handle(Viewer3dSamples)::DownCast(mySamples);
  if (aViewer3dSamples)
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    mySamples->Process(theSampleName.toUtf8().data());
    myCodeView->setPlainText(mySamples->GetCode().ToCString());
    myResultView->setPlainText(mySamples->GetResult().ToCString());
    myGeomWidget->FitAll();
    QApplication::restoreOverrideCursor();
  }
}

void ApplicationCommonWindow::onProcessViewer2d(const QString& theSampleName)
{
  setWindowTitle(GetTitle() + " - " + theSampleName);
  Handle(Viewer2dSamples) aViewer2dSamples = Handle(Viewer2dSamples)::DownCast(mySamples);
  if (aViewer2dSamples)
  {
    Standard_Boolean anIsFileSample = Viewer2dSamples::IsFileSample(theSampleName.toUtf8().data());
    QString aFileName;
    if(anIsFileSample)
    {
      int aMode; // not used
      aFileName = selectFileName(theSampleName, getOcafDialog(theSampleName), aMode);
      aViewer2dSamples->SetFileName(aFileName.toUtf8().data());
    }
    if (!anIsFileSample || (anIsFileSample && !aFileName.isEmpty()))
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      mySamples->Process(theSampleName.toUtf8().data());
      if(!Viewer2dSamples::IsShadedSample(theSampleName.toUtf8().data()))
        myDocument2d->SetObjects(mySamples->Get2dObjects(), Standard_False);
      else
        myDocument2d->SetObjects(mySamples->Get2dObjects(), Standard_True);
      myCodeView->setPlainText(mySamples->GetCode().ToCString());
      myResultView->setPlainText(mySamples->GetResult().ToCString());
      myGeomWidget->Show2d();
      QApplication::restoreOverrideCursor();
    }
    else
      myResultView->setPlainText("No file selected!");
  }
}

QString ApplicationCommonWindow::selectFileName(const QString& theSampleName,
                                                TranslateDialog* theDialog, int& theMode)
{
  std::shared_ptr<TranslateDialog> aDialog(theDialog);

  int ret = aDialog->exec();
  theMode = aDialog->getMode();

  qApp->processEvents();

  QString aFilename;
  QStringList fileNames;
  if (ret != QDialog::Accepted)
    return aFilename;

  fileNames = aDialog->selectedFiles();
  if (!fileNames.isEmpty())
    aFilename = fileNames[0];

  if (!QFileInfo(aFilename).completeSuffix().length())
  {
    QString selFilter = aDialog->selectedNameFilter();
    int idx = selFilter.indexOf("(*.");
    if (idx != -1)
    {
      QString tail = selFilter.mid(idx + 3);
      idx = tail.indexOf(" ");
      if (idx == -1)
        idx = tail.indexOf(")");
      QString ext = tail.left(idx);
      if (ext.length())
        aFilename += QString(".") + ext;
    }
  }

  return aFilename;
}

TranslateDialog* ApplicationCommonWindow::getDataExchangeDialog(const QString& theSampleName)
{
  TranslateDialog* aTranslateDialog = new TranslateDialog(this, 0, true);
  TCollection_AsciiString aSampleName(theSampleName.toUtf8().data());

  if (DataExchangeSamples::IsExportSample(aSampleName))
  {
    aTranslateDialog->setWindowTitle("Export file");
    aTranslateDialog->setFileMode(QFileDialog::AnyFile);
    aTranslateDialog->setAcceptMode(QFileDialog::AcceptSave);
  }
  else if (DataExchangeSamples::IsImportSample(aSampleName))
  {
    aTranslateDialog->setWindowTitle("Import file");
    aTranslateDialog->setFileMode(QFileDialog::ExistingFile);
    aTranslateDialog->setAcceptMode(QFileDialog::AcceptOpen);
  }
  QString aFormatFilter;
  if (DataExchangeSamples::IsBrepSample(aSampleName))
    aFormatFilter = "BREP Files(*.brep *.rle)";
  else if (DataExchangeSamples::IsStepSample(aSampleName))
  {
    aFormatFilter = "STEP Files (*.stp *.step)";
    aTranslateDialog->addMode(STEPControl_ManifoldSolidBrep, "Manifold Solid Brep");
    aTranslateDialog->addMode(STEPControl_FacetedBrep, "Faceted Brep");
    aTranslateDialog->addMode(STEPControl_ShellBasedSurfaceModel, "Shell Based Surface Model");
    aTranslateDialog->addMode(STEPControl_GeometricCurveSet, "Geometric Curve Set");
  }
  else if (DataExchangeSamples::IsIgesSample(aSampleName))
    aFormatFilter = "IGES Files (*.igs *.iges)";
  else if (DataExchangeSamples::IsStlSample(aSampleName))
    aFormatFilter = "STL Files (*.stl)";
  else if (DataExchangeSamples::IsVrmlSample(aSampleName))
    aFormatFilter = "VRML Files (*.vrml)";
  else if (DataExchangeSamples::IsImageSample(aSampleName))
    aFormatFilter = "All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)";

  QStringList aFilters;
  aFilters.append(aFormatFilter);
  aFilters.append("All Files(*.*)");

  aTranslateDialog->setNameFilters(aFilters);
  aTranslateDialog->clear();
  return aTranslateDialog;
}

TranslateDialog* ApplicationCommonWindow::getOcafDialog(const QString& theSampleName)
{
  TranslateDialog* aTranslateDialog = new TranslateDialog(this, 0, true);
  TCollection_AsciiString aSampleName(theSampleName.toUtf8().data());

  if (OcafSamples::IsExportSample(aSampleName))
  {
    aTranslateDialog->setWindowTitle("Export file");
    aTranslateDialog->setFileMode(QFileDialog::AnyFile);
    aTranslateDialog->setAcceptMode(QFileDialog::AcceptSave);
  }
  else if (OcafSamples::IsImportSample(aSampleName))
  {
    aTranslateDialog->setWindowTitle("Import file");
    aTranslateDialog->setFileMode(QFileDialog::ExistingFile);
    aTranslateDialog->setAcceptMode(QFileDialog::AcceptOpen);
  }
  QStringList aFilters;
  if (OcafSamples::IsBinarySample(aSampleName))
    aFilters.append("Binary OCAF Sample (*.cbf)");
  if (OcafSamples::IsXmlSample(aSampleName))
    aFilters.append("XML OCAF Sample (*.xml)");
  aFilters.append("All Files(*.*)");

  aTranslateDialog->setNameFilters(aFilters);
  aTranslateDialog->clear();
  return aTranslateDialog;
}


QMenu* ApplicationCommonWindow::MenuFromJsonObject(QJsonValue theJsonValue, const QString& theKey, QWidget* theParent, QSignalMapper* theMapper)
{
    QMenu* aMenu = new QMenu(theKey, theParent);
    if (theJsonValue.isObject())
    {
      QJsonObject aBranchObject = theJsonValue.toObject();
      for(const QString& aBranchKey: aBranchObject.keys())
      {        
        aMenu->addMenu(MenuFromJsonObject(aBranchObject.value(aBranchKey), aBranchKey, aMenu, theMapper));
      }
      //QList< QPair<QString, QMenu*> >  aMenuList;
      //for(const QString& aBranchKey: aBranchObject.keys())
      //{   

      //  aMenuList.append(QPair<QString, QMenu*>(aBranchKey, MenuFromJsonObject(aBranchObject.value(aBranchKey), aBranchKey, aMenu)));
      //}
      //qSort(aMenuList.begin(), aMenuList.end(), [](MyClass& a, MyClass& b) { return a.a < b.a; });
      //for(QPair<QString, QMenu*> aMenuPair : aMenuList)
      //{
      //  aMenu->addMenu(aMenuPair.second);
      //}


    }
    else if (theJsonValue.isArray())
    {
      QJsonArray aDataArray = theJsonValue.toArray();
      for(const QJsonValue& aDataValue: aDataArray)
      {
        if (aDataValue.isObject())
        {
          QJsonObject aDataObject = aDataValue.toObject();
          QString aSampleName = aDataObject["function"].toString();
          QAction* anAction = aMenu->addAction(aSampleName);
          anAction->setText(aDataObject["text"].toString());

          theMapper->setMapping(anAction, aSampleName);
          connect(anAction, &QAction::triggered, theMapper,
                  static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
         }
      }
    }
    return aMenu;
}

void ApplicationCommonWindow::MenuFormJson(const QString & thePath, QSignalMapper* theMapper)
{
  QFile aJsonFile(thePath);
  QString anErrorMessage;
  if (aJsonFile.error() != QFile::NoError)
  {
    anErrorMessage = aJsonFile.errorString();
    std::cout << "QFile creating error: " << anErrorMessage.toStdString();
    return;
  }
  if (!aJsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    std::cout << "File " << theMapper << " could not open";
    if (aJsonFile.error() != QFile::NoError)
    {
      anErrorMessage = aJsonFile.errorString();
      std::cout << "QFile opening error: " << anErrorMessage.toStdString();
    }
    return;
  }
  QString aJsonString = aJsonFile.readAll();
  aJsonFile.close();

  QJsonDocument aJsonDoc = QJsonDocument::fromJson(aJsonString.toUtf8());
  if (aJsonDoc.isObject())
  {
    QJsonObject aJsonObj = aJsonDoc.object();
    for (const QString& aKey : aJsonObj.keys())
    {
      QJsonValue aJsonValue = aJsonObj.value(aKey);
      if (aJsonValue.isObject())
      {
        mySamplePopups.push_back(MenuFromJsonObject(aJsonValue.toObject(), aKey, this, theMapper));
      }
    }
  }
}

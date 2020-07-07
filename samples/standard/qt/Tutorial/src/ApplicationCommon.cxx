#include "ApplicationCommon.h"
#include "DocumentCommon.h"
#include "View.h"
#include "GeometrySamples.h"
#include "TopologySamples.h"
#include "TriangulationSamples.h"
#include "DataExchangeSamples.h"
#include "OcafSamples.h"


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

static ApplicationCommonWindow* stApp = 0;
static QMdiArea* stWs = 0;

ApplicationCommonWindow::ApplicationCommonWindow(QString theSampleType)
  : QMainWindow(nullptr),
  mySampleMapper(new QSignalMapper(this)),
  myExchangeMapper(new QSignalMapper(this)),
  myOcafMapper(new QSignalMapper(this)),
  myStdToolBar(nullptr),
  myCasCadeBar(nullptr),
  myViewBar(nullptr),
  myFilePopup(nullptr),
  mySamples(nullptr)
{
  stApp = this;
  SetAppType(theSampleType);
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
    break;
  case ApplicationType::Viewer2d:
    break;
  default:
    setWindowTitle("Unknown application");
    break;
  }

  connect(mySampleMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessSample);
  connect(myExchangeMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessExchange);  
  connect(myOcafMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessOcaf);  
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
  createCasCadeOperations();

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
  resize(1280, 720);
}

void ApplicationCommonWindow::SetAppType(QString theParameter)
{
  if (theParameter == "Geometry")
    APP_TYPE = ApplicationType::Geometry;
  else if (theParameter == "Topology")
    APP_TYPE = ApplicationType::Topology;
  else if (theParameter == "Triangulation")
    APP_TYPE = ApplicationType::Triangulation;
  else if (theParameter == "DataExchange")
    APP_TYPE = ApplicationType::DataExchange;
  else if (theParameter == "Ocaf")
    APP_TYPE = ApplicationType::Ocaf;
  else  if (theParameter == "Viewer3d")
    APP_TYPE = ApplicationType::Viewer3d;
  else  if (theParameter == "Viewer2d")
    APP_TYPE = ApplicationType::Viewer2d;
  else  
    APP_TYPE = ApplicationType::Unknokwn;
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
  QAction* filePrefUseVBOAction = CreateAction(&ApplicationCommonWindow::onUseVBO, "Use VBO");
  filePrefUseVBOAction->setCheckable( true );
  filePrefUseVBOAction->setChecked( true );
  myStdActions.insert(StdActions::FilePrefUseVBO, filePrefUseVBOAction );

  QAction* fileQuitAction = CreateAction(&ApplicationCommonWindow::onCloseAllWindows, "Quit", "CTRL+Q");
  myStdActions.insert(StdActions::FileQuit, fileQuitAction );

  QAction* viewToolAction = CreateAction(&ApplicationCommonWindow::onViewToolBar, "Toolbar");
  viewToolAction->setCheckable( true );
  viewToolAction->setChecked( true );
  myStdActions.insert(StdActions::ViewTool, viewToolAction );

  QAction* helpAboutAction = CreateAction(&ApplicationCommonWindow::onAbout, "About", "F1", ":/icons/help.png");
  myStdActions.insert(StdActions::HelpAbout, helpAboutAction);

  // create preferences menu
  QMenu* aPrefMenu = new QMenu(tr("&Preferences"));
  aPrefMenu->addAction( filePrefUseVBOAction );

  // populate a menu with all actions
  myFilePopup = new QMenu( this );
  myFilePopup = menuBar()->addMenu(tr("&File"));
  myFilePopup->addMenu( aPrefMenu );
  myFilePopup->addAction( fileQuitAction );

  // add a view menu
  QMenu * view = new QMenu( this );

  view = menuBar()->addMenu(tr("&View"));
  view->addAction( viewToolAction );

  for(QMenu* aSampleMenu: mySamplePopups)
  {
    menuBar()->addMenu(aSampleMenu);
  }

  // add a help menu
  QMenu * help = new QMenu( this );
  menuBar()->addSeparator();
  help = menuBar()->addMenu(tr("&Help"));
  help->addAction( helpAboutAction );

  // populate a tool bar with some actions
  myStdToolBar = addToolBar(tr("File Operations"));
  myStdToolBar->addAction( helpAboutAction );

}

void ApplicationCommonWindow::createCasCadeOperations()
{
  myCasCadeBar = addToolBar( tr( "Shape Operations" ) );
  QAction* a;

  a = CreateAction(&ApplicationCommonWindow::onToolAction, tr("Wireframe"), "", ":/icons/tool_wireframe.png");
  myToolActions.insert(ToolActions::ToolWireframe, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, tr("Shading"), "", ":/icons/tool_shading.png");
  myToolActions.insert(ToolActions::ToolShading, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, tr("Color"), "", ":/icons/tool_color.png");
  myToolActions.insert(ToolActions::ToolColor, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, tr("Material"), "", ":/icons/tool_material.png");
  myToolActions.insert(ToolActions::ToolMaterial, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, tr("Transparency"), "", ":/icons/tool_transparency.png");
  myToolActions.insert(ToolActions::ToolTransparency, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, tr("Delete"), "", ":/icons/tool_delete.png");
  myToolActions.insert(ToolActions::ToolDelete, a );

  QSignalMapper* sm = new QSignalMapper( this );
  connect( sm, SIGNAL( mapped( int ) ), this, SLOT( onSetMaterial( int ) ) );

  a = new QAction(tr("Brass"), this );
  a->setToolTip(tr("Brass") );
  a->setStatusTip(tr("Brass") );
  sm->setMapping( a,(int)Graphic3d_NOM_BRASS );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_BRASS, a );

  a = new QAction(tr("Bronze"), this );
  a->setToolTip(tr("Bronze") );
  a->setStatusTip(tr("Bronze") );
  sm->setMapping( a, ( int )Graphic3d_NOM_BRONZE );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_BRONZE, a );

  a = new QAction(tr("Copper"), this );
  a->setToolTip(tr("Copper") );
  a->setStatusTip(tr("Copper") );
  sm->setMapping( a, ( int )Graphic3d_NOM_COPPER );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_COPPER, a );

  a = new QAction(tr("Gold"), this );
  a->setToolTip(tr("Gold") );
  a->setStatusTip(tr("Gold") );
  sm->setMapping( a, ( int )Graphic3d_NOM_GOLD );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_GOLD, a );

  a = new QAction(tr("Pewter"), this );
  a->setToolTip(tr("Pewter") );
  a->setStatusTip(tr("Pewter") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PEWTER );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PEWTER, a );

  a = new QAction(tr("Placter"), this );
  a->setToolTip(tr("Placter") );
  a->setStatusTip(tr("Placter") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PLASTER );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PLASTER, a );

  a = new QAction(tr("Plactic"), this );
  a->setToolTip(tr("Plactic") );
  a->setStatusTip(tr("Plactic") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PLASTIC );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PLASTIC, a );

  a = new QAction(tr("Silver"), this );
  a->setToolTip(tr("Silver") );
  a->setStatusTip(tr("Silver") );
  sm->setMapping( a, ( int )Graphic3d_NOM_SILVER );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_SILVER, a );

  for (QAction* anAction: myToolActions)
    myCasCadeBar->addAction(anAction);

  myViewBar = addToolBar(tr("View Operations"));
}

QAction*  ApplicationCommonWindow::getToolAction(ToolActions theAction)
{
    return myToolActions.value(theAction);
}

QList<QAction*> ApplicationCommonWindow::getMaterialActions()
{
    return myMaterialActions.values();
}


void ApplicationCommonWindow::windowsMenuActivated( bool checked )
{
  QAction* aSender = qobject_cast<QAction*>( sender() );
  if ( !aSender )
    return;
  QWidget * w = stWs->subWindowList().at( aSender->data().toInt() );
  if ( w && checked )
    w->setFocus();
}

ApplicationCommonWindow* ApplicationCommonWindow::getApplication()
{
  return stApp;
}

DocumentCommon* ApplicationCommonWindow::createNewDocument()
{
  return new DocumentCommon(this);
}

void ApplicationCommonWindow::onUseVBO()
{
  Handle(AIS_InteractiveContext) aContextAIS = myDocument3d->getContext();

  if (aContextAIS.IsNull())
    return;

  Handle(OpenGl_GraphicDriver) aDriver =
    Handle(OpenGl_GraphicDriver)::DownCast (aContextAIS->CurrentViewer()->Driver());

  if (!aDriver.IsNull())
  {
    aDriver->ChangeOptions().vboDisable = Standard_True;
  }
}

void ApplicationCommonWindow::onViewToolBar()
{
  bool show = myStdActions.value(StdActions::ViewTool)->isChecked();
  if ( show == myStdToolBar->isVisible() )
    return;
  if ( show )
    myStdToolBar->show();
  else
    myStdToolBar->hide();
}

void ApplicationCommonWindow::onAbout()
{
  QMessageBox::information( this, tr("Tutorial"), 
                            tr("Qt based application to study OpenCASCADE Technology"), 
                            tr("Ok" ), QString::null, QString::null, 0, 0 );
}


void ApplicationCommonWindow::onToolAction()
{
  QAction* sentBy = (QAction*)sender();
  if( sentBy == myToolActions.value(ToolActions::ToolWireframe) )
    myDocument3d->onWireframe();

  if( sentBy == myToolActions.value(ToolActions::ToolShading) )
    myDocument3d->onShading();

  if( sentBy == myToolActions.value(ToolActions::ToolColor) )
    myDocument3d->onColor();

  if( sentBy == myToolActions.value(ToolActions::ToolMaterial) )
    myDocument3d->onMaterial();

  if( sentBy == myToolActions.value(ToolActions::ToolTransparency) )
    myDocument3d->onTransparency();

  if( sentBy == myToolActions.value(ToolActions::ToolDelete) )
    myDocument3d->onDelete();
}

void ApplicationCommonWindow::onSetMaterial( int theMaterial )
{
    myDocument3d->onMaterial( theMaterial );
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
    myCodeView->setPlainText(mySamples->GetCode().ToCString());
    myResultView->setPlainText(mySamples->GetResult().ToCString());
    QApplication::restoreOverrideCursor();
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
  aJsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
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

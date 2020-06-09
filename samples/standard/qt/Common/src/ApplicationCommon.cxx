#include "ApplicationCommon.h"

#include "DocumentCommon.h"
#include "View.h"

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

static ApplicationCommonWindow* stApp = 0;
static QMdiArea* stWs = 0;

ApplicationCommonWindow::ApplicationCommonWindow()
  : QMainWindow(nullptr),
  mySampleMapper(new QSignalMapper(this)),
  myStdToolBar(nullptr),
  myCasCadeBar(nullptr),
  myViewBar(nullptr),
  myFilePopup(nullptr)
{
  stApp = this;

  if (true)
    mySamples = new GeometrySamples();

  connect(mySampleMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessSample);
  
  TCollection_AsciiString aSampleSourcePach = getSampleSourceDir();
  mySamples->SetCodePach(aSampleSourcePach);

  // create and define the central widget
  QSplitter* aGeomTextSplitter = new QSplitter(Qt::Horizontal);

  myDocument = createNewDocument();
  myView = new View(myDocument->getContext(), aGeomTextSplitter);
  resize(sizeHint());
  setFocusPolicy(Qt::StrongFocus);


  aGeomTextSplitter->addWidget(myView);
  QSplitter* aCodeResultSplitter = new QSplitter(Qt::Vertical);
  aGeomTextSplitter->addWidget(aCodeResultSplitter);

  myCodeView = new QTextEdit;
  myCodeView->setReadOnly(true);

  QFont aCodeViewFonf;
  aCodeViewFonf.setFamily("Courier");
  aCodeViewFonf.setFixedPitch(true);
  aCodeViewFonf.setPointSize(10);
  myCodeView->setFont(aCodeViewFonf);
  myCodeViewHighlighter = new OcctHighlighter(myCodeView->document());
  aCodeResultSplitter->addWidget(myCodeView);

  myResultView = new QTextEdit;
  myResultView->setReadOnly(true);
  myResultView->setFont(aCodeViewFonf);

  aCodeResultSplitter->addWidget(myResultView);

  setCentralWidget(aGeomTextSplitter);
  aGeomTextSplitter->setStretchFactor(0, 3);
  aGeomTextSplitter->setStretchFactor(1, 1);
  aGeomTextSplitter->show();

  Q_INIT_RESOURCE(Samples);
  QFile aJsonFile(":/menus/Geometry.json");
  bool b1 = aJsonFile.exists();
  aJsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
  bool b2 = aJsonFile.exists();
  QString aJsonString = aJsonFile.readAll();
  aJsonFile.close();  

  QJsonDocument aJsonDoc = QJsonDocument::fromJson(aJsonString.toUtf8());
  if (aJsonDoc.isObject())
  {
    QJsonObject aJsonObj = aJsonDoc.object();
    for(const QString& aKey: aJsonObj.keys()) 
    {
      QJsonValue aJsonValue = aJsonObj.value(aKey);
      if (aJsonValue.isObject())
      {
        mySamplePopups.push_back(MenuFromJsonObject(aJsonValue.toObject(), aKey, this));
      }
    }
  }

  createStandardOperations();
  createCasCadeOperations();

  statusBar()->showMessage( QObject::tr("INF_READY"), 5000 );
  resize( 1000, 700 );
}

void ApplicationCommonWindow::createStandardOperations()
{
  QAction* filePrefUseVBOAction = CreateAction(&ApplicationCommonWindow::onUseVBO, "Use VBO");
  filePrefUseVBOAction->setCheckable( true );
  filePrefUseVBOAction->setChecked( true );
  myStdActions.insert(StdActions::FilePrefUseVBOId, filePrefUseVBOAction );

  QAction* fileQuitAction = CreateAction(&ApplicationCommonWindow::onCloseAllWindows, "Quit", "CTRL+Q", "quit.png");
  myStdActions.insert(StdActions::FileQuitId, fileQuitAction );

  QAction* viewToolAction = CreateAction(&ApplicationCommonWindow::onViewToolBar, "Toolbar");
  viewToolAction->setCheckable( true );
  viewToolAction->setChecked( true );
  myStdActions.insert(StdActions::ViewToolId, viewToolAction );

  QAction* helpAboutAction = CreateAction(&ApplicationCommonWindow::onAbout, "About", "F1", "help.png");
  myStdActions.insert(StdActions::HelpAboutId, helpAboutAction);

  // create preferences menu
  QMenu* aPrefMenu = new QMenu( QObject::tr("MNU_PREFERENCES") );
  aPrefMenu->addAction( filePrefUseVBOAction );

  // populate a menu with all actions
  myFilePopup = new QMenu( this );
  myFilePopup = menuBar()->addMenu( QObject::tr("MNU_FILE") );
  myFilePopup->addMenu( aPrefMenu );
  myFilePopup->addAction( fileQuitAction );

  // add a view menu
  QMenu * view = new QMenu( this );

  view = menuBar()->addMenu( QObject::tr("MNU_VIEW") );
  view->addAction( viewToolAction );

  for(QMenu* aSampleMenu: mySamplePopups)
  {
    menuBar()->addMenu(aSampleMenu);
  }

  // add a help menu
  QMenu * help = new QMenu( this );
  menuBar()->addSeparator();
  help = menuBar()->addMenu( QObject::tr("MNU_HELP") );
  help->addAction( helpAboutAction );

  // populate a tool bar with some actions
  myStdToolBar = addToolBar( tr( "File Operations" ) );
  myStdToolBar->addAction( helpAboutAction );

}

void ApplicationCommonWindow::createCasCadeOperations()
{
  myCasCadeBar = addToolBar( tr( "Shape Operations" ) );

  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  QAction* a;

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Wireframe", "", "tool_wireframe.png");
  myToolActions.insert(ToolActions::ToolWireframeId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Shading", "", "tool_shading.png");
  myToolActions.insert(ToolActions::ToolShadingId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Color", "", "tool_color.png");
  myToolActions.insert(ToolActions::ToolColorId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Material", "", "tool_material.png");
  myToolActions.insert(ToolActions::ToolMaterialId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Transparency", "", "tool_transparency.png");
  myToolActions.insert(ToolActions::ToolTransparencyId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Delete", "", "tool_delete.png");
  myToolActions.insert(ToolActions::ToolDeleteId, a );

  QSignalMapper* sm = new QSignalMapper( this );
  connect( sm, SIGNAL( mapped( int ) ), this, SLOT( onSetMaterial( int ) ) );

  a = new QAction( QObject::tr("MNU_BRASS"), this );
  a->setToolTip( QObject::tr("TBR_BRASS") );
  a->setStatusTip( QObject::tr("TBR_BRASS") );
  sm->setMapping( a,(int)Graphic3d_NOM_BRASS );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_BRASS, a );

  a = new QAction( QObject::tr("MNU_BRONZE"), this );
  a->setToolTip( QObject::tr("TBR_BRONZE") );
  a->setStatusTip( QObject::tr("TBR_BRONZE") );
  sm->setMapping( a, ( int )Graphic3d_NOM_BRONZE );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_BRONZE, a );

  a = new QAction( QObject::tr("MNU_COPPER"), this );
  a->setToolTip( QObject::tr("TBR_COPPER") );
  a->setStatusTip( QObject::tr("TBR_COPER") );
  sm->setMapping( a, ( int )Graphic3d_NOM_COPPER );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_COPPER, a );

  a = new QAction( QObject::tr("MNU_GOLD"), this );
  a->setToolTip( QObject::tr("TBR_GOLD") );
  a->setStatusTip( QObject::tr("TBR_GOLD") );
  sm->setMapping( a, ( int )Graphic3d_NOM_GOLD );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_GOLD, a );

  a = new QAction( QObject::tr("MNU_PEWTER"), this );
  a->setToolTip( QObject::tr("TBR_PEWTER") );
  a->setStatusTip( QObject::tr("TBR_PEWTER") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PEWTER );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PEWTER, a );

  a = new QAction( QObject::tr("MNU_PLASTER"), this );
  a->setToolTip( QObject::tr("TBR_PLASTER") );
  a->setStatusTip( QObject::tr("TBR_PLASTER") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PLASTER );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PLASTER, a );

  a = new QAction( QObject::tr("MNU_PLASTIC"), this );
  a->setToolTip( QObject::tr("TBR_PLASTIC") );
  a->setStatusTip( QObject::tr("TBR_PLASTIC") );
  sm->setMapping( a, ( int )Graphic3d_NOM_PLASTIC );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_PLASTIC, a );

  a = new QAction( QObject::tr("MNU_SILVER"), this );
  a->setToolTip( QObject::tr("TBR_SILVER") );
  a->setStatusTip( QObject::tr("TBR_SILVER") );
  sm->setMapping( a, ( int )Graphic3d_NOM_SILVER );
  connect( a, SIGNAL( triggered() ), sm, SLOT( map() ) );
  myMaterialActions.insert( Graphic3d_NOM_SILVER, a );

  for (QAction* anAction: myToolActions)
    myCasCadeBar->addAction(anAction);
//  myCasCadeBar->hide();

  myViewBar = addToolBar(tr("View Operations"));
  myViewBar->addActions(myView->getViewActions());
}

QAction*  ApplicationCommonWindow::getToolAction(ToolActions theActionId)
{
    return myToolActions.value(theActionId);
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
  Handle(AIS_InteractiveContext) aContextAIS = myDocument->getContext();

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
  bool show = myStdActions.value(StdActions::ViewToolId )->isChecked();
  if ( show == myStdToolBar->isVisible() )
    return;
  if ( show )
    myStdToolBar->show();
  else
    myStdToolBar->hide();
}

void ApplicationCommonWindow::onAbout()
{
  QMessageBox::information( this, QObject::tr( "TIT_ABOUT" ), QObject::tr( "INF_ABOUT" ), QObject::tr("BTN_OK" ),
                            QString::null, QString::null, 0, 0 );
}


void ApplicationCommonWindow::onToolAction()
{
  QAction* sentBy = (QAction*)sender();
  if( sentBy == myToolActions.value(ToolActions::ToolWireframeId ) )
    myDocument->onWireframe();

  if( sentBy == myToolActions.value(ToolActions::ToolShadingId ) )
    myDocument->onShading();

  if( sentBy == myToolActions.value(ToolActions::ToolColorId ) )
    myDocument->onColor();

  if( sentBy == myToolActions.value(ToolActions::ToolMaterialId ) )
    myDocument->onMaterial();

  if( sentBy == myToolActions.value(ToolActions::ToolTransparencyId ) )
    myDocument->onTransparency();

  if( sentBy == myToolActions.value(ToolActions::ToolDeleteId ) )
    myDocument->onDelete();
}

void ApplicationCommonWindow::onSetMaterial( int theMaterial )
{
    myDocument->onMaterial( theMaterial );
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
                                               const char* theActionName, 
                                               const char* theShortcut, 
                                               const char* theIconName)
{
  QAction* aAction(NULL);
  if (theIconName)
  {
    QPixmap aIcon = QPixmap(getResourceDir() + QString("/") + theIconName);
    aAction = new QAction(aIcon, QObject::tr(theActionName), this);
  }
  else
  {
    aAction = new QAction(QObject::tr(theActionName), this);
  }
  aAction->setToolTip(QObject::tr(theActionName));
  aAction->setStatusTip(QObject::tr(theActionName));
  aAction->setShortcut(QString(theShortcut));
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
  mySamples->Process(theSampleName.toUtf8().data());
  myDocument->ClearContext();
  myCodeView->setPlainText(mySamples->GetCode().ToCString());
  myResultView->setPlainText(mySamples->GetResult().ToCString());
  if (mySamples->IsProcessed())
  {
    for (const Handle(AIS_InteractiveObject) aObject : mySamples->Get3dObject())
    {
      myDocument->getContext()->Display(aObject, Standard_True);
    }
  }
  myView->fitAll();
}

QMenu* ApplicationCommonWindow::MenuFromJsonObject(QJsonValue theJsonValue, const QString& theKey, QWidget* theParent)
{
    QMenu* aMenu = new QMenu(theKey, theParent);
    if (theJsonValue.isObject())
    {
      QJsonObject aBranchObject = theJsonValue.toObject();
      for(const QString& aBranchKey: aBranchObject.keys())
      {        
        aMenu->addMenu(MenuFromJsonObject(aBranchObject.value(aBranchKey), aBranchKey, aMenu));
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

          mySampleMapper->setMapping(anAction, aSampleName);
          connect(anAction, &QAction::triggered, mySampleMapper, 
                  static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
         }
      }
    }
    return aMenu;
}

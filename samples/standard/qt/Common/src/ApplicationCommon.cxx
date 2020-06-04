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


#include <Standard_WarningsDisable.hxx>
#include <Standard_WarningsRestore.hxx>


#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>

#include <stdlib.h>

static ApplicationCommonWindow* stApp = 0;
static QMdiArea* stWs = 0;

ApplicationCommonWindow::ApplicationCommonWindow()
: QMainWindow( 0 ),
mySampleMapper(new QSignalMapper(this)),
myNbDocuments( 0 ),
myIsDocuments(false),
myStdToolBar( 0 ),
myCasCadeBar( 0 ),
myFilePopup( 0 ),
myWindowPopup( 0 ),
myFileSeparator(nullptr)
{
  stApp = this;

  connect(mySampleMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &ApplicationCommonWindow::onProcessSample);
  
  TCollection_AsciiString aSampleSourcePach = getSampleSourceDir();
  mySamples.SetCodePach(aSampleSourcePach);

  // create and define the central widget


  QSplitter* aGeomTextSplitter = new QSplitter(Qt::Horizontal);
  QTextEdit* a3dView = new QTextEdit;


  //QFrame *vb = new QFrame(this);

  //QVBoxLayout *layout = new QVBoxLayout(vb);
  //layout->setMargin(0);

  //vb->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

//  setCentralWidget(vb);

  myDocument = onNewDoc();
  myView = new View(myDocument->getContext(), aGeomTextSplitter);
//  layout->addWidget(myView);

  //connect(myView, SIGNAL(selectionChanged()),
  //  this, SIGNAL(selectionChanged()));

  //createViewActions();
  //createRaytraceActions();

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
  myCodeView->setReadOnly(true);
  aCodeResultSplitter->addWidget(myResultView);

  setCentralWidget(aGeomTextSplitter);
  aGeomTextSplitter->resize(900, 600);
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
    foreach(const QString& aKey, aJsonObj.keys()) 
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

ApplicationCommonWindow::~ApplicationCommonWindow()
{
}

void ApplicationCommonWindow::createStandardOperations()
{
  QAction* fileNewAction = CreateAction(&ApplicationCommonWindow::onNewDoc, "new", "CTRL+N", "new.png");
  myStdActions.insert(FileNewId, fileNewAction);

  QAction* fileCloseAction = CreateAction(&ApplicationCommonWindow::onCloseWindow, "close", "CTRL+W", "close.png");
  myStdActions.insert(FileCloseId, fileCloseAction);

  QAction* filePrefUseVBOAction = CreateAction(&ApplicationCommonWindow::onUseVBO, "Use VBO");
  filePrefUseVBOAction->setCheckable( true );
  filePrefUseVBOAction->setChecked( true );
  myStdActions.insert( FilePrefUseVBOId, filePrefUseVBOAction );

  QAction* fileQuitAction = CreateAction(&ApplicationCommonWindow::onCloseAllWindows, "quit", "CTRL+Q", "quit.png");
  myStdActions.insert( FileQuitId, fileQuitAction );

  QAction* viewToolAction = CreateAction(&ApplicationCommonWindow::onViewToolBar, "Toolbar");
  viewToolAction->setCheckable( true );
  viewToolAction->setChecked( true );
  myStdActions.insert( ViewToolId, viewToolAction );

  QAction* viewStatusAction = CreateAction(&ApplicationCommonWindow::onViewStatusBar, "Statusbar");
  viewStatusAction->setCheckable( true );
  viewStatusAction->setChecked( true );
  myStdActions.insert( ViewStatusId, viewStatusAction );

  QAction* helpAboutAction = CreateAction(&ApplicationCommonWindow::onAbout, "About", "F1", "help.png");
  myStdActions.insert(HelpAboutId, helpAboutAction);

  // create preferences menu
  QMenu* aPrefMenu = new QMenu( QObject::tr("MNU_PREFERENCES") );
  aPrefMenu->addAction( filePrefUseVBOAction );

  // populate a menu with all actions
  myFilePopup = new QMenu( this );
  myFilePopup = menuBar()->addMenu( QObject::tr("MNU_FILE") );
  myFilePopup->addAction( fileNewAction );
  myFilePopup->addAction( fileCloseAction );
  myFileSeparator = myFilePopup->addSeparator();
  myFilePopup->addMenu( aPrefMenu );
  myFileSeparator = myFilePopup->addSeparator();
  myFilePopup->addAction( fileQuitAction );

  // add a view menu
  QMenu * view = new QMenu( this );

  view = menuBar()->addMenu( QObject::tr("MNU_VIEW") );
  view->addAction( viewToolAction );
  view->addAction( viewStatusAction );

  foreach(QMenu* aSampleMenu, mySamplePopups)
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
  myStdToolBar->addAction( fileNewAction );
  myStdToolBar->addAction( helpAboutAction );

//  myStdActions.at(FileCloseId)->setEnabled(myDocuments.count() > 0);

//  myStdActions.at(FilePrefUseVBOId)->setEnabled( true );
}

void ApplicationCommonWindow::createCasCadeOperations()
{
  createWindowPopup();

  // populate a tool bar with some actions
  myCasCadeBar = addToolBar( tr( "Shape Operations" ) );

  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  QAction* a;

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Wireframe", "", "tool_wireframe.png");
  myToolActions.insert( ToolWireframeId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Shading", "", "tool_shading.png");
  myToolActions.insert( ToolShadingId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Color", "", "tool_color.png");
  myToolActions.insert( ToolColorId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Material", "", "tool_material.png");
  myToolActions.insert( ToolMaterialId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Transparency", "", "tool_transparency.png");
  myToolActions.insert( ToolTransparencyId, a );

  a = CreateAction(&ApplicationCommonWindow::onToolAction, "Delete", "", "tool_delete.png");
  myToolActions.insert( ToolDeleteId, a );

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

  for ( int i = 0; i < myToolActions.size(); i++ )
    myCasCadeBar->addAction( myToolActions.at( i ) );
  myCasCadeBar->hide();
}

QList<QAction*>* ApplicationCommonWindow::getToolActions()
{
    return &myToolActions;
}

QList<QAction*>* ApplicationCommonWindow::getMaterialActions()
{
    return &myMaterialActions;
}

void ApplicationCommonWindow::createWindowPopup()
{
    if ( !myWindowPopup )
    {
      myWindowPopup = new QMenu( QObject::tr( "MNU_WINDOW" ), this );
      connect( myWindowPopup, SIGNAL( aboutToShow() ),
             this, SLOT( windowsMenuAboutToShow() ) );
    }
}

void ApplicationCommonWindow::windowsMenuAboutToShow()
{
  myWindowPopup->clear();
  QAction* a;

  QString dir = getResourceDir() + QString( "/" );

  a = new QAction( QPixmap( dir + QObject::tr( "ICON_WINDOW_NEW3D" ) ), QObject::tr( "MNU_WINDOW_NEW3D" ), this );
  a->setToolTip( QObject::tr( "TBR_WINDOW_NEW3D" ) );
  a->setStatusTip( QObject::tr( "TBR_WINDOW_NEW3D" ) );
  connect( a, SIGNAL( triggered() ), this, SLOT( onCreateNewView() ) );
  myWindowPopup->addAction( a );

  a = new QAction( QPixmap( dir + QObject::tr( "ICON_WINDOW_CASCADE" ) ), QObject::tr( "MNU_WINDOW_CASCADE" ), this );
  a->setToolTip( QObject::tr( "TBR_WINDOW_CASCADE" ) );
  a->setStatusTip( QObject::tr( "TBR_WINDOW_CASCADE" ) );
  connect( a, SIGNAL( triggered() ), stWs, SLOT( cascade() ) );
  myWindowPopup->addAction( a );

  a = new QAction( QPixmap( dir + QObject::tr( "ICON_WINDOW_TILE" ) ), QObject::tr( "MNU_WINDOW_TILE" ), this );
  a->setToolTip( QObject::tr( "TBR_WINDOW_TILE" ) );
  a->setStatusTip( QObject::tr( "TBR_WINDOW_TILE" ) );
  connect( a, SIGNAL( triggered() ), stWs, SLOT( tile() ) );
  myWindowPopup->addAction( a );

  myWindowPopup->addSeparator();
  QList<QMdiSubWindow *> windows = stWs->subWindowList();
  for (int i = 0; i < windows.count(); ++i)
  {
    QAction* aAction = new QAction( windows.at(i)->windowTitle(), this );
    aAction->setCheckable( true );
    aAction->setData( i );
    myWindowPopup->addAction( aAction );
    connect( aAction, SIGNAL( toggled( bool ) ), this, SLOT( windowsMenuActivated( bool ) ) );
    aAction->setChecked( stWs->activeSubWindow() == windows.at(i) );
  }
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

QMdiArea * ApplicationCommonWindow::getWorkspace()
{
  return stWs;
}

ApplicationCommonWindow* ApplicationCommonWindow::getApplication()
{
  return stApp;
}

void ApplicationCommonWindow::updateFileActions()
{
  //if ( !myIsDocuments )
  //{
  //  QAction* fileQuitAction = NULL;
  //  QAction* windowAction = NULL;
  //  QList<QAction *> aListActions = myFilePopup->actions();
  //  for ( int i = 0; i < aListActions.size(); i++ )
  //  {
  //    if( aListActions.at( i )->text() == QObject::tr("MNU_QUIT") )
  //    {
  //      fileQuitAction = aListActions.at( i );
  //      break;
  //    }
  //  }
  //      
  //  if( !fileQuitAction )
  //    return;
  //    
  //  myIsDocuments = true;
  //  myCasCadeBar->show();

  //  QList<QAction *> aListMenuActions = menuBar()->actions();
  //  for ( int i = 0; i < aListMenuActions.size(); i++ )
  //  {
  //    if( aListMenuActions.at( i )->text() == QObject::tr("MNU_HELP") )
  //    {
  //        windowAction= aListMenuActions.at( i );
  //        break;
  //    }
  //  }

  //  if( !windowAction )
  //    return;

  //  menuBar()->insertMenu( windowAction, myWindowPopup );
  //}
  //else
  //{
  //  myIsDocuments = false;
  //  myCasCadeBar->hide();
  //  menuBar()->removeAction( myWindowPopup->menuAction() );
  //}
}

DocumentCommon* ApplicationCommonWindow::createNewDocument()
{
  return new DocumentCommon( ++myNbDocuments, this );
}

int& ApplicationCommonWindow::getNbDocument()
{
  return myNbDocuments;
}

DocumentCommon* ApplicationCommonWindow::onNewDoc()
{
  updateFileActions();
  
  DocumentCommon* aDoc = createNewDocument();
  aDoc->onCreateNewView();
  onSelectionChanged();
  
  connect (aDoc, SIGNAL (sendCloseDocument (DocumentCommon*) ),
           this, SLOT (onCloseDocument (DocumentCommon*)));
  connect (stWs, SIGNAL (windowActivated (QWidget*)),
           this, SLOT (onWindowActivated (QWidget*)));
  connect (aDoc, SIGNAL (selectionChanged()),
           this, SLOT (onSelectionChanged()));
  
//  myDocuments.append (aDoc);
//  myStdActions.at (FileCloseId)->setEnabled (myDocuments.count() > 0);
  
  return aDoc;
}

void ApplicationCommonWindow::onCloseWindow()
{
    stWs->activeSubWindow()->close();
}

void ApplicationCommonWindow::onUseVBO()
{
  MDIWindow* aWindow = qobject_cast<MDIWindow*> (stWs->activeSubWindow()->widget());
    
  if (NULL == aWindow)
    return;

  Handle(AIS_InteractiveContext) aContextAIS = aWindow->getDocument()->getContext();

  if (aContextAIS.IsNull())
    return;

  Handle(OpenGl_GraphicDriver) aDriver =
    Handle(OpenGl_GraphicDriver)::DownCast (aContextAIS->CurrentViewer()->Driver());

  if (!aDriver.IsNull())
  {
    aDriver->ChangeOptions().vboDisable = Standard_True;
  }
}

//void ApplicationCommonWindow::onCloseDocument(DocumentCommon* theDoc)
//{
//  myDocuments.removeAll( theDoc );
//  theDoc->removeViews();
//  delete theDoc;
//  updateFileActions();
//  myStdActions.at(FileCloseId)->setEnabled(myDocuments.count() > 0);
//}

void ApplicationCommonWindow::onViewToolBar()
{
  bool show = myStdActions.at( ViewToolId )->isChecked();
  if ( show == myStdToolBar->isVisible() )
    return;
  if ( show )
    myStdToolBar->show();
  else
    myStdToolBar->hide();
}

void ApplicationCommonWindow::onViewStatusBar()
{
  bool show = myStdActions.at( ViewStatusId )->isChecked();
  if ( show == statusBar()->isVisible() )
    return;
  if ( show )
    statusBar()->show();
  else
    statusBar()->hide();
}

void ApplicationCommonWindow::onAbout()
{
  QMessageBox::information( this, QObject::tr( "TIT_ABOUT" ), QObject::tr( "INF_ABOUT" ), QObject::tr("BTN_OK" ),
                            QString::null, QString::null, 0, 0 );
}

void ApplicationCommonWindow::onCreateNewView()
{
  MDIWindow* window = qobject_cast< MDIWindow* >( stWs->activeSubWindow()->widget() );
  window->getDocument()->onCreateNewView();
}

void ApplicationCommonWindow::onWindowActivated ( QWidget * w )
{
  if (w == NULL)
  {
    return;
  }
  
  MDIWindow* window = qobject_cast< MDIWindow* >(w);

  window->onWindowActivated();
}

void ApplicationCommonWindow::onToolAction()
{
  QAction* sentBy = (QAction*) sender();
  QMdiArea* ws = ApplicationCommonWindow::getWorkspace();
  DocumentCommon* doc = qobject_cast<MDIWindow*>( ws->activeSubWindow()->widget() )->getDocument();

  if( sentBy == myToolActions.at( ToolWireframeId ) )
    doc->onWireframe();

  if( sentBy == myToolActions.at( ToolShadingId ) )
    doc->onShading();

  if( sentBy == myToolActions.at( ToolColorId ) )
    doc->onColor();

  if( sentBy == myToolActions.at( ToolMaterialId ) )
    doc->onMaterial();

  if( sentBy == myToolActions.at( ToolTransparencyId ) )
    doc->onTransparency();

  if( sentBy == myToolActions.at( ToolDeleteId ) )
    doc->onDelete();
}

void ApplicationCommonWindow::onSelectionChanged()
{
  //QMdiArea* ws = ApplicationCommonWindow::getWorkspace();
  //DocumentCommon* doc;

  //if( !qobject_cast<MDIWindow*>( ws->activeSubWindow()->widget() ) )
  //  return;

  //doc = ( qobject_cast<MDIWindow*>( ws->activeSubWindow()->widget() ) )->getDocument();
  //Handle(AIS_InteractiveContext) context = doc->getContext();

  //bool OneOrMoreInShading = false;
  //bool OneOrMoreInWireframe = false;
  //int numSel = context->NbSelected();
  //if ( numSel )
  //{
  //  for ( context->InitSelected(); context->MoreSelected(); context->NextSelected() )
  //  {
  //    if ( context->IsDisplayed( context->SelectedInteractive(), 1 ) )
  //      OneOrMoreInShading = true;
  //    if ( context->IsDisplayed( context->SelectedInteractive(), 0 ) )
  //      OneOrMoreInWireframe = true;
  //  }
  //  myToolActions.at( ToolWireframeId )->setEnabled( OneOrMoreInShading );
  //  myToolActions.at( ToolShadingId )->setEnabled( OneOrMoreInWireframe );
  //  myToolActions.at( ToolColorId )->setEnabled( true );
  //  myToolActions.at( ToolMaterialId )->setEnabled( true );
  //  myToolActions.at( ToolTransparencyId )->setEnabled( OneOrMoreInShading );
  //  myToolActions.at( ToolDeleteId )->setEnabled( true );
  //}
  //else
  //{
  //  myToolActions.at( ToolWireframeId )->setEnabled( false );
  //  myToolActions.at( ToolShadingId )->setEnabled( false );
  //  myToolActions.at( ToolColorId )->setEnabled( false );
  //  myToolActions.at( ToolMaterialId )->setEnabled( false );
  //  myToolActions.at( ToolTransparencyId )->setEnabled( false );
  //  myToolActions.at( ToolDeleteId )->setEnabled( false );
  //}
}

void ApplicationCommonWindow::onSetMaterial( int theMaterial )
{
    QMdiArea* ws = getWorkspace();
    DocumentCommon* doc = qobject_cast<MDIWindow*>( ws->activeSubWindow()->widget() )->getDocument();
    doc->onMaterial( theMaterial );
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

bool ApplicationCommonWindow::isDocument()
{
  return myIsDocuments;
}

QMenu* ApplicationCommonWindow::getFilePopup()
{
  return myFilePopup;
}

QAction* ApplicationCommonWindow::getFileSeparator()
{
  return myFileSeparator;
}

QToolBar* ApplicationCommonWindow::getCasCadeBar()
{
  return myCasCadeBar;
}

void ApplicationCommonWindow::SimpleAction()
{
  QMessageBox msgBox;
  msgBox.setText("Test.");
  msgBox.exec();
}

void ApplicationCommonWindow::onProcessSample(const QString& theSampleName)
{
  mySamples.Process(theSampleName.toUtf8().data());
  if (mySamples.IsProcessed())
  {
    myCodeView->setPlainText(mySamples.GetCode().ToCString());
    myResultView->setPlainText(mySamples.GetResult().ToCString());
    for (const Handle(AIS_InteractiveObject) aObject : mySamples.Get3dObject())
    {
      myDocument->getContext()->Display(aObject, Standard_False);
    }
  }
}

QMenu* ApplicationCommonWindow::MenuFromJsonObject(QJsonValue theJsonValue, const QString& theKey, QWidget* theParent)
{
    QMenu* aMenu = new QMenu(theKey, theParent);
    if (theJsonValue.isObject())
    {
      QJsonObject aBranchObject = theJsonValue.toObject();
      foreach(const QString& aBranchKey, aBranchObject.keys())
      {        
        aMenu->addMenu(MenuFromJsonObject(aBranchObject.value(aBranchKey), aBranchKey, aMenu));
      }

    }
    else if (theJsonValue.isArray())
    {
      QJsonArray aDataArray = theJsonValue.toArray();
      for(const QJsonValue& aDataValue: aDataArray)
      {
        if (aDataValue.isObject())
        {
          QJsonObject aDataObject = aDataValue.toObject();
          QString aSampleName = aDataObject["class"].toString();
          QAction* aAction = aMenu->addAction(aSampleName);

          mySampleMapper->setMapping(aAction, aSampleName);
          connect(aAction, &QAction::triggered, mySampleMapper, 
                  static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
         }
      }
    }
    return aMenu;
}

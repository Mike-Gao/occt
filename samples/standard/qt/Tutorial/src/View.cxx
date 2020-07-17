#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include "View.h"
#include "ApplicationCommon.h"
#include "OcctWindow.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QPainter>
#include <QMenu>
#include <QColorDialog>
#include <QCursor>
#include <QFileInfo>
#include <QFileDialog>
#include <QMouseEvent>
#include <QRubberBand>
#include <QMdiSubWindow>
#include <QStyleFactory>
#include <QBoxLayout>

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
  #include <QX11Info>
#endif
#include <Standard_WarningsRestore.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Aspect_DisplayConnection.hxx>

// the key for multi selection :
#define MULTISELECTIONKEY Qt::ShiftModifier

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY Qt::ControlModifier

// for elastic bean selection
#define ValZWMin 1

static QCursor* defCursor     = NULL;
static QCursor* handCursor    = NULL;
static QCursor* panCursor     = NULL;
static QCursor* globPanCursor = NULL;
static QCursor* zoomCursor    = NULL;
static QCursor* rotCursor     = NULL;

View::View( Handle(AIS_InteractiveContext) theContext, bool is3dView, QWidget* parent )
: QWidget( parent ),
  myIsRaytracing( false ),
  myIsShadowsEnabled (true),
  myIsReflectionsEnabled (false),
  myIsAntialiasingEnabled (false),
  myIis3dView(is3dView),
  myBackMenu( NULL )
{
#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
  XSynchronize(x11Info().display(),true);
#endif
  myContext = theContext;

  myXmin = 0;
  myYmin = 0;
  myXmax = 0;
  myYmax = 0;
  myCurZoom = 0;
  myRectBand = 0;

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);

  myCurrentMode = CurrentAction3d::Nothing;
  myHlrModeIsOn = Standard_False;
  setMouseTracking( true );

  initViewActions();
  initCursors();

  setBackgroundRole( QPalette::NoRole );//NoBackground );
  // set focus policy to threat QContextMenuEvent from keyboard  
  setFocusPolicy( Qt::StrongFocus );
  setAttribute( Qt::WA_PaintOnScreen );
  setAttribute( Qt::WA_NoSystemBackground );
  init();
}

View::~View()
{
  delete myBackMenu;
}

void View::init()
{
  if ( myV3dView.IsNull() )
    myV3dView = myContext->CurrentViewer()->CreateView();

  Handle(OcctWindow) hWnd = new OcctWindow(this);
  myV3dView->SetWindow(hWnd);
  if(!hWnd->IsMapped())
  {
    hWnd->Map();
  }

  if (myIis3dView)
  {    
    myV3dView->SetBackgroundColor(Quantity_Color(0.0, 0.0, 0.3, Quantity_TOC_RGB));
  }
  else
  {
    myV3dView->SetBackgroundColor(Quantity_Color(0.0, 0.2, 0.0, Quantity_TOC_RGB));
    myV3dView->SetProj(V3d_Zpos);
  }

  myV3dView->MustBeResized();

  if (myIsRaytracing)
    myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;
}

void View::paintEvent(QPaintEvent *)
{
  myV3dView->Redraw();
}

void View::resizeEvent(QResizeEvent *)
{
  if( !myV3dView.IsNull() )
  {
    myV3dView->MustBeResized();
  }
}

void View::fitAll()
{
  myV3dView->FitAll();
  myV3dView->ZFitAll();
  myV3dView->Redraw();
}

void View::fitArea()
{
  myCurrentMode = CurrentAction3d::WindowZooming;
}

void View::zoom()
{
  myCurrentMode = CurrentAction3d::DynamicZooming;
}

void View::pan()
{
  myCurrentMode = CurrentAction3d::DynamicPanning;
}

void View::rotation()
{
  if(myIis3dView)
    myCurrentMode = CurrentAction3d::DynamicRotation;
  else 
    myCurrentMode = CurrentAction3d::Nothing;
}

void View::globalPan()
{
  // save the current zoom value
  myCurZoom = myV3dView->Scale();
  // Do a Global Zoom
  myV3dView->FitAll();
  // Set the mode
  myCurrentMode = CurrentAction3d::GlobalPanning;
}

void View::front()
{
  if (myIis3dView)
    myV3dView->SetProj( V3d_Yneg );
}

void View::back()
{
  if (myIis3dView)
    myV3dView->SetProj( V3d_Ypos );
}

void View::top()
{
  myV3dView->SetProj( V3d_Zpos );
}

void View::bottom()
{
  if (myIis3dView)
    myV3dView->SetProj( V3d_Zneg );
}

void View::left()
{
  if (myIis3dView)
    myV3dView->SetProj( V3d_Xneg );
}

void View::right()
{
  if (myIis3dView)
    myV3dView->SetProj( V3d_Xpos );
}

void View::axo()
{
  if (myIis3dView)
    myV3dView->SetProj( V3d_XposYnegZpos );
}

void View::reset()
{
  myV3dView->Reset();
}

void View::hlrOff()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  myHlrModeIsOn = Standard_False;
  myV3dView->SetComputedMode (myHlrModeIsOn);
  myV3dView->Redraw();
  QApplication::restoreOverrideCursor();
}

void View::hlrOn()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  myHlrModeIsOn = Standard_True;
  myV3dView->SetComputedMode (myHlrModeIsOn);
  myV3dView->Redraw();
  QApplication::restoreOverrideCursor();
}

void View::SetRaytracedShadows (bool theState)
{
  myV3dView->ChangeRenderingParams().IsShadowEnabled = theState;

  myIsShadowsEnabled = theState;

  myContext->UpdateCurrentViewer();
}

void View::SetRaytracedReflections (bool theState)
{
  myV3dView->ChangeRenderingParams().IsReflectionEnabled = theState;

  myIsReflectionsEnabled = theState;

  myContext->UpdateCurrentViewer();
}

void View::onRaytraceAction()
{
  QAction* aSentBy = (QAction*)sender();
  
  if (aSentBy == myRaytraceActions.value(RaytraceAction::ToolRaytracing))
  {
    bool aState = myRaytraceActions.value(RaytraceAction::ToolRaytracing)->isChecked();

    QApplication::setOverrideCursor (Qt::WaitCursor);
    if (aState)
      EnableRaytracing();
    else
      DisableRaytracing();
    QApplication::restoreOverrideCursor();
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction::ToolShadows))
  {
    bool aState = myRaytraceActions.value(RaytraceAction::ToolShadows)->isChecked();
    SetRaytracedShadows (aState);
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction::ToolReflections))
  {
    bool aState = myRaytraceActions.value(RaytraceAction::ToolReflections)->isChecked();
    SetRaytracedReflections (aState);
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction::ToolAntialiasing))
  {
    bool aState = myRaytraceActions.value(RaytraceAction::ToolAntialiasing)->isChecked();
    SetRaytracedAntialiasing (aState);
  }
}

void View::SetRaytracedAntialiasing (bool theState)
{
  myV3dView->ChangeRenderingParams().IsAntialiasingEnabled = theState;

  myIsAntialiasingEnabled = theState;

  myContext->UpdateCurrentViewer();
}

void View::EnableRaytracing()
{
  if (!myIsRaytracing)
    myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;

  myIsRaytracing = true;

  myContext->UpdateCurrentViewer();
}

void View::DisableRaytracing()
{
  if (myIsRaytracing)
    myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RASTERIZATION;

  myIsRaytracing = false;

  myContext->UpdateCurrentViewer();
}

void View::updateToggled( bool isOn )
{
  QAction* sentBy = (QAction*)sender();

  if( !isOn )
    return;

  for (QAction* anAction: myViewActions)
  {
    if ( anAction && ( anAction != sentBy ) )
    {
      anAction->setCheckable( true );
      anAction->setChecked( false );
    }
    else
    {
      if ( sentBy == myViewActions.value(ViewAction::FitArea ) )
        setCursor( *handCursor );
      else if ( sentBy == myViewActions.value(ViewAction::Zoom ) )
        setCursor( *zoomCursor );
      else if ( sentBy == myViewActions.value(ViewAction::Pan ) )
        setCursor( *panCursor );
      else if ( sentBy == myViewActions.value(ViewAction::GlobalPan ) )
        setCursor( *globPanCursor );
      else if ( sentBy == myViewActions.value(ViewAction::Rotation ) )
        setCursor( *rotCursor );
      else
        setCursor( *defCursor );

      sentBy->setCheckable( false );
    }
  }
}

void View::initCursors()
{
  if ( !defCursor )
    defCursor = new QCursor( Qt::ArrowCursor );
  if ( !handCursor )
    handCursor = new QCursor( Qt::PointingHandCursor );
  if ( !panCursor )
    panCursor = new QCursor( Qt::SizeAllCursor );
  if ( !globPanCursor )
    globPanCursor = new QCursor( Qt::CrossCursor );
  if ( !zoomCursor )
    zoomCursor = new QCursor( QPixmap(":/icons/cursor_zoom.png"));
  if ( !rotCursor )
    rotCursor = new QCursor( QPixmap(":/icons/cursor_rotate.png") );
}

QAction* View::getViewAction(ViewAction theAction)
{
  initViewActions();
  return myViewActions.value(theAction);
}

QList<QAction*>   View::getViewActions()
{
  initViewActions();
  return myViewActions.values();
}

QAction* View::getRaytraceAction(RaytraceAction theAction)
{
  initRaytraceActions();
  return myRaytraceActions.value(theAction);
}

/*!
  Get paint engine for the OpenGL viewer. [ virtual public ]
*/
QPaintEngine* View::paintEngine() const
{
  return 0;
}

void View::initViewActions()
{
  if ( !myViewActions.empty() )
    return;

  QAction* a;

  a = new QAction( QPixmap(":/icons/view_fitall.png"), tr("FilAll"), this );
  a->setStatusTip(tr("FilAll") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( fitAll() ) );
  myViewActions.insert(ViewAction::FitAll, a);

  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions.insert(ViewAction::GlobalPan, a );

  if (myIis3dView)
  {
    a = new QAction( QPixmap(":/icons/view_front.png"), tr("Front"), this );
    a->setToolTip(tr("Front") );
    a->setStatusTip(tr("Front") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( front() ) );
    myViewActions.insert(ViewAction::Front, a );

    a = new QAction( QPixmap(":/icons/view_back.png"), tr("Back"), this );
    a->setToolTip(tr("Back") );
    a->setStatusTip(tr("Back") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( back() ) );
    myViewActions.insert(ViewAction::Back, a);

    a = new QAction( QPixmap(":/icons/view_top.png"), tr("Top"), this );
    a->setToolTip(tr("Top") );
    a->setStatusTip(tr("Top") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( top() ) );
    myViewActions.insert(ViewAction::Top, a );

    a = new QAction( QPixmap(":/icons/view_bottom.png"), tr("Bottom"), this );
    a->setToolTip(tr("Bottom") );
    a->setStatusTip(tr("Bottom") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( bottom() ) );
    myViewActions.insert(ViewAction::Bottom, a );

    a = new QAction( QPixmap(":/icons/view_left.png"), tr("Left"), this );
    a->setToolTip(tr("Left") );
    a->setStatusTip(tr("Left") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( left() ) );
    myViewActions.insert(ViewAction::Left, a );

    a = new QAction( QPixmap(":/icons/view_right.png"), tr("Right"), this );
    a->setToolTip(tr("Right") );
    a->setStatusTip(tr("Right") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( right() ) );
    myViewActions.insert(ViewAction::Right, a );

    a = new QAction( QPixmap(":/icons/view_axo.png"), tr("Isometric"), this );
    a->setToolTip(tr("Isometric") );
    a->setStatusTip(tr("Isometric") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( axo() ) );
    myViewActions.insert(ViewAction::Axo, a );
  }

  if (myIis3dView)
  {
    a = new QAction( QPixmap(":/icons/view_reset.png"), tr("Reset"), this );
    a->setToolTip(tr("Reset") );
    a->setStatusTip(tr("Reset") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( reset() ) );
    myViewActions.insert(ViewAction::Reset, a );

    QActionGroup* ag = new QActionGroup( this );

    a = new QAction( QPixmap(":/icons/view_comp_off.png"), tr("HLR off"), this );
    a->setToolTip(tr("HLR off") );
    a->setStatusTip(tr("HLR off") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( hlrOff() ) );
    a->setCheckable( true );
    ag->addAction(a);
    myViewActions.insert(ViewAction::HlrOff, a);

    a = new QAction( QPixmap(":/icons/view_comp_on.png"), tr("HLR on"), this );
    a->setToolTip(tr("HLR on") );
    a->setStatusTip(tr("HLR on") );
    connect( a, SIGNAL( triggered() ) ,this, SLOT( hlrOn() ) );  
    a->setCheckable( true );
    ag->addAction(a);
    myViewActions.insert(ViewAction::HlrOn, a );
  }

}

void View::initRaytraceActions()
{
  if (!myRaytraceActions.empty())
    return;

  QAction* a;
  a = new QAction( QPixmap(":/icons/raytracing.png"), tr("Ray-tracing"), this );
  a->setToolTip(tr("Ray-tracing") );
  a->setStatusTip(tr("Ray-tracing") );
  a->setCheckable( true );
  a->setChecked( false );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions.insert(RaytraceAction::ToolRaytracing, a );

  a = new QAction( QPixmap(":/icons/shadows.png"), tr("Shadows"), this );
  a->setToolTip(tr("Shadows") );
  a->setStatusTip(tr("Shadows") );
  a->setCheckable( true );
  a->setChecked( true );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions.insert(RaytraceAction::ToolShadows, a );

  a = new QAction( QPixmap(":/icons/reflections.png"), tr("Reflections"), this );
  a->setToolTip(tr("Reflections") );
  a->setStatusTip(tr("Reflections") );
  a->setCheckable( true );
  a->setChecked( false );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions.insert(RaytraceAction::ToolReflections, a );

  a = new QAction( QPixmap(":/icons/antialiasing.png"), tr("Anti-aliasing"), this );
  a->setToolTip(tr("Anti-aliasing") );
  a->setStatusTip(tr("Anti-aliasing") );
  a->setCheckable( true );
  a->setChecked( false );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions.insert(RaytraceAction::ToolAntialiasing, a );
}

void View::mousePressEvent( QMouseEvent* e )
{
  if ( e->button() == Qt::LeftButton )
    onLButtonDown( ( e->buttons() | e->modifiers() ), e->pos() );
  else if ( e->button() == Qt::MidButton )
    onMButtonDown( e->buttons() | e->modifiers(), e->pos() );
  else if ( e->button() == Qt::RightButton )
    onRButtonDown( e->buttons() | e->modifiers(), e->pos() );
}

void View::mouseReleaseEvent(QMouseEvent* e)
{
  if ( e->button() == Qt::LeftButton )
    onLButtonUp( e->buttons(), e->pos() );
  else if ( e->button() == Qt::MidButton )
    onMButtonUp( e->buttons(), e->pos() );
  else if( e->button() == Qt::RightButton )
    onRButtonUp( e->buttons(), e->pos() );
}

void View::mouseMoveEvent(QMouseEvent* e)
{
  onMouseMove( e->buttons(), e->pos() );
}

void View::activateCursor( const CurrentAction3d mode )
{
  switch( mode )
  {
  case CurrentAction3d::DynamicPanning:
      setCursor( *panCursor );
      break;
    case CurrentAction3d::DynamicZooming:
      setCursor( *zoomCursor );
      break;
    case CurrentAction3d::DynamicRotation:
      setCursor( *rotCursor );
      break;
    case CurrentAction3d::GlobalPanning:
      setCursor( *globPanCursor );
      break;
    case CurrentAction3d::WindowZooming:
      setCursor( *handCursor );
      break;
    case CurrentAction3d::Nothing:
    default:
      setCursor( *defCursor );
      break;
  }
}

void View::onLButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint point )
{
  //  save the current mouse coordinate in min
  myXmin = point.x();
  myYmin = point.y();
  myXmax = point.x();
  myYmax = point.y();

  myCurrentMode = CurrentAction3d::DynamicZooming;

  activateCursor( myCurrentMode );
}

void View::onMButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint /*point*/ )
{
  myCurrentMode = CurrentAction3d::DynamicPanning;
  activateCursor( myCurrentMode );
}

void View::onRButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint point )
{
  if (myIis3dView)
  { 
    myCurrentMode = CurrentAction3d::DynamicRotation;
    myV3dView->StartRotation(point.x(), point.y());
    activateCursor( myCurrentMode );
  }
}

void View::onLButtonUp( Qt::MouseButtons nFlags, const QPoint point )
{
    switch( myCurrentMode )
    {
    case CurrentAction3d::Nothing:
            if ( point.x() == myXmin && point.y() == myYmin )
            {
              // no offset between down and up --> selectEvent
              myXmax = point.x();
              myYmax = point.y();
              if ( nFlags & MULTISELECTIONKEY )
                  MultiInputEvent( point.x(), point.y() );
              else
                  InputEvent( point.x(), point.y() );
            }
            else
            {
              myXmax = point.x();
              myYmax = point.y();
              if ( nFlags & MULTISELECTIONKEY )
                  MultiDragEvent( point.x(), point.y(), 1 );
              else
                  DragEvent( point.x(), point.y(), 1 );
            }
            break;
        case CurrentAction3d::DynamicZooming:
            myCurrentMode = CurrentAction3d::Nothing;
            noActiveActions();
            break;
        case CurrentAction3d::WindowZooming:
            myXmax = point.x();
            myYmax = point.y();
            if ( (abs( myXmin - myXmax ) > ValZWMin ) ||
                 (abs( myYmin - myYmax ) > ValZWMin ) )
              myV3dView->WindowFitAll( myXmin, myYmin, myXmax, myYmax );
            myCurrentMode = CurrentAction3d::Nothing;
            noActiveActions();
            break;
        case CurrentAction3d::DynamicPanning:
            myCurrentMode = CurrentAction3d::Nothing;
            noActiveActions();
            break;
        case CurrentAction3d::GlobalPanning :
            myV3dView->Place( point.x(), point.y(), myCurZoom );
            myCurrentMode = CurrentAction3d::Nothing;
            noActiveActions();
            break;
        case CurrentAction3d::DynamicRotation:
            myCurrentMode = CurrentAction3d::Nothing;
            noActiveActions();
            break;
        default:
            throw Standard_Failure(" incompatible Current Mode ");
            break;
    }
    activateCursor( myCurrentMode );
}

void View::onMButtonUp( Qt::MouseButtons /*nFlags*/, const QPoint /*point*/ )
{
    myCurrentMode = CurrentAction3d::Nothing;
    activateCursor( myCurrentMode );
}

void View::onRButtonUp( Qt::MouseButtons /*nFlags*/, const QPoint point )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    // reset tyhe good Degenerated mode according to the strored one
    //   --> dynamic rotation may have change it
    if (myHlrModeIsOn)
    {
      myV3dView->SetComputedMode (myHlrModeIsOn);
      myV3dView->Redraw();
    }
    QApplication::restoreOverrideCursor();
    myCurrentMode = CurrentAction3d::Nothing;

    activateCursor( myCurrentMode );
}

void View::onMouseMove( Qt::MouseButtons nFlags, const QPoint point )
{
    if ( nFlags & Qt::LeftButton || nFlags & Qt::RightButton || nFlags & Qt::MidButton )
    {
    switch ( myCurrentMode )
    {
        case CurrentAction3d::Nothing:
          myXmax = point.x();
          myYmax = point.y();
          if ( nFlags & MULTISELECTIONKEY )
              MultiDragEvent( myXmax, myYmax, 0 );
          else
            DragEvent( myXmax, myYmax, 0 );
            break;
        case CurrentAction3d::DynamicZooming:
          myV3dView->Zoom( myXmax, myYmax, point.x(), point.y() );
          myXmax = point.x();
          myYmax = point.y();
          break;
        case CurrentAction3d::WindowZooming:
          myXmax = point.x();
          myYmax = point.y();
          break;
        case CurrentAction3d::DynamicPanning:
          myV3dView->Pan( point.x() - myXmax, myYmax - point.y() );
          myXmax = point.x();
          myYmax = point.y();
          break;
        case CurrentAction3d::GlobalPanning:
          break;
        case CurrentAction3d::DynamicRotation:
          myV3dView->Rotation( point.x(), point.y() );
          myV3dView->Redraw();
          break;
        default:
          throw Standard_Failure( "incompatible Current Mode" );
          break;
    }
    }
    else
    {
        myXmax = point.x();
        myYmax = point.y();
      if ( nFlags & MULTISELECTIONKEY )
          MultiMoveEvent( point.x(), point.y() );
      else
          MoveEvent( point.x(), point.y() );
    }
}

void View::DragEvent( const int x, const int y, const int TheState )
{
    // TheState == -1  button down
    // TheState ==  0  move
    // TheState ==  1  button up

    static Standard_Integer theButtonDownX = 0;
    static Standard_Integer theButtonDownY = 0;

    if ( TheState == -1 )
    {
        theButtonDownX = x;
        theButtonDownY = y;
    }

    if ( TheState == 1 )
    {
        myContext->Select( theButtonDownX, theButtonDownY, x, y, myV3dView, Standard_True );
        emit selectionChanged();
    }
}

void View::InputEvent( const int /*x*/, const int /*y*/ )
{
  myContext->Select (Standard_True);
  emit selectionChanged();
}

void View::MoveEvent( const int x, const int y )
{
  myContext->MoveTo( x, y, myV3dView, Standard_True );
}

void View::MultiMoveEvent( const int x, const int y )
{
  myContext->MoveTo( x, y, myV3dView, Standard_True );
}

void View::MultiDragEvent( const int x, const int y, const int TheState )
{
    static Standard_Integer theButtonDownX = 0;
    static Standard_Integer theButtonDownY = 0;

    if ( TheState == -1 )
    {
        theButtonDownX = x;
        theButtonDownY = y;
    }
    if ( TheState == 0 )
    {
        myContext->ShiftSelect( theButtonDownX, theButtonDownY, x, y, myV3dView, Standard_True );
        emit selectionChanged();
    }
}

void View::MultiInputEvent( const int /*x*/, const int /*y*/ )
{
  myContext->ShiftSelect (Standard_True);
  emit selectionChanged();
}

void View::addItemInPopup( QMenu* /*theMenu*/)
{
}

void View::noActiveActions()
{
    for(QAction* anAction: myViewActions)
    {
        setCursor( *defCursor );
        anAction->setCheckable( true );
        anAction->setChecked( false );
    }
}

void View::onBackground()
{
    QColor aColor ;
    Standard_Real R1;
    Standard_Real G1;
    Standard_Real B1;
    myV3dView->BackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
    aColor.setRgb((Standard_Integer)(R1 * 255), (Standard_Integer)(G1 * 255), (Standard_Integer)(B1 * 255));

    QColor aRetColor = QColorDialog::getColor(aColor);

    if( aRetColor.isValid() )
    {
        R1 = aRetColor.red()/255.;
        G1 = aRetColor.green()/255.;
        B1 = aRetColor.blue()/255.;
        myV3dView->SetBackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
    }
    myV3dView->Redraw();
}

void View::onEnvironmentMap()
{
  if (myBackMenu->actions().at(1)->isChecked())
  {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                           tr("All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)"));
    
    const TCollection_AsciiString anUtf8Path (fileName.toUtf8().data());
    
    Handle(Graphic3d_TextureEnv) aTexture = new Graphic3d_TextureEnv( anUtf8Path );

    myV3dView->SetTextureEnv (aTexture);
  }
  else
  {
    myV3dView->SetTextureEnv (Handle(Graphic3d_TextureEnv)());
  }
  
  myV3dView->Redraw();
}

bool View::dump(Standard_CString theFile)
{
  return myV3dView->Dump(theFile);
}

Handle(V3d_View) View::getView()
{
  return myV3dView;
}

Handle(AIS_InteractiveContext)& View::getContext()
{
  return myContext;
}

CurrentAction3d View::getCurrentMode()
{
  return myCurrentMode;
}




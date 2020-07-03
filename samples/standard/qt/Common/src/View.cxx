#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include "View.h"
#include "ApplicationCommon.h"

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

#include <OcctWindow.h>
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

  Handle(OcctWindow) hWnd = new OcctWindow ( this );
  myV3dView->SetWindow (hWnd);
  if ( !hWnd->IsMapped() )
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

void View::paintEvent( QPaintEvent *  )
{
//  QApplication::syncX();
  myV3dView->Redraw();
}

void View::resizeEvent( QResizeEvent * )
{
//  QApplication::syncX();
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
    zoomCursor = new QCursor( QPixmap( ApplicationCommonWindow::getResourceDir() + "/cursor_zoom.png"));
  if ( !rotCursor )
    rotCursor = new QCursor( QPixmap( ApplicationCommonWindow::getResourceDir() + "/cursor_rotate.png.png") );
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

  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  QAction* a;

  a = new QAction( QPixmap(dir + "view_fitall.png"), QObject::tr("MNU_FITALL"), this );
  a->setToolTip( QObject::tr("TBR_FITALL") );
  a->setStatusTip( QObject::tr("TBR_FITALL") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( fitAll() ) );
  myViewActions.insert(ViewAction::FitAll, a);

  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions.insert(ViewAction::GlobalPan, a );

  if (myIis3dView)
  {
    a = new QAction( QPixmap(dir + "view_front.png"), QObject::tr("MNU_FRONT"), this );
    a->setToolTip( QObject::tr("TBR_FRONT") );
    a->setStatusTip( QObject::tr("TBR_FRONT") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( front() ) );
    myViewActions.insert(ViewAction::Front, a );

    a = new QAction( QPixmap(dir + "view_back.png"), QObject::tr("MNU_BACK"), this );
    a->setToolTip( QObject::tr("TBR_BACK") );
    a->setStatusTip( QObject::tr("TBR_BACK") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( back() ) );
    myViewActions.insert(ViewAction::Back, a);

    a = new QAction( QPixmap(dir + "view_top.png"), QObject::tr("MNU_TOP"), this );
    a->setToolTip( QObject::tr("TBR_TOP") );
    a->setStatusTip( QObject::tr("TBR_TOP") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( top() ) );
    myViewActions.insert(ViewAction::Top, a );

    a = new QAction( QPixmap(dir + "view_bottom.png"), QObject::tr("MNU_BOTTOM"), this );
    a->setToolTip( QObject::tr("TBR_BOTTOM") );
    a->setStatusTip( QObject::tr("TBR_BOTTOM") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( bottom() ) );
    myViewActions.insert(ViewAction::Bottom, a );

    a = new QAction( QPixmap(dir + "view_left.png"), QObject::tr("MNU_LEFT"), this );
    a->setToolTip( QObject::tr("TBR_LEFT") );
    a->setStatusTip( QObject::tr("TBR_LEFT") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( left() ) );
    myViewActions.insert(ViewAction::Left, a );

    a = new QAction( QPixmap(dir + "view_right.png"), QObject::tr("MNU_RIGHT"), this );
    a->setToolTip( QObject::tr("TBR_RIGHT") );
    a->setStatusTip( QObject::tr("TBR_RIGHT") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( right() ) );
    myViewActions.insert(ViewAction::Right, a );

    a = new QAction( QPixmap(dir + "view_axo.png"), QObject::tr("MNU_AXO"), this );
    a->setToolTip( QObject::tr("TBR_AXO") );
    a->setStatusTip( QObject::tr("TBR_AXO") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( axo() ) );
    myViewActions.insert(ViewAction::Axo, a );
  }

  if (myIis3dView)
  {
    a = new QAction( QPixmap(dir + "view_reset.png"), QObject::tr("MNU_RESET"), this );
    a->setToolTip( QObject::tr("TBR_RESET") );
    a->setStatusTip( QObject::tr("TBR_RESET") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( reset() ) );
    myViewActions.insert(ViewAction::Reset, a );

    QActionGroup* ag = new QActionGroup( this );

    a = new QAction( QPixmap(dir + "view_comp_off.png"), QObject::tr("MNU_HLROFF"), this );
    a->setToolTip( QObject::tr("TBR_HLROFF") );
    a->setStatusTip( QObject::tr("TBR_HLROFF") );
    connect( a, SIGNAL( triggered() ) , this, SLOT( hlrOff() ) );
    a->setCheckable( true );
    ag->addAction(a);
    myViewActions.insert(ViewAction::HlrOff, a);

    a = new QAction( QPixmap(dir + "view_comp_on.png"), QObject::tr("MNU_HLRON"), this );
    a->setToolTip( QObject::tr("TBR_HLRON") );
    a->setStatusTip( QObject::tr("TBR_HLRON") );
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

  QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
  QAction* a;

  a = new QAction( QPixmap(dir + "raytracing.png"), QObject::tr("MNU_TOOL_RAYTRACING"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_RAYTRACING") );
  a->setStatusTip( QObject::tr("TBR_TOOL_RAYTRACING") );
  a->setCheckable( true );
  a->setChecked( false );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions.insert(RaytraceAction::ToolRaytracing, a );

  a = new QAction( QPixmap(dir + "shadows.png"), QObject::tr("MNU_TOOL_SHADOWS"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_SHADOWS") );
  a->setStatusTip( QObject::tr("TBR_TOOL_SHADOWS") );
  a->setCheckable( true );
  a->setChecked( true );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions.insert(RaytraceAction::ToolShadows, a );

  a = new QAction( QPixmap(dir + "reflections.png"), QObject::tr("MNU_TOOL_REFLECTIONS"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_REFLECTIONS") );
  a->setStatusTip( QObject::tr("TBR_TOOL_REFLECTIONS") );
  a->setCheckable( true );
  a->setChecked( false );
  connect( a, SIGNAL( triggered() ) , this, SLOT( onRaytraceAction() ) );
  myRaytraceActions.insert(RaytraceAction::ToolReflections, a );

  a = new QAction( QPixmap(dir + "antialiasing.png"), QObject::tr("MNU_TOOL_ANTIALIASING"), this );
  a->setToolTip( QObject::tr("TBR_TOOL_ANTIALIASING") );
  a->setStatusTip( QObject::tr("TBR_TOOL_ANTIALIASING") );
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

  //if ( nFlags & CASCADESHORTCUTKEY )
  //{
  //  myCurrentMode = CurAction3d_DynamicZooming;
  //}
  //else
  //{
  //  switch ( myCurrentMode )
  //  {
  //    case CurAction3d_Nothing:
  //         if ( nFlags & MULTISELECTIONKEY )
  //           MultiDragEvent( myXmax, myYmax, -1 );
  //         else
  //           DragEvent( myXmax, myYmax, -1 );
  //         break;
  //    case CurAction3d_DynamicZooming:
  //         break;
  //    case CurAction3d_WindowZooming:
  //         break;
  //    case CurAction3d_DynamicPanning:
  //         break;
  //    case CurAction3d_GlobalPanning:
  //         break;
  //    case CurAction3d_DynamicRotation:
  //         if (myHlrModeIsOn)
  //         {
  //           myV3dView->SetComputedMode (Standard_False);
  //         }
  //         myV3dView->StartRotation( point.x(), point.y() );
  //         break;
  //    default:
  //         throw Standard_Failure( "incompatible Current Mode" );
  //         break;
  //  }
  //}
  activateCursor( myCurrentMode );
}

void View::onMButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint /*point*/ )
{
//  if ( nFlags & CASCADESHORTCUTKEY )
    myCurrentMode = CurrentAction3d::DynamicPanning;
  activateCursor( myCurrentMode );
}

void View::onRButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint point )
{
  //if ( nFlags & CASCADESHORTCUTKEY )
  //{
  //  if (myHlrModeIsOn)
  //  {
  //    myV3dView->SetComputedMode (Standard_False);
  //  }
  //  myCurrentMode = CurAction3d_DynamicRotation;
  //  myV3dView->StartRotation( point.x(), point.y() );
  //}
  //else
  //{
  //  Popup( point.x(), point.y() );
  //}

  //if (myHlrModeIsOn)
  //{
  //  myV3dView->SetComputedMode(Standard_False);
  //}
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
              DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
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
            DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );//,LongDash);
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
    if ( myCurrentMode == CurrentAction3d::Nothing )
        Popup( point.x(), point.y() );
    else
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
    }
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
          DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
          if ( nFlags & MULTISELECTIONKEY )
              MultiDragEvent( myXmax, myYmax, 0 );
          else
            DragEvent( myXmax, myYmax, 0 );
            DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_True );
            break;
        case CurrentAction3d::DynamicZooming:
          myV3dView->Zoom( myXmax, myYmax, point.x(), point.y() );
          myXmax = point.x();
          myYmax = point.y();
          break;
        case CurrentAction3d::WindowZooming:
          myXmax = point.x();
          myYmax = point.y();
          DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
          DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_True );
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

void View::Popup( const int /*x*/, const int /*y*/ )
{
  ApplicationCommonWindow* stApp = ApplicationCommonWindow::getApplication();
  if ( myContext->NbSelected() )
  {
    QMenu* myToolMenu = new QMenu( 0 );
    myToolMenu->addAction(stApp->getToolAction(ToolActions::ToolWireframe) );
    myToolMenu->addAction(stApp->getToolAction(ToolActions::ToolShading) );
    myToolMenu->addAction(stApp->getToolAction(ToolActions::ToolColor) );
        
    QMenu* myMaterMenu = new QMenu( myToolMenu );

    QList<QAction*> aMeterActions = stApp->getMaterialActions();
        
    QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
    myMaterMenu = myToolMenu->addMenu( QPixmap( dir+QObject::tr("ICON_TOOL_MATER")), QObject::tr("MNU_MATER") );
    for (QAction* aMatAction: aMeterActions)
      myMaterMenu->addAction(aMatAction);
       
    myToolMenu->addAction(stApp->getToolAction(ToolActions::ToolTransparency) );
    myToolMenu->addAction(stApp->getToolAction(ToolActions::ToolDelete) );
    addItemInPopup(myToolMenu);
    myToolMenu->exec( QCursor::pos() );
    delete myToolMenu;
  }
  else
  {
    if (!myBackMenu)
    {
      myBackMenu = new QMenu( 0 );

      QAction* a = new QAction( QObject::tr("MNU_CH_BACK"), this );
      a->setToolTip( QObject::tr("TBR_CH_BACK") );
      connect( a, SIGNAL( triggered() ), this, SLOT( onBackground() ) );
      myBackMenu->addAction( a );  
      addItemInPopup(myBackMenu);

      a = new QAction( QObject::tr("MNU_CH_ENV_MAP"), this );
      a->setToolTip( QObject::tr("TBR_CH_ENV_MAP") );
      connect( a, SIGNAL( triggered() ), this, SLOT( onEnvironmentMap() ) );
      a->setCheckable( true );
      a->setChecked( false );
      myBackMenu->addAction( a );  
      addItemInPopup(myBackMenu);
    }

    myBackMenu->exec( QCursor::pos() );
  }
}

void View::addItemInPopup( QMenu* /*theMenu*/)
{
}

void View::DrawRectangle(const int MinX, const int MinY,
       const int MaxX, const int MaxY, const bool Draw)
{ 
  static Standard_Integer StoredMinX, StoredMaxX, StoredMinY, StoredMaxY;
  static Standard_Boolean m_IsVisible;

  StoredMinX = (MinX < MaxX) ? MinX: MaxX ;
  StoredMinY = (MinY < MaxY) ? MinY: MaxY ;
  StoredMaxX = (MinX > MaxX) ? MinX: MaxX ;
  StoredMaxY = (MinY > MaxY) ? MinY: MaxY ;

  QRect aRect;
  aRect.setRect( StoredMinX, StoredMinY, abs(StoredMaxX-StoredMinX), abs(StoredMaxY-StoredMinY));

  if ( !myRectBand ) 
  {
    myRectBand = new QRubberBand( QRubberBand::Rectangle, this );
    myRectBand->setStyle( QStyleFactory::create("windows") );
    myRectBand->setGeometry( aRect );
    myRectBand->show();

    /*QPalette palette;
    palette.setColor(myRectBand->foregroundRole(), Qt::white);
    myRectBand->setPalette(palette);*/
  }

  if ( m_IsVisible && !Draw ) // move or up  : erase at the old position
  {
    myRectBand->hide();
    delete myRectBand;
    myRectBand = 0;
    m_IsVisible = false;
  }

  if (Draw) // move : draw
  {
    //aRect.setRect( StoredMinX, StoredMinY, abs(StoredMaxX-StoredMinX), abs(StoredMaxY-StoredMinY));
    m_IsVisible = true;
    myRectBand->setGeometry( aRect );
    //myRectBand->show();
  }
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




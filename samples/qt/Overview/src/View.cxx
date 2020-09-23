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

#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include "ApplicationCommon.h"
#include "OcctWindow.h"
#include "Transparency.h"
#include "View.h"

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
#include <Standard_WarningsRestore.hxx>

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
#include <QX11Info>
#endif

#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>

// the key for multi selection :
#define MULTISELECTIONKEY Qt::ShiftModifier

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY Qt::ControlModifier

// for elastic bean selection
#define ValZWMin 1

static QCursor* defCursor = NULL;
static QCursor* handCursor = NULL;
static QCursor* panCursor = NULL;
static QCursor* globPanCursor = NULL;
static QCursor* zoomCursor = NULL;
static QCursor* rotCursor = NULL;

View::View(Handle(AIS_InteractiveContext) theContext, bool is3dView, QWidget* parent)
  : QWidget(parent),
  myIsRaytracing(false),
  myIsShadowsEnabled(true),
  myIsReflectionsEnabled(false),
  myIsAntialiasingEnabled(false),
  myIs3dView(is3dView),
  myBackMenu(NULL)
{
#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
  XSynchronize(x11Info().display(), true);
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
  setMouseTracking(true);

  initViewActions();
  initCursors();

  setBackgroundRole(QPalette::NoRole);//NoBackground );
  // set focus policy to threat QContextMenuEvent from keyboard  
  setFocusPolicy(Qt::StrongFocus);
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);
  init();
}

View::~View()
{
  delete myBackMenu;
}

void View::init()
{
  if (myV3dView.IsNull())
    myV3dView = myContext->CurrentViewer()->CreateView();

  Handle(OcctWindow) hWnd = new OcctWindow(this);
  myV3dView->SetWindow(hWnd);
  if (!hWnd->IsMapped()) {
    hWnd->Map();
  }

  if (myIs3dView) {
    myV3dView->SetBackgroundColor(Quantity_Color(0.0, 0.0, 0.3, Quantity_TOC_RGB));
  }
  else {
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
  if (!myV3dView.IsNull()) {
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
  if (myIs3dView)
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
  if (myIs3dView)
    myV3dView->SetProj(V3d_Yneg);
}

void View::back()
{
  if (myIs3dView)
    myV3dView->SetProj(V3d_Ypos);
}

void View::top()
{
  myV3dView->SetProj(V3d_Zpos);
}

void View::bottom()
{
  if (myIs3dView)
    myV3dView->SetProj(V3d_Zneg);
}

void View::left()
{
  if (myIs3dView)
    myV3dView->SetProj(V3d_Xneg);
}

void View::right()
{
  if (myIs3dView)
    myV3dView->SetProj(V3d_Xpos);
}

void View::axo()
{
  if (myIs3dView)
    myV3dView->SetProj(V3d_XposYnegZpos);
}

void View::reset()
{
  myV3dView->Reset();
}

void View::hlrOff()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  myV3dView->SetComputedMode(Standard_False);
  myV3dView->Redraw();
  QAction* aShadingAction = getViewAction(ViewAction::Shading);
  aShadingAction->setEnabled(true);
  QAction* aWireframeAction = getViewAction(ViewAction::Wireframe);
  aWireframeAction->setEnabled(true);
  QApplication::restoreOverrideCursor();
}

void View::hlrOn()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  myV3dView->SetComputedMode(Standard_True);
  myV3dView->Redraw();
  QAction* aShadingAction = getViewAction(ViewAction::Shading);
  aShadingAction->setEnabled(false);
  QAction* aWireframeAction = getViewAction(ViewAction::Wireframe);
  aWireframeAction->setEnabled(false);
  QApplication::restoreOverrideCursor();
}

void View::shading()
{
  myContext->SetDisplayMode(1, Standard_True);
}
void View::wireframe()
{
  myContext->SetDisplayMode(0, Standard_True);
}

void View::SetRaytracedShadows(bool theState)
{
  myV3dView->ChangeRenderingParams().IsShadowEnabled = theState;

  myIsShadowsEnabled = theState;

  myContext->UpdateCurrentViewer();
}

void View::SetRaytracedReflections(bool theState)
{
  myV3dView->ChangeRenderingParams().IsReflectionEnabled = theState;

  myIsReflectionsEnabled = theState;

  myContext->UpdateCurrentViewer();
}

void View::onRaytraceAction()
{
  QAction* aSentBy = (QAction*)sender();

  if (aSentBy == myRaytraceActions.value(RaytraceAction::ToolRaytracing)) {
    bool aState = myRaytraceActions.value(RaytraceAction::ToolRaytracing)->isChecked();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (aState)
      EnableRaytracing();
    else
      DisableRaytracing();
    QApplication::restoreOverrideCursor();
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction::ToolShadows)) {
    bool aState = myRaytraceActions.value(RaytraceAction::ToolShadows)->isChecked();
    SetRaytracedShadows(aState);
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction::ToolReflections)) {
    bool aState = myRaytraceActions.value(RaytraceAction::ToolReflections)->isChecked();
    SetRaytracedReflections(aState);
  }

  if (aSentBy == myRaytraceActions.value(RaytraceAction::ToolAntialiasing)) {
    bool aState = myRaytraceActions.value(RaytraceAction::ToolAntialiasing)->isChecked();
    SetRaytracedAntialiasing(aState);
  }
}

void View::SetRaytracedAntialiasing(bool theState)
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

void View::updateToggled(bool isOn)
{
  QAction* sentBy = (QAction*)sender();

  if (!isOn)
    return;

  for (QAction* anAction : myViewActions) {
    if (anAction && (anAction != sentBy)) {
      anAction->setCheckable(true);
      anAction->setChecked(false);
    }
    else {
      if (sentBy == myViewActions.value(ViewAction::FitArea))
        setCursor(*handCursor);
      else if (sentBy == myViewActions.value(ViewAction::Zoom))
        setCursor(*zoomCursor);
      else if (sentBy == myViewActions.value(ViewAction::Pan))
        setCursor(*panCursor);
      else if (sentBy == myViewActions.value(ViewAction::GlobalPan))
        setCursor(*globPanCursor);
      else if (sentBy == myViewActions.value(ViewAction::Rotation))
        setCursor(*rotCursor);
      else
        setCursor(*defCursor);

      sentBy->setCheckable(false);
    }
  }
}

void View::initCursors()
{
  if (!defCursor)
    defCursor = new QCursor(Qt::ArrowCursor);
  if (!handCursor)
    handCursor = new QCursor(Qt::PointingHandCursor);
  if (!panCursor)
    panCursor = new QCursor(Qt::SizeAllCursor);
  if (!globPanCursor)
    globPanCursor = new QCursor(Qt::CrossCursor);
  if (!zoomCursor)
    zoomCursor = new QCursor(QPixmap(":/icons/cursor_zoom.png"));
  if (!rotCursor)
    rotCursor = new QCursor(QPixmap(":/icons/cursor_rotate.png"));
}

QList<QAction*> View::getViewActions()
{
  initViewActions();
  return myViewActions.values();
}

QList<QAction*>  View::getRaytraceActions()
{
  initRaytraceActions();
  return myRaytraceActions.values();
}

QAction* View::getViewAction(ViewAction theAction)
{
  return myViewActions.value(theAction);
}

QAction* View::getRaytraceAction(RaytraceAction theAction)
{
  return myRaytraceActions.value(theAction);
}

/*!
  Get paint engine for the OpenGL viewer. [ virtual public ]
*/
QPaintEngine* View::paintEngine() const
{
  return 0;
}

QAction* View::RegisterAction(QString theIconPath, QString thePromt, void (View::*theSlot)(void))
{
  QAction* anAction = new QAction(QPixmap(theIconPath), thePromt, this);
  anAction->setToolTip(thePromt);
  anAction->setStatusTip(thePromt);
  connect(anAction, &QAction::triggered, this, theSlot);
  return anAction;
}

void View::initViewActions()
{
  if (!myViewActions.empty())
    return;
  myViewActions[ViewAction::FitAll] =
    RegisterAction(":/icons/view_fitall.png", tr("FilAll"), &View::fitAll);
  if (myIs3dView) {
    myViewActions[ViewAction::Front] =
      RegisterAction(":/icons/view_front.png", tr("Front"), &View::front);
    myViewActions[ViewAction::Back] =
      RegisterAction(":/icons/view_back.png", tr("Back"), &View::back);
    myViewActions[ViewAction::Top] =
      RegisterAction(":/icons/view_top.png", tr("Top"), &View::top);
    myViewActions[ViewAction::Bottom] =
      RegisterAction(":/icons/view_bottom.png", tr("Bottom"), &View::bottom);
    myViewActions[ViewAction::Left] =
      RegisterAction(":/icons/view_left.png", tr("Left"), &View::left);
    myViewActions[ViewAction::Right] =
      RegisterAction(":/icons/view_right.png", tr("Right"), &View::right);
    myViewActions[ViewAction::Axo] =
      RegisterAction(":/icons/view_axo.png", tr("Isometric"), &View::axo);

    myViewActions[ViewAction::Reset] =
      RegisterAction(":/icons/view_reset.png", tr("Reset"), &View::reset);

    QActionGroup* aShadingActionGroup = new QActionGroup(this);
    QAction* aShadingAction = RegisterAction(":/icons/tool_shading.png", tr("Shading"), &View::shading);
    aShadingAction->setCheckable(true);
    aShadingActionGroup->addAction(aShadingAction);
    myViewActions[ViewAction::Shading] = aShadingAction;

    QAction* aWireframeAction = RegisterAction(":/icons/tool_wireframe.png", tr("Wireframe"), &View::wireframe);
    aWireframeAction->setCheckable(true);
    aShadingActionGroup->addAction(aWireframeAction);
    myViewActions[ViewAction::Wireframe] = aWireframeAction;

    QActionGroup* aHlrActionGroup = new QActionGroup(this);
    QAction* aHlrOffAction = RegisterAction(":/icons/view_comp_off.png", tr("HLR off"), &View::hlrOff);
    aHlrOffAction->setCheckable(true);
    aHlrActionGroup->addAction(aHlrOffAction);
    myViewActions[ViewAction::HlrOff] = aHlrOffAction;

    QAction* aHlrOnAction = RegisterAction(":/icons/view_comp_on.png", tr("HLR on"), &View::hlrOn);
    aHlrOnAction->setCheckable(true);
    aHlrActionGroup->addAction(aHlrOnAction);
    myViewActions[ViewAction::HlrOn] = aHlrOnAction;

    myViewActions[ViewAction::Transparency] =
      RegisterAction(":/icons/tool_transparency.png", tr("Transparency"), &View::onTransparency);
  }
}

void View::initRaytraceActions()
{
  if (!myRaytraceActions.empty())
    return;

  QAction* aRayTraceAction = RegisterAction(":/icons/raytracing.png", tr("Ray-tracing"), &View::onRaytraceAction);
  myRaytraceActions[RaytraceAction::ToolRaytracing] = aRayTraceAction;
  aRayTraceAction->setCheckable(true);
  aRayTraceAction->setChecked(false);

  QAction* aShadowAction = RegisterAction(":/icons/shadows.png", tr("Shadows"), &View::onRaytraceAction);
  myRaytraceActions[RaytraceAction::ToolShadows] = aShadowAction;
  aShadowAction->setCheckable(true);
  aShadowAction->setChecked(true);

  QAction* aReflectAction = RegisterAction(":/icons/reflections.png", tr("Reflections"), &View::onRaytraceAction);
  myRaytraceActions[RaytraceAction::ToolReflections] = aReflectAction;
  aReflectAction->setCheckable(true);
  aReflectAction->setChecked(false);

  QAction* anAntiAliasingAction = RegisterAction(":/icons/antialiasing.png", tr("Anti-aliasing"), &View::onRaytraceAction);
  myRaytraceActions[RaytraceAction::ToolAntialiasing] = anAntiAliasingAction;
  anAntiAliasingAction->setCheckable(true);
  anAntiAliasingAction->setChecked(false);
}

void View::mousePressEvent(QMouseEvent* e)
{
  if (e->button() == Qt::LeftButton)
    onLButtonDown((e->buttons() | e->modifiers()), e->pos());
  else if (e->button() == Qt::MidButton)
    onMButtonDown(e->buttons() | e->modifiers(), e->pos());
  else if (e->button() == Qt::RightButton)
    onRButtonDown(e->buttons() | e->modifiers(), e->pos());
}

void View::mouseReleaseEvent(QMouseEvent* e)
{
  if (e->button() == Qt::LeftButton)
    onLButtonUp(e->buttons(), e->pos());
  else if (e->button() == Qt::MidButton)
    onMButtonUp(e->buttons(), e->pos());
  else if (e->button() == Qt::RightButton)
    onRButtonUp(e->buttons(), e->pos());
}

void View::mouseMoveEvent(QMouseEvent* e)
{
  onMouseMove(e->buttons(), e->pos());
}

void View::wheelEvent(QWheelEvent* theEvent)
{
  int x = theEvent->x();
  int y = theEvent->y();
  myV3dView->StartZoomAtPoint(x, y);
  double aDelta = (double)(theEvent->delta()) / (15 * 8);
  int x1 = (int)(x + width() * aDelta / 100);
  int y1 = (int)(x + height() * aDelta / 100);
  myV3dView->Zoom(x, y, x1, y1);
}

void View::activateCursor(const CurrentAction3d mode)
{
  switch (mode) {
  case CurrentAction3d::DynamicPanning:
    setCursor(*panCursor);
    break;
  case CurrentAction3d::DynamicZooming:
    setCursor(*zoomCursor);
    break;
  case CurrentAction3d::DynamicRotation:
    setCursor(*rotCursor);
    break;
  case CurrentAction3d::GlobalPanning:
    setCursor(*globPanCursor);
    break;
  case CurrentAction3d::WindowZooming:
    setCursor(*handCursor);
    break;
  case CurrentAction3d::Nothing:
  default:
    setCursor(*defCursor);
    break;
  }
}

void View::onLButtonDown(const int/*Qt::MouseButtons*/ nFlags, const QPoint point)
{
  Q_UNUSED(nFlags)

  //  save the current mouse coordinate in min
  myXmin = point.x();
  myYmin = point.y();
  myXmax = point.x();
  myYmax = point.y();


  if (myContext->HasDetected())
    myCurrentMode = CurrentAction3d::ObjectDececting;
  else {
    myCurrentMode = CurrentAction3d::DynamicZooming;
    myV3dView->StartZoomAtPoint(myXmax, myYmax);
  }
  activateCursor(myCurrentMode);
}

void View::onMButtonDown(const int/*Qt::MouseButtons*/ nFlags, const QPoint point)
{
  Q_UNUSED(nFlags)
  Q_UNUSED(point)

  myCurrentMode = CurrentAction3d::DynamicPanning;
  activateCursor(myCurrentMode);
}

void View::onRButtonDown(const int/*Qt::MouseButtons*/ nFlags, const QPoint point)
{
  Q_UNUSED(nFlags)
  Q_UNUSED(point)
  if (myIs3dView) {
    myCurrentMode = CurrentAction3d::DynamicRotation;
    myV3dView->StartRotation(point.x(), point.y());
    activateCursor(myCurrentMode);
  }
}

void View::onLButtonUp(Qt::MouseButtons nFlags, const QPoint point)
{
  Q_UNUSED(nFlags)
  Q_UNUSED(point)
  switch (myCurrentMode) {
  case CurrentAction3d::Nothing:
    if (point.x() == myXmin && point.y() == myYmin) {
      // no offset between down and up --> selectEvent
      myXmax = point.x();
      myYmax = point.y();
      if (nFlags & MULTISELECTIONKEY)
        MultiInputEvent(point.x(), point.y());
      else
        InputEvent(point.x(), point.y());
    }
    else {
      myXmax = point.x();
      myYmax = point.y();
      if (nFlags & MULTISELECTIONKEY)
        MultiDragEvent(point.x(), point.y(), 1);
      else
        DragEvent(point.x(), point.y(), 1);
    }
    break;
  case CurrentAction3d::ObjectDececting:
  {
    Handle(AIS_InteractiveObject) aDecectObject = myContext->DetectedInteractive();
    if (aDecectObject) {
      TColStd_ListOfInteger aModeList;
      myContext->ActivatedModes(aDecectObject, aModeList);
      // No selection for Neutral Mode
      if (!aModeList.IsEmpty() && aModeList.First() != 0)
        myContext->Select(Standard_True);
    }
    myCurrentMode = CurrentAction3d::Nothing;
    break;
  }
  case CurrentAction3d::DynamicZooming:
    myCurrentMode = CurrentAction3d::Nothing;
    break;
  case CurrentAction3d::WindowZooming:
    myXmax = point.x();
    myYmax = point.y();
    if ((abs(myXmin - myXmax) > ValZWMin) ||
      (abs(myYmin - myYmax) > ValZWMin))
      myV3dView->WindowFitAll(myXmin, myYmin, myXmax, myYmax);
    myCurrentMode = CurrentAction3d::Nothing;
    break;
  case CurrentAction3d::DynamicPanning:
    myCurrentMode = CurrentAction3d::Nothing;
    break;
  case CurrentAction3d::GlobalPanning:
    myV3dView->Place(point.x(), point.y(), myCurZoom);
    myCurrentMode = CurrentAction3d::Nothing;
    break;
  case CurrentAction3d::DynamicRotation:
    myCurrentMode = CurrentAction3d::Nothing;
    break;
  default:
    throw Standard_Failure(" incompatible Current Mode ");
    break;
  }
  activateCursor(myCurrentMode);
}

void View::onMButtonUp(Qt::MouseButtons nFlags, const QPoint point)
{
  Q_UNUSED(nFlags)
  Q_UNUSED(point)

  myCurrentMode = CurrentAction3d::Nothing;
  activateCursor(myCurrentMode);
}

void View::onRButtonUp(Qt::MouseButtons nFlags, const QPoint point)
{
  Q_UNUSED(nFlags)
  Q_UNUSED(point)

  QApplication::setOverrideCursor(Qt::WaitCursor);
  // reset tyhe good Degenerated mode according to the strored one
  //   --> dynamic rotation may have change it
  if (myHlrModeIsOn) {
    myV3dView->SetComputedMode(myHlrModeIsOn);
    myV3dView->Redraw();
  }
  QApplication::restoreOverrideCursor();
  myCurrentMode = CurrentAction3d::Nothing;

  activateCursor(myCurrentMode);
}

void View::onMouseMove(Qt::MouseButtons nFlags, const QPoint point)
{
  if (nFlags & Qt::LeftButton || nFlags & Qt::RightButton || nFlags & Qt::MidButton) {
    switch (myCurrentMode) {
    case CurrentAction3d::Nothing:
      myXmax = point.x();
      myYmax = point.y();
      if (nFlags & MULTISELECTIONKEY)
        MultiDragEvent(myXmax, myYmax, 0);
      else
        DragEvent(myXmax, myYmax, 0);
      break;
    case CurrentAction3d::DynamicZooming:
      myV3dView->Zoom(myXmax, myYmax, point.x(), point.y());
      myXmax = point.x();
      myYmax = point.y();
      break;
    case CurrentAction3d::WindowZooming:
      myXmax = point.x();
      myYmax = point.y();
      break;
    case CurrentAction3d::DynamicPanning:
      myV3dView->Pan(point.x() - myXmax, myYmax - point.y());
      myXmax = point.x();
      myYmax = point.y();
      break;
    case CurrentAction3d::GlobalPanning:
      break;
    case CurrentAction3d::DynamicRotation:
      myV3dView->Rotation(point.x(), point.y());
      myV3dView->Redraw();
      break;
    case CurrentAction3d::ObjectDececting:
      myXmax = point.x();
      myYmax = point.y();
      break;
    default:
      throw Standard_Failure("incompatible Current Mode");
      break;
    }
  }
  else {
    myXmax = point.x();
    myYmax = point.y();
    if (nFlags & MULTISELECTIONKEY)
      MultiMoveEvent(point.x(), point.y());
    else
      MoveEvent(point.x(), point.y());
  }
}

void View::DragEvent(const int x, const int y, const int TheState)
{
  // TheState == -1  button down
  // TheState ==  0  move
  // TheState ==  1  button up

  static Standard_Integer theButtonDownX = 0;
  static Standard_Integer theButtonDownY = 0;

  if (TheState == -1) {
    theButtonDownX = x;
    theButtonDownY = y;
  }

  if (TheState == 1) {
    myContext->Select(theButtonDownX, theButtonDownY, x, y, myV3dView, Standard_True);
    emit selectionChanged();
  }
}

void View::InputEvent(const int /*x*/, const int /*y*/)
{
  myContext->Select(Standard_True);
  emit selectionChanged();
}

void View::MoveEvent(const int x, const int y)
{
  myContext->MoveTo(x, y, myV3dView, Standard_True);
}

void View::MultiMoveEvent(const int x, const int y)
{
  myContext->MoveTo(x, y, myV3dView, Standard_True);
}

void View::MultiDragEvent(const int x, const int y, const int TheState)
{
  static Standard_Integer theButtonDownX = 0;
  static Standard_Integer theButtonDownY = 0;

  if (TheState == -1) {
    theButtonDownX = x;
    theButtonDownY = y;
  }
  if (TheState == 0) {
    myContext->ShiftSelect(theButtonDownX, theButtonDownY, x, y, myV3dView, Standard_True);
    emit selectionChanged();
  }
}

void View::MultiInputEvent(const int x, const int y)
{
  Q_UNUSED(x)
  Q_UNUSED(y)

  myContext->ShiftSelect(Standard_True);
  emit selectionChanged();
}

void View::addItemInPopup(QMenu* theMenu)
{
  Q_UNUSED(theMenu)
}

void View::onBackground()
{
  QColor aColor;
  Standard_Real R1;
  Standard_Real G1;
  Standard_Real B1;
  myV3dView->BackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
  aColor.setRgb((Standard_Integer)(R1 * 255), (Standard_Integer)(G1 * 255), (Standard_Integer)(B1 * 255));

  QColor aRetColor = QColorDialog::getColor(aColor);

  if (aRetColor.isValid()) {
    R1 = aRetColor.red() / 255.;
    G1 = aRetColor.green() / 255.;
    B1 = aRetColor.blue() / 255.;
    myV3dView->SetBackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
  }
  myV3dView->Redraw();
}

void View::onEnvironmentMap()
{
  if (myBackMenu->actions().at(1)->isChecked()) {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
      tr("All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)"));

    const TCollection_AsciiString anUtf8Path(fileName.toUtf8().data());

    Handle(Graphic3d_TextureEnv) aTexture = new Graphic3d_TextureEnv(anUtf8Path);

    myV3dView->SetTextureEnv(aTexture);
  }
  else {
    myV3dView->SetTextureEnv(Handle(Graphic3d_TextureEnv)());
  }

  myV3dView->Redraw();
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

void View::onTransparency()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  if (anAisObjectsList.Extent() == 0)
  {
    return;
  }
  double aTranspValue = anAisObjectsList.First()->Transparency();
  DialogTransparency aDlg(this);
  aDlg.setValue(int(aTranspValue * 10));
  connect(&aDlg, SIGNAL(sendTransparencyChanged(int)), SLOT(onTransparencyChanged(int)));
  aDlg.exec();
}


void View::onTransparencyChanged(int theVal)
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  double aTranspValue = theVal / 10.;
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList)
    myContext->SetTransparency(anAisObject, aTranspValue, Standard_False);

  myContext->UpdateCurrentViewer();
}
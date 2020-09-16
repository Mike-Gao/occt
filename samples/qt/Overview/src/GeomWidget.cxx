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

#include "GeomWidget.h"


#include <QFrame>
#include <QBoxLayout>
#include <QTextEdit>
#include <QStackedLayout>
#include <QToolBar>
#include <QStackedWidget>

GeomWidget::GeomWidget(DocumentCommon* theDocument3d,
  DocumentCommon* theDocument2d,
  QWidget* parent)
  : QWidget(parent),
  myDocument3d(theDocument3d),
  myDocument2d(theDocument2d)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  aMainLayout->setContentsMargins(0, 0, 0, 0);

  my2dVidget = new QWidget;
  QVBoxLayout* a2dLayout = new QVBoxLayout(my2dVidget);
  a2dLayout->setContentsMargins(0, 0, 0, 0);
  a2dLayout->setSpacing(0);
  myView2d = new View(myDocument2d->getContext(), false, this);
  QToolBar* aToolBar2d = new QToolBar;
  aToolBar2d->addActions(myView2d->getViewActions());
  a2dLayout->addWidget(aToolBar2d);
  a2dLayout->addWidget(myView2d);

  my3dVidget = new QWidget;
  QVBoxLayout* a3dLayout = new QVBoxLayout(my3dVidget);
  a3dLayout->setContentsMargins(0, 0, 0, 0);
  a3dLayout->setSpacing(0);
  myView3d = new View(myDocument3d->getContext(), true, this);
  QToolBar* aToolBar3d = new QToolBar;
  aToolBar3d->addActions(myView3d->getViewActions());
  aToolBar3d->addSeparator();
  aToolBar3d->addActions(myView3d->getRaytraceActions());
  a3dLayout->addWidget(aToolBar3d);
  a3dLayout->addWidget(myView3d);

  myStackWidget = new QStackedWidget(this);
  aMainLayout->addWidget(myStackWidget);
  myStackWidget->addWidget(my2dVidget);
  myStackWidget->addWidget(my3dVidget);

  FitAll();
}

void GeomWidget::FitAll()
{
  if (myDocument2d->IsEmpty())
    Show3d();
  else
    Show2d();
}

void GeomWidget::Show3d()
{
  myView3d->fitAll();
  QAction * aShadingAction = myView3d->getViewAction(ViewAction::Shading);
  aShadingAction->trigger();
  aShadingAction->setChecked(true);
  QAction * aHlrOffAction = myView3d->getViewAction(ViewAction::HlrOff);
  aHlrOffAction->trigger();
  aHlrOffAction->setChecked(true);
  myStackWidget->setCurrentWidget(my3dVidget);
  setStatusTip("Mouse buttons: Left-Zoom, Middle-Pan, Right-Rotate");
}

void GeomWidget::Show2d()
{
  myView2d->fitAll();
  myStackWidget->setCurrentWidget(my2dVidget);
  setStatusTip("Mouse buttons: Left-Zoom, Middle-Pan");
}

Handle(V3d_View) GeomWidget::Get3dView()
{
  return myView3d->getView();
}

Handle(V3d_View) GeomWidget::Get2dView()
{
  return myView2d->getView();
}
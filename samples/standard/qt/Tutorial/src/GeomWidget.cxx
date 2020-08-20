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

GeomWidget::GeomWidget(DocumentCommon* theDocument3d,
  DocumentCommon* theDocument2d,
  QWidget* parent)
  : QWidget(parent),
  myDocument3d(theDocument3d),
  myDocument2d(theDocument2d)
{
  my2dVidget = new QWidget;
  my2dVidget->setContentsMargins(0, 0, 0, 0);
  QVBoxLayout* a2dLayout = new QVBoxLayout;
  a2dLayout->setMargin(0);
  myView2d = new View(myDocument2d->getContext(), false, this);
  QToolBar* aToolBar2d = new QToolBar;
  aToolBar2d->addActions(myView2d->getViewActions());
  a2dLayout->addWidget(aToolBar2d);
  a2dLayout->addWidget(myView2d);
  my2dVidget->setLayout(a2dLayout);

  my3dVidget = new QWidget;
  my3dVidget->setContentsMargins(0, 0, 0, 0);
  QVBoxLayout* a3dLayout = new QVBoxLayout;
  a3dLayout->setMargin(0);
  myView3d = new View(myDocument3d->getContext(), true, this);
  QToolBar* aToolBar3d = new QToolBar;
  aToolBar3d->addActions(myView3d->getViewActions());
  aToolBar3d->addSeparator();
  aToolBar3d->addActions(myView3d->getRaytraceActions());
  a3dLayout->addWidget(aToolBar3d);
  a3dLayout->addWidget(myView3d);
  my3dVidget->setLayout(a3dLayout);

  QVBoxLayout* a3d2dLayout = new QVBoxLayout;
  a3d2dLayout->setMargin(0);
  a3d2dLayout->addWidget(my3dVidget);
  a3d2dLayout->addWidget(my2dVidget);

  QVBoxLayout* aWidgetLayout = new QVBoxLayout;
  aWidgetLayout->setMargin(0);
  aWidgetLayout->addLayout(a3d2dLayout);
  setLayout(aWidgetLayout);

  FitAll();
}

void GeomWidget::FitAll()
{
  if (myDocument3d->IsEmpty())
    my3dVidget->hide();
  else
    Show3d();

  if (myDocument2d->IsEmpty())
    my2dVidget->hide();
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
  my3dVidget->show();
  setStatusTip("Mouse buttons: Left-Zoom, Middle-Pan, Right-Rotate");
}

void GeomWidget::Show2d()
{
  myView2d->fitAll();
  my2dVidget->show();
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
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
  myView2d = new View(myDocument2d->getContext(), false, parent);
  QToolBar* aToolBar2d = new QToolBar;
  aToolBar2d->addActions(myView2d->getViewActions());
  a2dLayout->addWidget(aToolBar2d);
  a2dLayout->addWidget(myView2d);
  my2dVidget->setLayout(a2dLayout);

  my3dVidget = new QWidget;
  my3dVidget->setContentsMargins(0, 0, 0, 0);
  QVBoxLayout* a3dLayout = new QVBoxLayout;
  a3dLayout->setMargin(0);
  myView3d = new View(myDocument3d->getContext(), true, parent);
  QToolBar* aToolBar3d = new QToolBar;
  aToolBar3d->addActions(myView3d->getViewActions());
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
  {
    myView3d->fitAll();
    my3dVidget->show();
    setStatusTip("Mouse buttons: Left-Zoom, Middle-Pan, Right-Rotate");
  }


  if (myDocument2d->IsEmpty())
    my2dVidget->hide();
  else
  {
    myView2d->fitAll();
    my2dVidget->show();
    setStatusTip("Mouse buttons: Left-Zoom, Middle-Pan, Right-Menu");
  }
}

Handle(V3d_View) GeomWidget::Get3dView()
{
  return myView3d->getView();
}

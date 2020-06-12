#include "GeomWidget.h"


#include <QFrame>
#include <QBoxLayout>
#include <QTextEdit>
#include <QStackedLayout>

GeomWidget::GeomWidget(Handle(AIS_InteractiveContext) theContext, QWidget* parent)
  : QWidget(parent)
{
  QWidget* a3dVidget = new QWidget;
  QVBoxLayout* a3dLayout = new QVBoxLayout;
  a3dLayout->setMargin(0);
  myView = new View(theContext, parent);
  myToolBarBar = new QToolBar;
  myToolBarBar->addActions(myView->getViewActions());
  a3dLayout->addWidget(myToolBarBar);
  a3dLayout->addWidget(myView);
  a3dVidget->setLayout(a3dLayout);



  QWidget* a2dVidget = new QWidget;
  a2dVidget->setContentsMargins(0, 0, 0, 0);
  QVBoxLayout* a2dLayout = new QVBoxLayout;
  a2dLayout->setMargin(0);
  QTextEdit* aTe1 = new QTextEdit; 
  QTextEdit* aTe2 = new QTextEdit;
  a2dLayout->addWidget(aTe1);
  a2dLayout->addWidget(aTe2);
  a2dVidget->setLayout(a2dLayout);


  QStackedLayout* aStackedLayout = new QStackedLayout;
  aStackedLayout->setMargin(0);
  aStackedLayout->addWidget(a3dVidget);
  aStackedLayout->addWidget(a2dVidget);
  aStackedLayout->setCurrentIndex(0);



  QVBoxLayout* aWidgetLayout = new QVBoxLayout;
  aWidgetLayout->setMargin(0);
  aWidgetLayout->addLayout(aStackedLayout);
  setLayout(aWidgetLayout);
}

void GeomWidget::FitAll()
{
  if (true)
    myView->fitAll();
}

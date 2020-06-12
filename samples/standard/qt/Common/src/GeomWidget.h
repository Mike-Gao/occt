#ifndef GEOMWIDGET_H
#define GEOMWIDGET_H

#include "View.h"
#include "DocumentCommon.h"
//#include "ApplicationCommon.h"

#include <QWidget>
#include <QAction>
#include <QToolBar>
#include <QList>

#include <AIS_InteractiveContext.hxx>

class ApplicationCommon;

class GeomWidget : public QWidget
{
  Q_OBJECT
public:
  GeomWidget(Handle(AIS_InteractiveContext) theContext, QWidget* parent = nullptr);

  void FitAll();


private:
  View*           myView;
  QToolBar*       myToolBarBar;
//  DocumentCommon* myDocument;
};

#endif //GEOMWIDGET_H




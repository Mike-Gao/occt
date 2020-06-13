#ifndef GEOMWIDGET_H
#define GEOMWIDGET_H
#include "View.h"
#include "DocumentCommon.h"

#include <QWidget>

class ApplicationCommon;

class GeomWidget : public QWidget
{
  Q_OBJECT
public:
  GeomWidget(DocumentCommon* theDocument3d, 
             DocumentCommon* theDocument2d,
             QWidget* parent = nullptr);

  void FitAll();


private:
  View*           myView3d;
  View*           myView2d;

  QWidget* my3dVidget;
  QWidget* my2dVidget;

  DocumentCommon* myDocument3d;
  DocumentCommon* myDocument2d;
};

#endif //GEOMWIDGET_H




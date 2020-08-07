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
  Handle(V3d_View) Get3dView();
  Handle(V3d_View) Get2dView();
  void Show3d();  
  void Show2d();

private:
  View*    myView3d;
  View*    myView2d;

  QWidget* my3dVidget;
  QWidget* my2dVidget;

  DocumentCommon* myDocument3d;
  DocumentCommon* myDocument2d;
};

#endif //GEOMWIDGET_H




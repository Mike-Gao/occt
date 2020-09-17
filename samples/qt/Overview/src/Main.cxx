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

#include "ApplicationCommon.h"

#include <OSD_Environment.hxx>
#include <Standard_WarningsDisable.hxx>
#include <Standard_WarningsRestore.hxx>

#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLocale>
#include <QStringList>
#include <QCommandLineParser>

int main ( int argc, char* argv[] )
{
  Q_INIT_RESOURCE(Overview);

	QApplication aQApp( argc, argv );

  QSettings settings("Overview.conf", QSettings::IniFormat);
  settings.beginGroup("ApplicationSetting");
    ApplicationType aCategory = static_cast<ApplicationType>(settings.value("ApplicationType", "").toInt());
  settings.endGroup();

  ApplicationCommonWindow* aWindow = new ApplicationCommonWindow(aCategory);
  QString aResName(":/icons/lamp.png");
  aWindow->setWindowIcon(QPixmap(aResName));

  settings.beginGroup("WindowPosition");
    int x = settings.value("x", -1).toInt();
    int y = settings.value("y", -1).toInt();
    int width = settings.value("width", -1).toInt();
    int height = settings.value("height", -1).toInt();
  settings.endGroup();

  if (x > 0 && y > 0 && width > 0 && height > 0) 
  {
    aWindow->setGeometry(x, y, width, height);
  }
  aWindow->SetApplicationType(aCategory);

  aWindow->show();
  int result = aQApp.exec();

  settings.beginGroup("WindowPosition");
    settings.setValue("x", aWindow->x());
    settings.setValue("y", aWindow->y());
    settings.setValue("width", aWindow->width());
    settings.setValue("height", aWindow->height());
  settings.endGroup();

  settings.beginGroup("ApplicationSetting");
    settings.setValue("ApplicationType", aWindow->GetApplicationType());
  settings.endGroup();

  return result;
}

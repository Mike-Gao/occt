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
#include <QTranslator>
#include <QLocale>
#include <QStringList>
#include <QCommandLineParser>

int main ( int argc, char* argv[] )
{
  Q_INIT_RESOURCE(Tutorial);
	QApplication aQApp( argc, argv );

  ApplicationCommonWindow::ApplicationType anAppType = ApplicationCommonWindow::Unknown;
  if (argc == 2) {
    anAppType = ApplicationCommonWindow::appTypeFromString(argv[1]);
    if (anAppType == ApplicationCommonWindow::Unknown) {
      std::cout << "Wrong application type. Use the one of following options:" << std::endl;
      std::cout << "Geometry | Topology | Triangulation | DataExchange | Ocaf | Viewer2d | Viewer3d" << std::endl;
    }
  }

  int aChoise = 0;
  while (1) {
    if (anAppType != ApplicationCommonWindow::Unknown) {
      ApplicationCommonWindow* mw = new ApplicationCommonWindow(anAppType);
      QString aResName(":/icons/lamp.png");
      mw->setWindowIcon(QPixmap(aResName));
      mw->show();
      return aQApp.exec();
    }
    std::cout << "Select a tutorial number:" << std::endl;
    std::cout << "1. Geometry" << std::endl;
    std::cout << "2. Topology" << std::endl;
    std::cout << "3. Triangulation" << std::endl;
    std::cout << "4. Data Exchange" << std::endl;
    std::cout << "5. OCAF" << std::endl;
    std::cout << "6. Viewer2d" << std::endl;
    std::cout << "7. Viewer3d" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "> ";
    std::cin >> aChoise;
    if (aChoise == 0)
      return 0;
    if ((aChoise < 1) || (aChoise > 7))
      anAppType = ApplicationCommonWindow::Unknown;
    else
      anAppType = (ApplicationCommonWindow::ApplicationType) (aChoise - 1);
  }
}

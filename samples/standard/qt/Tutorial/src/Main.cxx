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
#include <conio.h>

int main ( int argc, char* argv[] )
{
  Q_INIT_RESOURCE(Tutorial);
    QApplication a( argc, argv );

    QString resDir = ApplicationCommonWindow::getResourceDir();

    QLocale systemLocale = QLocale::system();
    QStringList ll = systemLocale.uiLanguages();

    QTranslator strTrans( 0 );
    Standard_Boolean isOK = strTrans.load("Tutorial-string.ts", "C:\Work\occt-CR31570\samples\standard\qt\Tutorial\res");
    if( isOK )
      a.installTranslator( &strTrans );

    if (argc < 2)
    {
      std::cout << "An argumetn absent";
      return 0;
    }

    QString aSampleType(argv[1]);
    ApplicationCommonWindow::ApplicationType anAppType = ApplicationCommonWindow::AppTypeFromString(aSampleType);
    if (anAppType != ApplicationCommonWindow::ApplicationType::Unknokwn)
    {
      ApplicationCommonWindow* mw = new ApplicationCommonWindow(anAppType);
      QString aResName(":/icons/lamp.png");
      mw->setWindowIcon( QPixmap( aResName ) );

      mw->show();
    
      return a.exec();
    }
    else
    {
      std::cerr << "Incorrect application type: \"" << aSampleType.toStdString()
        << "\". Pleace use: Geometry | Topology | Triangulation | DataExchange | Ocaf | Viewer3d | Viewer2d"
        << std::endl;
      std::cout << "Press any key...";
      getch();
      return -1;
    }

}

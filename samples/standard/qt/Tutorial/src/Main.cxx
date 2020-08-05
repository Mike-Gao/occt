//#include "ApplicationTut.h"
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

    QString anSampleType(argv[1]);
    ApplicationCommonWindow* mw = new ApplicationCommonWindow(anSampleType);
    QString aResName(":/icons/lamp.png");
    mw->setWindowIcon( QPixmap( aResName ) );

    mw->show();
    
    return a.exec();
}

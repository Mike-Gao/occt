//#include "ApplicationTut.h"
#include "ApplicationCommon.h"

#include <OSD_Environment.hxx>
#include <Standard_WarningsDisable.hxx>
#include <Standard_WarningsRestore.hxx>

#include <QApplication>
#include <QTranslator>
#include <QCommandLineParser>

int main ( int argc, char* argv[] )
{
#if QT_VERSION > 0x050000
    TCollection_AsciiString aPlugindsDirName = OSD_Environment ("QTDIR").Value();
    if (!aPlugindsDirName.IsEmpty())
      QApplication::addLibraryPath (QString (aPlugindsDirName.ToCString()) + "/plugins");
#endif
    QApplication a( argc, argv );

    QString resDir = ApplicationCommonWindow::getResourceDir();

    QTranslator strTrans( 0 );
    Standard_Boolean isOK = strTrans.load( "Common-string", resDir );
    if( isOK )
      a.installTranslator( &strTrans );

    QTranslator iconTrans( 0 );
    isOK = iconTrans.load( "Common-icon", resDir );
    if( isOK )
      a.installTranslator( &iconTrans );

    if (argc < 2)
    {
      std::cout << "An argumetn absent";
      return 0;
    }

    QString anSampleType(argv[1]);
    ApplicationCommonWindow* mw = new ApplicationCommonWindow(anSampleType);
    mw->setWindowTitle("Tutorial");
    QString aResName( resDir + "/lamp.png");
    mw->setWindowIcon( QPixmap( aResName ) );

    mw->show();
    
    return a.exec();
}

#include "ApplicationTut.h"
#include "DocumentTut.h"

#include <OSD_Environment.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QFileDialog>
#include <QStatusBar>
#include <QMdiSubWindow>
#include <Standard_WarningsRestore.hxx>

ApplicationTut::ApplicationTut(QString theSampleType)
    : ApplicationCommonWindow(theSampleType)
{
  createMakeBottleOperation();
}

ApplicationTut::~ApplicationTut()
{
}

void ApplicationTut::createMakeBottleOperation(){
	QPixmap MakeBottleIcon;
	QString dir = getTutResourceDir() + QString( "/" );
  MakeBottleIcon = QPixmap( dir+QObject::tr( "ICON_MAKE_BOTTLE" ) );
	
  QAction * MakeBottleAction = new QAction( MakeBottleIcon, QObject::tr("TBR_MAKEBOT"), this );
  MakeBottleAction->setToolTip( QObject::tr( "TBR_MAKEBOT" ) );
  MakeBottleAction->setStatusTip( QObject::tr("TBR_MAKEBOT") );
  MakeBottleAction->setShortcut( QObject::tr( "CTRL+M" ) );
	
	myMakeBottleBar = addToolBar( tr( "Make Bottle" ) );
  insertToolBar( getCasCadeBar(), myMakeBottleBar );
  myMakeBottleBar->addAction( MakeBottleAction );
	myMakeBottleBar->hide();
}

QString ApplicationTut::getTutResourceDir()
{
  static QString resDir (OSD_Environment ("CSF_TutorialResourcesDefaults").Value().ToCString());
  if (resDir.isEmpty())
    resDir = QString (OSD_Environment ("CSF_OCCTResourcePath").Value().ToCString()) + "/samples";
  return resDir;
}

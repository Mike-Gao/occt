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

#ifndef APPLICATION_COMMON_OVERVIEW_H
#define APPLICATION_COMMON_OVERVIEW_H

#include "BaseSample.h"
#include "CommonSample.h"
#include "DocumentCommon.h"
#include "GeomWidget.h"
#include "OcctHighlighter.h"
#include "TranslateDialog.h"
#include "View.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QMdiArea>
#include <QTextEdit>
#include <QList>
#include <QSignalMapper>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

enum StdActions
{
  FileNew, FilePrefUseVBO, FileClose, FilePreferences, FileQuit, ViewTool, ViewStatus, HelpAbout
};

enum ToolActions
{
  ToolWireframe, ToolShading, ToolColor, ToolMaterial, ToolTransparency, ToolDelete
};

enum ApplicationType 
{
  Geometry,
  Topology,
  Triangulation,
  DataExchange,
  Ocaf,
  Viewer2d,
  Viewer3d,
  Unknown
};

const QMap<ApplicationType, QString> ALL_CATEGORIES = 
{ 
  { ApplicationType::Geometry,"Geometry"}, 
  { ApplicationType::Topology, "Topology"},
  { ApplicationType::Triangulation, "Triangulation"},
  { ApplicationType::DataExchange, "DataExchange"},
  { ApplicationType::Ocaf, "OCAF"},
  { ApplicationType::Viewer3d, "3D viewer"},
  { ApplicationType::Viewer2d, "2D Viewer"}
};

class COMMONSAMPLE_EXPORT ApplicationCommonWindow: public QMainWindow
{
    Q_OBJECT

public:
  ApplicationCommonWindow(ApplicationType theCategory);

  ApplicationType GetApplicationType() { return myAppType; }
  void SetApplicationType(ApplicationType theApplicationType) { myAppType = theApplicationType; }

  static TCollection_AsciiString  getSampleSourceDir();
 	
protected:
  virtual DocumentCommon* createNewDocument();

public slots:
  virtual void    onAbout();
  virtual void    onChangeCategory(const QString& theCategory);

protected:
  template <typename PointerToMemberFunction>
  QAction* CreateAction(PointerToMemberFunction theHandlerMethod,
                        QString theActionName,
                        QString theShortcut = "",
                        QString theIconName = "");

  template <typename PointerToMemberFunction>
  QAction* CreateSample(PointerToMemberFunction theHandlerMethod,
                        const char* theActionName);

  virtual void  resizeEvent( QResizeEvent* );
  QMenu*        getFilePopup();
  QToolBar*     getCasCadeBar();

  QMenu* MenuFromJsonObject(QJsonValue theJsonValue, const QString& theKey, QWidget* theParent, QSignalMapper* theMapper);
  void MenuFormJson(const QString& thePath, QSignalMapper* theMapper);

private slots:
  void onCloseAllWindows() { qApp->closeAllWindows(); };

  void onProcessSample(const QString& theSampleName);
  void onProcessExchange(const QString& theSampleName);
  void onProcessOcaf(const QString& theSampleName);
  void onProcessViewer3d(const QString& theSampleName);
  void onProcessViewer2d(const QString& theSampleName);

private:

  ApplicationType myAppType;

	void createStandardOperations();

  QString selectFileName(const QString& theSampleName, TranslateDialog* theDialog, int& theMode);
  TranslateDialog* getDataExchangeDialog(const QString& theSampleName);
  TranslateDialog* getOcafDialog(const QString& theSampleName);

private:
  Handle(BaseSample) mySamples;


	QMap<StdActions,               QAction*>  myStdActions;
	QMap<ApplicationType,          QAction*>  myCategoryActions;
  QMap<ToolActions,              QAction*>  myToolActions;
  QMap<Graphic3d_NameOfMaterial, QAction*>  myMaterialActions;

	QToolBar*        myStdToolBar;
	QToolBar*        myCasCadeBar;
	QToolBar*        myViewBar;
	QMenu*           myFilePopup;
	QMenu*           myCategoryPopup;

  QList<QMenu*>    mySamplePopups;
  QSignalMapper*   mySampleMapper;
  QSignalMapper*   myExchangeMapper;
  QSignalMapper*   myOcafMapper;
  QSignalMapper*   myViewer3dMapper;
  QSignalMapper*   myViewer2dMapper;

  QSignalMapper*   myCatigoryMapper;

  QTextEdit*       myCodeView;
  QTextEdit*       myResultView;
  OcctHighlighter* myCodeViewHighlighter;

  GeomWidget*  myGeomWidget;

protected:
  DocumentCommon*  myDocument3d;
  DocumentCommon*  myDocument2d;
};

#endif



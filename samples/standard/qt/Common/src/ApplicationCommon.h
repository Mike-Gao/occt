#ifndef APPLICATIONCOMMON_H
#define APPLICATIONCOMMON_H

#include "DocumentCommon.h"
#include "BaseSample.h"
#include "GeomWidget.h"
#include "TranslateDialog.h"

#include "OcctHighlighter.h"

#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QMdiArea>
#include <QTextEdit>
#include <QList>
#include <QSignalMapper>

#include <Standard_WarningsRestore.hxx>
#include <Standard_WarningsDisable.hxx>

#include "CommonSample.h"
#include "View.h"

enum class StdActions
{
  FileNew, FilePrefUseVBO, FileClose, FilePreferences, FileQuit, ViewTool, ViewStatus, HelpAbout
};
enum class ToolActions
{
  ToolWireframe, ToolShading, ToolColor, ToolMaterial, ToolTransparency, ToolDelete
};

class COMMONSAMPLE_EXPORT ApplicationCommonWindow: public QMainWindow
{
    Q_OBJECT

public:
  ApplicationCommonWindow(QString theSampleType);

	static ApplicationCommonWindow* getApplication();
	static QString                  getResourceDir();
  static TCollection_AsciiString  getSampleSourceDir();

  QAction* getToolAction(ToolActions theActionId);
  QList<QAction*> getMaterialActions();

  enum class ApplicationType { Geometry, Topology, Triangulation, DataExchange, Ocaf, Viewer2d, Viewer3d, Unknokwn };
	
protected:
  virtual DocumentCommon* createNewDocument();

public slots:
  void            onUseVBO();
  virtual void    onAbout();
  void            onViewToolBar();
  void            onToolAction();
  void            windowsMenuActivated( bool checked/*int id*/ );
	void            onSetMaterial( int theMaterial );

protected:
  template <typename PointerToMemberFunction>
  QAction* CreateAction(PointerToMemberFunction theHandlerMethod,
                        const char* theActionName,
                        const char* theShortcut = NULL,
                        const char* theIconName = NULL);

  template <typename PointerToMemberFunction>
  QAction* CreateSample(PointerToMemberFunction theHandlerMethod,
                        const char* theActionName);

  virtual void  resizeEvent( QResizeEvent* );
  QMenu*        getFilePopup();
  QToolBar*     getCasCadeBar();

  QMenu* MenuFromJsonObject(QJsonValue theJsonValue, const QString& theKey, QWidget* theParent, QSignalMapper* theMapper);
  void MenuFormJson(const QString& thePath, QSignalMapper* theMapper);

private slots:
  void onCloseAllWindows() { qApp->closeAllWindows(); }

  void onProcessSample(const QString& theSampleName);
  void onProcessExchange(const QString& theSampleName);

private:

  ApplicationType APP_TYPE = ApplicationType::Topology;

  void    SetAppType(QString theParameter);
  QString GetTitle();

	void createStandardOperations();
	void createCasCadeOperations();

  QString selectFileName(const QString& theSampleName, int& theMode);
  TranslateDialog* ApplicationCommonWindow::getDialog(const QString& theSampleName);

private:
  Handle(BaseSample) mySamples;


	QMap<StdActions,               QAction*>  myStdActions;
  QMap<ToolActions,              QAction*>  myToolActions;
  QMap<Graphic3d_NameOfMaterial, QAction*>  myMaterialActions;

	QToolBar*        myStdToolBar;
	QToolBar*        myCasCadeBar;
	QToolBar*        myViewBar;
	QMenu*           myFilePopup;

  QList<QMenu*>    mySamplePopups;
  QSignalMapper*   mySampleMapper;
  QSignalMapper*   myExchangeMapper;
  QTextEdit*       myCodeView;
  QTextEdit*       myResultView;
  OcctHighlighter* myCodeViewHighlighter;

  GeomWidget*  myGeomWidget;

protected:
  DocumentCommon*  myDocument3d;
  DocumentCommon*  myDocument2d;
};

#endif



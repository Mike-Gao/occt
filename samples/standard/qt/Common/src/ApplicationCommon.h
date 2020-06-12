#ifndef APPLICATIONCOMMON_H
#define APPLICATIONCOMMON_H

#include "DocumentCommon.h"

#include "GeometrySamples.h"
#include "TopologySamples.h"
#include "GeomWidget.h"

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
  FileNewId, FilePrefUseVBOId, FileCloseId, FilePreferencesId, FileQuitId, ViewToolId, ViewStatusId, HelpAboutId
};
enum class ToolActions
{
  ToolWireframeId, ToolShadingId, ToolColorId, ToolMaterialId, ToolTransparencyId, ToolDeleteId
};

class COMMONSAMPLE_EXPORT ApplicationCommonWindow: public QMainWindow
{
    Q_OBJECT

public:
  ApplicationCommonWindow();

	static ApplicationCommonWindow* getApplication();
	static QString                  getResourceDir();
  static TCollection_AsciiString  getSampleSourceDir();

  QAction* getToolAction(ToolActions theActionId);
  QList<QAction*> getMaterialActions();
	
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

  QMenu* MenuFromJsonObject(QJsonValue theJsonValue, const QString& theKey, QWidget* theParent);
  void MenuFormJson(const QString& thePath);

private slots:
  void onCloseAllWindows() { qApp->closeAllWindows(); }

  void onProcessSample(const QString& theSampleName);

private:
  enum class ApplicationType { Geometry, Topology, Triangulation, Ocaf, Viewer2d, Viewer3d };
  static const ApplicationType APP_TYPE = ApplicationType::Geometry;

	void createStandardOperations();
	void createCasCadeOperations();

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
  QTextEdit*       myCodeView;
  QTextEdit*       myResultView;
  OcctHighlighter* myCodeViewHighlighter;

//  View*            myView;
  GeomWidget*  myGeomWidget;

protected:
  DocumentCommon*  myDocument3d;
  DocumentCommon*  myDocument2d;
};

#endif



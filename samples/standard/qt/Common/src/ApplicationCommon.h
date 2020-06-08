#ifndef APPLICATIONCOMMON_H
#define APPLICATIONCOMMON_H

#include "DocumentCommon.h"

#include "BaseSample.h"
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

class COMMONSAMPLE_EXPORT ApplicationCommonWindow: public QMainWindow
{
    Q_OBJECT

public:
	enum { FileNewId, FilePrefUseVBOId, FileCloseId, FilePreferencesId, FileQuitId, ViewToolId, ViewStatusId, HelpAboutId };
  enum { ToolWireframeId, ToolShadingId, ToolColorId, ToolMaterialId, ToolTransparencyId, ToolDeleteId };

  ApplicationCommonWindow();
  ~ApplicationCommonWindow();

	static ApplicationCommonWindow* getApplication();
	static QString                  getResourceDir();
  static TCollection_AsciiString  getSampleSourceDir();

  QList<QAction*>* getToolActions();
  QList<QAction*>* getMaterialActions();
	
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

private slots:
  void onCloseAllWindows() { qApp->closeAllWindows(); }

  void onProcessSample(const QString& theSampleName);

private:
	void createStandardOperations();
	void createCasCadeOperations();

private:
  BaseSample mySamples;

	QList<QAction*>  myStdActions;
  QList<QAction*>  myToolActions;
  QList<QAction*>  myMaterialActions;

	QToolBar*        myStdToolBar;
	QToolBar*        myCasCadeBar;
	QMenu*           myFilePopup;

  QList<QMenu*>    mySamplePopups;
  QSignalMapper*   mySampleMapper;
  QTextEdit*       myCodeView;
  QTextEdit*       myResultView;
  OcctHighlighter* myCodeViewHighlighter;

  View*            myView;

protected:
  DocumentCommon*  myDocument;
};

#endif



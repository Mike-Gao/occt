#ifndef APPLICATIONCOMMON_H
#define APPLICATIONCOMMON_H

#include "DocumentCommon.h"

#include "BaseSample.hxx"


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

class COMMONSAMPLE_EXPORT ApplicationCommonWindow: public QMainWindow
{
    Q_OBJECT

public:
	enum { FileNewId, FilePrefUseVBOId, FileCloseId, FilePreferencesId, FileQuitId, ViewToolId, ViewStatusId, HelpAboutId };
  enum { ToolWireframeId, ToolShadingId, ToolColorId, ToolMaterialId, ToolTransparencyId, ToolDeleteId };

  ApplicationCommonWindow();
  ~ApplicationCommonWindow();

	static QMdiArea*                getWorkspace();
	static ApplicationCommonWindow* getApplication();
	static QString                  getResourceDir();
  static TCollection_AsciiString  getSampleSourceDir();

	virtual void     updateFileActions();
  QList<QAction*>* getToolActions();
  QList<QAction*>* getMaterialActions();
	
protected:
  virtual DocumentCommon* createNewDocument();
  int&                    getNbDocument();

public slots:
	
  DocumentCommon* onNewDoc();
  void            onCloseWindow();
  void            onUseVBO();
	virtual void    onCloseDocument( DocumentCommon* theDoc );
  virtual void    onSelectionChanged();
  virtual void    onAbout();
  void            onViewToolBar();
	void            onViewStatusBar();
  void            onToolAction();
	void            onCreateNewView();
  void            onWindowActivated ( QWidget * w );
	void            windowsMenuAboutToShow();
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
  bool          isDocument();
  QMenu*        getFilePopup();
  QAction*      getFileSeparator();
  QToolBar*     getCasCadeBar();

  QMenu* MenuFromJsonObject(QJsonValue theJsonValue, const QString& theKey, QWidget* theParent);

private slots:
  void onCloseAllWindows() { qApp->closeAllWindows(); }

  void onProcessSample(const QString& theSampleName);
  void SimpleAction();
private:
	void createStandardOperations();
	void createCasCadeOperations();
	void createWindowPopup();

private:
  int                             myNbDocuments;
	bool                            myIsDocuments;

  BaseSample mySamples;

	QList<QAction*>                 myStdActions;
  QList<QAction*>                 myToolActions;
  QList<QAction*>                 myMaterialActions;
  //QList<DocumentCommon*>          myDocuments;

	QToolBar*                       myStdToolBar;
	QToolBar*                       myCasCadeBar;
	QMenu*                          myFilePopup;
	QMenu*                          myWindowPopup;

  QList<QMenu*> mySamplePopups;
  QSignalMapper* mySampleMapper;
  QTextEdit* myCodeView;
  QTextEdit* myResultView;

  QAction*                        myFileSeparator;

protected:
  QList<DocumentCommon*>          myDocuments;
};

#endif



#ifndef DOCUMENTCOMMON_H
#define DOCUMENTCOMMON_H

#include "CommonSample.h"

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QList>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

class ApplicationCommonWindow;

class COMMONSAMPLE_EXPORT DocumentCommon : public QObject
{
	Q_OBJECT

public:
	DocumentCommon(ApplicationCommonWindow* );
	~DocumentCommon();

	Handle(AIS_InteractiveContext) getContext();
  Handle(V3d_Viewer)             getViewer();
  void                           setViewer(Handle(V3d_Viewer) theViewer);

  void SetObjects(const NCollection_Vector<Handle(AIS_InteractiveObject)>& theObjects,
                  Standard_Boolean theDisplayShaded = Standard_False);
  bool IsEmpty() const { return myContextIsEmpty; } 
	
protected:

signals:
  void selectionChanged();
	void sendCloseDocument( DocumentCommon* );

private:
  Handle(V3d_Viewer) Viewer (const Standard_ExtString theName,
                             const Standard_CString theDomain,
                             const Standard_Real theViewSize,
                             const V3d_TypeOfOrientation theViewProj,
                             const Standard_Boolean theComputedMode,
                             const Standard_Boolean theDefaultComputedMode );

protected:
	Handle(V3d_Viewer)             myViewer;
	Handle(AIS_InteractiveContext) myContext;
  bool myContextIsEmpty;
};

#endif



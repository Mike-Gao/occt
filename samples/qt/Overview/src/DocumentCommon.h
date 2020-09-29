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

#ifndef DOCUMENT_COMMON_OVERVIEW_H
#define DOCUMENT_COMMON_OVERVIEW_H

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
  void Clear();
  bool IsEmpty() const { return myContextIsEmpty; } 

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

	Handle(V3d_Viewer)             myViewer;
	Handle(AIS_InteractiveContext) myContext;
  bool myContextIsEmpty;
};

#endif



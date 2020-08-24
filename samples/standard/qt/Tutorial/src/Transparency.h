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

#include <Standard_WarningsDisable.hxx>
#include <QDialog>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>

class DialogTransparency : public QDialog
{
	Q_OBJECT
public:
	DialogTransparency ( QWidget * parent=0, Qt::WindowFlags f=0, bool modal=true );
	~DialogTransparency();
signals:
	void sendTransparencyChanged(int value);
};
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

#include "Transparency.h"

#include <Standard_WarningsDisable.hxx>
#include <QHBoxLayout>
#include <QSpinBox>
#include <Standard_WarningsRestore.hxx>

DialogTransparency::DialogTransparency(QWidget* parent, Qt::WindowFlags f, bool modal)
  : QDialog(parent, f)
{
  setModal(modal);
  QHBoxLayout* base = new QHBoxLayout(this);
  base->setMargin(3);
  base->setSpacing(3);
  QSpinBox* aSpin = new QSpinBox(this);
  aSpin->setRange(0, 10);
  aSpin->setSingleStep(1);
  base->addWidget(aSpin);
  connect(aSpin, SIGNAL(valueChanged(int)), this, SIGNAL(sendTransparencyChanged(int)));
}

DialogTransparency::~DialogTransparency()
{
}

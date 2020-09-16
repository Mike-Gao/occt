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
#include <QSlider>
#include <QLabel>
#include <Standard_WarningsRestore.hxx>

DialogTransparency::DialogTransparency(QWidget* parent)
  : QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
  setWindowTitle(tr("Transparency"));
  QHBoxLayout* base = new QHBoxLayout(this);

  base->addWidget(new QLabel("0", this));

  mySlider = new QSlider(Qt::Horizontal, this);
  mySlider->setRange(0, 10);
  mySlider->setTickPosition(QSlider::TicksBelow);
  mySlider->setTickInterval(1);
  mySlider->setPageStep(2);
  base->addWidget(mySlider);
  connect(mySlider, SIGNAL(valueChanged(int)), this, SIGNAL(sendTransparencyChanged(int)));

  base->addWidget(new QLabel("10", this));
}

DialogTransparency::~DialogTransparency()
{
}


int DialogTransparency::value() const
{
  return mySlider->value();
}

void DialogTransparency::setValue(int theVal) const
{
  mySlider->setValue(theVal);
}


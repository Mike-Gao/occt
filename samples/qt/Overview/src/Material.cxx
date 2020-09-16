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

#include "Material.h"

#include <Standard_WarningsDisable.hxx>
#include <QPushButton>
#include <QLayout>
#include <QSignalMapper>
#include <Standard_WarningsRestore.hxx>

#include <Graphic3d_NameOfMaterial.hxx>

DialogMaterial::DialogMaterial(QWidget * parent,
  bool modal, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  setModal(modal);
  QPushButton* b;
  QVBoxLayout* vbl = new QVBoxLayout(this);
  vbl->setMargin(8);

  QSignalMapper *sm = new QSignalMapper(this);
  connect(sm, SIGNAL(mapped(int)), this, SIGNAL(sendMaterialChanged(int)));

  b = new QPushButton(tr("Plaster"), this);
  sm->setMapping(b, (int)Graphic3d_NOM_PLASTER);
  connect(b, SIGNAL(clicked()), sm, SLOT(map()));
  b->setCheckable(true);
  connect(b, SIGNAL(toggled(bool)), this, SLOT(updateButtons(bool)));
  myButtons.append(b);
  vbl->addWidget(b);

  b = new QPushButton(QObject::tr("Brass"), this);
  sm->setMapping(b, (int)Graphic3d_NOM_BRASS);
  connect(b, SIGNAL(clicked()), sm, SLOT(map()));
  b->setCheckable(true);
  connect(b, SIGNAL(toggled(bool)), this, SLOT(updateButtons(bool)));
  myButtons.append(b);
  vbl->addWidget(b);

  b = new QPushButton(tr("Bronze"), this);
  sm->setMapping(b, (int)Graphic3d_NOM_BRONZE);
  connect(b, SIGNAL(clicked()), sm, SLOT(map()));
  b->setCheckable(true);
  connect(b, SIGNAL(toggled(bool)), this, SLOT(updateButtons(bool)));
  myButtons.append(b);
  vbl->addWidget(b);

  b = new QPushButton(tr("Copper"), this);
  sm->setMapping(b, (int)Graphic3d_NOM_COPPER);
  connect(b, SIGNAL(clicked()), sm, SLOT(map()));
  b->setCheckable(true);
  connect(b, SIGNAL(toggled(bool)), this, SLOT(updateButtons(bool)));
  myButtons.append(b);
  vbl->addWidget(b);

  b = new QPushButton(tr("Gold"), this);
  sm->setMapping(b, (int)Graphic3d_NOM_GOLD);
  connect(b, SIGNAL(clicked()), sm, SLOT(map()));
  b->setCheckable(true);
  connect(b, SIGNAL(toggled(bool)), this, SLOT(updateButtons(bool)));
  myButtons.append(b);
  vbl->addWidget(b);

  b = new QPushButton(tr("Pewter"), this);
  sm->setMapping(b, (int)Graphic3d_NOM_PEWTER);
  connect(b, SIGNAL(clicked()), sm, SLOT(map()));
  b->setCheckable(true);
  connect(b, SIGNAL(toggled(bool)), this, SLOT(updateButtons(bool)));
  myButtons.append(b);
  vbl->addWidget(b);

  b = new QPushButton(tr("Plastic"), this);
  sm->setMapping(b, (int)Graphic3d_NOM_PLASTIC);
  connect(b, SIGNAL(clicked()), sm, SLOT(map()));
  b->setCheckable(true);
  connect(b, SIGNAL(toggled(bool)), this, SLOT(updateButtons(bool)));
  myButtons.append(b);
  vbl->addWidget(b);

  b = new QPushButton(tr("Silver"), this);
  sm->setMapping(b, (int)Graphic3d_NOM_SILVER);
  connect(b, SIGNAL(clicked()), sm, SLOT(map()));
  b->setCheckable(true);
  connect(b, SIGNAL(toggled(bool)), this, SLOT(updateButtons(bool)));
  myButtons.append(b);
  vbl->addWidget(b);
}

DialogMaterial::~DialogMaterial()
{
}

void DialogMaterial::updateButtons(bool isOn)
{
  if (!isOn)
    return;

  QPushButton*sentBy = (QPushButton*)sender();

  for (int i = 0; i < myButtons.size(); i++) {
    QPushButton* b = myButtons.at(i);
    if (b != sentBy) {
      b->setEnabled(true);
      b->setChecked(false);
    }
    else {
      b->setEnabled(false);
    }
  }
}


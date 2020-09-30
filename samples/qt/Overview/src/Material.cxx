// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "Material.h"

#include <Standard_WarningsDisable.hxx>
#include <QLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QWidget>
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

void DialogMaterial::updateButtons(bool isOn)
{
  if (!isOn)
  {
    return;
  }
  QPushButton*sentBy = (QPushButton*)sender();

  for (int i = 0; i < myButtons.size(); i++) 
  {
    QPushButton* b = myButtons.at(i);
    if (b != sentBy) 
    {
      b->setEnabled(true);
      b->setChecked(false);
    }
    else 
    {
      b->setEnabled(false);
    }
  }
}


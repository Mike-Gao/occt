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

#ifndef TRANSLATEDIALOG_H
#define TRANSLATEDIALOG_H

#include <Standard_WarningsDisable.hxx>
#include <QFileDialog>
#include <QWidget>
#include <QShowEvent>
#include <QListView>
#include <QComboBox>
#include <QList>
#include <Standard_WarningsRestore.hxx>

class TranslateDialog : public QFileDialog
{
public:
  TranslateDialog(QWidget* = 0, Qt::WindowFlags flags = 0, bool = true);
  ~TranslateDialog();
  int                   getMode() const;
  void                  setMode(const int);
  void                  addMode(const int, const QString&);
  void                  clear();

protected:
  void                  showEvent(QShowEvent* event);

private:
  QListView*            findListView(const QObjectList&);

private:
  QComboBox*            myBox;
  QList<int>            myList;
};

#endif // TRANSLATEDIALOG_H

//-----------------------------------------------------------------------------
// Created on: 2019-03-28
// Created by: Vadim LEONTIEV
// Copyright (c) 2019 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS.
//
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of this copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any third party.
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.
//-----------------------------------------------------------------------------

#ifndef ViewControl_TableDoubleVector_H
#define ViewControl_TableDoubleVector_H

#include <inspector/ViewControl.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <Standard_Macro.hxx>
#include <QDialog>
#include <QString>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

class QDialogButtonBox;
class QTableView;

//! \class ViewControl_PointCoordinates
//! \dialog of change the point coordinates
class VIEWCONTROL_EXPORT ViewControl_TableDoubleVector : public QDialog
{
  Q_OBJECT
public:

  //! Constructor
  ViewControl_TableDoubleVector(QWidget* theParent);

  //! Destructor
  virtual ~ViewControl_TableDoubleVector() Standard_OVERRIDE{}

  //! Inits control by the vector value
  //! \param theVector text vector value
  void SetVectorValue(const QString& theVector);

  //! Returns vector value
  //! \return QList<QVariant> vector value
  static QList<QVariant> ViewControl_TableDoubleVector::GetListVector(const QString& theVector);

  //! Returns vector value
  //! \return text vector value
  QString GetVector() const;

  //! Converts vector to string value in form
  //! \param theVector vector value
  //! \return text value
  QString VectorToString(const QList<QVariant>& theVector) const;

private:
  //! Returns symbol used as a separtor of vector components in string conversion
  //! \return symbol value
  static QString DoubleSeparator() { return ","; }

private:
  QTableView* myParameters; //! current vector parameters
  QDialogButtonBox* myDialogButtons; //! OK/Cancel buttons

};

#endif // ViewControl_TableDoubleVector_H

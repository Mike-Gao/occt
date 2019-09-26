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

#include <inspector/ViewControl_TableDoubleVector.hxx>
#include <inspector/ViewControl_TableItemDelegate.hxx>
#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/TreeModel_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QTableView>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

//! Model for a table of parameters: Values: X, Y, Z
class ViewControl_ParametersModelVector : public ViewControl_TableModelValues
{
public:
  ViewControl_ParametersModelVector(ViewControl_TableDoubleVector* theDoubleVector)
    : ViewControl_TableModelValues(), myDoubleVector(theDoubleVector) {}

  virtual ~ViewControl_ParametersModelVector() {}
  
  //! Inits model by the parameters vector
  //! \param theVector model the vector
  void SetVector(const QList<QVariant>& theVector)
  {
    if (myVector.isEmpty())
    { myVector.clear(); }

    for (int aNumberVector = 0; aNumberVector < theVector.size(); ++aNumberVector)
    {
      myVector.append(theVector[aNumberVector]);
    }
  }

  //! Returns current vector
  //! \return vector value to string 
  QList<QVariant> GetVector() const { return myVector; }

  //! Returns current vector
  //! \return vector value to QList
  QList<QVariant> GetListFromString(const QString& theVector) const
  {
    QList<QVariant> aDoubleList;

    QStringList aList = theVector.split(QString(","), QString::SkipEmptyParts);
    if (aList.isEmpty())
      return aDoubleList;

    for (int aNumberValue = 0; aNumberValue < aList.size(); ++aNumberValue)
    {
      aDoubleList.append(QVariant(aList[aNumberValue]));
    }

    return aDoubleList;
  }
  
  //! Returns item information(short) for display role.
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant Data(const int theRow, const int theColumn,
    int theRole = Qt::DisplayRole) const Standard_OVERRIDE
  {
    if (theRole != Qt::DisplayRole)
      return QVariant();

    bool isFirstColumn = theColumn == 0;
    switch (theRow)
    {
    case 0: return isFirstColumn ? QVariant("X") : myVector[theRow];
    case 1: return isFirstColumn ? QVariant("Y") : myVector[theRow];
    case 2: return isFirstColumn ? QVariant("Z") : myVector[theRow];
    }
    return QVariant();
  }

  //! Sets content of the model index for the given role, it is applyed to internal container of values
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return true if the value is changed
  virtual bool SetData(const int theRow, const int theColumn, const QVariant& theValue, int)
  {
    if (theColumn != 1 || theRow < 0 || theRow > 2)
      return false;

    switch (theRow)
    {
    case 0: myVector[theRow] = theValue; break;
    case 1: myVector[theRow] = theValue; break;
    case 2: myVector[theRow] = theValue; break;
    }

    return true;
  }

  //! Returns number of tree level line items = colums in table view
  virtual int ColumnCount(const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 2; }

  //! Returns onlly one row in table view
  virtual int RowCount(const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 3; }

  //! Returns editable flag for DoubleVector
  //! \return flags
  Qt::ItemFlags Flags(const QModelIndex& theIndex) const
  {
    Qt::ItemFlags aFlags = ViewControl_TableModelValues::Flags(theIndex);

    if (theIndex.column() == 1 && theIndex.row() >= 0 && theIndex.row() <= 2)
      aFlags = aFlags | Qt::ItemIsEditable;

    return aFlags;
  }

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  virtual ViewControl_EditType GetEditType(const int theRow, const int theColumn) const
  {
    if (theColumn == 1 && theRow >= 0 && theRow <= 2)
      return ViewControl_EditType_Double;

    return ViewControl_EditType_None;
  }

private:
  QList<QVariant> myVector;
  ViewControl_TableDoubleVector* myDoubleVector;
};
  

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================

ViewControl_TableDoubleVector::ViewControl_TableDoubleVector(QWidget* theParent)
: QDialog(theParent)
{
  QGridLayout* aLayout = new QGridLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);

  myParameters = new QTableView(this);

  ViewControl_TableModel* aTableModel = new ViewControl_TableModel(myParameters);
  aTableModel->SetModelValues(new ViewControl_ParametersModelVector(this));
  myParameters->setModel(aTableModel);

  ViewControl_TableItemDelegate* anItemDelegate = new ViewControl_TableItemDelegate();
  anItemDelegate->SetModelValues(aTableModel->GetModelValues());
  myParameters->setItemDelegate(anItemDelegate);

  myParameters->verticalHeader()->setDefaultSectionSize(myParameters->verticalHeader()->minimumSectionSize());
  myParameters->verticalHeader()->setVisible(false);
  myParameters->horizontalHeader()->setVisible(false);
  myParameters->setMinimumHeight(myParameters->verticalHeader()->minimumSectionSize() * aTableModel->rowCount() +
    TreeModel_Tools::HeaderSectionMargin());
  myParameters->setMinimumWidth(myParameters->horizontalHeader()->defaultSectionSize() * aTableModel->columnCount() +
    TreeModel_Tools::HeaderSectionMargin());

  QItemSelectionModel* aSelectionModel = new QItemSelectionModel(myParameters->model());
  myParameters->setSelectionMode(QAbstractItemView::SingleSelection);
  myParameters->setSelectionModel(aSelectionModel);

  aLayout->addWidget(myParameters, 0, 0);

  myDialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(myDialogButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(myDialogButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  aLayout->addWidget(myDialogButtons, 1, 0, 1, 2);
}

// =======================================================================
// function : SetVectorValue
// purpose :
// =======================================================================

void ViewControl_TableDoubleVector::SetVectorValue(const QString& theVector)
{
  QList<QVariant> aVector = GetListVector(theVector);
  // parameters model
  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*> (myParameters->model());
  ViewControl_ParametersModelVector* aParametersModel = dynamic_cast<ViewControl_ParametersModelVector*> (aTableModel->GetModelValues());
  aParametersModel->SetVector(aVector);
}

// =======================================================================
// function : GetListVector
// purpose :
// =======================================================================

QList<QVariant> ViewControl_TableDoubleVector::GetListVector(const QString& theVector)
{
  QList<QVariant> aDoubleList;

  QStringList aList = theVector.split(ViewControl_TableDoubleVector::DoubleSeparator(), QString::SkipEmptyParts);

  if (aList.isEmpty())
    return aDoubleList;

  for (int aNumberValue = 0; aNumberValue < aList.size(); ++aNumberValue)
  {
    aDoubleList.append(QVariant(aList[aNumberValue]));
  }

  return aDoubleList;
}

// =======================================================================
// function : GetVector
// purpose :
// =======================================================================

QString ViewControl_TableDoubleVector::GetVector() const
{
  ViewControl_TableModel* aTableModel = dynamic_cast<ViewControl_TableModel*> (myParameters->model());
  ViewControl_ParametersModelVector* aParametersModel = dynamic_cast<ViewControl_ParametersModelVector*> (aTableModel->GetModelValues());

  return VectorToString(aParametersModel->GetVector());
}

// =======================================================================
// function : VectorToString
// purpose :
// =======================================================================

QString ViewControl_TableDoubleVector::VectorToString(const QList<QVariant>& theVector) const
{
  QString aVectorToString;

  for (int aNumberValue = 0; aNumberValue < theVector.size(); ++aNumberValue)
  {
    aVectorToString += theVector[aNumberValue].toString() + DoubleSeparator();
  }
  aVectorToString.remove(aVectorToString.length() - 1, 1);

  return aVectorToString;
}


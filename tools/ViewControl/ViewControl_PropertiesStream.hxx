// Created on: 2019-04-28
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef ViewControl_PropertiesStream_H
#define ViewControl_PropertiesStream_H

#include <Standard.hxx>
#include <Standard_SStream.hxx>
#include <NCollection_Vector.hxx>
#include <TCollection_AsciiString.hxx>

#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

DEFINE_STANDARD_HANDLE (ViewControl_PropertiesStream, TreeModel_ItemProperties)

//! \class ViewControl_PropertiesStream
//! \brief This is an interace for ViewControl_TableModel to give real values of the model
//! It should be filled or redefined.
class ViewControl_PropertiesStream : public TreeModel_ItemProperties
{
public:

  //! Constructor
  Standard_EXPORT ViewControl_PropertiesStream (const TreeModel_ItemBasePtr& theItem)
    : TreeModel_ItemProperties (theItem) {}

  //! Destructor
  virtual ~ViewControl_PropertiesStream() {}

  //! If me has internal values, it should be initialized here.
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! If the item has internal values, there should be reseted here.
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns number of table columns
  //! \return an integer value
  virtual int GetTableColumnCount() const { initItem(); return myColumnCount; }

  //! Returns number of rows, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int GetTableRowCount() const Standard_OVERRIDE;

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It returns value only for DisplayRole.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant GetTableData (const int theRow, const int theColumn, const int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns presentation of the attribute to be visualized in the view
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \thePresentations [out] container of presentation handles to be visualized
  Standard_EXPORT virtual void GetPresentations (const int theRow, const int theColumn,
    NCollection_List<Handle(Standard_Transient)>& thePresentations) Standard_OVERRIDE;

protected:
  //! Returns values
  //! @return values
  const NCollection_Vector<TCollection_AsciiString>& getValues() const { return myValues; }

protected:

  //! Initialize me.
  Standard_EXPORT void initItem() const;

public:
  DEFINE_STANDARD_RTTIEXT (ViewControl_PropertiesStream, TreeModel_ItemProperties)

protected:
  NCollection_Vector<TCollection_AsciiString> myValues; //!< container of values
  Standard_Integer myColumnCount; //!< value to present container of values into table
};

#endif

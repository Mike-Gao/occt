// Created on: 2018-06-10
// Created by: Natalia Ermolaeva
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Message_AttributeVectorOfReal_HeaderFile
#define _Message_AttributeVectorOfReal_HeaderFile

#include <Message_Attribute.hxx>
#include <TCollection_AsciiString.hxx>

#include <NCollection_Vector.hxx>

class Message_PerfMeter;
class Message_Report;

//! Alert object storing container of Standard_Real values in its field
//! Values might be presented as table if column size is defined
class Message_AttributeVectorOfReal : public Message_Attribute
{
public:

  //! Constructor with string argument
  Standard_EXPORT Message_AttributeVectorOfReal (const NCollection_Vector<Standard_Real>& theValues,
                                                 const Standard_Integer& theColumnCount,
                                                 const TCollection_AsciiString& theName = TCollection_AsciiString(),
                                                 const TCollection_AsciiString& theDescription = TCollection_AsciiString());

  //! Sets the values
  //! @param theValues container of values
  Standard_EXPORT void SetValues (const NCollection_Vector<Standard_Real>& theValues);

  //! Returns values
  //! @return values
  const NCollection_Vector<Standard_Real>& GetValues() const { return myValues; }

  //! Sets value to present values in a table view
  //! \param theValue value of division the values on sub-containers
  void SetColumnCount (const Standard_Integer& theValue) { myColumnCount = theValue; }

  //! Gets value to present values in a table view
  //! \param theSubSize value of division the values on sub-containers
  Standard_Integer GetColumnCount() const { return myColumnCount; }

  //! Returns description of alert if it is set
  //! @return alert description
  virtual const TCollection_AsciiString& GetDescription() const;

  //! Return true if the current and parameter alert have the same content
  //! \param theAttribute an attribute to compare
  //! @return True if equal.
  virtual Standard_EXPORT Standard_Boolean IsSame (const Handle(Message_Attribute)& theAttribute) Standard_OVERRIDE;

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(Message_AttributeVectorOfReal, Message_Attribute)

private:
  NCollection_Vector<Standard_Real> myValues; //!< container of values
  Standard_Integer myColumnCount; //!< value to present container of values into table

  TCollection_AsciiString myCachedValue; //!< short description of the values in form: [size] : first ... last
};

#endif // _Message_AttributeVectorOfReal_HeaderFile

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

#ifndef _Message_AttributeVectorOfValues_HeaderFile
#define _Message_AttributeVectorOfValues_HeaderFile

#include <Message_Attribute.hxx>
#include <TCollection_AsciiString.hxx>

#include <NCollection_Vector.hxx>

class Message_PerfMeter;
class Message_Report;

//! Alert object storing container of Standard_Real values in its field
class Message_AttributeVectorOfValues : public Message_Attribute
{
public:

  //! Constructor with string argument
  Standard_EXPORT Message_AttributeVectorOfValues (const Standard_SStream& theStream,
                                                   const TCollection_AsciiString& theName = TCollection_AsciiString(),
                                                   const TCollection_AsciiString& theDescription = TCollection_AsciiString());

  //! Sets the values
  //! @param theValues container of values
  Standard_EXPORT void SetValues (const NCollection_Vector<TCollection_AsciiString>& theValues);

  //! Returns values
  //! @return values
  const NCollection_Vector<TCollection_AsciiString>& GetValues() const { return myValues; }

  //! Returns description of alert if it is set
  //! @return alert description
  virtual const TCollection_AsciiString& GetDescription() const;


  //! Sets value to present values in a table view
  //! \param theValue value of division the values on sub-containers
  void SetColumnCount (const Standard_Integer& theValue) { myColumnCount = theValue; }

  //! Gets value to present values in a table view
  //! \param theSubSize value of division the values on sub-containers
  Standard_Integer GetColumnCount() const { return myColumnCount; }

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(Message_AttributeVectorOfValues, Message_Attribute)

private:
  NCollection_Vector<TCollection_AsciiString> myValues; //!< container of values
  Standard_Integer myColumnCount; //!< value to present container of values into table

  TCollection_AsciiString myCachedValue; //!< short description of the values in form: [size] : first ... last
};

#endif // _Message_AttributeVectorOfValues_HeaderFile
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

#ifndef _Message_AttributeObject_HeaderFile
#define _Message_AttributeObject_HeaderFile

#include <Message_Attribute.hxx>

class Standard_Transient;

//! Alert object storing a transient object
class Message_AttributeObject : public Message_Attribute
{
public:
  //! Constructor with string argument
  Message_AttributeObject (const Handle(Standard_Transient)& theObject,
    const TCollection_AsciiString& theName = TCollection_AsciiString())
  : Message_Attribute (theName) { myObject = theObject; }

  //! Returns object
  //! @return the object instance
  const Handle(Standard_Transient)& Object() { return myObject; }

  //! Sets the object
  //! @param theObject an instance
  void SetObject (const Handle(Standard_Transient)& theObject) { myObject = theObject; }

  DEFINE_STANDARD_RTTI_INLINE(Message_AttributeObject, Message_Attribute)

private:
  Handle(Standard_Transient) myObject; //!< alert object
};

#endif // _Message_AttributeObject_HeaderFile

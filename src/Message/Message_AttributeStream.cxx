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

#include <Message_AttributeStream.hxx>

#include <Message.hxx>
#include <Message_Msg.hxx>
#include <Message_PerfMeter.hxx>
#include <Message_Report.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeStream, Message_Attribute)

//=======================================================================
//function : SetValues
//purpose  : 
//=======================================================================
Message_AttributeStream::Message_AttributeStream (const Standard_SStream& theStream,
                                                  const TCollection_AsciiString& theName,
                                                  const TCollection_AsciiString& theDescription)
: Message_Attribute(theName, theDescription)
{
  SetStream (theStream);
}

//=======================================================================
//function : SetStream
//purpose  : 
//=======================================================================

void Message_AttributeStream::SetStream (const Standard_SStream& theStream)
{
  TCollection_AsciiString aStreamStr (theStream.str().c_str());
  myStream << aStreamStr;
}

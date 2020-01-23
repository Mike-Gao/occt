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

#ifndef _Message_Alerts_HeaderFile
#define _Message_Alerts_HeaderFile

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeStream.hxx>
#include <Message_Gravity.hxx>
#include <Message_Messenger.hxx>
#include <Message_Report.hxx>

#include <NCollection_Vector.hxx>
#include <TCollection_AsciiString.hxx>

#define MESSAGE_INFO(Name) \
  { \
    if (!Message::DefaultReport().IsNull()) \
    { \
     if (Message::DefaultReport()->IsActiveInMessenger())\
     { Message::DefaultMessenger() << Name << Message_EndLine; } \
     else \
      Message_AlertExtended::AddAlert (Message::DefaultReport(), \
        new Message_Attribute (Name), Message_Info); \
    } \
  }

#define MESSAGE_INFO_OBJECT(Object, Name) \
  { \
    if (!Message::DefaultReport().IsNull()) \
    { \
     if (Message::DefaultReport()->IsActiveInMessenger())\
     { Message::DefaultMessenger() << Name << Object << Message_EndLine; } \
     else \
      Message_AlertExtended::AddAlert (Message::DefaultReport(), \
        new Message_AttributeObject (Object, Name), Message_Info); \
    } \
  }

#define MESSAGE_INFO_STREAM(StreamValues, Name) \
  { \
    if (!Message::DefaultReport().IsNull()) \
    { \
     if (Message::DefaultReport()->IsActiveInMessenger())\
     { Message::DefaultMessenger() << Name << StreamValues << Message_EndLine; } \
     else \
      Message_AlertExtended::AddAlert (Message::DefaultReport(), \
        new Message_AttributeStream (StreamValues, Name), Message_Info); \
    } \
  }

#endif // _Message_Alerts_HeaderFile

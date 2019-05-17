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

#ifndef _TopoDS_AlertAttribute_HeaderFile
#define _TopoDS_AlertAttribute_HeaderFile

#include <Message_Attribute.hxx>
#include <TopoDS_Shape.hxx>

//! Alert attributre object storing TopoDS shape in its field
class TopoDS_AlertAttribute : public Message_Attribute 
{
public:
  //! Constructor with shape argument
  Standard_EXPORT TopoDS_AlertAttribute (const TopoDS_Shape& theShape,
                                         const TCollection_AsciiString& theName = TCollection_AsciiString(),
                                         const TCollection_AsciiString& theDescription = TCollection_AsciiString());

  //! Returns contained shape
  const TopoDS_Shape& GetShape() const { return myShape; }
  
  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(TopoDS_AlertAttribute, Message_Attribute)

private:
  TopoDS_Shape myShape;
};

#define MESSAGE_INFO_SHAPE(Shape, Name, Description, PerfMeter, ParentAlert) \
  { \
    if (!Message_Report::CurrentReport().IsNull() && \
         Message_Report::CurrentReport()->IsActive (Message_Info)) \
    { \
      OCCT_Message_Alert = Message_AlertExtended::AddAlert (Message_Report::CurrentReport(), \
        new TopoDS_AlertAttribute (Shape, Name, Description), PerfMeter, ParentAlert); \
    } \
  }

#endif // _TopoDS_AlertAttribute_HeaderFile
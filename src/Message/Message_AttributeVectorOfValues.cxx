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

#include <Message_AttributeVectorOfValues.hxx>

#include <Message.hxx>
#include <Message_Msg.hxx>
#include <Message_PerfMeter.hxx>
#include <Message_Report.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeVectorOfValues, Message_Attribute)

//=======================================================================
//function : SetValues
//purpose  : 
//=======================================================================
Message_AttributeVectorOfValues::Message_AttributeVectorOfValues (const Standard_SStream& theStream,
                                                                  const TCollection_AsciiString& theName,
                                                                  const TCollection_AsciiString& theDescription)
: Message_Attribute(theName, theDescription)
{
  TCollection_AsciiString aStream (theStream.str().c_str());
  Standard_Character aSeparator = Message::DumpSeparator();
  Standard_Integer aColumnCount = 0;

  TCollection_AsciiString aCurrentString = aStream;
  Standard_Integer aPosition = aCurrentString.Search (aSeparator);
  if (aPosition >= 1)
  {
    TCollection_AsciiString aTailString = aCurrentString.Split (aPosition);
    Standard_Boolean aClassNameFound = Standard_False;
    while (!aCurrentString.IsEmpty())
    {
      TCollection_AsciiString aValueString = aCurrentString;
      aPosition = aValueString.Search (aSeparator);
      if (aPosition < 0 )
        break;
      aCurrentString = aValueString.Split (aPosition - 1);

      if (!aColumnCount)
      {
        if (!aClassNameFound)
          aClassNameFound = Standard_True;
        else
        {
          if (!aValueString.IsIntegerValue())
            break; // not correct Dump, in correct the first value is number of property columns
          aColumnCount = aValueString.IntegerValue();
        }
      }
      else
        myValues.Append (aValueString);

      if (aTailString.IsEmpty())
        break;
      aCurrentString = aTailString;
      aPosition = aCurrentString.Search (aSeparator);
      if (aPosition < 0 )
      {
        aCurrentString = aTailString;
        aTailString = TCollection_AsciiString();
      }
      else
        aTailString = aCurrentString.Split (aPosition);
    }
  }
  myColumnCount = aColumnCount;
}

//=======================================================================
//function : SetValues
//purpose  : 
//=======================================================================

void Message_AttributeVectorOfValues::SetValues (const NCollection_Vector<TCollection_AsciiString>& theValues)
{
  myValues = theValues;

  int aLength = (Standard_Integer)myValues.Length();
  if (aLength < 2)
    return;

  if (myColumnCount <= 0)
    myCachedValue = TCollection_AsciiString ("[") + aLength + "] : ";
  else
    myCachedValue = TCollection_AsciiString ("[") + (aLength / myColumnCount) +  ", " + myColumnCount + "] : ";

  myCachedValue += myValues.First();
  myCachedValue += " ... ";
  myCachedValue += myValues.Last();
}

//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

const TCollection_AsciiString& Message_AttributeVectorOfValues::GetDescription() const
{
  if (!Message_Attribute::GetDescription().IsEmpty())
    return Message_Attribute::GetDescription();

  return myCachedValue;
}


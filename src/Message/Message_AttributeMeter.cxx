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

#include <Message_AttributeMeter.hxx>

#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeMeter, Message_Attribute)

//=======================================================================
//function : SetValues
//purpose  : 
//=======================================================================
Message_AttributeMeter::Message_AttributeMeter (const TCollection_AsciiString& theName)
: Message_Attribute(theName)
{
}

//=======================================================================
//function : HasMetric
//purpose  : 
//=======================================================================
Standard_Boolean Message_AttributeMeter::HasMetric (const Message_MetricType& theMetric) const
{
  return myMetrics.IsBound (theMetric);
}

//=======================================================================
//function : IsMetricValid
//purpose  : 
//=======================================================================

Standard_Boolean Message_AttributeMeter::IsMetricValid (const Message_MetricType& theMetric) const
{
  return fabs (StartValue (theMetric) - GetUndefinedMetric()) > Precision::Confusion() &&
         fabs (StopValue (theMetric) - GetUndefinedMetric()) > Precision::Confusion();
}

//=======================================================================
//function : StartValue
//purpose  : 
//=======================================================================
Standard_Real Message_AttributeMeter::StartValue (const Message_MetricType& theMetric) const
{
  if (!HasMetric (theMetric))
    return GetUndefinedMetric();

  return myMetrics.Find (theMetric).first;
}

//=======================================================================
//function : SetStartValue
//purpose  : 
//=======================================================================
void Message_AttributeMeter::SetStartValue (const Message_MetricType& theMetric, const Standard_Real theValue)
{
  if (!HasMetric (theMetric))
  {
    myMetrics.Bind (theMetric, std::make_pair (theValue, GetUndefinedMetric()));
  }
  myMetrics.ChangeFind (theMetric).first = theValue;
}

//=======================================================================
//function : StopValue
//purpose  : 
//=======================================================================
Standard_Real Message_AttributeMeter::StopValue (const Message_MetricType& theMetric) const
{
  if (!HasMetric (theMetric))
    return GetUndefinedMetric();

  return myMetrics.Find (theMetric).second;
}

//=======================================================================
//function : SetStopValue
//purpose  : 
//=======================================================================
void Message_AttributeMeter::SetStopValue (const Message_MetricType& theMetric, const Standard_Real theValue)
{
  if (!HasMetric (theMetric))
  {
    // start value should be already set
    return;
  }
  myMetrics.ChangeFind (theMetric).second = theValue;
}

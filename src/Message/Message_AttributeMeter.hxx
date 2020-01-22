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

#ifndef _Message_AttributeMeter_HeaderFile
#define _Message_AttributeMeter_HeaderFile

#include <Message_Attribute.hxx>
#include <Message_MetricType.hxx>

#include <NCollection_DataMap.hxx>

#include <TCollection_AsciiString.hxx>

class Message_Alert;

//! Alert object storing container metrics values
class Message_AttributeMeter : public Message_Attribute
{
public:

  //! Constructor with string argument
  Standard_EXPORT Message_AttributeMeter (const TCollection_AsciiString& theName = TCollection_AsciiString());

  //! Checks whether the attribute has values for the metric
  //! \param theMetric [in] metric type
  //! \return true if the metric values exist
  Standard_EXPORT Standard_Boolean HasMetric (const Message_MetricType& theMetric) const;

  //! Returns true if metric is computed
  //! @return value
  Standard_EXPORT Standard_Boolean IsMetricValid (const Message_MetricType& theMetric) const;

  //! Returns start value for the metric
  //! \param theMetric [in] metric type
  Standard_EXPORT Standard_Real StartValue (const Message_MetricType& theMetric) const;

  //! Sets start values for the metric
  //! \param theMetric [in] metric type
  Standard_EXPORT void SetStartValue (const Message_MetricType& theMetric, const Standard_Real theValue);

  //! Returns stop value for the metric
  //! \param theMetric [in] metric type
  Standard_EXPORT Standard_Real StopValue (const Message_MetricType& theMetric) const;

  //! Sets stop values for the metric
  //! \param theMetric [in] metric type
  Standard_EXPORT void SetStopValue (const Message_MetricType& theMetric, const Standard_Real theValue);

  //! Returns default value of the time/mem when it is not defined
  //! \return integer value
  static Standard_Real GetUndefinedMetric() { return -1.0; }

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(Message_AttributeMeter, Message_Attribute)

private:

  typedef std::pair<Standard_Real, Standard_Real> MinToMaxValue;
  NCollection_DataMap<Message_MetricType, MinToMaxValue> myMetrics; //!< computed metrics
};

#endif // _Message_AttributeMeter_HeaderFile

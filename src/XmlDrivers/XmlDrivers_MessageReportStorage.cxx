// Created on: 2018-06-10
// Created by: Natalia Ermolaeva
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <XmlDrivers_MessageReportStorage.hxx>

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeStream.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_Level.hxx>
#include <Message_Report.hxx>

#include <BinTools.hxx>
#include <Standard_Dump.hxx>

#include <TCollection.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

#include <TopoDS_AlertAttribute.hxx>

#include <XmlDrivers.hxx>

// =======================================================================
// function : GetApplication
// purpose :
// =======================================================================
const Handle(TDocStd_Application)& GetApplication()
{
  static Handle(TDocStd_Application) anApp;
  if (anApp.IsNull())
  {
    anApp = new TDocStd_Application;
    XmlDrivers::DefineFormat (anApp);
  }
  return anApp;
}

// =======================================================================
// function : ExportReport
// purpose :
// =======================================================================
Standard_Boolean XmlDrivers_MessageReportStorage::ExportReport (const Handle(Message_Report)& theReport)
{

  if (theReport.IsNull())
    return Standard_False;

  Handle(TDocStd_Document) aDocument;
  GetApplication()->NewDocument (TCollection_ExtendedString ("XmlOcaf"), aDocument);

  TDF_Label aMainLabel = aDocument->Main();

  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    Message_Gravity aGravity = (Message_Gravity)aGravityId;

    if (theReport->GetAlerts (aGravity).Size() == 0)
      continue;
    // Gravity Label
    TDF_Label aGravityLabel = aMainLabel.NewChild();
    // set gravity kind string
    TCollection_ExtendedString aName (aGravity);
    if (!aName.IsEmpty())
      TDataStd_Name::Set (aGravityLabel, aName);

    /// reserved label to store gravity information
    //TDF_Label aFirstAlertLabel = aGravityLabel.NewChild();

    // Alert Labels: labels are started from the second index
    const Message_ListOfAlert& anAlerts = theReport->GetAlerts (aGravity);
    for (Message_ListOfAlert::Iterator anAlertsIt (anAlerts); anAlertsIt.More(); anAlertsIt.Next())
    {
      exportAlert (anAlertsIt.Value(), aGravityLabel);
    }
  }
  if (GetApplication()->SaveAs (aDocument, FileName()) != PCDM_SS_OK)
    return Standard_False;

  GetApplication()->Close (aDocument);
  return Standard_True;
}

// =======================================================================
// function : ImportReport
// purpose :
// =======================================================================
Standard_Boolean XmlDrivers_MessageReportStorage::ImportReport (const Handle(Message_Report)& theReport)
{
  theReport->SetLimit(1000);

  Handle(TDocStd_Application) anApplication = GetApplication();
  Standard_Integer aDocumentId = anApplication->IsInSession (FileName());
  if (aDocumentId > 0)
  {
    Handle(TDocStd_Document) aDocument;
    anApplication->GetDocument (aDocumentId, aDocument);
    anApplication->Close (aDocument);
  }

  Handle(TDocStd_Document) aDocument;
  GetApplication()->Open (FileName(), aDocument);
  if (aDocument.IsNull())
    return Standard_False;

  TDF_Label aMainLabel = aDocument->Main();
  if (aMainLabel.IsNull())
    return Standard_False;

  TDF_Label aLabel;
  for (TDF_ChildIterator aLabelsIt(aMainLabel); aLabelsIt.More(); aLabelsIt.Next())
  {
    TDF_Label aGravityLabel = aLabelsIt.Value();
    if (aGravityLabel.IsNull())
      continue;
    Handle(TDF_Attribute) anAttribute;
    if (!aGravityLabel.FindAttribute (TDataStd_Name::GetID(), anAttribute))
      continue;
    Handle(TDataStd_Name) aNameAttribute = Handle(TDataStd_Name)::DownCast (anAttribute);
    if (aNameAttribute.IsNull())
      continue;

    // get gravity type
    const TCollection_ExtendedString& aGravityName = aNameAttribute->Get();
    Message_Gravity aGravity = (Message_Gravity) (TCollection_AsciiString (aGravityName).IntegerValue());

    /// reserved label to store gravity information
    //TDF_Label aFirstAlertLabel = aGravityLabel.FindChild (1, Standard_False);

    // find alerts information, add corresponded alerts to the report
    for (TDF_ChildIterator anAlertLabelsIt (aGravityLabel); anAlertLabelsIt.More(); anAlertLabelsIt.Next())
    {
      TDF_Label anAlertLabel = anAlertLabelsIt.Value();
      if (anAlertLabel.IsNull())
        continue;

      importAlert (anAlertLabel, aGravity, theReport, Handle(Message_Alert)());
    }
  }
  return Standard_True;
}

// =======================================================================
// function : exportAlert
// purpose :
// =======================================================================
void XmlDrivers_MessageReportStorage::exportAlert (const Handle(Message_Alert)& theAlert, const TDF_Label& theParentLabel)
{
  TDF_Label anAlertLabel = theParentLabel.NewChild();
  TDataStd_Name::Set (anAlertLabel, theAlert->DynamicType()->Name());

  /// reserved label to store parameters of the current label
  TDF_Label anAlertParmetersLabel = anAlertLabel.NewChild();
  exportAlertParameters (theAlert, anAlertParmetersLabel);

  for (int aGravityId = Message_Trace; aGravityId <= Message_Fail; aGravityId++)
  {
    // Gravity Label
    TDF_Label aGravityLabel = anAlertLabel.NewChild();
    // set gravity kind string
    TDataStd_Name::Set (aGravityLabel, aGravityId);

    Handle(Message_AlertExtended) anAlertExtended = Handle(Message_AlertExtended)::DownCast (theAlert);
    if (anAlertExtended.IsNull())
      continue;
    Handle(Message_CompositeAlerts) aComposite = anAlertExtended->CompositeAlerts();
    if (aComposite.IsNull())
      continue;

    const Message_ListOfAlert& anAlerts = aComposite->Alerts ((Message_Gravity)aGravityId);
    for (Message_ListOfAlert::Iterator anAlertsIt (anAlerts); anAlertsIt.More(); anAlertsIt.Next())
      exportAlert (anAlertsIt.Value(), aGravityLabel);
  }
}

// =======================================================================
// function : importAlert
// purpose :
// =======================================================================
void XmlDrivers_MessageReportStorage::importAlert (const TDF_Label& theAlertLabel,
                                                   const Message_Gravity theGravity,
                                                   const Handle(Message_Report)& theReport,
                                                   const Handle(Message_Alert)& theParentAlert)
{
  TDF_Label aParametersLabel = theAlertLabel.FindChild (1, Standard_False);
  Handle(Message_Alert) anAlert = importAlertParameters (aParametersLabel);
  if (anAlert.IsNull())
    return;

  TDF_Label aLabel;
  TDF_ChildIterator aSubAlertsLabelsIt (theAlertLabel);
  aSubAlertsLabelsIt.Next(); // do not processing the first (parameters) label
  for (; aSubAlertsLabelsIt.More(); aSubAlertsLabelsIt.Next())
  {
    TDF_Label aGravityLabel = aSubAlertsLabelsIt.Value();
    if (aGravityLabel.IsNull())
      continue;
    Handle(TDF_Attribute) anAttribute;
    if (!aGravityLabel.FindAttribute (TDataStd_Name::GetID(), anAttribute))
      continue;
    Handle(TDataStd_Name) aNameAttribute = Handle(TDataStd_Name)::DownCast (anAttribute);
    if (aNameAttribute.IsNull())
      continue;

    // get gravity type
    Message_Gravity aGravity = (Message_Gravity) (TCollection_AsciiString (aNameAttribute->Get()).IntegerValue());
    // find alerts information, add corresponded alerts to the report
    for (TDF_ChildIterator anAlertLabelsIt (aGravityLabel); anAlertLabelsIt.More(); anAlertLabelsIt.Next())
    {
      TDF_Label anAlertLabel = anAlertLabelsIt.Value();
      if (anAlertLabel.IsNull())
        continue;

      importAlert (anAlertLabel, aGravity, theReport, anAlert);
    }
  }
  if (theParentAlert.IsNull())
    theReport->AddAlert (theGravity, anAlert);
  else
  {
    Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast (theParentAlert);
    if (anExtendedAlert.IsNull())
    {
      theReport->AddAlert (theGravity, anAlert);
    }
    else
    {
      anExtendedAlert->CompositeAlerts (Standard_True)->AddAlert (theGravity, anAlert);
    }
  }
}

// =======================================================================
// function : exportAlertParameters
// purpose :
// =======================================================================
void XmlDrivers_MessageReportStorage::exportAlertParameters (const Handle(Message_Alert)& theAlert, const TDF_Label& theAlertLabel)
{
  Handle(Message_AlertExtended) anAlertExtended = Handle(Message_AlertExtended)::DownCast (theAlert);
  if (anAlertExtended.IsNull()) // name attribute is empty
    return;

  // store attribute time
  Handle(Message_Attribute) anAttribute = anAlertExtended->Attribute();

  Standard_CString aDynamicTypeName = anAttribute->DynamicType()->Name();
  TDataStd_Name::Set (theAlertLabel, aDynamicTypeName);
  //TDataStd_Real::Set (theAlertLabel, anAlertExtended->CumulativeMetric());

  TDataStd_AsciiString::Set (theAlertLabel, anAttribute->GetName());

  TCollection_AsciiString aStreamText;
  if (aDynamicTypeName == STANDARD_TYPE (Message_AttributeStream)->Name())
  {
    Handle(Message_AttributeStream) aValuesArrayAlert = Handle(Message_AttributeStream)::DownCast (anAttribute);
    aStreamText = Standard_Dump::Text (aValuesArrayAlert->Stream());
  }
  else if (aDynamicTypeName == STANDARD_TYPE (Message_AttributeMeter)->Name())
  {
    Handle(Message_AttributeMeter) aMeterAttribute = Handle(Message_AttributeMeter)::DownCast (anAttribute);

    Standard_Integer aMetricsCount = 0;
    for (Standard_Integer aMetricId = (Standard_Integer)Message_MetricType_None; aMetricId <= (Standard_Integer)Message_MetricType_MemHeapUsage; aMetricId++)
    {
      if (aMeterAttribute->HasMetric ((Message_MetricType)aMetricId))
        aMetricsCount++;
    }
    if (aMetricsCount > 0)
    {
      Handle(TDataStd_IntegerArray) anActiveMetrics = TDataStd_IntegerArray::Set (theAlertLabel, 0, aMetricsCount - 1);
      Handle(TDataStd_RealArray) aMetricValues = TDataStd_RealArray::Set (theAlertLabel, 0, aMetricsCount * 2 - 1);

      Standard_Integer aCurIndex = 0;
      for (Standard_Integer aMetricId = (Standard_Integer)Message_MetricType_None; aMetricId <= (Standard_Integer)Message_MetricType_MemHeapUsage; aMetricId++)
      {
        Message_MetricType aMetricType = (Message_MetricType)aMetricId;
        if (!aMeterAttribute->HasMetric (aMetricType))
          continue;

        anActiveMetrics->SetValue (aCurIndex, aMetricId);
        aMetricValues->SetValue (2 * aCurIndex,     aMeterAttribute->StartValue (aMetricType));
        aMetricValues->SetValue (2 * aCurIndex + 1, aMeterAttribute->StopValue (aMetricType));
        aCurIndex++;
      }
    }
  }
  else if (aDynamicTypeName == STANDARD_TYPE (TopoDS_AlertAttribute)->Name())
  {
    Handle(TopoDS_AlertAttribute) aShapeAttribute = Handle(TopoDS_AlertAttribute)::DownCast (anAttribute);
    Standard_SStream aStream;
    BinTools::Write (aShapeAttribute->GetShape(), aStream);
    aStreamText = Standard_Dump::Text (aStream);
  }
  if (!aStreamText.IsEmpty())
  {
    Handle(TDataStd_ExtStringArray) aListAttribute = TDataStd_ExtStringArray::Set (theAlertLabel, 0, 0);
    aListAttribute->SetValue (0, aStreamText);
  }
}

// =======================================================================
// function : importAlertParameters
// purpose :
// =======================================================================
Handle(Message_Alert) XmlDrivers_MessageReportStorage::importAlertParameters (const TDF_Label& aParametersLabel)
{
  Handle(TDF_Attribute) anAttribute;
  if (!aParametersLabel.FindAttribute (TDataStd_Name::GetID(), anAttribute))
    return Handle(Message_Alert)();

  Handle(TDataStd_Name) aDynamicTypeAttribute = Handle(TDataStd_Name)::DownCast (anAttribute);
  if (aDynamicTypeAttribute.IsNull())
    return Handle(Message_Alert)();
  const TCollection_ExtendedString& aDynamicTypeName = aDynamicTypeAttribute->Get();

  Handle(Message_AlertExtended) anAlert = new Message_AlertExtended();
  Handle(Message_Attribute) aMessageAttribute;
  if (aDynamicTypeName == STANDARD_TYPE (Message_Attribute)->Name())
    aMessageAttribute = new Message_Attribute();
  else if (aDynamicTypeName == STANDARD_TYPE (Message_AttributeStream)->Name() ||
           aDynamicTypeName == STANDARD_TYPE (TopoDS_AlertAttribute)->Name())
  {
    // values
    if (!aParametersLabel.FindAttribute (TDataStd_ExtStringArray::GetID(), anAttribute))
      return Handle(Message_Alert)();

    Handle(TDataStd_ExtStringArray) aValuesAttribute = Handle(TDataStd_ExtStringArray)::DownCast (anAttribute);
    if (aValuesAttribute.IsNull())
      return Handle(Message_Alert)();

    Standard_SStream aStream;
    for (int aValueId = aValuesAttribute->Lower(); aValueId <= aValuesAttribute->Upper(); aValueId++)
    {
      aStream << aValuesAttribute->Value (aValueId);
    }

    if (aDynamicTypeName == STANDARD_TYPE (Message_AttributeStream)->Name())
    {
      aMessageAttribute = new Message_AttributeStream (aStream);
    }
    else if (aDynamicTypeName == STANDARD_TYPE (TopoDS_AlertAttribute)->Name())
    {
      TopoDS_Shape aShape;
      BinTools::Read (aShape, aStream);
      aMessageAttribute = new TopoDS_AlertAttribute (aShape);
    }
  }
  else if (aDynamicTypeName == STANDARD_TYPE (Message_AttributeMeter)->Name())
  {
    Handle (Message_AttributeMeter) aMetricAttribute = new Message_AttributeMeter();
    // values
    Handle(TDataStd_IntegerArray) anActiveMetrics;
    Handle(TDataStd_RealArray) aMetricValues;
    if (aParametersLabel.FindAttribute (TDataStd_IntegerArray::GetID(), anActiveMetrics) &&
        aParametersLabel.FindAttribute (TDataStd_RealArray::GetID(), aMetricValues))
    {
      for (int aValueId = anActiveMetrics->Lower(); aValueId <= anActiveMetrics->Upper(); aValueId++)
      {
        Message_MetricType aMetricType = (Message_MetricType)anActiveMetrics->Value (aValueId);

        Standard_Real aStartValue = aMetricValues->Value (2 * aValueId);
        Standard_Real aStopValue = aMetricValues->Value (2 * aValueId + 1);

        aMetricAttribute->SetStartValue (aMetricType, aStartValue);
        aMetricAttribute->SetStopValue (aMetricType, aStopValue);
      }
    }
    aMessageAttribute = aMetricAttribute;
  }


  if (!aMessageAttribute.IsNull())
  {
    // name
    if (!aParametersLabel.FindAttribute (TDataStd_AsciiString::GetID(), anAttribute))
      return Handle(Message_Alert)();
    Handle(TDataStd_AsciiString) aNameAttribute = Handle(TDataStd_AsciiString)::DownCast (anAttribute);
    if (aNameAttribute.IsNull())
      return Handle(Message_Alert)();

    aMessageAttribute->SetName (aNameAttribute->Get());
    anAlert->SetAttribute (aMessageAttribute);
  }
  return anAlert;
}

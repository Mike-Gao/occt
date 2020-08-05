#ifndef OCAFSAMPLES_H
#define OCAFSAMPLES_H


#include "BaseSample.h"
#include "TOcaf_Application.h"

#include <V3d_Viewer.hxx>
#include <TDocStd_Document.hxx>
#include <AIS_InteractiveContext.hxx>



class OcafSamples;
DEFINE_STANDARD_HANDLE(OcafSamples, BaseSample)

class OcafSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(OcafSamples, BaseSample)

  OcafSamples();
  virtual ~OcafSamples() {}

  enum class ExchangeType { None, Binary, Xml };
  virtual void Process(TCollection_AsciiString theSampleName) override;

  void               SetViewer(Handle(V3d_Viewer) theViewer) { myViewer = theViewer; };
  void               SetContext(Handle(AIS_InteractiveContext) theContext) { myContext = theContext; };
  void               SetFileName(TCollection_AsciiString theFileName) { myFileName = theFileName; };
  Handle(V3d_Viewer) GetViewer() { return myViewer; };


  static Standard_Boolean IsExportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsImportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsBinarySample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsXmlSample(TCollection_AsciiString theSampleName);

protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:
  ExchangeType myExchangeType;
  TCollection_AsciiString   myFileName;
  Handle(V3d_Viewer)             myViewer;
  Handle(TDocStd_Document)       myOcafDoc;
  Handle(AIS_InteractiveContext) myContext;

  // One function for every sample
  void CreateOcafDocument();
  void CreateBoxOcafSample();
  void CreateCylinderOcafSample();
  void ModifyBoxOcafSample();
  void ModifyCylinderOcafSample();
  void UndoOcafSample();
  void RedoOcafSample();
  void DialogOpenOcafSample();
  void DialogSaveBinOcafSample();
  void DialogSaveXmlOcafSample();

  void DisplayPresentation();

};

#endif  //OCAFSAMPLES_H

#pragma once

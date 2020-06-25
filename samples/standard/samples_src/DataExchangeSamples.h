#ifndef DATAEXCHANGESAMPLES_H
#define DATAEXCHANGESAMPLES_H


#include "BaseSample.h"

#include <STEPControl_StepModelType.hxx>


class DataExchangeSamples;
DEFINE_STANDARD_HANDLE(DataExchangeSamples, BaseSample)

class DataExchangeSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(DataExchangeSamples, BaseSample)
  
  DataExchangeSamples();
  virtual ~DataExchangeSamples() {}
  virtual void Process(TCollection_AsciiString theSampleName) override;
  void AppendBottle();
  void SetFileName(TCollection_AsciiString theFileName) { myFileName = theFileName; };
  void SetStepType(STEPControl_StepModelType theStepType) { myStepType = theStepType; };

  static Standard_Boolean IsExportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsImportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsBrepSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsStepSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsIgesSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsStlSample(TCollection_AsciiString theSampleName); 
  static Standard_Boolean IsVrmlSample(TCollection_AsciiString theSampleName);


protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:
  TCollection_AsciiString myFileName;
  STEPControl_StepModelType myStepType;

  Standard_Boolean CheckFacetedBrep();
  // One function for every sample
  void BrepExportSample();
  void StepExportSample();
  void IgesExportSample();
  void StlExportSample();
  void VrmlExportSample();
  void ImageExportSample();
  void BrepImportSample();
  void StepImportSample();
  void IgesImportSample();
};

#endif  //DATAEXCHANGESAMPLES_H

#pragma once
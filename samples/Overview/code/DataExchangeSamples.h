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

#ifndef DATAEXCHANGESAMPLES_H
#define DATAEXCHANGESAMPLES_H


#include "BaseSample.h"

#include <STEPControl_StepModelType.hxx>
#include <V3d_View.hxx>

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
  void SetView(Handle(V3d_View) theView) { myView = theView; };

  static Standard_Boolean IsExportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsImportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsBrepSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsStepSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsIgesSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsStlSample(TCollection_AsciiString theSampleName); 
  static Standard_Boolean IsVrmlSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsImageSample(TCollection_AsciiString theSampleName);

protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:
  TCollection_AsciiString   myFileName;
  STEPControl_StepModelType myStepType;
  Handle(V3d_View)          myView;

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
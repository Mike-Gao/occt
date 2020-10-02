// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef DATAEXCHANGESAMPLES_H
#define DATAEXCHANGESAMPLES_H

#include "BaseSample.h"

#include <STEPControl_StepModelType.hxx>
#include <V3d_View.hxx>

class DataExchangeSamples;
DEFINE_STANDARD_HANDLE(DataExchangeSamples, BaseSample)

//! Implements Data Exchange samples
class DataExchangeSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(DataExchangeSamples, BaseSample)

  DataExchangeSamples(TCollection_AsciiString theSampleSourcePath, 
                      Handle(V3d_View) theView,
                      Handle(AIS_InteractiveContext) theContext):
    BaseSample(theSampleSourcePath, theContext),
    myStepType(STEPControl_StepModelType::STEPControl_AsIs),
    myView(theView)

  { }
  virtual ~DataExchangeSamples() {}
  virtual void Process(TCollection_AsciiString theSampleName) Standard_OVERRIDE;
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
  static Standard_Boolean IsImageSample(TCollection_AsciiString theSampleName);

protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) Standard_OVERRIDE;

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
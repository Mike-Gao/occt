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

#ifndef OCAFSAMPLES_H
#define OCAFSAMPLES_H


#include "BaseSample.h"
#include "TOcaf_Application.h"

#include <V3d_Viewer.hxx>
#include <TDocStd_Document.hxx>
#include <AIS_InteractiveContext.hxx>



class OcafSamples;
DEFINE_STANDARD_HANDLE(OcafSamples, BaseSample)

//! Implements OCAF samples
class OcafSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(OcafSamples, BaseSample)

  OcafSamples(TCollection_AsciiString theSampleSourcePath,
              Handle(V3d_Viewer) theViewer,
    Handle(AIS_InteractiveContext) theContext) :
    BaseSample(theSampleSourcePath, theContext),
    myViewer(theViewer)
  { }
  virtual ~OcafSamples() {}

  enum ExchangeType { None, Binary, Xml };
  virtual void Process(TCollection_AsciiString theSampleName) Standard_OVERRIDE;
  void ClearExtra();
  void SetFileName(TCollection_AsciiString theFileName) { myFileName = theFileName; };

  static Standard_Boolean IsExportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsImportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsBinarySample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsXmlSample(TCollection_AsciiString theSampleName);

protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) Standard_OVERRIDE;

private:
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

private:

  ExchangeType             myExchangeType;
  TCollection_AsciiString  myFileName;
  Handle(V3d_Viewer)       myViewer;
  Handle(TDocStd_Document) myOcafDoc;
};

#endif  //OCAFSAMPLES_H

#pragma once

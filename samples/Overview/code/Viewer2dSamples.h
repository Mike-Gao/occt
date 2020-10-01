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

#ifndef VIEWER2DSAMPLES_H
#define VIEWER2DSAMPLES_H


#include "BaseSample.h"

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>

class Viewer2dSamples;
DEFINE_STANDARD_HANDLE(Viewer2dSamples, BaseSample)

//! Implements viewer 2D samples
class Viewer2dSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(Viewer2dSamples, BaseSample)

  Viewer2dSamples(TCollection_AsciiString theSampleSourcePath,
                  Handle(V3d_View) theView,
                  Handle(V3d_Viewer) theViewer,
                  Handle(AIS_InteractiveContext) theContext) :
    BaseSample(theSampleSourcePath, theContext),
    myView(theView),
    myViewer(theViewer)
  {}

  virtual ~Viewer2dSamples() {}

  void SetFileName(TCollection_AsciiString theFileName) { myFileName = theFileName; };
  void ClearExtra();
  static Standard_Boolean IsFileSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsShadedSample(TCollection_AsciiString theSampleName);

protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) Standard_OVERRIDE;

private:

  // One function for every sample
  void TextView2dSample();
  void MarkerView2dSample();
  void FillAreaView2dSample();
  void LoopOnFaceView2dSample();
  void RectagularLineGrid2dSample();
  void RectagularPointGrid2dSample();
  void CircularLineGrid2dSample();
  void CircularPointGrid2dSample();
  void ClearGrid2dSample();
  void BackgroungImage2dSample();

  TCollection_AsciiString        myFileName;
  Handle(V3d_View)               myView;
  Handle(V3d_Viewer)             myViewer;
};

#endif  //VIEWER2DSAMPLES_H
#pragma once


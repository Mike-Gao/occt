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

#ifndef VIEWER3DSAMPLES_H
#define VIEWER3DSAMPLES_H


#include "BaseSample.h"

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>

class Viewer3dSamples;
DEFINE_STANDARD_HANDLE(Viewer3dSamples, BaseSample)

//! Implements viewer 3D samples
class Viewer3dSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(Viewer3dSamples, BaseSample)

  Viewer3dSamples(TCollection_AsciiString theSampleSourcePath,
                  Handle(V3d_View) theView,
                  Handle(AIS_InteractiveContext) theContext) :
    BaseSample(theSampleSourcePath, theContext),
    myView(theView)
  {}

  virtual ~Viewer3dSamples() {}
  void AppendBottle();
  void ClearExtra();

protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) Standard_OVERRIDE;

private:

  Handle(V3d_View) myView;

private:

  // One function for every sample
  void SpotLight3dSample();
  void PositionalLight3dSample();
  void DirectionalLight3dSample();
  void AmbientLight3dSample();
  void ClearLight3dSample();
  void VerticesSelect3dSample();
  void EdgesSelect3dSample();
  void FacesSelect3dSample();
  void NeutralPointSelect3dSample();
  void WireFramePresentation3dSample();
  void ShadingPresentation3dSample();
  void RedColorPresentation3dSample();
  void GrayColorPresentation3dSample();
  void PlasticPresentation3dSample();
  void BronzePresentation3dSample();
  void OpaquePresentation3dSample();
  void HalfTransparencyPresentation3dSample();
  void VboOn3dSample();
  void VboOff3dSample();
};

#endif  //VIEWER3DSAMPLES_H


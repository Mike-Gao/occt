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

#ifndef VIEWER2DSAMPLES_H
#define VIEWER2DSAMPLES_H


#include "BaseSample.h"

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>

class Viewer2dSamples;
DEFINE_STANDARD_HANDLE(Viewer2dSamples, BaseSample)

class Viewer2dSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(Viewer2dSamples, BaseSample)

  Viewer2dSamples(TCollection_AsciiString theSampleSourcePath,
                  Handle(V3d_View) theView,
                  Handle(V3d_Viewer) theViewer,
                  Handle(AIS_InteractiveContext) theContext);
  virtual ~Viewer2dSamples() {}

  void SetFileName(TCollection_AsciiString theFileName) { myFileName = theFileName; };
  static Standard_Boolean IsFileSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsShadedSample(TCollection_AsciiString theSampleName);

protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

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
  Handle(AIS_InteractiveContext) myContext;
};

#endif  //VIEWER2DSAMPLES_H
#pragma once


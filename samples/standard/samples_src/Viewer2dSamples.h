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

  virtual ~Viewer2dSamples() {}
  void SetView(Handle(V3d_View) theView) { myView = theView; };
  void SetViewer(Handle(V3d_Viewer) theViewer) { myViewer = theViewer; };
  void SetContext(Handle(AIS_InteractiveContext) theContext) { myContext = theContext; };
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


  Handle(V3d_View)               myView;
  Handle(V3d_Viewer)             myViewer;
  Handle(AIS_InteractiveContext) myContext;
};

#endif  //VIEWER2DSAMPLES_H
#pragma once


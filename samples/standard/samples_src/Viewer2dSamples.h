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
  void AppendBottle();
  void SetView(Handle(V3d_View) theView) { myView = theView; };
  void SetContext(Handle(AIS_InteractiveContext) theContext) { myContext = theContext; };
protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:

  // One function for every sample
  void SpotLight3dSample();


  Handle(V3d_View) myView;
  Handle(AIS_InteractiveContext) myContext;
};

#endif  //VIEWER2DSAMPLES_H
#pragma once


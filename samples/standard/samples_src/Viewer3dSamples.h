#ifndef VIEWER3DSAMPLES_H
#define VIEWER3DSAMPLES_H


#include "BaseSample.h"

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>

class Viewer3dSamples;
DEFINE_STANDARD_HANDLE(Viewer3dSamples, BaseSample)

class Viewer3dSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(Viewer3dSamples, BaseSample)

  virtual ~Viewer3dSamples() {}
  void AppendBottle();
  void SetView(Handle(V3d_View) theView) { myView = theView; };
  void SetContext(Handle(AIS_InteractiveContext) theContext) { myContext = theContext; };
protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

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

  Handle(V3d_View) myView;
  Handle(AIS_InteractiveContext) myContext;
};

#endif  //VIEWER3DSAMPLES_H
#pragma once

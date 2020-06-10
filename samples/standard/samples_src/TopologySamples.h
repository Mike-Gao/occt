#ifndef TOPOLOGYSAMPLES_H
#define TOPOLOGYSAMPLES_H


#include "BaseSample.h"


class TopologySamples;
DEFINE_STANDARD_HANDLE(TopologySamples, BaseSample)

class TopologySamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TopologySamples, BaseSample)

    virtual ~TopologySamples() {}
protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:
  // One function for every sample
  void Vertex3dSample();
  void Edge3dSample();
  void Face3dSample();
  void Wire3dSample();
  void Shell3dSample();
  void Solid3dSample();
  void Edge2dSample();
  void Box3dSample();
  void Cylinder3dSample();
  void Revolution3dSample();
  void TopologyIterator3dSample();
  void AssessToCurve3dSample();
  void AssessToCompositeCurve3dSample();
  void AssessToSurface3dSample();
  void Common3dSample();
  void Cut3dSample();
  void Fuse3dSample();
  void Section3dSample();
  void Splitter3dSample();
  void Defeaturing3dSample();
  void Fillet3dSample();
  void Chamfer3dSample();
  void Offset3dSample();
  void Sweep3dSample();
  void Evolved3dSample();
  void Copy3dSample();
  void Transform3dSample();
  void ConvertToNurbs3dSample();
  void SewContiguousFaces3dSample();
  void CheckValidity3dSample();
  void ComputeLinearProperties3dSample();
  void ComputeSurfaceProperties3dSample();
  void ComputeVolumeProperties3dSample();
};

#endif  //TOPOLOGYSAMPLES_H


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

#ifndef TOPOLOGYSAMPLES_H
#define TOPOLOGYSAMPLES_H


#include "BaseSample.h"


class TopologySamples;
DEFINE_STANDARD_HANDLE(TopologySamples, BaseSample)

class TopologySamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TopologySamples, BaseSample)

  TopologySamples(TCollection_AsciiString theSampleSourcePath) :
    BaseSample(theSampleSourcePath) {}
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
  void TopologyExplorer3dSample();
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


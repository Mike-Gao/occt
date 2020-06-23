#ifndef TRIANGULATIONSAMPLES_H
#define TRIANGULATIONSAMPLES_H


#include "BaseSample.h"


class TriangulationSamples;
DEFINE_STANDARD_HANDLE(TriangulationSamples, BaseSample)

class TriangulationSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TriangulationSamples, BaseSample)

    virtual ~TriangulationSamples() {}
protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:
  // One function for every sample
  void Triangulation3dSample();

};

#endif  //TRIANGULATIONSAMPLES_H

#pragma once

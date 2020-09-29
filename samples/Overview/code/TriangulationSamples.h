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

#ifndef TRIANGULATIONSAMPLES_H
#define TRIANGULATIONSAMPLES_H


#include "BaseSample.h"


class TriangulationSamples;
DEFINE_STANDARD_HANDLE(TriangulationSamples, BaseSample)

class TriangulationSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TriangulationSamples, BaseSample)

  TriangulationSamples(TCollection_AsciiString theSampleSourcePath) :
    BaseSample(theSampleSourcePath) {}
  virtual ~TriangulationSamples() {}
protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:
  // One function for every sample
  void Triangulation3dSample();

};

#endif  //TRIANGULATIONSAMPLES_H

#pragma once

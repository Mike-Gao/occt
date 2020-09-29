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

#ifndef OCAFSAMPLES_H
#define OCAFSAMPLES_H


#include "BaseSample.h"
#include "TOcaf_Application.h"

#include <V3d_Viewer.hxx>
#include <TDocStd_Document.hxx>
#include <AIS_InteractiveContext.hxx>



class OcafSamples;
DEFINE_STANDARD_HANDLE(OcafSamples, BaseSample)

class OcafSamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(OcafSamples, BaseSample)

  OcafSamples(TCollection_AsciiString theSampleSourcePath,
              Handle(V3d_Viewer) theViewer,
              Handle(AIS_InteractiveContext) theContext);
  virtual ~OcafSamples() {}

  enum ExchangeType { None, Binary, Xml };
  virtual void Process(TCollection_AsciiString theSampleName) override;
  void ClearDocument() { myOcafDoc = nullptr; }
  void SetFileName(TCollection_AsciiString theFileName) { myFileName = theFileName; };

  static Standard_Boolean IsExportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsImportSample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsBinarySample(TCollection_AsciiString theSampleName);
  static Standard_Boolean IsXmlSample(TCollection_AsciiString theSampleName);

protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:
  ExchangeType myExchangeType;
  TCollection_AsciiString   myFileName;
  Handle(V3d_Viewer)             myViewer;
  Handle(AIS_InteractiveContext) myContext;
  Handle(TDocStd_Document)       myOcafDoc;

  // One function for every sample
  void CreateOcafDocument();
  void CreateBoxOcafSample();
  void CreateCylinderOcafSample();
  void ModifyBoxOcafSample();
  void ModifyCylinderOcafSample();
  void UndoOcafSample();
  void RedoOcafSample();
  void DialogOpenOcafSample();
  void DialogSaveBinOcafSample();
  void DialogSaveXmlOcafSample();

  void DisplayPresentation();

};

#endif  //OCAFSAMPLES_H

#pragma once

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

#ifndef BASESAMPLE_H
#define BASESAMPLE_H

#include <sstream>

#include <Standard_Transient.hxx>
#include <NCollection_Vector.hxx>
#include <AIS_InteractiveObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Handle.hxx>

class BaseSample;
DEFINE_STANDARD_HANDLE(BaseSample, Standard_Transient)

class BaseSample: public Standard_Transient
{
public:
  BaseSample(TCollection_AsciiString theSampleSourcePath)
  {
    myCodePath = theSampleSourcePath;
  }
  void Clear();
  void AppendCube();
	Standard_Boolean                                  IsProcessed();
  const NCollection_Vector<Handle(AIS_InteractiveObject)>& Get2dObjects();
  const NCollection_Vector<Handle(AIS_InteractiveObject)>& Get3dObjects();
  TCollection_AsciiString                           GetResult();
  TCollection_AsciiString                           GetCode();

  virtual void Process(TCollection_AsciiString theSampleName);

  DEFINE_STANDARD_RTTI_INLINE(BaseSample, Standard_Transient)

protected:
  virtual void ExecuteSample(TCollection_AsciiString theSampleName) = 0;
          void FindSourceCode(TCollection_AsciiString theSampleName);
          void TraceError(TCollection_AsciiString theErrorMessage);

protected:
  Standard_Boolean                                  myIsProcessed;
	NCollection_Vector<Handle(AIS_InteractiveObject)> myObject2d;
  NCollection_Vector<Handle(AIS_InteractiveObject)> myObject3d;

  // Standard_OStream* strem = dynamic_cast<Standard_OStream*>(&myResult);
  std::ostringstream                                myResult;
  TCollection_AsciiString                           myCode;
	TCollection_AsciiString                           myCodePath;

  static const TCollection_AsciiString FILE_EXTENSION;

private:
  Standard_Integer FindEndOfPhrase(TCollection_AsciiString theText, 
                                   TCollection_AsciiString theRegexpTemplate);
  Standard_Integer FindClosingBracket(TCollection_AsciiString theText,
                                      Standard_Integer theOpeningBracketIndex,
                                      Standard_Character theClosingBracketSymbol);
};

#endif  //BASESAMPLE_H

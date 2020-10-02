// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

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

//! Base class for specified category classes
class BaseSample: public Standard_Transient
{
public:
  BaseSample(TCollection_AsciiString theSampleSourcePath, 
            Handle(AIS_InteractiveContext) theContext):
  myCodePath(theSampleSourcePath),
  myContext(theContext)
  { }

  void Clear();
  void AppendCube();

  Standard_Boolean IsProcessed()
  {
    return myIsProcessed;
  }

  const NCollection_Vector<Handle(AIS_InteractiveObject)>& Get2dObjects()
  {
    return myObject2d;
  }

  const NCollection_Vector<Handle(AIS_InteractiveObject)>& Get3dObjects()
  {
    return myObject3d;
  }
  TCollection_AsciiString GetResult();

  TCollection_AsciiString GetCode()
  {
    return myCode;
  }

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

  std::ostringstream             myResult;
  TCollection_AsciiString        myCode;
  TCollection_AsciiString        myCodePath;
  Handle(AIS_InteractiveContext) myContext;
  static const TCollection_AsciiString FILE_EXTENSION;

private:
  Standard_Integer FindEndOfPhrase(TCollection_AsciiString theText, 
                                   TCollection_AsciiString theRegexpTemplate);
  Standard_Integer FindClosingBracket(TCollection_AsciiString theText,
                                      Standard_Integer theOpeningBracketIndex,
                                      Standard_Character theClosingBracketSymbol);
};

#endif  //BASESAMPLE_H

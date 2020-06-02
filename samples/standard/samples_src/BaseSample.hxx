

#ifndef BASESAMPLE_H
#define BASESAMPLE_H

#include <Standard_Transient.hxx>
#include <NCollection_Vector.hxx>
#include <AIS_InteractiveObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Handle.hxx>


class BaseSample 
{
public:


	Standard_Boolean                                  IsProcessed();
  NCollection_Vector<Handle(AIS_InteractiveObject)> Get2dObject();
  NCollection_Vector<Handle(AIS_InteractiveObject)> Get3dObject();
  TCollection_AsciiString                           GetResult();
  TCollection_AsciiString                           GetCode();

  void Process(TCollection_AsciiString theSampleName);
  void SetCodePach(TCollection_AsciiString theSampleSourcePach);

private:
  void TraceError(TCollection_AsciiString theErrorMessage);
  void FindSourceCode(TCollection_AsciiString theSampleName);

  // One function for every sample
  void ZeroDimensionObjects3dSample();
  void Vectors3dSample();


private:
  Standard_Boolean                                  myIsProcessed;
	NCollection_Vector<Handle(AIS_InteractiveObject)> myObject2d;
  NCollection_Vector<Handle(AIS_InteractiveObject)> myObject3d;
  TCollection_AsciiString                           myResult;
  TCollection_AsciiString                           myCode;
	TCollection_AsciiString                           myCodePath;
};

#endif  //BASESAMPLE_H

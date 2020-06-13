

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
	Standard_Boolean                                  IsProcessed();
  const NCollection_Vector<Handle(AIS_InteractiveObject)>& Get2dObjects();
  const NCollection_Vector<Handle(AIS_InteractiveObject)>& Get3dObjects();
  TCollection_AsciiString                           GetResult();
  TCollection_AsciiString                           GetCode();

  void Process(TCollection_AsciiString theSampleName);
  void SetCodePach(TCollection_AsciiString theSampleSourcePach);

  DEFINE_STANDARD_RTTI_INLINE(BaseSample, Standard_Transient)

protected:
  virtual void ExecuteSample(TCollection_AsciiString theSampleName) = 0;
          void FindSourceCode(TCollection_AsciiString theSampleName);

private:
  void             TraceError(TCollection_AsciiString theErrorMessage);

  Standard_Integer FindEndOfPhrase(TCollection_AsciiString theText, 
                                   TCollection_AsciiString theRegexpTemplate);
  Standard_Integer FindClosingBracket(TCollection_AsciiString theText,
                                      Standard_Integer theOpeningBracketIndex,
                                      Standard_Character theClosingBracketSymbol);

protected:
  Standard_Boolean                                  myIsProcessed;
	NCollection_Vector<Handle(AIS_InteractiveObject)> myObject2d;
  NCollection_Vector<Handle(AIS_InteractiveObject)> myObject3d;

  // Standard_OStream* strem = dynamic_cast<Standard_OStream*>(&myResult);
  std::ostringstream                                myResult;
  TCollection_AsciiString                           myCode;
	TCollection_AsciiString                           myCodePath;

  static const TCollection_AsciiString FILE_EXTENSION;
};

#endif  //BASESAMPLE_H

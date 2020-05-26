

#ifndef BASESAMPLE_H
#define BASESAMPLE_H

#include <Standard_Transient.hxx>
#include <NCollection_Vector.hxx>
#include <AIS_InteractiveObject.hxx>
#include <NCollection_String.hxx>
#include <Standard_Handle.hxx>


class BaseSample : public Standard_Transient 
{
public:
	virtual void Process() = 0;
	Standard_Boolean IsProcessed();
	NCollection_Vector<Handle(AIS_InteractiveObject) > Get2dObject();
	NCollection_Vector<Handle(AIS_InteractiveObject) > Get3dObject();
	NCollection_Vector<NCollection_String> GetResult();
	NCollection_Vector<NCollection_String> GetCode();
protected:
	int mySampleName;
	NCollection_Vector<Handle(AIS_InteractiveObject) > myObject2d;
	NCollection_Vector<Handle(AIS_InteractiveObject) > myObject3d;
	NCollection_Vector<NCollection_String> myResult;
	static NCollection_String CODE_PATH;
};

#endif  //BASESAMPLE_H

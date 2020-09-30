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

#ifndef _Sample2D_Image_HeaderFile
#define _Sample2D_Image_HeaderFile
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

#include <OSD_File.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Face.hxx>

class Sample2D_Image;
DEFINE_STANDARD_HANDLE(Sample2D_Image,AIS_Shape)
class Sample2D_Image : public AIS_Shape 
{
public:
  DEFINE_STANDARD_RTTI_INLINE(Sample2D_Image,AIS_Shape)

  Standard_EXPORT Sample2D_Image(TCollection_AsciiString& aFileName,
                                 const Standard_Real X = 0.0,
                                 const Standard_Real Y = 0.0,
                                 const Standard_Real aScale = 1.0);
  Standard_EXPORT inline virtual  void GetCoord(Standard_Real& X,Standard_Real& Y) const;
  Standard_EXPORT inline virtual  void SetCoord(const Standard_Real X,const Standard_Real Y) ;
  Standard_EXPORT inline   Standard_Real GetScale() const;
  Standard_EXPORT inline   void SetScale(const Standard_Real aNewScale) ;
  Standard_EXPORT virtual void SetContext(const Handle(AIS_InteractiveContext)& theContext) ;


private:
  void MakeShape();
protected:
  TopoDS_Face myFace;
  TCollection_AsciiString myFilename;
  Standard_Real myX;
  Standard_Real myY;
  Standard_Real myScale;

};

// other inline functions and methods (like "C++: function call" methods)
//

inline  void Sample2D_Image::GetCoord(Standard_Real& X, Standard_Real& Y) const
{
  X = myX;
  Y = myY;
}

inline  void Sample2D_Image::SetCoord(const Standard_Real X, const Standard_Real Y) 
{
  myX = X;
  myY = Y;
}

inline Standard_Real Sample2D_Image::GetScale() const
{
  return myScale;
}

inline void Sample2D_Image::SetScale(const Standard_Real aNewScale) 
{
  myScale = aNewScale;
}



#endif

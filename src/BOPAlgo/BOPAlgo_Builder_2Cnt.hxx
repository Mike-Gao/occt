// Created by: Peter KURNEV
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef BOPAlgo_Builder_2Cnt_HeaderFile
#define BOPAlgo_Builder_2Cnt_HeaderFile

#include <NCollection_IndexedDataMap.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

#include <BOPCol_TBB.hxx>
#include <BOPCol_NCVector.hxx>

#include <BOPTools_Set.hxx>
#include <BOPTools_SetMapHasher.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <BOPInt_Context.hxx>

#include <BOPAlgo_BuilderFace.hxx>

//=======================================================================
//class    : BOPAlgo_VectorOfBuilderFace
//purpose  : 
//=======================================================================
typedef BOPCol_NCVector<BOPAlgo_BuilderFace> BOPAlgo_VectorOfBuilderFace;
//
//=======================================================================
//class    : BOPAlgo_BuilderFaceFunctor
//purpose  : 
//=======================================================================
class BOPAlgo_BuilderFaceFunctor {
 protected:
  BOPAlgo_VectorOfBuilderFace* myPVBF;
  //
 public:
  //
  BOPAlgo_BuilderFaceFunctor(BOPAlgo_VectorOfBuilderFace& aVBF) 
    : myPVBF(&aVBF) {
  }
  //
  void operator()( const flexible_range<Standard_Size>& aBR ) const{
    Standard_Size i, iBeg, iEnd;
    //
    BOPAlgo_VectorOfBuilderFace& aVBF=*myPVBF;
    //
    iBeg=aBR.begin();
    iEnd=aBR.end();
    for(i=iBeg; i!=iEnd; ++i) {
      BOPAlgo_BuilderFace& aBF=aVBF((Standard_Integer)i);
      //
      aBF.Perform();
    }
  }
};
//=======================================================================
//class    : BOPAlgo_BuilderFaceCnt
//purpose  : 
//=======================================================================
class BOPAlgo_BuilderFaceCnt {
 public:
  //-------------------------------
  // Perform
  Standard_EXPORT static void Perform(const Standard_Boolean bRunParallel,
				      BOPAlgo_VectorOfBuilderFace& aVBF) {
    //
    BOPAlgo_BuilderFaceFunctor aBFF(aVBF);
    Standard_Size aNbBF=aVBF.Extent();
    //
    if (bRunParallel) {
      flexible_for(flexible_range<Standard_Size>(0,aNbBF), aBFF);
    }
    else {
      aBFF.operator()(flexible_range<Standard_Size>(0,aNbBF));
    }
  }
  //
};
//
//-------------------------------------------------------------------------
typedef BOPCol_NCVector<TopoDS_Shape> BOPAlgo_VectorOfShape;
typedef BOPCol_NCVector<BOPAlgo_VectorOfShape> BOPAlgo_VectorOfVectorOfShape;

typedef NCollection_IndexedDataMap\
  <BOPTools_Set, Standard_Integer, BOPTools_SetMapHasher> \
    BOPAlgo_IndexedDataMapOfSetInteger;
//
//=======================================================================
//class    : BOPAlgo_PairOfShapeBoolean
//purpose  : 
//=======================================================================
class BOPAlgo_PairOfShapeBoolean {
 public:
  BOPAlgo_PairOfShapeBoolean()
    : myFlag(Standard_False) {
  }
  //
  TopoDS_Shape& Shape1() {
    return myShape1;
  }
  //
  TopoDS_Shape& Shape2() {
    return myShape2;
  }
  //
  Standard_Boolean& Flag() {
    return myFlag;
  }
  //
 protected: 
  Standard_Boolean myFlag;
  TopoDS_Shape myShape1;
  TopoDS_Shape myShape2;
};

typedef BOPCol_NCVector<BOPAlgo_PairOfShapeBoolean> BOPAlgo_VectorOfPairOfShapeBoolean;
//

//=======================================================================
//function : BOPAlgo_BuilderSDFaceFunctor
//purpose  : 
//=======================================================================
class BOPAlgo_BuilderSDFaceFunctor {
 protected:
  BOPAlgo_VectorOfPairOfShapeBoolean* myPVPSB;
  
 public:
  //
  BOPAlgo_BuilderSDFaceFunctor(BOPAlgo_VectorOfPairOfShapeBoolean& aVPSB)
    : myPVPSB(&aVPSB){
  }
  //
  void operator()( const flexible_range<Standard_Integer>& aBR ) const {
    Standard_Boolean bFlag;
    Standard_Integer i, iBeg, iEnd;
    Handle(BOPInt_Context) aContext;
    //
    aContext=new BOPInt_Context;
    //
    BOPAlgo_VectorOfPairOfShapeBoolean& aVPSB=*myPVPSB;
    //
    iBeg=aBR.begin();
    iEnd=aBR.end();
    for(i=iBeg; i!=iEnd; ++i) {
      BOPAlgo_PairOfShapeBoolean& aPSB=aVPSB(i);
      const TopoDS_Face& aFj=(*(TopoDS_Face*)(&aPSB.Shape1()));
      const TopoDS_Face& aFk=(*(TopoDS_Face*)(&aPSB.Shape2()));
      bFlag=BOPTools_AlgoTools::AreFacesSameDomain(aFj, aFk, aContext);
      if (bFlag) {
	aPSB.Flag()=bFlag;
      }
    }
  }
};
//
//=======================================================================
//function : BOPAlgo_BuilderSDFaceCnt
//purpose  : 
//=======================================================================
class BOPAlgo_BuilderSDFaceCnt {
 public:
  //-------------------------------
  // Perform
  Standard_EXPORT static 
    void Perform(const Standard_Boolean bRunParallel,
		 BOPAlgo_VectorOfPairOfShapeBoolean& aVPSB) {
    Standard_Integer aNbVPSB;
    //
    aNbVPSB=aVPSB.Extent();
    BOPAlgo_BuilderSDFaceFunctor aBFF(aVPSB);
    //
    if (bRunParallel) {
      flexible_for(flexible_range<Standard_Integer>(0,aNbVPSB), aBFF);
    }
    else {
      aBFF.operator()(flexible_range<Standard_Integer>(0,aNbVPSB));
    }
  }
  //
};


#endif

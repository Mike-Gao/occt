// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include <BOPAlgo_PaveFiller.ixx>

#include <GeomAPI_ProjectPointOnSurf.hxx>
//
#include <Precision.hxx>

#include <Bnd_Box.hxx>
//
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
//
#include <IntTools_EdgeFace.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>
//
#include <BOPCol_BoxBndTree.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
//
#include <IntTools_Context.hxx>
#include <IntTools_Tools.hxx>
//
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfInteger.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
//
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
//
#include <BOPAlgo_Tools.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <TopoDS.hxx>


//=======================================================================
//class    : BOPAlgo_EdgeFace
//purpose  : 
//=======================================================================
class BOPAlgo_EdgeFace : 
  public IntTools_EdgeFace,
  public BOPAlgo_Algo {
 
 public:
  DEFINE_STANDARD_ALLOC
  
  BOPAlgo_EdgeFace() : 
    IntTools_EdgeFace(), 
    BOPAlgo_Algo(),
    myIE(-1), myIF(-1) {
  };
  //
  virtual ~BOPAlgo_EdgeFace(){
  };
  //
  void SetIndices(const Standard_Integer nE,
                  const Standard_Integer nF) {
    myIE=nE;
    myIF=nF;
  }
  //
  void Indices(Standard_Integer& nE,
               Standard_Integer& nF) {
    nE=myIE;
    nF=myIF;
  }
  //
  void SetNewSR(const IntTools_Range& aR){
    myNewSR=aR;
  }
  //
  IntTools_Range& NewSR(){
    return myNewSR;
  }
  //
  void SetPaveBlock(const Handle(BOPDS_PaveBlock)& aPB) {
    myPB=aPB;
  }
  //
  Handle(BOPDS_PaveBlock)& PaveBlock() {
    return myPB;
  }
  //
  virtual void Perform() {
    BOPAlgo_Algo::UserBreak();
    IntTools_EdgeFace::Perform();
  }
  //
 protected:
  Standard_Integer myIE;
  Standard_Integer myIF;
  IntTools_Range myNewSR;
  Handle(BOPDS_PaveBlock) myPB;
};
//
//=======================================================================
typedef BOPCol_NCVector<BOPAlgo_EdgeFace> BOPAlgo_VectorOfEdgeFace; 
//
typedef BOPCol_ContextFunctor 
  <BOPAlgo_EdgeFace,
  BOPAlgo_VectorOfEdgeFace,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_EdgeFaceFunctor;
//
typedef BOPCol_ContextCnt 
  <BOPAlgo_EdgeFaceFunctor,
  BOPAlgo_VectorOfEdgeFace,
  Handle(IntTools_Context)> BOPAlgo_EdgeFaceCnt;
//
//=======================================================================
//function : PerformEF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::PerformEF()
{
  Standard_Integer iSize;
  //
  myErrorStatus=0;
  //
  FillShrunkData(TopAbs_EDGE, TopAbs_FACE);
  //
  myIterator->Initialize(TopAbs_EDGE, TopAbs_FACE);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  Standard_Boolean bJustAdd, bV[2];
  Standard_Integer nE, nF, aDiscretize, i, aNbCPrts, iX, nV[2];
  Standard_Integer aNbEdgeFace, k;
  Standard_Real aTolE, aTolF, aTS1, aTS2, aT1, aT2, aDeflection;
  Handle(NCollection_BaseAllocator) aAllocator;
  TopAbs_ShapeEnum aType;
  BOPDS_ListIteratorOfListOfPaveBlock aIt;
  BOPAlgo_VectorOfEdgeFace aVEdgeFace; 
  BRep_Builder aBB;
  //-----------------------------------------------------scope f
  //
  aAllocator=NCollection_BaseAllocator::CommonBaseAllocator();
  //
  BOPCol_MapOfInteger aMIEFC(100, aAllocator);
  BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks aMVCPB(100, aAllocator);
  BOPDS_IndexedDataMapOfPaveBlockListOfInteger aMPBLI(100, aAllocator);
  //
  aDiscretize=35;
  aDeflection=0.01;
  //
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  aEFs.SetIncrement(iSize);
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nE, nF, bJustAdd);
    if(bJustAdd) {
      continue;
    }
    //
    const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
    if (aSIE.HasFlag()){//degenerated 
      continue;
    }
    //
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aSIE.Shape()));
    const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF)));
    const Bnd_Box& aBBF=myDS->ShapeInfo(nF).Box(); 
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    const BOPDS_IndexedMapOfPaveBlock& aMPBF=aFI.PaveBlocksOn();
    //
    aTolE=BRep_Tool::Tolerance(aE);
    aTolF=BRep_Tool::Tolerance(aF);
    //
    BOPDS_ListOfPaveBlock& aLPB=myDS->ChangePaveBlocks(nE);
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      Handle(BOPDS_PaveBlock)& aPB=aIt.ChangeValue();
      //
      const Handle(BOPDS_PaveBlock) aPBR=myDS->RealPaveBlock(aPB);
      if (aMPBF.Contains(aPBR)) {
        continue;
      }
      //
      if (!aPB->HasShrunkData()) {
        continue;
      }
      //
      Bnd_Box aBBE;
      aPB->ShrunkData(aTS1, aTS2, aBBE);
      //
      if (aBBF.IsOut (aBBE)) {
        continue;
      }
      //
      BOPAlgo_EdgeFace& aEdgeFace=aVEdgeFace.Append1();
      //
      aEdgeFace.SetIndices(nE, nF);
      aEdgeFace.SetPaveBlock(aPB);
      //
      aEdgeFace.SetEdge (aE);
      aEdgeFace.SetFace (aF);
      aEdgeFace.SetTolE (aTolE);
      aEdgeFace.SetTolF (aTolF);
      aEdgeFace.SetDiscretize (aDiscretize);
      aEdgeFace.SetDeflection (aDeflection);
      //
      IntTools_Range aSR(aTS1, aTS2);
      IntTools_Range anewSR=aSR;
      BOPTools_AlgoTools::CorrectRange(aE, aF, aSR, anewSR);
      aEdgeFace.SetNewSR(anewSR);
      //
      aPB->Range(aT1, aT2);
      IntTools_Range aPBRange(aT1, aT2);
      aSR = aPBRange;
      BOPTools_AlgoTools::CorrectRange(aE, aF, aSR, aPBRange);
      aEdgeFace.SetRange (aPBRange);
      aEdgeFace.SetProgressIndicator(myProgressIndicator);

      // Save the pair to avoid their forced intersection
      BOPDS_MapOfPaveBlock* pMPB = myFPBDone.ChangeSeek(nF);
      if (!pMPB)
        pMPB = myFPBDone.Bound(nF, BOPDS_MapOfPaveBlock());
      pMPB->Add(aPB);
    }//for (; aIt.More(); aIt.Next()) {
  }//for (; myIterator->More(); myIterator->Next()) {
  //
  aNbEdgeFace=aVEdgeFace.Extent();
  //=================================================================
  BOPAlgo_EdgeFaceCnt::Perform(myRunParallel, aVEdgeFace, myContext);
  //=================================================================
  //
  for (k=0; k < aNbEdgeFace; ++k) {
    BOPAlgo_EdgeFace& aEdgeFace=aVEdgeFace(k);
    if (!aEdgeFace.IsDone()) {
      continue;
    }
    //~~~
    aEdgeFace.Indices(nE, nF);
    //
    const TopoDS_Edge& aE=aEdgeFace.Edge();
    const TopoDS_Face& aF=aEdgeFace.Face();
    //
    aTolE=aEdgeFace.TolE();
    aTolF=aEdgeFace.TolF();
    const IntTools_Range& anewSR=aEdgeFace.NewSR();
    Handle(BOPDS_PaveBlock)& aPB=aEdgeFace.PaveBlock();
    //
    aPB->Range(aT1, aT2);
    aPB->Indices(nV[0], nV[1]);
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    const BOPCol_MapOfInteger& aMIFOn=aFI.VerticesOn();
    const BOPCol_MapOfInteger& aMIFIn=aFI.VerticesIn();
    //~~~
    const IntTools_SequenceOfCommonPrts& aCPrts=aEdgeFace.CommonParts();
    aNbCPrts=aCPrts.Length();
    for (i=1; i<=aNbCPrts; ++i) {
      const IntTools_CommonPrt& aCPart=aCPrts(i);
      aType=aCPart.Type();
      switch (aType) {
        case TopAbs_VERTEX:  {
          Standard_Boolean bIsOnPave[2];
          Standard_Integer j;
          Standard_Real aT, aTolToDecide; 
          TopoDS_Vertex aVnew;
          //
          IntTools_Tools::VertexParameter(aCPart, aT);
          BOPTools_AlgoTools::MakeNewVertex(aE, aT, aF, aVnew);
          //
          const IntTools_Range& aR=aCPart.Range1();
          aTolToDecide=5.e-8;
          //
          IntTools_Range aR1(aT1,anewSR.First()),aR2(anewSR.Last(), aT2);
          //
          bIsOnPave[0]=IntTools_Tools::IsInRange(aR1, aR, aTolToDecide); 
          bIsOnPave[1]=IntTools_Tools::IsInRange(aR2, aR, aTolToDecide); 
          //
          if (bIsOnPave[0] && bIsOnPave[1]) {
            bV[0]=CheckFacePaves(nV[0], aMIFOn, aMIFIn);
            bV[1]=CheckFacePaves(nV[1], aMIFOn, aMIFIn);
            if (bV[0] && bV[1]) {
              IntTools_CommonPrt aCP = aCPart;
              aCP.SetType(TopAbs_EDGE);
              BOPDS_InterfEF& aEF=aEFs.Append1();
              iX=aEFs.Extent()-1;
              aEF.SetIndices(nE, nF);
              aEF.SetCommonPart(aCP);
              myDS->AddInterf(nE, nF);
              //
              aMIEFC.Add(nF);
              //           
              BOPAlgo_Tools::FillMap(aPB, nF, aMPBLI, aAllocator);
              break;
            }
          }
          for (j=0; j<2; ++j) {
            if (bIsOnPave[j]) {
              bV[j]=CheckFacePaves(nV[j], aMIFOn, aMIFIn);
              if (bV[j]) {
                const TopoDS_Vertex& aV = 
                  (*(TopoDS_Vertex *)(&myDS->Shape(nV[j])));
                //
                Standard_Real f, l, aTolVnew, aDistPP, aTolPC, aTolV;
                //
                const Handle(Geom_Curve)& aCur = BRep_Tool::Curve(aE, f, l);
                //
                gp_Pnt aP1 = BRep_Tool::Pnt(aV);
                gp_Pnt aP2 = aCur->Value(aT);
                //
                
                aDistPP=aP1.Distance(aP2);
                
                aTolPC=Precision::PConfusion();
                aTolV=BRep_Tool::Tolerance(aV);
                if (aDistPP > (aTolV+aTolPC)) {
                  aTolVnew=Max(aTolE, aDistPP);
                  UpdateVertex(nV[j], aTolVnew);
                }
              }
              else {
                bIsOnPave[j] = ForceInterfVF(nV[j], nF);
              }
            }
          }
          //
          if (!bIsOnPave[0] && !bIsOnPave[1]) {
            if (CheckFacePaves(aVnew, aMIFOn)) {
              continue;
            }
            //
            const gp_Pnt& aPnew = BRep_Tool::Pnt(aVnew);
            Standard_Real aTolV = BRep_Tool::Tolerance(aVnew);
            aTolV = Max(aTolV, Max(aTolE, aTolF));
            //
            if (!myContext->IsPointInFace(aPnew, aF, aTolV)) {
              continue;
            }
            //
            aBB.UpdateVertex(aVnew, aTolV);
            //
            aMIEFC.Add(nF);
            // 1
            BOPDS_InterfEF& aEF=aEFs.Append1();
            iX=aEFs.Extent()-1;
            aEF.SetIndices(nE, nF);
            aEF.SetCommonPart(aCPart);
            // 2
            myDS->AddInterf(nE, nF);
            // 3
            BOPDS_CoupleOfPaveBlocks aCPB;
            //
            aCPB.SetPaveBlocks(aPB, aPB);
            aCPB.SetIndexInterf(iX);
            aMVCPB.Add(aVnew, aCPB);
          }
        }
          break;
        case TopAbs_EDGE:  {
          aMIEFC.Add(nF);
          //
          // 1
          BOPDS_InterfEF& aEF=aEFs.Append1();
          iX=aEFs.Extent()-1;
          aEF.SetIndices(nE, nF);
          //
          bV[0]=CheckFacePaves(nV[0], aMIFOn, aMIFIn);
          bV[1]=CheckFacePaves(nV[1], aMIFOn, aMIFIn);
          if (!bV[0] || !bV[1]) {
            myDS->AddInterf(nE, nF);
            break;
          }
          aEF.SetCommonPart(aCPart);
          // 2
          myDS->AddInterf(nE, nF);
          // 3
          BOPAlgo_Tools::FillMap(aPB, nF, aMPBLI, aAllocator);
          
        }
          break; 
        default:
          break; 
      }//switch (aType) {
    }//for (i=1; i<=aNbCPrts; ++i) {
  }// for (k=0; k < aNbEdgeEdge; ++k) {
  // 
  //=========================================
  // post treatment
  //=========================================
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLI, aAllocator, myDS);
  PerformVerticesEF(aMVCPB, aAllocator);
  //
  // Update FaceInfoIn for all faces having EF common parts
  BOPCol_MapIteratorOfMapOfInteger aItMI;
  aItMI.Initialize(aMIEFC);
  for (; aItMI.More(); aItMI.Next()) {
    nF=aItMI.Value();
    myDS->UpdateFaceInfoIn(nF);
  }
  //-----------------------------------------------------scope t
  aMIEFC.Clear();
  aMVCPB.Clear();
  aMPBLI.Clear();
  ////aAllocator.Nullify();
}
//=======================================================================
//function : PerformVertices1
//purpose  : 
//=======================================================================
Standard_Integer BOPAlgo_PaveFiller::PerformVerticesEF
  (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
   Handle(NCollection_BaseAllocator)& theAllocator)
{
  Standard_Integer aNbV, iRet;
  //
  iRet=0;
  aNbV=theMVCPB.Extent();
  if (!aNbV) {
    return iRet;
  }
  //
  Standard_Integer nVx, nVSD, iV, iErr, nE, iFlag, iX, i, aNbPBLI;
  Standard_Real aT;
  TopoDS_Shape aV;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  BOPDS_PDS aPDS;
  BOPDS_ShapeInfo aSI;
  BOPDS_Pave aPave;
  //
  BOPCol_ListOfShape aLS(theAllocator);
  BOPCol_DataMapOfShapeInteger aMVI(100, theAllocator);
  BOPDS_IndexedDataMapOfPaveBlockListOfInteger aMPBLI(100, theAllocator);
  BOPAlgo_PaveFiller aPF(theAllocator); 
  //
  aSI.SetShapeType(TopAbs_VERTEX);
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  //
  // 1 prepare arguments
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aS=theMVCPB.FindKey(i);
    aLS.Append(aS);
  }
  //
  // 2 Fuse vertices
  aPF.SetIsPrimary(Standard_False);
  aPF.SetNonDestructive(myNonDestructive);
  aPF.SetArguments(aLS);
  aPF.Perform();
  iErr=aPF.ErrorStatus();
  if (iErr) {
    iRet=1;
    return iRet;
  }
  aPDS=aPF.PDS();
  //
  // 3 Add new vertices to theDS; 
  // 4 Map PaveBlock/ListOfVertices to add to this PaveBlock ->aMPBLI
  aItLS.Initialize(aLS);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aVx=aItLS.Value();
    nVx=aPDS->Index(aVx);
    //
    if (aPDS->HasShapeSD(nVx, nVSD)) {
      aV=aPDS->Shape(nVSD);
    }
    else {
      aV=aVx;
    }
    // index of new vertex in theDS -> iV
    if (!aMVI.IsBound(aV)) {
      aSI.SetShape(aV);
      iV=myDS->Append(aSI);
      //
      BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(iV);
      Bnd_Box& aBox=aSIDS.ChangeBox();
      BRepBndLib::Add(aV, aBox);
      //
      aMVI.Bind(aV, iV);
    }
    else {
      iV=aMVI.Find(aV);
    }
    //
    BOPDS_CoupleOfPaveBlocks &aCPB=theMVCPB.ChangeFromKey(aVx);
    aCPB.SetIndex(iV);
    // update EF interference
    iX=aCPB.IndexInterf();
    BOPDS_InterfEF& aEF=aEFs(iX);
    aEF.SetIndexNew(iV);
    // map aMPBLI
    const Handle(BOPDS_PaveBlock)& aPB=aCPB.PaveBlock1();
    if (aMPBLI.Contains(aPB)) {
      BOPCol_ListOfInteger& aLI=aMPBLI.ChangeFromKey(aPB);
      aLI.Append(iV);
    }
    else {
      BOPCol_ListOfInteger aLI(theAllocator);
      aLI.Append(iV);
      aMPBLI.Add(aPB, aLI);
    }
  }
  //
  // 5 
  // 5.1  Compute Extra Paves and 
  // 5.2. Add Extra Paves to the PaveBlocks
  aNbPBLI=aMPBLI.Extent();
  for (i=1; i<=aNbPBLI; ++i) {
    Handle(BOPDS_PaveBlock) aPB=aMPBLI.FindKey(i);
    const BOPCol_ListOfInteger& aLI=aMPBLI.FindFromIndex(i);
    nE=aPB->OriginalEdge();
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE)));
    // 
    aItLI.Initialize(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      nVx=aItLI.Value();
      const TopoDS_Vertex& aVx=(*(TopoDS_Vertex *)(&myDS->Shape(nVx)));
      //
      iFlag=myContext->ComputeVE (aVx, aE, aT);
      if (!iFlag) {
        aPave.SetIndex(nVx);
        aPave.SetParameter(aT);
        aPB->AppendExtPave(aPave);
      }
    }
  }
  // 6  Split PaveBlocks
  for (i=1; i<=aNbPBLI; ++i) {
    Handle(BOPDS_PaveBlock) aPB=aMPBLI.FindKey(i);
    nE=aPB->OriginalEdge();
    // 3
    if (!myDS->IsCommonBlock(aPB)) {
      myDS->UpdatePaveBlock(aPB);
    }
    else {
      const Handle(BOPDS_CommonBlock)& aCB=myDS->CommonBlock(aPB);
      myDS->UpdateCommonBlock(aCB);
    }    
  }//for (; aItMPBLI.More(); aItMPBLI.Next()) {
  // 
  return iRet;
}
//=======================================================================
// function: CheckFacePaves
// purpose: 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::CheckFacePaves 
  (const Standard_Integer nVx,
   const BOPCol_MapOfInteger& aMIFOn,
   const BOPCol_MapOfInteger& aMIFIn)
{
  Standard_Boolean bRet;
  Standard_Integer nV;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  bRet=Standard_False;
  //
  aIt.Initialize(aMIFOn);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    if (nV==nVx) {
      bRet=!bRet;
      return bRet;
    }
  }
  aIt.Initialize(aMIFIn);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    if (nV==nVx) {
      bRet=!bRet;
      return bRet;
    }
  }
  //
  return bRet;
}
//=======================================================================
// function: CheckFacePaves
// purpose: 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::CheckFacePaves 
  (const TopoDS_Vertex& aVnew,
   const BOPCol_MapOfInteger& aMIF)
{
  Standard_Boolean bRet;
  Standard_Integer nV, iFlag;
  BOPCol_MapIteratorOfMapOfInteger aIt;
  //
  bRet=Standard_True;
  //
  aIt.Initialize(aMIF);
  for (; aIt.More(); aIt.Next()) {
    nV=aIt.Value();
    const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nV)));
    iFlag=BOPTools_AlgoTools::ComputeVV(aVnew, aV);
    if (!iFlag) {
      return bRet;
    }
  }
  //
  return !bRet;
}
//=======================================================================
//function : ForceInterfVF
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_PaveFiller::ForceInterfVF
  (const Standard_Integer nV, 
   const Standard_Integer nF)
{
  Standard_Boolean bRet;
  Standard_Integer iFlag, nVx;
  Standard_Real U, V, aTolVNew;
  //
  bRet = Standard_False;
  const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&myDS->Shape(nV);
  const TopoDS_Face&   aF = *(TopoDS_Face*)  &myDS->Shape(nF);
  //
  iFlag = myContext->ComputeVF(aV, aF, U, V, aTolVNew);
  if (iFlag == 0 || iFlag == -2) {
    bRet=!bRet;
    //
    BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
    aVFs.SetIncrement(10);
    // 1
    BOPDS_InterfVF& aVF=aVFs.Append1();
    //
    aVF.SetIndices(nV, nF);
    aVF.SetUV(U, V);
    // 2
    myDS->AddInterf(nV, nF);
    //
    // 3 update vertex V/F if necessary
    nVx=UpdateVertex(nV, aTolVNew);
    // 4
    if (myDS->IsNewShape(nVx)) {
      aVF.SetIndexNew(nVx);
    }
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    BOPCol_MapOfInteger& aMVIn=aFI.ChangeVerticesIn();
    aMVIn.Add(nVx);
  }
  return bRet;
}


//=======================================================================
//function : ForceInterfEF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ForceInterfEF()
{
  if (!myIsPrimary)
    return;

  // Now that we have vertices increased and unified, try to find additional
  // edge/face common blocks among the pairs of edge/face.
  // Here, we are interested in common blocks only, as all real intersections
  // should have happened already. Thus, we need to check only those pairs
  // of edge/face which have the same vertices.

  // Collect all pave blocks
  BOPDS_IndexedMapOfPaveBlock aMPB;
  const Standard_Integer aNbS = myDS->NbSourceShapes();
  for (Standard_Integer nE = 0; nE < aNbS; ++nE)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nE);
    if (aSI.ShapeType() != TopAbs_EDGE)
      // Not an edge
      continue;

    if (!aSI.HasReference())
      // Edge has no pave blocks
      continue;

    if (aSI.HasFlag())
      // Degenerated edge
      continue;

    const BOPDS_ListOfPaveBlock& aLPB = myDS->PaveBlocks(nE);
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
    for (; aItLPB.More(); aItLPB.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      const Handle(BOPDS_PaveBlock)& aPBR = myDS->RealPaveBlock(aPB);
      aMPB.Add(aPBR);
    }
  }

  // Perform intersection of collected pave blocks with faces
  ForceInterfEF(aMPB, Standard_True);
}

//=======================================================================
//function : ForceInterfEF
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ForceInterfEF(const BOPDS_IndexedMapOfPaveBlock& theMPB,
                                       const Standard_Boolean theAddInterf)
{
  if (theMPB.IsEmpty())
    return;

  // Fill the tree with bounding boxes of the pave blocks
  NCollection_UBTree<Standard_Integer, Bnd_Box> aBBTree;
  NCollection_UBTreeFiller<Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);

  Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
  BOPDS_IndexedMapOfPaveBlock aPBMap(1, anAlloc);

  Standard_Integer aNbPB = theMPB.Extent();
  for (Standard_Integer iPB = 1; iPB <= aNbPB; ++iPB)
  {
    Handle(BOPDS_PaveBlock) aPB = theMPB(iPB);
    if (!aPB->HasShrunkData())
    {
      FillShrunkData(aPB);
      if (!aPB->HasShrunkData())
        continue;
    }

    Standard_Real f, l;
    Bnd_Box aPBBox;
    aPB->ShrunkData(f, l, aPBBox);

    aTreeFiller.Add(aPBMap.Add(aPB), aPBBox);
  }

  // Shake the tree
  aTreeFiller.Fill();

  const Standard_Boolean bSICheckMode = (myArguments.Extent() == 1);

  // Find pairs of Face/PaveBlock containing the same vertices
  // and prepare those pairs for intersection.
  BOPAlgo_VectorOfEdgeFace aVEdgeFace;

  const Standard_Integer aNbS = myDS->NbSourceShapes();
  for (Standard_Integer nF = 0; nF < aNbS; ++nF)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nF);
    if (aSI.ShapeType() != TopAbs_FACE)
      // Not a face
      continue;

    if (!aSI.HasReference())
      // Face has no face info
      continue;

    const Bnd_Box& aBoxF = aSI.Box();
    BOPCol_BoxBndTreeSelector aSelector;
    aSelector.SetBox(aBoxF);

    if (!aBBTree.Select(aSelector))
      continue;

    const TopoDS_Face& aF = TopoDS::Face (aSI.Shape());
    const BOPDS_FaceInfo& aFI = myDS->FaceInfo(nF);
    // Vertices of the face
    BOPCol_MapOfInteger aMVF;
    const BOPCol_MapOfInteger* pMVF[] = { &aFI.VerticesOn(),
      &aFI.VerticesIn(),
      &aFI.VerticesSc() };
    for (Standard_Integer iM = 0; iM < 3; ++iM)
    {
      BOPCol_MapIteratorOfMapOfInteger itM(*pMVF[iM]);
      for (; itM.More(); itM.Next())
        aMVF.Add(itM.Value());
    }

    // Pave Blocks of the face
    const BOPDS_IndexedMapOfPaveBlock* pMPBF[] = { &aFI.PaveBlocksOn(),
      &aFI.PaveBlocksIn(),
      &aFI.PaveBlocksSc() };
    for (Standard_Integer iM = 0; iM < 3; ++iM)
    {
      const Standard_Integer aNb = pMPBF[iM]->Extent();
      for (Standard_Integer iPB = 1; iPB <= aNb; ++iPB)
      {
        const Handle(BOPDS_PaveBlock)& aPB = pMPBF[iM]->FindKey(iPB);
        aMVF.Add(aPB->Pave1().Index());
        aMVF.Add(aPB->Pave2().Index());
      }
    }

    // Projection tool
    GeomAPI_ProjectPointOnSurf& aProjPS = myContext->ProjPS(aF);
    BRepAdaptor_Surface aSurfAdaptor (aF, Standard_False);

    // Iterate on pave blocks and combine pairs containing
    // the same vertices
    const BOPCol_ListOfInteger& aLIPB = aSelector.Indices();
    BOPCol_ListOfInteger::Iterator itLIPB(aLIPB);
    for (; itLIPB.More(); itLIPB.Next())
    {
      const Handle(BOPDS_PaveBlock)& aPB = aPBMap(itLIPB.Value());
      if (pMPBF[0]->Contains(aPB) ||
          pMPBF[1]->Contains(aPB) ||
          pMPBF[2]->Contains(aPB))
        continue;

      // Check if the face contains both vertices of the pave block
      Standard_Integer nV1, nV2;
      aPB->Indices(nV1, nV2);
      if (!aMVF.Contains(nV1) || !aMVF.Contains(nV2))
        // Face does not contain the vertices
        continue;

      // Get the edge
      Standard_Integer nE;
      if (!aPB->HasEdge(nE))
      {
        nE = aPB->OriginalEdge();
        if (nE < 0)
          continue;

        // Make sure that the edge and face came from different arguments
        if (myDS->Rank(nF) == myDS->Rank(nE))
          continue;
      }

      const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(nE));
      BRepAdaptor_Curve aBAC(aE);

      // Check directions coincidence at middle point on the edge
      // and projection of that point on the face.
      // If the angle between tangent vector to the curve and normal
      // of the face is not in the range of 65 - 115 degrees, do not use the additional
      // tolerance, as it may lead to undesired unification of edge with the face.
      Standard_Boolean bUseAddTol = Standard_True;

      Standard_Real aTS[2];
      Bnd_Box aPBBox;
      aPB->ShrunkData(aTS[0], aTS[1], aPBBox);

      // Middle point
      gp_Pnt aPOnE;
      // Tangent vector in the middle point
      gp_Vec aVETgt;
      aBAC.D1(BOPTools_AlgoTools2D::IntermediatePoint(aTS[0], aTS[1]), aPOnE, aVETgt);
      if (aVETgt.SquareMagnitude() < gp::Resolution())
        continue;

      aProjPS.Perform(aPOnE);
      if (!aProjPS.NbPoints())
        continue;

      // Check the distance in the middle point, using the max vertices
      // tolerance as the criteria.
      const TopoDS_Vertex& aV1 = TopoDS::Vertex(myDS->Shape(nV1));
      const TopoDS_Vertex& aV2 = TopoDS::Vertex(myDS->Shape(nV2));

      // In the Self-Interference check mode we are interested in real
      // intersections only, so use only the real tolerance of edges,
      // no need to use the extended tolerance.
      Standard_Real aTolCheck = (bSICheckMode ? myFuzzyValue :
                                 2 * Max(BRep_Tool::Tolerance(aV1), BRep_Tool::Tolerance(aV2)));

      if (aProjPS.LowerDistance() > aTolCheck + myFuzzyValue)
        continue;

      Standard_Real U, V;
      aProjPS.LowerDistanceParameters(U, V);
      if (!myContext->IsPointInFace(aF, gp_Pnt2d(U, V)))
        continue;

      if (aSurfAdaptor.GetType() != GeomAbs_Plane ||
          aBAC.GetType() != GeomAbs_Line)
      {
        gp_Pnt aPOnS = aProjPS.NearestPoint();
        gp_Vec aVFNorm(aPOnS, aPOnE);
        if (aVFNorm.SquareMagnitude() > gp::Resolution())
        {
          // Angle between vectors should be close to 90 degrees.
          // We allow deviation of 10 degrees.
          Standard_Real aCos = aVFNorm.Normalized().Dot (aVETgt.Normalized());
          if (Abs(aCos) > 0.17365)
            bUseAddTol = Standard_False;
        }
      }

      // Compute an addition to Fuzzy value
      Standard_Real aTolAdd = 0.0;
      if (bUseAddTol)
      {
        // Compute the distance from the bounding points of the edge
        // to the face and use the maximal of these distances as a
        // fuzzy tolerance for the intersection.
        // Use the maximal tolerance of the pave block's vertices
        // as a max criteria for the computed distance.

        for (Standard_Integer iP = 0; iP < 2; ++iP)
        {
          gp_Pnt aP = aBAC.Value(aTS[iP]);
          aProjPS.Perform(aP);
          if (aProjPS.NbPoints())
          {
            Standard_Real aDistEF = aProjPS.LowerDistance();
            if (aDistEF < aTolCheck && aDistEF > aTolAdd)
              aTolAdd = aDistEF;
          }
        }
        if (aTolAdd > 0.)
        {
          aTolAdd -= (BRep_Tool::Tolerance(aE) + BRep_Tool::Tolerance(aF));
          if (aTolAdd < 0.)
            aTolAdd = 0.;
        }
      }

      Standard_Boolean bIntersect = aTolAdd > 0;
      if (!bIntersect)
      {
        const BOPDS_MapOfPaveBlock* pMPB = myFPBDone.Seek(nF);
        bIntersect = !pMPB || !(pMPB->Contains(aPB));
      }

      if (bIntersect)
      {
        // Prepare pair for intersection
        BOPAlgo_EdgeFace& aEdgeFace = aVEdgeFace.Append1();
        aEdgeFace.SetIndices(nE, nF);
        aEdgeFace.SetPaveBlock(aPB);
        aEdgeFace.SetEdge(aE);
        aEdgeFace.SetFace(aF);
        aEdgeFace.SetTolE (BRep_Tool::Tolerance (aE) + aTolAdd / 2.);
        aEdgeFace.SetTolF (BRep_Tool::Tolerance (aF) + aTolAdd / 2.);
        aEdgeFace.UseQuickCoincidenceCheck(Standard_True);
        aEdgeFace.SetRange(IntTools_Range(aPB->Pave1().Parameter(), aPB->Pave2().Parameter()));
        aEdgeFace.SetProgressIndicator(myProgressIndicator);
      }
    }
  }

  Standard_Integer aNbEFs = aVEdgeFace.Length();
  if (!aNbEFs)
    return;

  aPBMap.Clear();
  anAlloc->Reset();

  // Perform intersection of the found pairs
  BOPAlgo_EdgeFaceCnt::Perform (myRunParallel, aVEdgeFace, myContext);

  BOPDS_VectorOfInterfEF& aEFs = myDS->InterfEF();
  if (theAddInterf && aEFs.IsEmpty())
    aEFs.SetIncrement(10);

  // Analyze the results of intersection looking for TopAbs_EDGE
  // intersection type only.

  // Collect all pairs for common block creation
  BOPDS_IndexedDataMapOfPaveBlockListOfInteger aMPBLI(1, anAlloc);

  for (Standard_Integer i = 0; i < aNbEFs; ++i)
  {
    BOPAlgo_EdgeFace& anEdgeFace = aVEdgeFace(i);
    if (!anEdgeFace.IsDone())
      continue;

    const IntTools_SequenceOfCommonPrts& aCParts = anEdgeFace.CommonParts();
    if (aCParts.Length() != 1)
      continue;

    const IntTools_CommonPrt& aCP = aCParts(1);
    if (aCP.Type() != TopAbs_EDGE)
      continue;

    Standard_Integer nE, nF;
    anEdgeFace.Indices(nE, nF);
    if (theAddInterf)
    {
      // Add interference
      BOPDS_InterfEF& aEF = aEFs.Append1();
      aEF.SetIndices(nE, nF);
      aEF.SetCommonPart(aCP);
      myDS->AddInterf(nE, nF);
    }

    const Handle(BOPDS_PaveBlock)& aPB = anEdgeFace.PaveBlock();
    // Update face information with new IN pave block
    myDS->ChangeFaceInfo(nF).ChangePaveBlocksIn().Add(aPB);
    if (theAddInterf)
      // Fill map for common blocks creation
      BOPAlgo_Tools::FillMap(aPB, nF, aMPBLI, anAlloc);
  }

  if (aMPBLI.Extent())
    // Create new common blocks for coinciding pairs
    BOPAlgo_Tools::PerformCommonBlocks(aMPBLI, anAlloc, myDS);
}

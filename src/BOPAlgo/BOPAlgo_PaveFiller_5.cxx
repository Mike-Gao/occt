// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_PaveFiller.ixx>

#include <NCollection_IncAllocator.hxx>

#include <Bnd_Box.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>

#include <IntTools_EdgeFace.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <BOPCol_MapOfInteger.hxx>

#include <BOPInt_Context.hxx>

#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfInteger.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_Pave.hxx>

#include <BOPTools_AlgoTools.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <BRepBndLib.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPInt_Tools.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>


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
  myIterator->Initialize(TopAbs_EDGE, TopAbs_FACE);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //----------------------------------------------------------------------
  Standard_Boolean bJustAdd;
  Standard_Integer nE, nF, aDiscretize, i, aNbCPrts, iX;
  Standard_Real aTolE, aTolF, aTS1, aTS2, aDeflection;
  Handle(NCollection_IncAllocator) aAllocator;
  TopAbs_ShapeEnum aType;
  Handle(IntTools_Context) aCtx;
  BOPDS_ListIteratorOfListOfPaveBlock aIt;
  //-----------------------------------------------------scope f
  //
  //modified by NIZHNY-EMV Wed Nov 30 12:01:10 2011
  BRep_Builder aBB;
  //modified by NIZHNY-EMV Wed Nov 30 12:01:12 2011
  //
  aAllocator=new NCollection_IncAllocator();
  
  BOPCol_MapOfInteger aMIEFC(100, aAllocator);
  BOPDS_DataMapOfShapeCoupleOfPaveBlocks aMVCPB(100, aAllocator);
  BOPDS_DataMapOfPaveBlockListOfInteger aMPBLI(100, aAllocator);
  //
  aDiscretize=35;
  aDeflection=0.01;
  //
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  aEFs.SetStartSize(iSize);
  aEFs.SetIncrement(iSize);
  aEFs.Init();
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
    const BOPDS_MapOfPaveBlock& aMPBF=aFI.PaveBlocksOn();
    const BOPCol_MapOfInteger& aMIFOn=aFI.VerticesOn();
    const BOPCol_MapOfInteger& aMIFIn=aFI.VerticesIn();
    //
    aTolE=BRep_Tool::Tolerance(aE);
    aTolF=BRep_Tool::Tolerance(aF);
    //
    BOPDS_ListOfPaveBlock& aLPB=myDS->ChangePaveBlocks(nE);
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      Handle(BOPDS_PaveBlock)& aPB=aIt.ChangeValue();
      //
      const Handle(BOPDS_PaveBlock)& aPBR=aPB->RealPaveBlock();
      if (aMPBF.Contains(aPBR)) {
        continue;
      }
      //
      if (!aPB->HasShrunkData()) {
        FillShrunkData(aPB);
        if (myErrorStatus) {
          return;
        }
      }
      //
      Bnd_Box aBBE;
      aPB->ShrunkData(aTS1, aTS2, aBBE);
      //
      if (aBBF.IsOut (aBBE)) {
        continue;
      }
      //
      // -----------f
      IntTools_EdgeFace aEdgeFace;
      //
      aEdgeFace.SetEdge (aE);
      aEdgeFace.SetFace (aF);
      aEdgeFace.SetTolE (aTolE);
      aEdgeFace.SetTolF (aTolF);
      aEdgeFace.SetDiscretize (aDiscretize);
      aEdgeFace.SetDeflection (aDeflection);
      aEdgeFace.SetContext(aCtx);
      //
      IntTools_Range aSR(aTS1, aTS2);
      IntTools_Range anewSR=aSR;
      //
      BOPTools_AlgoTools::CorrectRange(aE, aF, aSR, anewSR);
      aEdgeFace.SetRange (anewSR);
      //
      aEdgeFace.Perform();
      if (!aEdgeFace.IsDone()) {
        continue;
      }
      //
      const IntTools_SequenceOfCommonPrts& aCPrts=aEdgeFace.CommonParts();
      aNbCPrts=aCPrts.Length();
      //modified by NIZHNY-EMV Wed Dec 07 14:17:07 2011
      /*if (aNbCPrts) {
        if (myWarningStatus) {
          myErrorStatus = 40;
          return;
        }
	}*/
      //modified by NIZHNY-EMV Wed Dec 07 14:17:09 2011
      for (i=1; i<=aNbCPrts; ++i) {
        const IntTools_CommonPrt& aCPart=aCPrts(i);
        aType=aCPart.Type();
        switch (aType) {
        case TopAbs_VERTEX:  {
          Standard_Boolean bIsOnPave1, bIsOnPave2, bV1, bV2;
          Standard_Integer nV1, nV2;
          Standard_Real aT, aTolToDecide; 
          TopoDS_Vertex aVnew;
          
          BOPInt_Tools::VertexParameter(aCPart, aT);
          BOPTools_AlgoTools::MakeNewVertex(aE, aT, aF, aVnew);
          //
          const IntTools_Range& aR=aCPart.Range1();
          aTolToDecide=5.e-8;
          bIsOnPave1=BOPInt_Tools::IsOnPave1(anewSR.First(), aR, aTolToDecide); 
          bIsOnPave2=BOPInt_Tools::IsOnPave1(anewSR.Last() , aR, aTolToDecide); 
          //
          aPB->Indices(nV1, nV2);
          //

          //modified by NIZHNY-EMV Thu Mar 31 16:29:33 2011
          if (bIsOnPave1 && bIsOnPave2) {
            bV1=CheckFacePaves(nV1, aMIFOn, aMIFIn);
            bV2=CheckFacePaves(nV2, aMIFOn, aMIFIn);
            if (bV1 && bV2) {
              iX=aEFs.Append()-1;
              IntTools_CommonPrt aCP = aCPart;
              aCP.SetType(TopAbs_EDGE);
              BOPDS_InterfEF& aEF=aEFs(iX);
              aEF.SetIndices(nE, nF);
              aEF.SetCommonPart(aCP);
              myDS->AddInterf(nE, nF);
              // 3          
              BOPAlgo_Tools::FillMap(aPB, nF, aMPBLI, aAllocator);
              break;
            }
          }
          //modified by NIZHNY-EMV Thu Mar 31 16:29:34 2011
          if (bIsOnPave1) {
            bV1=CheckFacePaves(nV1, aMIFOn, aMIFIn);
            if (bV1) {
              //modified by NIZHNY-EMV Fri Feb 18 16:37:29 2011
              const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
              BOPTools_AlgoTools::UpdateVertex(aE, aT, aV);
              //modified by NIZHNY-EMV Tue Sep 13 11:14:27 2011
              BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV1);
              Bnd_Box& aBoxDS=aSIDS.ChangeBox();
              BRepBndLib::Add(aV, aBoxDS);
              //modified by NIZHNY-EMV Tue Sep 13 11:14:29 2011
              //modified by NIZHNY-EMV Fri Feb 18 16:37:45 2011
              continue;
            }
            bIsOnPave1=!bIsOnPave1;
          }
          //
          if (bIsOnPave2) {
            bV2=CheckFacePaves(nV2, aMIFOn, aMIFIn);
            if (bV2) {
              //modified by NIZHNY-EMV Fri Feb 18 16:38:34 2011
              const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
              BOPTools_AlgoTools::UpdateVertex(aE, aT, aV);
              //modified by NIZHNY-EMV Tue Sep 13 11:14:33 2011
              BOPDS_ShapeInfo& aSIDS=myDS->ChangeShapeInfo(nV2);
              Bnd_Box& aBoxDS=aSIDS.ChangeBox();
              BRepBndLib::Add(aV, aBoxDS);
              //modified by NIZHNY-EMV Tue Sep 13 11:14:34 2011
              //modified by NIZHNY-EMV Fri Feb 18 16:38:39 2011
              continue;
            }
            bIsOnPave2=!bIsOnPave2;
          }
          //
          if (!bIsOnPave1 && !bIsOnPave2) {
            if (CheckFacePaves(aVnew, aMIFOn)) {
              continue;
            }
            //modified by NIZHNY-EMV Wed Nov 30 12:01:34 2011
            aBB.UpdateVertex(aVnew, aTolE);
            //modified by NIZHNY-EMV Wed Nov 30 12:01:36 2011
            //
            aMIEFC.Add(nF);
            // 1
            iX=aEFs.Append()-1;
            BOPDS_InterfEF& aEF=aEFs(iX);
            aEF.SetIndices(nE, nF);
            aEF.SetCommonPart(aCPart);
            // 2
            myDS->AddInterf(nE, nF);
            // 3
            BOPDS_CoupleOfPaveBlocks aCPB;
            //
            aCPB.SetPaveBlocks(aPB, aPB);
            aCPB.SetIndexInterf(iX);
            aMVCPB.Bind(aVnew, aCPB);
          }
        }
          break;
        case TopAbs_EDGE:  {
          aMIEFC.Add(nF);
          //
          // 1
          iX=aEFs.Append()-1;
          BOPDS_InterfEF& aEF=aEFs(iX);
          aEF.SetIndices(nE, nF);
          //
          Standard_Boolean aCoinsideFlag;
          aCoinsideFlag=BOPTools_AlgoTools::IsBlockInOnFace(anewSR, aF, aE, myContext);
          if (!aCoinsideFlag) {
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
      // -----------t
    }//for (; aIt.More(); aIt.Next()) {
  }//for (; myIterator->More(); myIterator->Next()) {
  // 
  //=========================================
  // post treatment
  //=========================================
  BOPAlgo_Tools::PerformCommonBlocks(aMPBLI, aAllocator);
  PerformVertices1(aMVCPB, aAllocator);
  //
  // Update FaceInfoIn for all faces having EF common parts
  BOPCol_MapIteratorOfMapOfInteger aItMI;
  aItMI.Initialize(aMIEFC);
  for (; aItMI.More(); aItMI.Next()) {
    nF=aItMI.Value();
    myDS->UpdateFaceInfoIn(nF);
  }
  // Refine FaceInfoOn to remove all formal pave blocks 
  // made during EF processing 
  //myDS->RefineFaceInfoOn();
  //-----------------------------------------------------scope t
  aMIEFC.Clear();
  aMVCPB.Clear();
  aMPBLI.Clear();
  aAllocator.Nullify();
  //
  
}
//=======================================================================
//function : PerformVertices1
//purpose  : 
//=======================================================================
  Standard_Integer BOPAlgo_PaveFiller::PerformVertices1
    (BOPDS_DataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
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
  Standard_Integer nVx, nVSD, iV, iErr, nE, iFlag, iX;
  Standard_Real aT;
  TopoDS_Shape aV;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  BOPDS_DataMapIteratorOfDataMapOfShapeCoupleOfPaveBlocks aItMVCPB;
  BOPDS_DataMapIteratorOfDataMapOfPaveBlockListOfInteger aItMPBLI;
  BOPDS_PDS aPDS;
  BOPDS_ShapeInfo aSI;
  BOPDS_Pave aPave;
  //
  BOPCol_ListOfShape aLS(theAllocator);
  BOPCol_DataMapOfShapeInteger aMVI(100, theAllocator);
  BOPDS_DataMapOfPaveBlockListOfInteger aMPBLI(100, theAllocator);
  BOPAlgo_PaveFiller aPF(theAllocator); 
  //
  aSI.SetShapeType(TopAbs_VERTEX);
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  //
  // 1 prepare arguments
  aItMVCPB.Initialize(theMVCPB);
  for (; aItMVCPB.More(); aItMVCPB.Next()) {
    const TopoDS_Shape& aS=aItMVCPB.Key();
    aLS.Append(aS);
  }
  //
  // 2 Fuse vertices
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
    BOPDS_CoupleOfPaveBlocks &aCPB=theMVCPB.ChangeFind(aVx);
    aCPB.SetIndex(iV);
    // update EF interference
    iX=aCPB.IndexInterf();
    BOPDS_InterfEF& aEF=aEFs(iX);
    aEF.SetIndexNew(iV);
    // map aMPBLI
    const Handle(BOPDS_PaveBlock)& aPB=aCPB.PaveBlock1();
    if (aMPBLI.IsBound(aPB)) {
      BOPCol_ListOfInteger& aLI=aMPBLI.ChangeFind(aPB);
      aLI.Append(iV);
    }
    else {
      BOPCol_ListOfInteger aLI(theAllocator);
      aLI.Append(iV);
      aMPBLI.Bind(aPB, aLI);
    }
  }
  //
  // 5 
  // 5.1  Compute Extra Paves and 
  // 5.2. Add Extra Paves to the PaveBlocks
  aItMPBLI.Initialize(aMPBLI);
  for (; aItMPBLI.More(); aItMPBLI.Next()) {
    Handle(BOPDS_PaveBlock) aPB=aItMPBLI.Key();
    nE=aPB->OriginalEdge();
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE)));
    // 
    const BOPCol_ListOfInteger& aLI=aItMPBLI.Value();
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
  // 6  Split PaveBlocksa
  aItMPBLI.Initialize(aMPBLI);
  for (; aItMPBLI.More(); aItMPBLI.Next()) {
    Handle(BOPDS_PaveBlock) aPB=aItMPBLI.Key();
    nE=aPB->OriginalEdge();
    // 3
    if (!aPB->IsCommonBlock()) {
      myDS->UpdatePaveBlock(aPB);
    }
    else {
      const Handle(BOPDS_CommonBlock)& aCB=aPB->CommonBlock();
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
  Standard_Boolean BOPAlgo_PaveFiller::CheckFacePaves (const Standard_Integer nVx,
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
  Standard_Boolean BOPAlgo_PaveFiller::CheckFacePaves (const TopoDS_Vertex& aVnew,
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

// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPAlgo_PaveFiller.ixx>

#include <NCollection_IncAllocator.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>

#include <BOPCol_MapOfInteger.hxx>

#include <BOPInt_Context.hxx>

#include <BOPDS_Iterator.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_SubIterator.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPCol_MapOfInteger.hxx>

//=======================================================================
// function: PerformVF
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformVF()
{
  Standard_Boolean bJustAdd;
  Standard_Integer iSize, nV, nF, nVSD, iFlag, nVx, i;
  Standard_Real aT1, aT2, aTolF, aTolV, aTolVnew;
  //
  myErrorStatus=0;
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_FACE);
  iSize=myIterator->ExpectedLength();
  if (iSize) {
    //
    BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
    aVFs.SetStartSize(iSize);
    aVFs.SetIncrement(iSize);
    aVFs.Init();
    //
    for (; myIterator->More(); myIterator->Next()) {
      myIterator->Value(nV, nF, bJustAdd);
      if(bJustAdd) {
        continue;
      }
      //
      if (myDS->IsSubShape(nV, nF)) {
        continue;
      }
      //
      if (myDS->HasInterfShapeSubShapes(nV, nF)) {
        myDS->ChangeFaceInfo(nF);
        continue;
      }
      //
      nVx=nV;
      if (myDS->HasShapeSD(nV, nVSD)) {
        nVx=nVSD;
      }
      //
      if (myDS->HasInterf(nVx, nF)) {
        continue;
      }
      //
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nVx))); 
      const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF))); 
      //
      aTolV = BRep_Tool::Tolerance(aV);
      aTolF = BRep_Tool::Tolerance(aF);
      //
      iFlag=myContext->ComputeVF(aV, aF, aT1, aT2, aTolVnew);
      if (iFlag) {
        continue; 
      }
      // 1
      i=aVFs.Append()-1;
      BOPDS_InterfVF& aVF=aVFs(i);
      aVF.SetIndices(nVx, nF);
      aVF.SetUV(aT1, aT2);
      // 2
      myDS->AddInterf(nVx, nF);
      //
      // 3 update vertex V/F if necessary
      nVx=UpdateVertex(nVx, aTolVnew);
      //
      // 4
      if (myDS->IsNewShape(nVx)) {
        aVF.SetIndexNew(nVx);
      }
      // update FaceInfo
      BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
      BOPCol_MapOfInteger& aMVIn=aFI.ChangeVerticesIn();
      aMVIn.Add(nVx);
    }// for (; myIterator->More(); myIterator->Next()) {
  }// if (iSize) {
  else {
    iSize=10;
    BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
    aVFs.SetStartSize(iSize);
    aVFs.SetIncrement(iSize);
    aVFs.Init();
  }
  //
  TreatVerticesEE();
} 
//=======================================================================
//function : TreatVerticesEE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::TreatVerticesEE()
{
  Standard_Integer i, aNbS, aNbEEs, nF, nV, iFlag;
  Standard_Real aT1, aT2;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  Handle(NCollection_IncAllocator) aAllocator;
  //
  //-----------------------------------------------------scope_1 f
  aAllocator=new NCollection_IncAllocator();
  BOPCol_ListOfInteger aLIV(aAllocator), aLIF(aAllocator);
  BOPCol_MapOfInteger aMI(100, aAllocator);
  BOPDS_MapOfPaveBlock aMPBF(100, aAllocator);
  //
  myErrorStatus=0;
  //
  aNbS=myDS->NbSourceShapes();
  //
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aNbEEs=aEEs.Extent();
  for (i=0; i<aNbEEs; ++i) {
    BOPDS_InterfEE& aEE=aEEs(i);
    if (aEE.HasIndexNew()) {
      nV=aEE.IndexNew();
      if (aMI.Add(nV)) {
        aLIV.Append(nV);
      }   
    }   
  }
  if (!aLIV.Extent()) {
    aAllocator.Nullify();
    return;
  }
  //
  aNbS=myDS->NbSourceShapes();
  for (nF=0; nF<aNbS; ++nF) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(nF);
    if (aSI.ShapeType()==TopAbs_FACE) {
      aLIF.Append(nF);
    }
  }
  if (!aLIF.Extent()) {
    aAllocator.Nullify();
    return;
  }
  //-------------------------------------------------------------
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  //
  BOPDS_SubIterator aIt(aAllocator);
  //
  aIt.SetDS(myDS);
  aIt.SetSubSet1(aLIF);
  aIt.SetSubSet2(aLIV);
  aIt.Prepare();
  aIt.Initialize();
  for (; aIt.More(); aIt.Next()) {
    aIt.Value(nV, nF);
    //
    BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
    const BOPCol_MapOfInteger& aMVOn=aFI.VerticesOn();
    //
    if (!aMVOn.Contains(nV)) {
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&myDS->Shape(nV))); 
      const TopoDS_Face& aF=(*(TopoDS_Face *)(&myDS->Shape(nF))); 
      iFlag=myContext->ComputeVF(aV, aF, aT1, aT2);
      if (!iFlag) {
        // 1
        i=aVFs.Append()-1;
        BOPDS_InterfVF& aVF=aVFs(i);
        aVF.SetIndices(nV, nF);
        aVF.SetUV(aT1, aT2);
        // 2
        myDS->AddInterf(nV, nF);
        //
        BOPCol_MapOfInteger& aMVIn=aFI.ChangeVerticesIn();
        aMVIn.Add(nV);
      }
    }
  }
  //
  aAllocator.Nullify();
  //-----------------------------------------------------scope_1 t
}

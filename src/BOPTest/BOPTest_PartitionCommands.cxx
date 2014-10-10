// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <BOPTest.ixx>

#include <stdio.h>
#include <string.h>

#include <NCollection_IncAllocator.hxx>

#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Color.hxx>
#include <DrawTrSurf.hxx>

#include <TopoDS_Shape.hxx>
//
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_BOP.hxx>
//
#include <BOPTest_DrawableShape.hxx>
#include <BOPTest_Objects.hxx>

//
#ifdef HAVE_TBB
#include <BOPCol_TBB.hxx>
//=======================================================================
//class : BOPTime_Chronometer
//purpose  : 
//=======================================================================
class BOPTime_Chronometer {
 public:
  BOPTime_Chronometer() {
  }
  //
  ~BOPTime_Chronometer() {
  }
  //
  void Start() {
    myT0 = tick_count::now();
  }
  //
  void Stop() {
    myTime=(tick_count::now() - myT0).seconds();
  }
  //
  double Time() const{
    return myTime;
  };
  //
 protected:
  tick_count myT0;
  double myTime;
};
////////////////////////////////////////////////////////////////////////
#else
#include <OSD_Chronometer.hxx>
//=======================================================================
//class    : BOPTime_Chronometer
//purpose  : 
//=======================================================================
class BOPTime_Chronometer {
 public:
  BOPTime_Chronometer() {
  }
  //
  ~BOPTime_Chronometer() {
  }
  //
  void Start() {
    myChronometer.Reset();
    myChronometer.Start();
  }
  //
  void Stop() {
    myChronometer.Stop();
    myChronometer.Show(myTime);
  }
  //
  double Time() const{
    return myTime;
  };
  //
 protected:
  OSD_Chronometer myChronometer;
  double myTime;
};
#endif

static Standard_Integer bfillds  (Draw_Interpretor&, Standard_Integer, const char**); 
static Standard_Integer bbuild   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bbop     (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bclear   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bnsection(Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : PartitionCommands
//purpose  : 
//=======================================================================
void BOPTest::PartitionCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "Partition commands";
  // Commands  
  theCommands.Add("bfillds"  , "use bfillds [-s -t]" , __FILE__, bfillds, g);
  theCommands.Add("bbuild"   , "use bbuild r [-s -t]", __FILE__, bbuild, g);
  theCommands.Add("bbop"     , "use bbop r op"       , __FILE__, bbop, g);
  theCommands.Add("bclear"   , "use bclear"          , __FILE__, bclear, g);
  theCommands.Add("bnsection", "use bnsection r"     , __FILE__, bnsection, g);
}

//=======================================================================
//function : bclear
//purpose  : 
//=======================================================================
Standard_Integer bclear(Draw_Interpretor& di, 
                        Standard_Integer n, 
                        const char** ) 
{
  if (n!=1) {
    di << " use bclear\n";
    return 0;
  }
  //
  BOPTest_Objects::Clear(); 
  return 0;
}
//=======================================================================
//function : bfillds
//purpose  : 
//=======================================================================
Standard_Integer bfillds(Draw_Interpretor& di, 
                         Standard_Integer n, 
                         const char** a) 
{ 
  if (n>3) {
    di << " use bfillds [-s -t]\n";
    return 0;
  }
  //
  char buf[32];
  Standard_Boolean bRunParallel, bShowTime;
  Standard_Integer i, aNbS, iErr;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_ListOfShape aLC;
  BOPTime_Chronometer aChrono;
  
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Shapes();
  aNbS=aLS.Extent();
  if (!aNbS) {
    di << " no objects to process\n";
    return 0;
  }
  //
  bShowTime=Standard_False;
  bRunParallel=Standard_True;
  for (i=1; i<n; ++i) {
    if (!strcmp(a[i], "-s")) {
      bRunParallel=Standard_False;
    }
    else if (!strcmp(a[i], "-t")) {
      bShowTime=Standard_True;
    }
  }
  //
  BOPCol_ListOfShape& aLT=BOPTest_Objects::Tools();
  //
  aIt.Initialize(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aLC.Append(aS);
  }
  //
  aIt.Initialize(aLT);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
     aLC.Append(aS);
  }
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  aPF.SetArguments(aLC);
  //aPF.SetRunParallel(bRunParallel);
  //
  aChrono.Start();
  //
  aPF.Perform();
  iErr=aPF.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  aChrono.Stop();
  //
  if (bShowTime) {
    Standard_Real aTime;
    //
    aTime=aChrono.Time();
    Sprintf(buf, "  Tps: %7.2lf\n", aTime);
    di << buf;
  }
  //
  return 0;
}
//=======================================================================
//function : bbuild
//purpose  : 
//=======================================================================
Standard_Integer bbuild(Draw_Interpretor& di,
                        Standard_Integer n, 
                        const char** a) 
{ 
  if (n<2) {
    di << " use bbuild r [-s -t]\n";
    return 0;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[128];
  Standard_Boolean bRunParallel, bShowTime;
  Standard_Integer i, iErr;
  
  BOPTime_Chronometer aChrono;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_Builder& aBuilder=BOPTest_Objects::Builder();
  aBuilder.Clear();
  //
  BOPCol_ListOfShape& aLSObj=BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBuilder.AddArgument(aS);
  }
  //
  BOPCol_ListOfShape& aLSTool=BOPTest_Objects::Tools();
  aIt.Initialize(aLSTool);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBuilder.AddArgument(aS);
  }
  //
  bShowTime=Standard_False;
  bRunParallel=Standard_True;
  for (i=2; i<n; ++i) {
    if (!strcmp(a[i], "-s")) {
      bRunParallel=Standard_False;
    }
    else if (!strcmp(a[i], "-t")) {
      bShowTime=Standard_True;
    }
  }
  aBuilder.SetRunParallel(bRunParallel);
  //
  //
  aChrono.Start();
  //
  aBuilder.PerformWithFiller(aPF); 
  iErr=aBuilder.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  aChrono.Stop();
  //
  if (bShowTime) {
    Standard_Real aTime;
    //
    aTime=aChrono.Time();
    Sprintf(buf, "  Tps: %7.2lf\n", aTime);
    di << buf;
  }
  //
  const TopoDS_Shape& aR=aBuilder.Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bbop
//purpose  : 
//=======================================================================
Standard_Integer bbop(Draw_Interpretor& di, 
                      Standard_Integer n, 
                      const char** a) 
{ 
  if (n!=3) {
    di << " use bbop r op\n";
    return 0;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[32];
  Standard_Integer iErr, iOp;
  BOPAlgo_Operation aOp;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  iOp=Draw::Atoi(a[2]);
  if (iOp<0 || iOp>4) {
    di << " invalid operation type\n";
  }
  aOp=(BOPAlgo_Operation)iOp;
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_BOP& aBOP=BOPTest_Objects::BOP();
  aBOP.Clear();
  //
  BOPCol_ListOfShape& aLSObj=BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBOP.AddArgument(aS);
  }
  //
  BOPCol_ListOfShape& aLSTools=BOPTest_Objects::Tools();
  aIt.Initialize(aLSTools);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBOP.AddTool(aS);
  }
  //
  aBOP.SetOperation(aOp);
  //
  aBOP.PerformWithFiller(aPF);
  iErr=aBOP.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  const TopoDS_Shape& aR=aBOP.Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

#include <BRep_Builder.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPTools.hxx>

//=======================================================================
//function : nsection
//purpose  : 
//=======================================================================
Standard_Integer bnsection(Draw_Interpretor& di, 
                           Standard_Integer n, 
                           const char** a) 
{ 
  if (n != 2) {
    di << "use bnsection r\n";
    return 0;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  Standard_Integer i, j, k, nE, nF1, nF2, aNbPB, aNbFF;
  Standard_Boolean bFlag;
  TopoDS_Compound aRC;
  BRep_Builder aBB;
  BOPCol_MapOfShape aME;
  BOPCol_IndexedMapOfShape aME1, aME2;
  //
  aBB.MakeCompound(aRC);
  BOPDS_VectorOfInterfFF& aFFs = pDS->InterfFF();
  aNbFF = aFFs.Extent();
  //
  for (i = 0; i < aNbFF; ++i) {
    BOPDS_InterfFF& aFF = aFFs(i);
    aFF.Indices(nF1, nF2);
    const BOPDS_FaceInfo& aFI1 = pDS->FaceInfo(nF1);
    const BOPDS_FaceInfo& aFI2 = pDS->FaceInfo(nF2);
    //
    const BOPDS_IndexedMapOfPaveBlock& aMPBIn1 = aFI1.PaveBlocksIn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBOn1 = aFI1.PaveBlocksOn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBSc1 = aFI1.PaveBlocksSc();
    //
    const BOPDS_IndexedMapOfPaveBlock& aMPBIn2 = aFI2.PaveBlocksIn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBOn2 = aFI2.PaveBlocksOn();
    //
    //1. Section edges
    aNbPB = aMPBSc1.Extent();
    for (j = 1; j <= aNbPB; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB = aMPBSc1(j);
      nE = aPB->Edge();
      const TopoDS_Shape& aE = pDS->Shape(nE);
      if (aME.Add(aE)) {
        aBB.Add(aRC, aE);
      }
    }
    //2. Common edges
    BOPDS_IndexedMapOfPaveBlock aMPB[4] = {aMPBOn2, aMPBIn1, aMPBIn2, aMPBOn1};
    for (k = 0; k < 3; ++k) {
      aNbPB = aMPB[k].Extent();
      for (j = 1; j <= aNbPB; ++j) {
        const Handle(BOPDS_PaveBlock)& aPB = aMPB[k](j);
        bFlag = (k==0) ? aMPB[3].Contains(aPB) :
          (aMPB[k-1].Contains(aPB) || aMPB[k+1].Contains(aPB));
        if (bFlag) {
          nE = aPB->Edge();
          const TopoDS_Shape& aE = pDS->Shape(nE);
          if (aME.Add(aE)) {
            aBB.Add(aRC, aE);
          }
        }
      }
    }
    //3. Shared edges
    aME1.Clear();
    aME2.Clear();
    //
    const TopoDS_Face& aF1 = (*(TopoDS_Face *)(&pDS->Shape(nF1)));
    const TopoDS_Face& aF2 = (*(TopoDS_Face *)(&pDS->Shape(nF2)));
    //
    BOPTools::MapShapes(aF1, TopAbs_EDGE, aME1);
    BOPTools::MapShapes(aF2, TopAbs_EDGE, aME2);
    //
    aNbPB = aME1.Extent();
    for (j = 1; j <= aNbPB; ++j) {
      const TopoDS_Shape& aE = aME1(j);
      if (aME2.Contains(aE)) {
        if (aME.Add(aE)) {
          aBB.Add(aRC, aE);
        }
      }
    }
  }
  //
  DBRep::Set(a[1], aRC);
  return 0;
}

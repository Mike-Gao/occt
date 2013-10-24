// Created on: 2002-05-21
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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


#include <QABugs.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Ax1.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <DrawTrSurf.hxx>

#include <Precision.hxx>

#include <PCollection_HAsciiString.hxx>

#include <cstdio>
#include <cmath>
#include <iostream>
#include <OSD_PerfMeter.hxx>
#include <OSD_Timer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgo_Cut.hxx>

#include <TCollection_HAsciiString.hxx>

#define QCOMPARE(val1, val2) \
  di << "Checking " #val1 " == " #val2 << \
        ((val1) == (val2) ? ": OK\n" : ": Error\n")

static Standard_Integer OCC230 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 4) {
    di << "ERROR OCC230: Usage : " << argv[0] << " TrimmedCurve Pnt2d Pnt2d" << "\n";
    return 1;
  }

  gp_Pnt2d P1, P2;
  if ( !DrawTrSurf::GetPoint2d(argv[2],P1)) {
    di << "ERROR OCC230: " << argv[2] << " is not Pnt2d" << "\n";
    return 1;
  }
  if ( !DrawTrSurf::GetPoint2d(argv[3],P2)) {
    di << "ERROR OCC230: " << argv[3] << " is not Pnt2d" << "\n";
    return 1;
  }

  GCE2d_MakeSegment MakeSegment(P1,P2);
  Handle(Geom2d_TrimmedCurve) TrimmedCurve = MakeSegment.Value();
  DrawTrSurf::Set(argv[1], TrimmedCurve);
  return 0;
}

static Standard_Integer OCC142 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  for(Standard_Integer i= 0;i <= 20;i++){
    Handle(PCollection_HAsciiString) pstr = new PCollection_HAsciiString("TEST");
    pstr->Clear();
  }
  di << "OCC142: OK" << "\n";
  return 0;
}

static Standard_Integer OCC23361 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  gp_Pnt p(0, 0, 2);
  
  gp_Trsf t1, t2;
  t1.SetRotation(gp_Ax1(p, gp_Dir(0, 1, 0)), -0.49328285294022267);
  t2.SetRotation(gp_Ax1(p, gp_Dir(0, 0, 1)), 0.87538474718473880);

  gp_Trsf tComp = t2 * t1;

  gp_Pnt p1(10, 3, 4);
  gp_Pnt p2 = p1.Transformed(tComp);
  gp_Pnt p3 = p1.Transformed(t1);
  p3.Transform(t2);

  // points must be equal
  if ( ! p2.IsEqual(p3, Precision::Confusion()) )
    di << "ERROR OCC23361: equivalent transformations does not produce equal points" << "\n";
  else 
    di << "OCC23361: OK" << "\n";

  return 0;
}

static Standard_Integer OCC23237 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  OSD_PerfMeter aPM("TestMeter",0);
  OSD_Timer aTM;
  
  // run some operation in cycle for about 2 seconds to have good values of times to compare
  int count = 0;
  printf("OSD_PerfMeter test.\nRunning Boolean operation on solids in loop.\n");
  for (; aTM.ElapsedTime() < 2.; count++)
  {
    aPM.Start();
    aTM.Start();

    // do some operation that will take considerable time compared with time or starting / stopping timers
    BRepPrimAPI_MakeBox aBox (10., 10., 10.);
    BRepPrimAPI_MakeSphere aSphere (10.);
    BRepAlgo_Cut (aBox.Shape(), aSphere.Shape());

    aTM.Stop();
    aPM.Stop();
  }
 
  int aNbEnters = 0;
  Standard_Real aPerfMeter_CPUtime = 0., aTimer_ElapsedTime = aTM.ElapsedTime();

  perf_get_meter("TestMeter", &aNbEnters, &aPerfMeter_CPUtime);

  Standard_Real aTimeDiff = (fabs(aTimer_ElapsedTime - aPerfMeter_CPUtime) / aTimer_ElapsedTime);

  printf("\nMeasurement results (%d cycles):\n", count);
  printf("\nOSD_PerfMeter CPU time: %lf\nOSD_Timer elapsed time: %lf\n", aPerfMeter_CPUtime, aTimer_ElapsedTime);
  printf("Time delta is: %.3lf %%\n", aTimeDiff * 100);

  if (aTimeDiff > 0.2)
    di << "OCC23237: Error: too much difference between CPU and elapsed times";
  else if (aNbEnters != count)
    di << "OCC23237: Error: counter reported by PerfMeter (" << aNbEnters << ") does not correspond to actual number of cycles";
  else
    di << "OCC23237: OK";

  return 0;
}

#ifdef HAVE_TBB

#include <Standard_Atomic.hxx>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

class IncrementerDecrementer
{
public:
    IncrementerDecrementer (Standard_Integer* theVal, Standard_Boolean thePositive) : myVal (theVal), myPositive (thePositive)
    {}
    void operator() (const tbb::blocked_range<size_t>& r) const
    {
        if (myPositive)
            for (size_t i = r.begin(); i != r.end(); ++i)
                Standard_Atomic_Increment (myVal);
        else
            for (size_t i = r.begin(); i != r.end(); ++i)
                Standard_Atomic_Decrement (myVal);
    }
private:
    Standard_Integer*   myVal;
    Standard_Boolean   myPositive;
};
#endif

#ifdef HAVE_TBB
static Standard_Integer OCC22980 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  int aSum = 0;

  //check returned value
  QCOMPARE (Standard_Atomic_Decrement (&aSum), -1);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 0);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 1);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 2);
//  QCOMPARE (Standard_Atomic_DecrementTest (&aSum), 0);
//  QCOMPARE (Standard_Atomic_DecrementTest (&aSum), 1);

  //check atomicity 
  aSum = 0;
  const int N = 1 << 24; //big enough to ensure concurrency

  //increment
  tbb::parallel_for (tbb::blocked_range<size_t> (0, N), IncrementerDecrementer (&aSum, true));
  QCOMPARE (aSum, N);

  //decrement
  tbb::parallel_for (tbb::blocked_range<size_t> (0, N), IncrementerDecrementer (&aSum, false));
  QCOMPARE (aSum, 0);

  return 0;
}

#else /* HAVE_TBB */

static Standard_Integer OCC22980 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char **argv)
{
  di << "Test skipped: command " << argv[0] << " requires TBB library\n";
  return 0;
}

#endif /* HAVE_TBB */

#include <TDocStd_Application.hxx>
#include <XCAFApp_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>

static Standard_Integer OCC23595 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  const Handle(TDocStd_Application)& anApp = XCAFApp_Application::GetApplication();
  Handle(TDocStd_Document) aDoc;
  anApp->NewDocument ("XmlXCAF", aDoc);
  QCOMPARE (!aDoc.IsNull(), Standard_True);

  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool (aDoc->Main());

  //check default value
  Standard_Boolean aValue = XCAFDoc_ShapeTool::AutoNaming();
  QCOMPARE (aValue, Standard_True);

  //true
  XCAFDoc_ShapeTool::SetAutoNaming (Standard_True);
  TopoDS_Shape aShape = BRepPrimAPI_MakeBox (100., 200., 300.).Shape();
  TDF_Label aLabel = aShTool->AddShape (aShape);
  Handle(TDataStd_Name) anAttr;
  QCOMPARE (aLabel.FindAttribute (TDataStd_Name::GetID(), anAttr), Standard_True);

  //false
  XCAFDoc_ShapeTool::SetAutoNaming (Standard_False);
  aShape = BRepPrimAPI_MakeBox (300., 200., 100.).Shape();
  aLabel = aShTool->AddShape (aShape);
  QCOMPARE (!aLabel.FindAttribute (TDataStd_Name::GetID(), anAttr), Standard_True);

  //restore
  XCAFDoc_ShapeTool::SetAutoNaming (aValue);

  return 0;
}

#include <ExprIntrp_GenExp.hxx>
Standard_Integer OCC22611 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{

  if (argc != 3) {
    di << "Usage : " << argv[0] << " string nb\n";
    return 1;
  }

  TCollection_AsciiString aToken = argv[1];
  Standard_Integer aNb = atoi(argv[2]);

  Handle(ExprIntrp_GenExp) aGen = ExprIntrp_GenExp::Create();
  for (Standard_Integer i=0; i < aNb; i++)
  {
    aGen->Process(aToken);
    Handle(Expr_GeneralExpression) aExpr = aGen->Expression();
  }

  return 0;
}

Standard_Integer OCC22595 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  gp_Mat M0;
  di << "M0 = "
  << " {" << M0(1,1) << "} {" << M0(1,2) << "} {" << M0(1,3) <<"}"
  << " {" << M0(2,1) << "} {" << M0(2,2) << "} {" << M0(2,3) <<"}"
  << " {" << M0(1,1) << "} {" << M0(1,2) << "} {" << M0(1,3) <<"}";
  return 0;
}

#include <TopoDS_Face.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTools.hxx>

static Standard_Boolean OCC23774Test(const TopoDS_Face& grossPlateFace, const TopoDS_Shape& originalWire, Draw_Interpretor& di)
{
  BRepExtrema_DistShapeShape distShapeShape(grossPlateFace,originalWire,Extrema_ExtFlag_MIN);
  if(!distShapeShape.IsDone()) {
    di <<"Distance ShapeShape is Not Done\n";
    return Standard_False;
  }

  if(distShapeShape.Value() > 0.01) {
    di << "Wrong Dist = " <<distShapeShape.Value() << "\n";
    return Standard_False;
  } else
    di << "Dist0 = " <<distShapeShape.Value() <<"\n";

  //////////////////////////////////////////////////////////////////////////
  /// First Flip Y
  const gp_Pnt2d axis1P1(1474.8199035519228,1249.9995745636970);
  const gp_Pnt2d axis1P2(1474.8199035519228,1250.9995745636970);

  gp_Vec2d mirrorVector1(axis1P1,axis1P2);

  gp_Trsf2d mirror1;
  mirror1.SetMirror(gp_Ax2d(axis1P1,mirrorVector1));

  BRepBuilderAPI_Transform transformer1(mirror1);
  transformer1.Perform(originalWire);
  if(!transformer1.IsDone()) {
    di << "Not Done1 " << "\n";
    return Standard_False;
  }
  const TopoDS_Shape& step1ModifiedShape = transformer1.ModifiedShape(originalWire);
  
  BRepExtrema_DistShapeShape distShapeShape1(grossPlateFace,step1ModifiedShape,Extrema_ExtFlag_MIN);
  if(!distShapeShape1.IsDone())
    return Standard_False;
  if(distShapeShape1.Value() > 0.01) {
    di << "Dist = " <<distShapeShape1.Value() <<"\n";
    return Standard_False;
  } else
    di << "Dist1 = " <<distShapeShape1.Value() <<"\n";

  //////////////////////////////////////////////////////////////////////////
  /// Second flip Y
  transformer1.Perform(step1ModifiedShape);
  if(!transformer1.IsDone()) {
    di << "Not Done1 \n";
    return Standard_False;
  }
  const TopoDS_Shape& step2ModifiedShape = transformer1.ModifiedShape(step1ModifiedShape);

  //This is identity matrix for values but for type is gp_Rotation ?!
  gp_Trsf2d mirror11 = mirror1;
  mirror11.PreMultiply(mirror1);

  BRepExtrema_DistShapeShape distShapeShape2(grossPlateFace,step2ModifiedShape);//,Extrema_ExtFlag_MIN);
  if(!distShapeShape2.IsDone())
    return Standard_False;

  //This last test case give error (the value is 1008.8822038689706)
  if(distShapeShape2.Value() > 0.01) {
    di  << "Wrong Dist2 = " <<distShapeShape2.Value() <<"\n";
    Standard_Integer N = distShapeShape2.NbSolution();
    di << "Nb = " <<N <<"\n";
    for (Standard_Integer i=1;i <= N;i++)
        di <<"Sol(" <<i<<") = " <<distShapeShape2.PointOnShape1(i).Distance(distShapeShape2.PointOnShape2(i)) <<"\n";
    return Standard_False;
  }
  di << "Distance2 = " <<distShapeShape2.Value() <<"\n";
 
  return Standard_True;
}
static Standard_Integer OCC23774(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ 

  if (n != 3) {
	di <<"OCC23774: invalid number of input parameters\n";
	return 1;
  }

  const char *ns1 = (a[1]), *ns2 = (a[2]);
  TopoDS_Shape S1(DBRep::Get(ns1)), S2(DBRep::Get(ns2));
  if (S1.IsNull() || S2.IsNull()) {
	di <<"OCC23774: Null input shapes\n";
	return 1;
  }
  const TopoDS_Face& aFace  = TopoDS::Face(S1);
  if(!OCC23774Test(aFace, S2, di))
	di << "Something is wrong\n";

 return 0;
}

#include <GeomConvert_ApproxSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Draw.hxx>
#include <OSD_Thread.hxx>
static void GeomConvertTest (Draw_Interpretor& di, Standard_Integer theTargetNbUPoles, Standard_CString theFileName)
{
	Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(theFileName);
	GeomConvert_ApproxSurface aGAS (aSurf, 1e-4, GeomAbs_C1, GeomAbs_C1, 9, 9, 100, 1);
	if (!aGAS.IsDone()) {
		di << "ApproxSurface is not done!" << "\n";
		return;
	}
	const Handle(Geom_BSplineSurface)& aBSurf = aGAS.Surface();
	if (aBSurf.IsNull()) {
		di << "BSplineSurface is not created!" << "\n";
		return;
	}
	di << "Number of UPoles:" << aBSurf->NbUPoles() << "\n";
	QCOMPARE (aBSurf->NbUPoles(), theTargetNbUPoles);
}

struct aData {
	Draw_Interpretor* di;
	Standard_Integer nbupoles;
	Standard_CString filename;
};

Standard_EXPORT Standard_Address convert(Standard_Address data)
{
	aData* info = (aData*) data;
	GeomConvertTest(*(info->di),info->nbupoles,info->filename);
	return NULL;
}

static Standard_Integer OCC23952sweep (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
	if (argc != 3) {
		di << "Usage: " << argv[0] << " invalid number of arguments" << "\n";
		return 1;
	}
	struct aData aStorage;
	aStorage.di = &di;
	aStorage.nbupoles = Draw::Atoi(argv[1]); 
	aStorage.filename = argv[2];

	OSD_Thread aThread1(convert);
	aThread1.Run(&aStorage);
	GeomConvertTest(di,aStorage.nbupoles,aStorage.filename);
	cout << "result of thread: " << aThread1.Wait() << endl;

	return 0;
}

#include <GeomInt_IntSS.hxx>
static void GeomIntSSTest (Draw_Interpretor& di, Standard_Integer theNbSol, Standard_CString theFileName1, Standard_CString theFileName2)
{
	Handle(Geom_Surface) aSurf1 = DrawTrSurf::GetSurface(theFileName1);
	Handle(Geom_Surface) aSurf2 = DrawTrSurf::GetSurface(theFileName2);
	GeomInt_IntSS anInter;
	anInter.Perform(aSurf1, aSurf2, Precision::Confusion(), Standard_True);
	if (!anInter.IsDone()) {
		di << "An intersection is not done!" << "\n";
		return;
	}

	di << "Number of Lines:" << anInter.NbLines() << "\n";
	QCOMPARE (anInter.NbLines(), theNbSol);
}

struct aNewData {
	Draw_Interpretor* di;
	Standard_Integer nbsol;
	Standard_CString filename1;
	Standard_CString filename2;
};
Standard_EXPORT Standard_Address convert_inter(Standard_Address data)
{
	aNewData* info = (aNewData*) data;
	GeomIntSSTest(*(info->di),info->nbsol,info->filename1,info->filename2);
	return NULL;
}

static Standard_Integer OCC23952intersect (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
	if (argc != 4) {
		di << "Usage: " << argv[0] << " invalid number of arguments" << "\n";
		return 1;
	}
	struct aNewData aStorage;
	aStorage.di = &di;
	aStorage.nbsol = Draw::Atoi(argv[1]); 
	aStorage.filename1 = argv[2];
	aStorage.filename2 = argv[3];

	OSD_Thread aThread1(convert_inter);
	aThread1.Run(&aStorage);
	GeomIntSSTest(di,aStorage.nbsol,aStorage.filename1,aStorage.filename2);
	cout << "result of thread: " << aThread1.Wait() << endl;

	return 0;
}

#include <Geom_SurfaceOfRevolution.hxx> 
static Standard_Integer OCC23683 (Draw_Interpretor& di, Standard_Integer argc,const char ** argv)
{
  if (argc < 2) {
    di<<"Usage: " << argv[0] << " invalid number of arguments"<<"\n";
    return 1;
  }

  Standard_Integer ucontinuity = 1;
  Standard_Integer vcontinuity = 1;
  Standard_Boolean iscnu = false;
  Standard_Boolean iscnv = false;
  
  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(argv[1]);

  QCOMPARE (aSurf->IsCNu (ucontinuity), iscnu);
  QCOMPARE (aSurf->IsCNv (vcontinuity), iscnv);

  return 0;
}

#include <gp_Ax1.hxx>
#include <gp_Ax22d.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <Geom2d_OffsetCurve.hxx>

static int test_offset(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // Check the command arguments
  if ( argc != 1 )
  {
    di << "Error: " << argv[0] << " - invalid number of arguments" << "\n";
    di << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  gp_Ax1 RotoAx( gp::Origin(), gp::DZ() );
  gp_Ax22d Ax2( gp::Origin2d(), gp::DY2d(), gp::DX2d() );
  Handle(Geom_Surface) Plane = new Geom_Plane( gp::YOZ() );

  di << "<<<< Preparing sample surface of revolution based on trimmed curve >>>>" << "\n";
  di << "-----------------------------------------------------------------------" << "\n";

  Handle(Geom2d_Circle) C2d1 = new Geom2d_Circle(Ax2, 1.0);
  Handle(Geom2d_TrimmedCurve) C2d1Trimmed = new Geom2d_TrimmedCurve(C2d1, 0.0, M_PI/2.0);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(C2d1Trimmed, Plane);

  DBRep::Set("e1", E1);

  BRepPrimAPI_MakeRevol aRevolBuilder1(E1, RotoAx);
  TopoDS_Face F1 = TopoDS::Face( aRevolBuilder1.Shape() );

  DBRep::Set("f1", F1);

  di << "Result: f1" << "\n";

  di << "<<<< Preparing sample surface of revolution based on offset curve  >>>>" << "\n";
  di << "-----------------------------------------------------------------------" << "\n";

  Handle(Geom2d_OffsetCurve) C2d2Offset = new Geom2d_OffsetCurve(C2d1Trimmed, -0.5);
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(C2d2Offset, Plane);

  DBRep::Set("e2", E2);

  BRepPrimAPI_MakeRevol aRevolBuilder2(E2, RotoAx);
  TopoDS_Face F2 = TopoDS::Face( aRevolBuilder2.Shape() );

  DBRep::Set("f2", F2);

  di << "Result: f2" << "\n";

  return 0;
}

#include <GeomAdaptor_Surface.hxx>
#include <Draw.hxx>
//=======================================================================
//function : OCC23945
//purpose  : 
//=======================================================================

static Standard_Integer OCC23945 (Draw_Interpretor& /*di*/,Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Handle(Geom_Surface) aS = DrawTrSurf::GetSurface(a[1]);
  if (aS.IsNull()) return 1;

  GeomAdaptor_Surface GS(aS);

  Standard_Real U = Draw::Atof(a[2]);
  Standard_Real V = Draw::Atof(a[3]);

  Standard_Boolean DrawPoint = ( n%3 == 2);
  if ( DrawPoint) n--;

  gp_Pnt P;
  if (n >= 13) {
    gp_Vec DU,DV;
    if (n >= 22) {
      gp_Vec D2U,D2V,D2UV;
      GS.D2(U,V,P,DU,DV,D2U,D2V,D2UV);
      Draw::Set(a[13],D2U.X());
      Draw::Set(a[14],D2U.Y());
      Draw::Set(a[15],D2U.Z());
      Draw::Set(a[16],D2V.X());
      Draw::Set(a[17],D2V.Y());
      Draw::Set(a[18],D2V.Z());
      Draw::Set(a[19],D2UV.X());
      Draw::Set(a[20],D2UV.Y());
      Draw::Set(a[21],D2UV.Z());
    }
    else
      GS.D1(U,V,P,DU,DV);

    Draw::Set(a[7],DU.X());
    Draw::Set(a[8],DU.Y());
    Draw::Set(a[9],DU.Z());
    Draw::Set(a[10],DV.X());
    Draw::Set(a[11],DV.Y());
    Draw::Set(a[12],DV.Z());
  }
  else 
    GS.D0(U,V,P);

  if ( n > 6) {
    Draw::Set(a[4],P.X());
    Draw::Set(a[5],P.Y());
    Draw::Set(a[6],P.Z());
  }
  if ( DrawPoint) {
    DrawTrSurf::Set(a[n],P);
  }

  return 0;
}

#include <Voxel_BoolDS.hxx>
#include <Voxel_FastConverter.hxx>
#include <Voxel_BooleanOperation.hxx>
static Standard_Integer OCC24019 (Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  if ( argc != 2 ) {
    di << "Error: " << argv[0] << " - invalid number of arguments" << "\n";
	return 1;
  }

  TCollection_AsciiString aFileName = argv[1];
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;

  if (!BRepTools::Read(aShape, aFileName.ToCString(), aBuilder)) {
    di << "Error: Could not read a shape!" << "\n";
	return 1;
  }
  
  TopoDS_Solid aShape1 = BRepPrimAPI_MakeSphere(gp_Pnt(20,25,35), 7);

  Standard_Real deflection = 0.005;
  Standard_Integer nbThreads = 1;
  Standard_Integer nbx = 200, nby = 200, nbz = 200;
  Voxel_BoolDS theVoxels(0,0,0, 50, 50, 50, nbx, nby, nbz);
  Voxel_BoolDS theVoxels1(0,0,0, 50, 50, 50, nbx, nby, nbz);

  Standard_Integer progress = 0;
  Voxel_FastConverter fcp(aShape, theVoxels, deflection, nbx, nby, nbz, nbThreads);
  fcp.ConvertUsingSAT(progress, 1);
  fcp.FillInVolume(1);

  Voxel_FastConverter fcp1(aShape1, theVoxels1, deflection, nbx, nby, nbz, nbThreads);
  fcp1.ConvertUsingSAT(progress, 1);
  fcp1.FillInVolume(1);

  Voxel_BooleanOperation op;
  Standard_Boolean result = op.Cut(theVoxels1, theVoxels);
  if ( result != 1 ) {
    di << "Error: invalid boolean operation" << "\n";
  } else {
	di << "OK: boolean operation is ok" << "\n";
  }

  return 0;
}

//=======================================================================
//function : OCC11758
//purpose  : 
//=======================================================================
static Standard_Integer OCC11758 (Draw_Interpretor& di, Standard_Integer n, const char**)
{
  if (n != 1) return 1;

  const char* theStr = "0123456789";
  Standard_Integer i, j;
  for ( i = 0; i < 5; ++i ) {
    // TCollection_AsciiString(const Standard_CString astring)
    TCollection_AsciiString a(theStr+i);
    // IsEqual (const Standard_CString other)const
    //assert( a == theStr+i );
    QCOMPARE ( a , theStr+i );

    //TCollection_AsciiString(const Standard_CString astring,const Standard_Integer aLen )
    TCollection_AsciiString b(theStr+i, 3);
    //assert( b.Length() == 3 );
    //assert( strncmp( b.ToCString(), theStr+i, 3 ) == 0 );
    //assert( strlen( b.ToCString() ) == 3 );
    QCOMPARE ( b.Length() , 3 );
    QCOMPARE ( strncmp( b.ToCString() , theStr+i, 3 ) , 0 );
    QCOMPARE ( b.Length() , 3 );

    //TCollection_AsciiString(const Standard_Integer aValue)
    TCollection_AsciiString c(i);
    //assert( c.IsIntegerValue() );
    //assert( c.IntegerValue() == i );
    QCOMPARE ( c.IsIntegerValue() , Standard_True );
    QCOMPARE ( c.IntegerValue() , i );

    //TCollection_AsciiString(const Standard_Real aValue)
    TCollection_AsciiString d( 0.1*i );
    //assert( d.IsRealValue() );
    //assert( TCollection_AsciiString(3.3) == "3.3");
    QCOMPARE ( d.IsRealValue() , Standard_True );
    QCOMPARE ( TCollection_AsciiString(3.3) , "3.3" );

    //TCollection_AsciiString(const TCollection_AsciiString& astring)
    TCollection_AsciiString e(d);
    //assert( e == d );
    //assert( e.Length() == d.Length() );
    //assert( strcmp( e.ToCString(), d.ToCString() ) == 0 );
    QCOMPARE ( e ,d  );
    QCOMPARE ( e.Length() , d.Length() );
    QCOMPARE ( strcmp( e.ToCString(), d.ToCString() ) , 0 );

    // TCollection_AsciiString(const TCollection_AsciiString& astring ,
    //                         const Standard_Character other )
    TCollection_AsciiString f(e,'\a');
    //assert( f.Length() == e.Length() + 1 );
    //assert( strncmp( f.ToCString(), e.ToCString(), e.Length() ) == 0 );
    //assert( f.Value( f.Length() ) == '\a');
    QCOMPARE ( f.Length() , e.Length() + 1 );
    QCOMPARE ( strncmp( f.ToCString(), e.ToCString(), e.Length() ) , 0 );
    QCOMPARE ( f.Value( f.Length() ) , '\a' );

    // TCollection_AsciiString(const TCollection_AsciiString& astring ,
    //                         const Standard_CString other )
    TCollection_AsciiString g(f, theStr);
    //assert( g.Length() == f.Length() + strlen( theStr ));
    //assert( strncmp( g.ToCString(), f.ToCString(), f.Length() ) == 0 );
    //assert( g.Search( theStr ) == f.Length() + 1 );
    QCOMPARE ( g.Length() , f.Length() + (Standard_Integer)strlen( theStr ) );
    QCOMPARE ( strncmp( g.ToCString(), f.ToCString(), f.Length() ) , 0 );
    QCOMPARE ( g.Search( theStr ) , f.Length() + 1 );

    // TCollection_AsciiString(const TCollection_AsciiString& astring ,
    //                         const TCollection_AsciiString& other )
    TCollection_AsciiString h(d,a);
    //assert( h.Length() == d.Length() + a.Length() );
    //assert( strncmp( h.ToCString(), d.ToCString(), d.Length() ) == 0 );
    //assert( strncmp( h.ToCString() + d.Length(), a.ToCString(), a.Length() ) == 0 );
    QCOMPARE ( h.Length() , d.Length() + a.Length() );
    QCOMPARE ( strncmp( h.ToCString(), d.ToCString(), d.Length() ) , 0 );
    QCOMPARE ( strncmp( h.ToCString() + d.Length(), a.ToCString(), a.Length() ) , 0 );

    // AssignCat(const Standard_CString other)
    c.AssignCat( a.ToCString() );
    //assert( c.Length() == 1 + a.Length() );
    //assert( c.Search( a ) == 2 );
    QCOMPARE ( c.Length() , 1 + a.Length() );
    QCOMPARE ( c.Search( a ) , 2 );

    // AssignCat(const TCollection_AsciiString& other)
    Standard_Integer dl = d.Length();
    d.AssignCat( a );
    //assert( d.Length() == dl + a.Length() );
    //assert( d.Search( a ) == dl + 1 );
    QCOMPARE ( d.Length() , dl + a.Length() );
    QCOMPARE ( d.Search( a ) , dl + 1 );

    // Capitalize()
    TCollection_AsciiString capitalize("aBC");
    capitalize.Capitalize();
    //assert( capitalize == "Abc" );
    QCOMPARE ( capitalize , "Abc" );

    // Copy(const Standard_CString fromwhere)
    d = theStr+i;
    //assert( d == theStr+i );
    QCOMPARE ( d , theStr+i );

    // Copy(const TCollection_AsciiString& fromwhere)
    d = h;
    // IsEqual (const TCollection_AsciiString& other)const
    //assert( d == h );
    QCOMPARE ( d , h );

    // Insert(const Standard_Integer where, const Standard_CString what)
    dl = d.Length();
    d.Insert( 2, theStr );
    //assert( d.Length() == dl + strlen( theStr ));
    //assert( strncmp( d.ToCString() + 1, theStr, strlen( theStr )) == 0 );
    QCOMPARE ( d.Length() , dl + (Standard_Integer)strlen( theStr ) );
    QCOMPARE ( strncmp( d.ToCString() + 1, theStr, strlen( theStr )) , 0 );

    //Insert(const Standard_Integer where,const Standard_Character what)
    d = theStr;
    d.Insert( i+1, 'i' );
    //assert( d.Length() == strlen( theStr ) + 1 );
    //assert( d.Value( i+1 ) == 'i');
    //assert( strcmp( d.ToCString() + i + 1, theStr+i ) == 0 );
    QCOMPARE ( d.Length() , (Standard_Integer)strlen( theStr ) + 1 );
    QCOMPARE ( d.Value( i+1 ) , 'i' );
    QCOMPARE ( strcmp( d.ToCString() + i + 1, theStr+i ) , 0 );

    //Insert(const Standard_Integer where,const TCollection_AsciiString& what)
    d = theStr;
    d.Insert( i+1, TCollection_AsciiString( "i" ));
    //assert( d.Length() == strlen( theStr ) + 1 );
    //assert( d.Value( i+1 ) == 'i');
    //assert( strcmp( d.ToCString() + i + 1, theStr+i ) == 0 );
    QCOMPARE ( d.Length() , (Standard_Integer)strlen( theStr ) + 1 );
    QCOMPARE ( d.Value( i+1 ) , 'i' );
    QCOMPARE ( strcmp( d.ToCString() + i + 1, theStr+i ) , 0 );

    // IsDifferent (const Standard_CString other)const
    //assert( d.IsDifferent( theStr ));
    //assert( d.IsDifferent( "theStr" ));
    //assert( d.IsDifferent( "" ));
    //assert( !d.IsDifferent( d.ToCString() ));
    QCOMPARE ( d.IsDifferent( theStr ) , Standard_True );
    QCOMPARE ( d.IsDifferent( "theStr" ) , Standard_True );
    QCOMPARE ( d.IsDifferent( "" ) , Standard_True );
    QCOMPARE ( !d.IsDifferent( d.ToCString() ) , Standard_True );

    // IsDifferent (const TCollection_AsciiString& other)const
    //assert( d.IsDifferent( TCollection_AsciiString() ));
    //assert( d.IsDifferent( a ));
    //assert( d.IsDifferent( h ));
    //assert( !d.IsDifferent( d ));
    QCOMPARE ( d.IsDifferent( TCollection_AsciiString() ) , Standard_True );
    QCOMPARE ( d.IsDifferent( a ) , Standard_True );
    QCOMPARE ( d.IsDifferent( h ) , Standard_True );
    QCOMPARE ( !d.IsDifferent( d ) , Standard_True );

    // IsLess (const Standard_CString other)const
    //assert( TCollection_AsciiString ("0"). IsLess("1"));
    //assert( TCollection_AsciiString ("0"). IsLess("00"));
    //assert( TCollection_AsciiString ("").  IsLess("0"));
    //assert( !TCollection_AsciiString("1"). IsLess("0"));
    //assert( !TCollection_AsciiString("00").IsLess("0"));
    //assert( !TCollection_AsciiString("0"). IsLess(""));
    //assert( TCollection_AsciiString (theStr+i).IsLess(theStr+i+1));
    QCOMPARE ( TCollection_AsciiString ("0"). IsLess("1") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("0"). IsLess("00") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("").  IsLess("0") , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("1"). IsLess("0"), Standard_True );
    QCOMPARE ( !TCollection_AsciiString("00").IsLess("0") , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("0"). IsLess("") , Standard_True );
    QCOMPARE ( TCollection_AsciiString (theStr+i).IsLess(theStr+i+1) , Standard_True );

    // IsLess (const TCollection_AsciiString& other)const
    //assert( TCollection_AsciiString ("0"). IsLess(TCollection_AsciiString("1" )));
    //assert( TCollection_AsciiString ("0"). IsLess(TCollection_AsciiString("00")));
    //assert( TCollection_AsciiString ("").  IsLess(TCollection_AsciiString("0" )));
    //assert( !TCollection_AsciiString("1"). IsLess(TCollection_AsciiString("0" )));
    //assert( !TCollection_AsciiString("00").IsLess(TCollection_AsciiString("0" )));
    //assert( !TCollection_AsciiString("0"). IsLess(TCollection_AsciiString(""  )));
    //assert( TCollection_AsciiString (theStr+i).IsLess(TCollection_AsciiString(theStr+i+1)));
    QCOMPARE ( TCollection_AsciiString ("0"). IsLess(TCollection_AsciiString("1" )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("0"). IsLess(TCollection_AsciiString("00")) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("").  IsLess(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("1"). IsLess(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("00").IsLess(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("0"). IsLess(TCollection_AsciiString(""  )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString (theStr+i).IsLess(TCollection_AsciiString(theStr+i+1)) , Standard_True );

    // IsGreater (const Standard_CString other)const
    //assert( !TCollection_AsciiString("0"). IsGreater("1"));
    //assert( !TCollection_AsciiString("0"). IsGreater("00"));
    //assert( !TCollection_AsciiString("").  IsGreater("0"));
    //assert( TCollection_AsciiString ("1"). IsGreater("0"));
    //assert( TCollection_AsciiString ("00").IsGreater("0"));
    //assert( TCollection_AsciiString ("0"). IsGreater(""));
    //assert( TCollection_AsciiString (theStr+i+1).IsGreater(theStr+i));
    QCOMPARE ( !TCollection_AsciiString("0"). IsGreater("1") , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("0"). IsGreater("00") , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("").  IsGreater("0") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("1"). IsGreater("0") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("00").IsGreater("0") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("0"). IsGreater("") , Standard_True );
    QCOMPARE ( TCollection_AsciiString (theStr+i+1).IsGreater(theStr+i) , Standard_True );

    // IsGreater (const TCollection_AsciiString& other)const
    //assert( !TCollection_AsciiString("0"). IsGreater(TCollection_AsciiString("1" )));
    //assert( !TCollection_AsciiString("0"). IsGreater(TCollection_AsciiString("00")));
    //assert( !TCollection_AsciiString("").  IsGreater(TCollection_AsciiString("0" )));
    //assert( TCollection_AsciiString ("1"). IsGreater(TCollection_AsciiString("0" )));
    //assert( TCollection_AsciiString ("00").IsGreater(TCollection_AsciiString("0" )));
    //assert( TCollection_AsciiString ("0"). IsGreater(TCollection_AsciiString(""  )));
    //assert( TCollection_AsciiString (theStr+i+1).IsGreater(TCollection_AsciiString(theStr+i)));
    QCOMPARE ( !TCollection_AsciiString("0"). IsGreater(TCollection_AsciiString("1" )) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("0"). IsGreater(TCollection_AsciiString("00")) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("").  IsGreater(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("1"). IsGreater(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("00").IsGreater(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("0"). IsGreater(TCollection_AsciiString(""  )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString (theStr+i+1).IsGreater(TCollection_AsciiString(theStr+i)) , Standard_True );

    // void Read(Standard_IStream& astream)
    std::istringstream is( theStr );
    e.Read( is );
    //assert( e == theStr );
    QCOMPARE ( e , theStr );

    // Standard_Integer SearchFromEnd (const Standard_CString what)const
    //assert( e.SearchFromEnd( theStr + i ) == i + 1 );
    QCOMPARE ( e.SearchFromEnd( theStr + i ) , i + 1 );

    // SetValue(const Standard_Integer where, const Standard_CString what)
    e.SetValue( i+1, "what");
    //assert( e.Search( "what" ) == i+1 );
    //assert( e.Length() == strlen( theStr ));
    QCOMPARE ( e.Search( "what" ) , i+1 );
    QCOMPARE ( e.Length() , (Standard_Integer)strlen( theStr ) );

    // TCollection_AsciiString Split (const Standard_Integer where)
    e = theStr;
    d = e.Split( i+1 );
    //assert( d.Length() + e.Length() == strlen( theStr ));
    QCOMPARE ( d.Length() + e.Length() , (Standard_Integer)strlen( theStr ) );

    // TCollection_AsciiString SubString (const Standard_Integer FromIndex,
    //                                    const Standard_Integer ToIndex) const
    e = theStr;
    d = e.SubString( (unsigned int)i+1, (unsigned int)i+3 );
    //assert( d.Length() == 3 );
    //assert( d.Value(1) == theStr[ i ]);
    QCOMPARE ( d.Length() , 3 );
    QCOMPARE ( d.Value(1) , theStr[ i ] );

    // TCollection_AsciiString Token (const Standard_CString separators,
    //                                const Standard_Integer whichone) const
    e = " ";
    for ( j = 0; j < i; ++j ) {
      e += TCollection_AsciiString( theStr[j] ) + " ";
      //assert( e.Token(" ", j+1 ) == TCollection_AsciiString( theStr+j, 1 ));
      QCOMPARE ( e.Token(" ", j+1 ) , TCollection_AsciiString( theStr+j, 1 ) );
    }
  }
  for ( i = 0; i < 5; ++i )
  {
    // TCollection_ExtendedString (const Standard_CString astring, 
    //                             const Standard_Boolean isMultiByte) 
    const TCollection_ExtendedString a( theStr+i );
    //assert( TCollection_AsciiString( a ) == theStr+i );
    QCOMPARE ( TCollection_AsciiString( a ) , theStr+i );

    //TCollection_ExtendedString (const Standard_ExtString astring)
    const TCollection_ExtendedString b( a.ToExtString() );
    //assert( a == b );
    QCOMPARE ( a , b );

    // TCollection_ExtendedString (const Standard_Integer      length,
    //                             const Standard_ExtCharacter filler )
    const TCollection_ExtendedString c( i, 1 );
    //assert( c.Length() == i );
    QCOMPARE ( c.Length() , i );
    if ( c.Length() > 0 ) {
      //assert( c.Value( i ) == 1 );
      QCOMPARE ( c.Value( i ) , 1 );
    }

    // TCollection_ExtendedString (const Standard_Integer aValue)
    TCollection_ExtendedString d( i );
    const TCollection_AsciiString da( d );
    //assert( da.IsIntegerValue() );
    //assert( da.IntegerValue() == i );
    QCOMPARE ( da.IsIntegerValue() , Standard_True );
    QCOMPARE (  da.IntegerValue(), i );

    // TCollection_ExtendedString (const Standard_Real aValue)
    const TCollection_ExtendedString e( 0.1 * i );
    const TCollection_AsciiString ea( e );
    //assert( ea.IsRealValue() );
    //assert( Abs( ea.RealValue() - 0.1 * i ) < 1e-10 );
    QCOMPARE ( ea.IsRealValue() , Standard_True );
    QCOMPARE ( Abs( ea.RealValue() - 0.1 * i ) < 1e-10 , Standard_True );

    // TCollection_ExtendedString (const TCollection_ExtendedString& astring)
    const TCollection_ExtendedString f(e);
    //assert( f.Length() == e.Length());
    //assert( f == e );
    QCOMPARE ( f.Length() , e.Length() );
    QCOMPARE ( f , e );

    // TCollection_ExtendedString (const TCollection_AsciiString& astring)
    const TCollection_ExtendedString g( ea );
    //assert( g.Length() == ea.Length() );
    //assert( TCollection_AsciiString( g ) == ea );
    QCOMPARE ( g.Length() , ea.Length() );
    QCOMPARE ( TCollection_AsciiString( g ) , ea );

    // AssignCat (const TCollection_ExtendedString& other)
    const TCollection_ExtendedString sep(",");
    d.AssignCat( sep );
    d.AssignCat( g );
    //assert( d.Length() == 2 + g.Length() );
    //assert( d.Token( sep.ToExtString(), 1 ) == TCollection_ExtendedString( i ));
    //assert( d.Token( sep.ToExtString(), 2 ) == g );
    QCOMPARE ( d.Length() , 2 + g.Length() );
    QCOMPARE ( d.Token( sep.ToExtString(), 1 ) , TCollection_ExtendedString( i ) );
    QCOMPARE ( d.Token( sep.ToExtString(), 2 ) , g );

    // TCollection_ExtendedString Cat (const TCollection_ExtendedString& other) const
    const TCollection_ExtendedString cat = a.Cat( sep );
    //assert( cat.Length() == a.Length() + sep.Length() );
    //assert( cat.Search( a ) == 1 );
    //assert( cat.Search( sep ) == a.Length() + 1 );
    QCOMPARE ( cat.Length() , a.Length() + sep.Length() );
    QCOMPARE ( cat.Search( a ) , 1 );
    QCOMPARE ( cat.Search( sep ) , a.Length() + 1 );

    // Copy (const TCollection_ExtendedString& fromwhere)
    d = cat;
    //assert( d.Length() == cat.Length() );
    //assert( d == cat );
    QCOMPARE ( d.Length() , cat.Length() );
    QCOMPARE ( d , cat );

    // IsEqual (const Standard_ExtString other) const
    //assert( d.IsEqual( d.ToExtString() ));
    QCOMPARE ( d.IsEqual( d.ToExtString() ) , Standard_True );

    // IsDifferent (const Standard_ExtString other ) const
    //assert( d.IsDifferent( a.ToExtString() ));
    QCOMPARE ( d.IsDifferent( a.ToExtString() ) , Standard_True );

    // IsDifferent (const TCollection_ExtendedString& other) const
    //assert( d.IsDifferent( a ));
    QCOMPARE ( d.IsDifferent( a ) , Standard_True );

    // IsLess (const Standard_ExtString other) const
    const TCollection_ExtendedString l0("0"), l1("1"), l00("00"), l, ls(theStr+i), ls1(theStr+i+1);
    //assert( l0. IsLess( l1.ToExtString() ));
    //assert( l0. IsLess( l00.ToExtString() ));
    //assert( l.  IsLess( l0.ToExtString() ));
    //assert( ! l1. IsLess( l0.ToExtString() ));
    //assert( ! l00.IsLess( l0.ToExtString() ));
    //assert( ! l0. IsLess( l.ToExtString() ));
    //assert( ls.IsLess( ls1.ToExtString() ));
    QCOMPARE ( l0. IsLess( l1.ToExtString() ) , Standard_True );
    QCOMPARE ( l0. IsLess( l00.ToExtString() ) , Standard_True );
    QCOMPARE ( l.  IsLess( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l1. IsLess( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l00.IsLess( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l0. IsLess( l.ToExtString() ) , Standard_True );
    QCOMPARE ( ls.IsLess( ls1.ToExtString() ) , Standard_True );

    // IsLess (const TCollection_ExtendedString& other) const
    //assert( l0. IsLess( l1 ));
    //assert( l0. IsLess( l00 ));
    //assert( l.  IsLess( l0 ));
    //assert( ! l1. IsLess( l0 ));
    //assert( ! l00.IsLess( l0 ));
    //assert( ! l0. IsLess( l ));
    //assert( ls.IsLess( ls1 ));
    QCOMPARE ( l0. IsLess( l1 ) , Standard_True );
    QCOMPARE ( l0. IsLess( l00 ) , Standard_True );
    QCOMPARE ( l.  IsLess( l0 ) , Standard_True );
    QCOMPARE ( ! l1. IsLess( l0 ) , Standard_True );
    QCOMPARE ( ! l00.IsLess( l0 ) , Standard_True );
    QCOMPARE ( ! l0. IsLess( l ) , Standard_True );
    QCOMPARE ( ls.IsLess( ls1 ) , Standard_True );

    // IsGreater (const Standard_ExtString other) const
    //assert( ! l0.IsGreater( l1.ToExtString() ));
    //assert( ! l0.IsGreater( l00.ToExtString() ));
    //assert( ! l. IsGreater( l0.ToExtString() ));
    //assert(  l1. IsGreater( l0.ToExtString() ));
    //assert(  l00.IsGreater( l0.ToExtString() ));
    //assert(  l0. IsGreater( l.ToExtString() ));
    //assert(  ls1.IsGreater( ls.ToExtString() ));
    QCOMPARE ( ! l0.IsGreater( l1.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l0.IsGreater( l00.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l. IsGreater( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( l1. IsGreater( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( l00.IsGreater( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( l0. IsGreater( l.ToExtString() ) , Standard_True );
    QCOMPARE ( ls1.IsGreater( ls.ToExtString() ) ,Standard_True  );

    // IsGreater (const TCollection_ExtendedString& other) const
    //assert( ! l0.IsGreater( l1));
    //assert( ! l0.IsGreater( l00));
    //assert( ! l. IsGreater( l0));
    //assert(  l1. IsGreater( l0));
    //assert(  l00.IsGreater( l0));
    //assert(  l0. IsGreater( l));
    //assert(  ls1.IsGreater( ls));
    QCOMPARE ( ! l0.IsGreater( l1) , Standard_True );
    QCOMPARE ( ! l0.IsGreater( l00) , Standard_True );
    QCOMPARE ( ! l. IsGreater( l0) , Standard_True );
    QCOMPARE ( l1. IsGreater( l0) , Standard_True );
    QCOMPARE ( l00.IsGreater( l0) , Standard_True );
    QCOMPARE ( l0. IsGreater( l) , Standard_True );
    QCOMPARE ( ls1.IsGreater( ls) , Standard_True );

    // ==========================
    //TCollection_HAsciiString::
    // ==========================

    // IsDifferent(const Handle(TCollection_HAsciiString)& S)
    Handle(TCollection_HAsciiString) ha1 = new TCollection_HAsciiString( theStr+i );
    Handle(TCollection_HAsciiString) ha2 = new TCollection_HAsciiString( theStr+i+1 );
    //assert( ha1->IsDifferent( ha2 ));
    //assert( !ha1->IsDifferent( ha1 ));
    QCOMPARE ( ha1->IsDifferent( ha2 ) , Standard_True );
    QCOMPARE ( !ha1->IsDifferent( ha1 ) , Standard_True );

    // IsSameString (const Handle(TCollection_HAsciiString)& S)
    //assert( !ha1->IsSameString( ha2 ));
    //assert( ha1->IsSameString( ha1 ));
    QCOMPARE ( !ha1->IsSameString( ha2 ) , Standard_True );
    QCOMPARE ( ha1->IsSameString( ha1 ) , Standard_True );

    // IsSameState (const Handle(TCollection_HAsciiString)& other) const
    //assert( !ha1->IsSameState( ha2 ));
    //assert( ha1->IsSameState( ha1 ));
    QCOMPARE ( !ha1->IsSameState( ha2 ) , Standard_True );
    QCOMPARE ( ha1->IsSameState( ha1 ) , Standard_True );

    // IsSameString (const Handle(TCollection_HAsciiString)& S ,
    //               const Standard_Boolean CaseSensitive) const
    //assert( !ha1->IsSameString( ha2, true ));
    //assert( ha1->IsSameString( ha1, true ));
    //assert( !ha1->IsSameString( ha2, false ));
    //assert( ha1->IsSameString( ha1, false ));
    QCOMPARE ( !ha1->IsSameString( ha2, Standard_True ) , Standard_True );
    QCOMPARE ( ha1->IsSameString( ha1, Standard_True ) , Standard_True );
    QCOMPARE ( !ha1->IsSameString( ha2, Standard_False ) , Standard_True );
    QCOMPARE ( ha1->IsSameString( ha1, Standard_False ) , Standard_True );

    ha1->SetValue( 1, "AbC0000000");
    ha2->SetValue( 1, "aBc0000000");
    //assert( !ha1->IsSameString( ha2, true ));
    //assert( ha1->IsSameString( ha2, false ));
    QCOMPARE ( !ha1->IsSameString( ha2, Standard_True ) , Standard_True );
    QCOMPARE (  ha1->IsSameString( ha2, Standard_False ), Standard_True );
  }
  return 0;
}

#include <Geom_CylindricalSurface.hxx>
#include <IntTools_FaceFace.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_PntOn2Faces.hxx>

static Standard_Integer OCC24005 (Draw_Interpretor& theDI, Standard_Integer theNArg, const char** theArgv) 
{
  if(theNArg < 2)
  {
    theDI << "Wrong a number of arguments!\n";
    return 1;
  }

  Handle_Geom_Plane plane(new Geom_Plane(
                                  gp_Ax3( gp_Pnt(-72.948737453424499, 754.30437716359393, 259.52151854671678),
                                  gp_Dir(6.2471473085930200e-007, -0.99999999999980493, 0.00000000000000000),
                                  gp_Dir(0.99999999999980493, 6.2471473085930200e-007, 0.00000000000000000))));
  Handle(Geom_CylindricalSurface) cylinder(
                  new Geom_CylindricalSurface(
                                  gp_Ax3(gp_Pnt(-6.4812490053250649, 753.39408794522092, 279.16400974257465),
                                  gp_Dir(1.0000000000000000, 0.0, 0.00000000000000000),
                                  gp_Dir(0.0, 1.0000000000000000, 0.00000000000000000)),
                                                                                          19.712534607908712));

  DrawTrSurf::Set("pln", plane);
  theDI << "pln\n";
  DrawTrSurf::Set("cyl", cylinder);
  theDI << "cyl\n";

  BRep_Builder builder;
  TopoDS_Face face1, face2;
  builder.MakeFace(face1, plane, Precision::Confusion());
  builder.MakeFace(face2, cylinder, Precision::Confusion());
  IntTools_FaceFace anInters;
  anInters.SetParameters(false, true, true, Precision::Confusion());
  anInters.Perform(face1, face2);

  if (!anInters.IsDone())
  {
    theDI<<"No intersections found!"<<"\n";

    return 1;
  }

  //Handle(Geom_Curve) aResult;
  //gp_Pnt             aPoint;

  const IntTools_SequenceOfCurves& aCvsX=anInters.Lines();
  const IntTools_SequenceOfPntOn2Faces& aPntsX=anInters.Points();

  char buf[1024];  
  Standard_Integer aNbCurves, aNbPoints;

  aNbCurves=aCvsX.Length();
  aNbPoints=aPntsX.Length();

  if (aNbCurves >= 2)
  {
    for (Standard_Integer i=1; i<=aNbCurves; ++i)
    {
      Sprintf(buf, "%s_%d",theArgv[1],i);
      theDI << buf << " ";
      
      const IntTools_Curve& aIC = aCvsX(i);
      const Handle(Geom_Curve)& aC3D= aIC.Curve();
      DrawTrSurf::Set(buf,aC3D);
    }
  }
  else if (aNbCurves == 1)
  {
    const IntTools_Curve& aIC = aCvsX(1);
    const Handle(Geom_Curve)& aC3D= aIC.Curve();
    Sprintf(buf, "%s",theArgv[1]);
    theDI << buf << " ";
    DrawTrSurf::Set(buf,aC3D);
  }

  for (Standard_Integer i = 1; i<=aNbPoints; ++i)
  {
    const IntTools_PntOn2Faces& aPi=aPntsX(i);
    const gp_Pnt& aP=aPi.P1().Pnt();
    
    Sprintf(buf,"%s_p_%d",theArgv[1],i);
    theDI << buf << " ";
    DrawTrSurf::Set(buf, aP);
  }

  return 0;
}

#include <Extrema_FuncExtPS.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
static Standard_Integer OCC24137 (Draw_Interpretor& theDI, Standard_Integer theNArg, const char** theArgv) 
{
  Standard_Integer anArgIter = 1;
  if (theNArg < 5)
    {
      theDI <<"Usage: " << theArgv[0] << " face vertex U V [N]"<<"\n";
      return 1;
    }

  // get target shape
  Standard_CString aFaceName = theArgv[anArgIter++];
  Standard_CString aVertName = theArgv[anArgIter++];
  const TopoDS_Shape aShapeF = DBRep::Get (aFaceName);
  const TopoDS_Shape aShapeV = DBRep::Get (aVertName);
  const Standard_Real aUFrom = Atof (theArgv[anArgIter++]);
  const Standard_Real aVFrom = Atof (theArgv[anArgIter++]);
  const Standard_Integer aNbIts = (anArgIter < theNArg) ? atol (theArgv[anArgIter++]) : 100;
  if (aShapeF.IsNull() || aShapeF.ShapeType() != TopAbs_FACE)
    {
      std::cout << "Error: " << aFaceName << " shape is null / not a face" << std::endl;
      return 1;
    }
  if (aShapeV.IsNull() || aShapeV.ShapeType() != TopAbs_VERTEX)
    {
      std::cout << "Error: " << aVertName << " shape is null / not a vertex" << std::endl;
      return 1;
    }
  const TopoDS_Face   aFace = TopoDS::Face   (aShapeF);
  const TopoDS_Vertex aVert = TopoDS::Vertex (aShapeV);
  GeomAdaptor_Surface aSurf (BRep_Tool::Surface (aFace));

  gp_Pnt aPnt = BRep_Tool::Pnt (aVert), aRes;

  Extrema_FuncExtPS    anExtFunc;
  math_FunctionSetRoot aRoot (anExtFunc, aNbIts);

  math_Vector aTolUV (1, 2), aUVinf  (1, 2), aUVsup  (1, 2), aFromUV (1, 2);
  aTolUV (1) =  Precision::Confusion(); aTolUV (2) =  Precision::Confusion();
  aUVinf (1) = -Precision::Infinite();  aUVinf (2) = -Precision::Infinite();
  aUVsup (1) =  Precision::Infinite();  aUVsup (2) =  Precision::Infinite();
  aFromUV(1) =  aUFrom; aFromUV(2) = aVFrom;

  anExtFunc.Initialize (aSurf);
  anExtFunc.SetPoint (aPnt);
  aRoot.SetTolerance (aTolUV);
  aRoot.Perform (anExtFunc, aFromUV, aUVinf, aUVsup);
  if (!aRoot.IsDone())
    {
      std::cerr << "No results!\n";
      return 1;
    }

  theDI << aRoot.Root()(1) << " " << aRoot.Root()(2) << "\n";
  
  aSurf.D0 (aRoot.Root()(1), aRoot.Root()(2), aRes);
  DBRep::Set ("result", BRepBuilderAPI_MakeVertex (aRes));
  return 0;
}

void QABugs::Commands_19(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("OCC230", "OCC230 TrimmedCurve Pnt2d Pnt2d", __FILE__, OCC230, group);
  theCommands.Add ("OCC142", "OCC142", __FILE__, OCC142, group);
  theCommands.Add ("OCC23361", "OCC23361", __FILE__, OCC23361, group);
  theCommands.Add ("OCC23237", "OCC23237", __FILE__, OCC23237, group); 
  theCommands.Add ("OCC22980", "OCC22980", __FILE__, OCC22980, group);
  theCommands.Add ("OCC23595", "OCC23595", __FILE__, OCC23595, group);
  theCommands.Add ("OCC22611", "OCC22611 string nb", __FILE__, OCC22611, group);
  theCommands.Add ("OCC22595", "OCC22595", __FILE__, OCC22595, group);
  theCommands.Add ("OCC23774", "OCC23774 shape1 shape2", __FILE__, OCC23774, group);
  theCommands.Add ("OCC23683", "OCC23683 shape", __FILE__, OCC23683, group);
  theCommands.Add ("OCC23952sweep", "OCC23952sweep nbupoles shape", __FILE__, OCC23952sweep, group);
  theCommands.Add ("OCC23952intersect", "OCC23952intersect nbsol shape1 shape2", __FILE__, OCC23952intersect, group);
  theCommands.Add ("test_offset", "test_offset", __FILE__, test_offset, group);
  theCommands.Add("OCC23945", "OCC23945 surfname U V X Y Z [DUX DUY DUZ DVX DVY DVZ [D2UX D2UY D2UZ D2VX D2VY D2VZ D2UVX D2UVY D2UVZ]]", __FILE__, OCC23945,group);
  theCommands.Add ("OCC24019", "OCC24019 aShape", __FILE__, OCC24019, group);
  theCommands.Add ("OCC11758", "OCC11758", __FILE__, OCC11758, group);
  theCommands.Add ("OCC24005", "OCC24005 result", __FILE__, OCC24005, group);
  theCommands.Add ("OCC24137", "OCC24137 face vertex U V [N]", __FILE__, OCC24137, group);
  return;
}

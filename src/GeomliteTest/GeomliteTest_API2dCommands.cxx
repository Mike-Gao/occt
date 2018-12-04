// Created on: 1995-01-11
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// modified : pmn 11/04/97 : mis dans GeomliteTest


#include <GeomliteTest.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomLib_Tool.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>
#include <Geom2d_Circle.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Geom2dAdaptor_GHCurve.hxx>
#include <memory>

#include <stdio.h>
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
//function : proj
//purpose  : 
//=======================================================================

static Standard_Integer proj (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;

  gp_Pnt2d P(Draw::Atof(a[2]),Draw::Atof(a[3]));

  char name[100];

  Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(a[1]);

  if (GC.IsNull())
    return 1;

  Geom2dAPI_ProjectPointOnCurve proj(P,GC,GC->FirstParameter(),
				          GC->LastParameter());
  
  for (Standard_Integer i = 1; i <= proj.NbPoints(); i++)
  {
    gp_Pnt2d aP1 = proj.Point(i);
    const Standard_Real aDist = P.Distance(aP1);
    Sprintf(name, "%s%d", "ext_", i);

    if (aDist > Precision::PConfusion())
    {
      Handle(Geom2d_Line) L = new Geom2d_Line(P, gp_Dir2d(aP1.XY() - P.XY()));
      Handle(Geom2d_TrimmedCurve) CT = new Geom2d_TrimmedCurve(L, 0., aDist);
      DrawTrSurf::Set(name, CT);
    }
    else
    {
      DrawTrSurf::Set(name, aP1);
    }

    di << name << " ";
  }

  return 0;
}

//=======================================================================
//function : appro
//purpose  : 
//=======================================================================

static Standard_Integer appro(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  // Approximation et interpolation 2d

  // 2dappro
  //     - affiche la tolerance
  // 2dappro tol
  //     - change la tolerance
  // 2dappro result nbpoint 
  //     - saisie interactive
  // 2dappro result nbpoint curve 
  //     - calcule des points sur la courbe
  // 2dappro result nbpoint x1 y1 x2 y2 .. 
  //     - tableau de points
  // 2dappro result nbpoint x1 dx y1 y2 ..
  //     - tableau de points (x1,y1) (x1+dx,y2) ... avec x = t
  

  static Standard_Real Tol2d = 1.e-6;

  if (n < 3) {
    if (n == 2) 
      Tol2d = Draw::Atof(a[1]);
    
    di << "Tolerance for 2d approx : "<< Tol2d << "\n";
    return 0;
  }


  Standard_Integer i, Nb = Draw::Atoi(a[2]);
  
  Standard_Boolean hasPoints = Standard_True;
  TColgp_Array1OfPnt2d Points(1, Nb);
  TColStd_Array1OfReal YValues(1,Nb);
  Standard_Real X0=0,DX=0;
  
  Handle(Draw_Marker2D) mark;
  
  if (n == 3)  {
    // saisie interactive
    Standard_Integer id,XX,YY,b;
    dout.Select(id,XX,YY,b);
    Standard_Real zoom = dout.Zoom(id);

    Points(1) = gp_Pnt2d( ((Standard_Real)XX)/zoom, 
		          ((Standard_Real)YY)/zoom );
    
    mark = new Draw_Marker2D( Points(1), Draw_X, Draw_vert); 
    
    dout << mark;
    
    for (i = 2; i<=Nb; i++) {
      dout.Select(id,XX,YY,b);
      Points(i) = gp_Pnt2d( ((Standard_Real)XX)/zoom, 
			    ((Standard_Real)YY)/zoom );
      mark = new Draw_Marker2D( Points(i), Draw_X, Draw_vert); 
      dout << mark;
    }
  }    
  else {
    if ( n == 4) {
    // points sur courbe
      Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(a[3]);
      if ( GC.IsNull()) 
	return 1;

      Standard_Real U, U1, U2;
      U1 = GC->FirstParameter();
      U2 = GC->LastParameter();
      Standard_Real Delta = ( U2 - U1) / (Nb-1);
      for ( i = 1 ; i <= Nb; i++) {
	U = U1 + (i-1) * Delta;
	Points(i) = GC->Value(U);
      }
    }

    else {
      // test points ou ordonnees
      hasPoints = Standard_False;
      Standard_Integer nc = n - 3;
      if (nc == 2 * Nb) {
	// points
	nc = 3;
	for (i = 1; i <= Nb; i++) {
	  Points(i).SetCoord(Draw::Atof(a[nc]),Draw::Atof(a[nc+1]));
	  nc += 2;
	}
      }
      else if (nc - 2 == Nb) {
	// YValues
	nc = 5;
	X0 = Draw::Atof(a[3]);
	DX = Draw::Atof(a[4]);
      	for (i = 1; i <= Nb; i++) {
	  YValues(i) = Draw::Atof(a[nc]);
	  Points(i).SetCoord(X0+(i-1)*DX,YValues(i));
	  nc++;
	}
      }
      else
	return 1;
    }
    // display the points
    for ( i = 1 ; i <= Nb; i++) {
      mark = new Draw_Marker2D( Points(i), Draw_X, Draw_vert); 
      dout << mark;
    }
  }
  dout.Flush();
  Standard_Integer Dmin = 3;
  Standard_Integer Dmax = 8;
  
  Handle(Geom2d_BSplineCurve) TheCurve;
  if (hasPoints)
    TheCurve = Geom2dAPI_PointsToBSpline(Points,Dmin,Dmax,GeomAbs_C2,Tol2d);
  else
    TheCurve = Geom2dAPI_PointsToBSpline(YValues,X0,DX,Dmin,Dmax,GeomAbs_C2,Tol2d);
  
  DrawTrSurf::Set(a[1], TheCurve);
  di << a[1];

  return 0;

}

//=======================================================================
//function : extrema
//purpose  : 
//=======================================================================

static Standard_Integer extrema(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n<3) return 1;

  Handle(Geom2d_Curve)   GC1, GC2;

  Standard_Real U1f,U1l,U2f,U2l;

  GC1 = DrawTrSurf::GetCurve2d(a[1]);
  if ( GC1.IsNull())
    return 1;
  U1f = GC1->FirstParameter();
  U1l = GC1->LastParameter();

  GC2 = DrawTrSurf::GetCurve2d(a[2]);
  if ( GC2.IsNull())
    return 1;
  U2f = GC2->FirstParameter();
  U2l = GC2->LastParameter();

  char name[100];

  Geom2dAPI_ExtremaCurveCurve Ex(GC1,GC2,U1f,U1l,U2f,U2l);
  Standard_Boolean isInfinitySolutions = Ex.Extrema().IsParallel();
  const Standard_Integer aNExtr = Ex.NbExtrema();

  if (aNExtr == 0 || isInfinitySolutions)
  {
    // Infinity solutions flag may be set with 0 number of 
    // solutions in analytic extrema Curve/Curve.
    if (isInfinitySolutions) 
      di << "Infinite number of extremas, distance = " << Ex.LowerDistance() << "\n";
    else
      di << "No solutions!\n";
  }

  for (Standard_Integer i = 1; i <= aNExtr; i++)
  {
    gp_Pnt2d P1,P2;
    Ex.Points(i,P1,P2);
    di << "dist " << i << ": " << Ex.Distance(i) << "  ";
    if (Ex.Distance(i) <= Precision::PConfusion())
    {
      Handle(Draw_Marker2D) mark = new Draw_Marker2D( P1, Draw_X, Draw_vert); 
      dout << mark;
      dout.Flush();
      Sprintf(name,"%s%d","ext_",i);
      char* temp = name;
      DrawTrSurf::Set(temp, P1);
      di << name << "\n";
    }
    else
    {
      Handle(Geom2d_Line) L = new Geom2d_Line(P1,gp_Vec2d(P1,P2));
      Handle(Geom2d_TrimmedCurve) CT = new Geom2d_TrimmedCurve(L, 0., P1.Distance(P2));
      Sprintf(name,"%s%d","ext_",i);
      char* temp = name; // portage WNT
      DrawTrSurf::Set(temp, CT);
      di << name << "\n";
    }
  }

  return 0;
}

//=======================================================================
//function : intersect
//purpose  : 
//=======================================================================
static Standard_Integer intersect(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  Handle(Geom2d_Curve) C1 = DrawTrSurf::GetCurve2d(a[1]);
  if (C1.IsNull())
  {
    di << "Curve " << a[1] << " is null\n";
    return 1;
  }

  Handle(Geom2d_Curve) C2;
  Standard_Real Tol = 0.001;
  Standard_Boolean bPrintState = Standard_False;

  // Retrieve other parameters if any
  for (Standard_Integer i = 2; i < n; ++i)
  {
    if (!strcmp(a[i], "-tol"))
    {
      Tol = Draw::Atof(a[++i]);
    }
    else if (!strcmp(a[i], "-state"))
    {
      bPrintState = Standard_True;
    }
    else
    {
      C2 = DrawTrSurf::GetCurve2d(a[i]);
      if (C2.IsNull())
      {
        di << "Curve " << a[i] << " is null\n";
        return 1;
      }
    }
  }

  Geom2dAPI_InterCurveCurve Intersector;

  if (!C2.IsNull())
    // Curves intersection
    Intersector.Init(C1, C2, Tol);
  else
    // Self-intersection of the curve
    Intersector.Init(C1, Tol);

  const Geom2dInt_GInter& anIntTool = Intersector.Intersector();
  if (!anIntTool.IsDone())
  {
    di << "Intersection failed\n";
    return 0;
  }

  if (anIntTool.IsEmpty())
    return 0;

  Standard_Integer aNbPoints = Intersector.NbPoints();
  for (Standard_Integer i = 1; i <= aNbPoints; i++)
  {
    // API simplified result
    gp_Pnt2d P = Intersector.Point(i);
    di << "Intersection point " << i << " : " << P.X() << " " << P.Y() << "\n";
    // Intersection extended results from intersection tool
    const IntRes2d_IntersectionPoint& aPInt = anIntTool.Point(i);
    di << "parameter on the fist: " << aPInt.ParamOnFirst();
    di << " parameter on the second: " << aPInt.ParamOnSecond() << "\n";
    if (bPrintState)
    {
      di << "Intersection type: " <<
        (aPInt.TransitionOfFirst().IsTangent() ? "TOUCH" : "INTERSECTION") << "\n";
    }
    Handle(Draw_Marker2D) mark = new Draw_Marker2D(P, Draw_X, Draw_vert);
    dout << mark;
  }
  dout.Flush();

  Handle(Geom2d_Curve) S1, S2;
  Handle(DrawTrSurf_Curve2d) CD;
  Standard_Integer aNbSegments = Intersector.NbSegments();
  for (Standard_Integer i = 1; i <= aNbSegments; i++)
  {
    di << "Segment #" << i << " found.\n";
    Intersector.Segment(i,S1,S2);
    CD = new DrawTrSurf_Curve2d(S1, Draw_bleu, 30);
    dout << CD;
    CD = new DrawTrSurf_Curve2d(S2, Draw_violet, 30);
    dout << CD;
  }
  
  dout.Flush();

  return 0;
}

//=======================================================================
//function : intersect_ana
//purpose  : 
//=======================================================================

static Standard_Integer intersect_ana(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2)
  {
    cout << "2dintana circle circle " << endl;
    return 1;
  }

  Handle(Geom2d_Curve) C1 = DrawTrSurf::GetCurve2d(a[1]);
  if (C1.IsNull() && !C1->IsKind(STANDARD_TYPE(Geom2d_Circle)))
    return 1;

  Handle(Geom2d_Curve) C2 = DrawTrSurf::GetCurve2d(a[2]);
  if (C2.IsNull() && !C2->IsKind(STANDARD_TYPE(Geom2d_Circle)))
    return 1;

  Handle(Geom2d_Circle) aCir1 = Handle(Geom2d_Circle)::DownCast(C1);
  Handle(Geom2d_Circle) aCir2 = Handle(Geom2d_Circle)::DownCast(C2);

  IntAna2d_AnaIntersection Intersector(aCir1->Circ2d(), aCir2->Circ2d());

  Standard_Integer i;

  for (i = 1; i <= Intersector.NbPoints(); i++) {
    gp_Pnt2d P = Intersector.Point(i).Value();
    di << "Intersection point " << i << " : " << P.X() << " " << P.Y() << "\n";
    di << "parameter on the fist: " << Intersector.Point(i).ParamOnFirst();
    di << " parameter on the second: " << Intersector.Point(i).ParamOnSecond() << "\n";
    Handle(Draw_Marker2D) mark = new Draw_Marker2D(P, Draw_X, Draw_vert);
    dout << mark;
  }
  dout.Flush();

  return 0;
}

//=======================================================================
//function : intconcon
//purpose  : 
//=======================================================================

static Standard_Integer intconcon(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if( n < 2) 
  {
    cout<< "intconcon con1 con2 "<<endl;
    return 1;
  }
  
  Handle(Geom2d_Curve) C1 = DrawTrSurf::GetCurve2d(a[1]);
  if (C1.IsNull())
  {
    cout << a[1] << " is Null " << endl;
    return 1;
  }

  Handle(Geom2d_Curve) C2 = DrawTrSurf::GetCurve2d(a[2]);
  if (C2.IsNull())
  {
    cout << a[2] << " is Null " << endl;
    return 1;
  }

  Geom2dAdaptor_Curve AC1(C1), AC2(C2);
  GeomAbs_CurveType T1 = AC1.GetType(), T2 = AC2.GetType();
#if (defined(_MSC_VER) && (_MSC_VER < 1600))
  std::auto_ptr<IntAna2d_Conic> pCon;
#else
  std::unique_ptr<IntAna2d_Conic> pCon;
#endif  
  switch (T2)
  {
  case GeomAbs_Line:
  {
    pCon.reset(new IntAna2d_Conic(AC2.Line()));
    break;
  }
  case GeomAbs_Circle:
  {
    pCon.reset(new IntAna2d_Conic(AC2.Circle()));
    break;
  }
  case GeomAbs_Ellipse:
  {
    pCon.reset(new IntAna2d_Conic(AC2.Ellipse()));
    break;
  }
  case GeomAbs_Hyperbola:
  {
    pCon.reset(new IntAna2d_Conic(AC2.Hyperbola()));
    break;
  }
  case GeomAbs_Parabola:
  {
    pCon.reset(new IntAna2d_Conic(AC2.Parabola()));
    break;
  }
  default:
    cout << a[2] << " is not conic " << endl;
    return 1;
  }

  IntAna2d_AnaIntersection Intersector;
  switch (T1)
  {
  case GeomAbs_Line:
    Intersector.Perform(AC1.Line(), *pCon);
    break;
  case GeomAbs_Circle:
    Intersector.Perform(AC1.Circle(), *pCon);
    break;
  case GeomAbs_Ellipse:
    Intersector.Perform(AC1.Ellipse(), *pCon);
    break;
  case GeomAbs_Hyperbola:
    Intersector.Perform(AC1.Hyperbola(), *pCon);
    break;
  case GeomAbs_Parabola:
    Intersector.Perform(AC1.Parabola(), *pCon);
    break;
  default:
    cout << a[1] << " is not conic " << endl;
    return 1;
  }
  
  Standard_Integer i;
  for ( i = 1; i <= Intersector.NbPoints(); i++) {
    gp_Pnt2d P = Intersector.Point(i).Value();
    di<<"Intersection point "<<i<<" : "<<P.X()<<" "<<P.Y()<<"\n";
    di << "parameter on the fist: " << Intersector.Point(i).ParamOnFirst();
    if (!Intersector.Point(i).SecondIsImplicit())
    {
      di << " parameter on the second: " << Intersector.Point(i).ParamOnSecond() << "\n";
    }
    else
    {
      di << "\n";
    }
    Handle(Draw_Marker2D) mark = new Draw_Marker2D( P, Draw_X, Draw_vert); 
    dout << mark;
  }
  dout.Flush();

  return 0;
}

//=======================================================================
//function : deviation
//purpose  : 
//=======================================================================
static Standard_Integer deviation(Draw_Interpretor& theDI, Standard_Integer theNArg, const char** theArgv)
{
  if (theNArg < 3)
  {
    theDI << "Use: deviation2d result curve [-i U0] [-d N]\n"
             "\"-i\" sets an initial parameter for computation by iterative method;\n"
             "\"-d\" sets number of sub-intervals for searching. Default value is 2.";
    return 1;
  }

  const Handle(Geom2d_Curve) aC = DrawTrSurf::GetCurve2d(theArgv[2]);

  if (aC.IsNull())
  {
    theDI << "Error: " << theArgv[2] << " is not a 2D-curve.\n";
    return 1;
  }

  Geom2dAdaptor_Curve anAC(aC);

  Standard_Integer aNbInterv = 2;
  Standard_Real aU0 = RealLast();

  for (Standard_Integer aCurrArg = 3; aCurrArg < theNArg; aCurrArg++)
  {
    if (!strcmp(theArgv[aCurrArg], "-i"))
    {
      aU0 = Draw::Atof(theArgv[++aCurrArg]);
    }
    else if (!strcmp(theArgv[aCurrArg], "-d"))
    {
      aNbInterv = Draw::Atoi(theArgv[++aCurrArg]);
    }
    else
    {
      theDI << "Error: Wrong option " << theArgv[aCurrArg] << "\n";
      return 1;
    }
  }

  const Standard_Real aU1 = anAC.FirstParameter();
  const Standard_Real aU2 = anAC.LastParameter();
  
  Standard_Real aRetCurvParam = aU0;
  gp_Pnt2d aPtOnCurv;
  gp_Vec2d aRetVec;
  gp_Lin2d aLinSegm;

  Standard_Real aDefl = RealLast();

  if (aU0 == RealLast())
  {
    aDefl = GeomLib_Tool::ComputeDevation(anAC, aU1, aU2,
                                          aNbInterv, 10, &aU0);

    if (aDefl < 0.0)
    {
      theDI << "Error: Cannot compute deviation on interval.\n";
      return 0;
    }
  }

  aDefl = GeomLib_Tool::ComputeDevation(anAC, aU1, aU2, aU0, 100,
                                        &aRetCurvParam, &aPtOnCurv,
                                        &aRetVec, &aLinSegm);

  if (aDefl < 0.0)
  {
    theDI << "Error: Cannot compute a deviation!\n";
    return 0;
  }

  char aBuff[1000];
  theDI << "Computed value is: " << aDefl << "\n";

  Sprintf(aBuff, "%s_pnt", theArgv[1]);
  DrawTrSurf::Set(aBuff, aPtOnCurv);
  theDI << "From point " << aBuff << " (with parameter " << aRetCurvParam << ") to ";
  Sprintf(aBuff, "%s_lin", theArgv[1]);

  Handle(Geom2d_Curve) aLine = new Geom2d_Line(aLinSegm);
  DrawTrSurf::Set(aBuff, aLine);
  theDI << "the line " << aBuff << ".\n";
  Sprintf(aBuff, "%s_norm", theArgv[1]);
  aLine = new Geom2d_Line(aPtOnCurv, aRetVec);
  aLine = new Geom2d_TrimmedCurve(aLine, 0.0, aDefl);
  DrawTrSurf::Set(aBuff, aLine);
  theDI << "The deflection is measured along the line " << aBuff << ".\n";

  return 0;
}

void GeomliteTest::API2dCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;

  const char *g;

  done = Standard_True;
  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("2dproj", "proj curve x y",__FILE__, proj,g);

  g = "GEOMETRY approximations";

  theCommands.Add("2dapprox", "2dapprox result nbpoint [curve] [[x] y [x] y...]",__FILE__, 
		  appro,g);
  theCommands.Add("2dinterpole", "2dinterpole result nbpoint [curve] [[x] y [x] y ...]",__FILE__, 
		  appro,g);

  g = "GEOMETRY curves and surfaces analysis";

  theCommands.Add("2dextrema", "extrema curve curve",__FILE__,
		  extrema,g);

  g = "GEOMETRY intersections";

  theCommands.Add("2dintersect", "2dintersect curve1 [curve2] [-tol tol] [-state]\n"
                   "Intersects the given 2d curve(s)."
                   "If only one curve is given, it will be checked on self-intersection.\n"
                   "Options:\n"
                   " -tol - allows changing the intersection tolerance (default value is 1.e-3);\n"
                   " -state - allows printing the intersection state for each point.",
                   __FILE__, intersect, g);

  theCommands.Add("2dintanalytical", "intersect circle1 and circle2 using IntAna",__FILE__,
		  intersect_ana,g);
  theCommands.Add("intconcon", "intersect conic curve1 and conic curve2 using IntAna", __FILE__,
    intconcon, g);

  theCommands.Add("2ddeviation", "deviation2d result curve [-i U0] [-d N]\n"
                  __FILE__, deviation, g);

}

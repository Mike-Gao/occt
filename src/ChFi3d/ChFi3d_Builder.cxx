// Created on: 1993-11-18
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <AppBlend_Approx.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepLib.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <GCE2d_MakeLine.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <ShapeFix.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
//#include <BRepAlgoAPI_Fuse.hxx>
#include <BOPAlgo_Builder.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <BRepTools_ReShape.hxx>
#include <BRepTools.hxx>

#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>



// variables for performances 


OSD_Chronometer cl_total,cl_extent,cl_perfsetofsurf,cl_perffilletonvertex,
cl_filds,cl_reconstruction,cl_setregul,cl_perform1corner,cl_perform2corner,
cl_performatend,cl_perform3corner,cl_performmore3corner;

Standard_EXPORT Standard_Real t_total, t_extent,t_perfsetofsurf,
t_perffilletonvertex, t_filds,t_reconstruction,t_setregul, t_perfsetofkgen,
t_perfsetofkpart,t_makextremities,t_performatend,t_startsol,t_performsurf,
t_perform1corner,t_perform2corner,t_perform3corner,t_performmore3corner,
t_batten,t_inter,t_sameinter,t_same,t_plate,t_approxplate,t_t2cornerinit,
t_perf2cornerbyinter,t_chfikpartcompdata,t_cheminement,t_remplissage,
t_t3cornerinit ,t_spherique,t_torique, t_notfilling,t_filling,t_sameparam,
t_computedata,t_completedata,t_t2cornerDS,t_t3cornerDS;
               
extern void ChFi3d_InitChron(OSD_Chronometer& ch);
extern void ChFi3d_ResultChron(OSD_Chronometer & ch, Standard_Real& time);
extern Standard_Boolean ChFi3d_GettraceCHRON();
#endif


//=======================================================================
//function : NearestWholePeriod
//purpose  : 
//=======================================================================

Standard_Real NearestWholePeriod(const Standard_Real    theValue,
                                 const Standard_Real    thePeriod,
                                 const Standard_Boolean theFromBelow)
{
  Standard_Real Result = 0.;
  Standard_Real Sign = (theValue < 0.)? -1 : 1;

  while (Abs(Result - theValue) > thePeriod)
    Result += Sign * thePeriod;

  if (Result > theValue && theFromBelow)
    Result -= thePeriod;

  return Result;
}

//=======================================================================
//function : FindPeriod
//purpose  : 
//=======================================================================

Standard_Real FindPeriod(const TopoDS_Face&     theFace,
                         const Standard_Boolean theIsUdirection)
{
  Standard_Real aPeriod = 0.;
  
  BRepAdaptor_Surface BAsurf(theFace, Standard_False);
  if (theIsUdirection)
  {
    if (BAsurf.IsUPeriodic())
      aPeriod = BAsurf.UPeriod();
    else
    {
      Standard_Real Umin = BAsurf.FirstUParameter(),
        Umax = BAsurf.LastUParameter();
      aPeriod = Umax - Umin;
    }
  }
  else
  {
    //temporary
  }

  return aPeriod;
}

//=======================================================================
//function : ContainsEdge
//purpose  : 
//=======================================================================

Standard_Boolean ContainsEdge(const TopoDS_Shape& theShape,
                              const TopoDS_Shape& theEdge)
{
  TopExp_Explorer Explo(theShape, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Shape& anEdge = Explo.Current();
    if (anEdge.IsSame(theEdge))
      return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : FindProperSubShape
//purpose  : 
//=======================================================================

TopoDS_Shape FindProperSubShape(const TopoDS_Shape& theNewFace,
                                const TopoDS_Shape& theShape)
{
  TopoDS_Shape NullShape;

  TopTools_IndexedMapOfShape Emap;
  TopExp::MapShapes(theNewFace, TopAbs_EDGE, Emap);

  TopoDS_Iterator iter(theShape);
  for (; iter.More(); iter.Next())
  {
    const TopoDS_Shape& aShape = iter.Value();
    if (aShape.ShapeType() == TopAbs_FACE)
    {
      TopExp_Explorer Explo(aShape, TopAbs_EDGE);
      for (; Explo.More(); Explo.Next())
      {
        const TopoDS_Shape& anEdge = Explo.Current();
        if (Emap.Contains(anEdge))
          return theShape;
      }
    }
    else
    {
      TopoDS_Shape aResult = FindProperSubShape(theNewFace, aShape);
      if (!aResult.IsNull())
        return aResult;
    }
  }

  return NullShape;
}

//=======================================================================
//function : DeleteEdgesFromMap
//purpose  : 
//=======================================================================

void DeleteEdgesFromMap(TopTools_IndexedDataMapOfShapeListOfShape& theVEmap,
                        const TopTools_SequenceOfShape&            theEdges,
                        //const TopoDS_Edge&                         theCurEdge,
                        const TopoDS_Edge&                         theNextEdge,
                        const TopoDS_Vertex&                       theVertex,
                        const Standard_Integer                     theIndOr)
{
  for (Standard_Integer i = 1; i <= theEdges.Length(); i++)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(theEdges(i));
    if (anEdge == theNextEdge)
      continue;
    TopoDS_Vertex VV [2];
    TopExp::Vertices(anEdge, VV[0], VV[1], Standard_True);
    if (VV[theIndOr].IsSame(theVertex))
    {
      //theDeletedEdges.Append(anEdge);
      for (Standard_Integer j = 1; j <= theVEmap.Extent(); j++)
      {
        TopoDS_Vertex aVertex = TopoDS::Vertex(theVEmap.FindKey(j));
        TopTools_ListIteratorOfListOfShape itl(theVEmap(j));
        for (; itl.More(); itl.Next())
        {
          if (anEdge == itl.Value())
          {
            theVEmap(j).Remove(itl);
            break;
          }
        }
      }
    }
  }
}

//=======================================================================
//function : BuildNewWire
//purpose  : 
//=======================================================================

TopoDS_Wire BuildNewWire(const TopoDS_Wire& theWire,
                         TopTools_IndexedDataMapOfShapeListOfShape& theVEmap,
                         //const TopoDS_Compound& theNewEdges,
                         const TopTools_IndexedMapOfShape& theNewEdges,
                         const TopoDS_Face& theFace)
{
  //Temporary
  BRep_Builder BB;
  TopoDS_Compound aNE;
  BB.MakeCompound(aNE);
  for (Standard_Integer ind = 1; ind <= theNewEdges.Extent(); ind++)
  {
    const TopoDS_Shape& aNewEdge = theNewEdges(ind);
    BB.Add(aNE, aNewEdge);
  }
  ///////////

  Standard_Boolean Uclosed, Vclosed;
  Standard_Real Uperiod = 0., Vperiod = 0.;
  BRepTools::DetectClosedness(theFace, Uclosed, Vclosed);
  if (Uclosed)
    Uperiod = FindPeriod(theFace, Standard_True);
  if (Vclosed)
    Vperiod = FindPeriod(theFace, Standard_False);
  
  TopTools_IndexedMapOfShape OldVertices;
  TopExp::MapShapes(theWire, TopAbs_VERTEX, OldVertices);
  //TopExp::MapShapes(theNewEdges, TopAbs_EDGE, NewEdges);
  
  //Find <StartEdge>, <StartVertex> and calculate minimum distance
  //between extremities of edge in 2d
  TopoDS_Vertex StartVertex;
  TopoDS_Edge   StartEdge, SecondEdge;
  Standard_Real MinDist = RealLast();
  TopTools_ListIteratorOfListOfShape itl;
  TopTools_MapOfOrientedShape Emap;
  for (Standard_Integer i = 1; i <= OldVertices.Extent(); i++)
  {
    TopoDS_Vertex aVertex = TopoDS::Vertex(OldVertices(i));
    const TopTools_ListOfShape& Elist = theVEmap.FindFromKey(aVertex);
    for (itl.Initialize(Elist); itl.More(); itl.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      if (!Emap.Add(anEdge))
        continue;
      if (StartEdge.IsNull() &&
          theNewEdges.Contains(anEdge))
      {
        //StartEdge   = anEdge;
        Standard_Integer anIndex = theNewEdges.FindIndex(anEdge);
        StartEdge   = TopoDS::Edge(theNewEdges(anIndex));
        StartVertex = aVertex;
      }
      BRepAdaptor_Curve2d BAcurve(anEdge, theFace);
      gp_Pnt2d aFirstPoint = BAcurve.Value(BAcurve.FirstParameter());
      gp_Pnt2d aLastPoint  = BAcurve.Value(BAcurve.LastParameter());
      Standard_Real aDist = aFirstPoint.SquareDistance(aLastPoint);
      if (aDist < MinDist)
        MinDist = aDist;
    }
  }

  if (StartEdge.IsNull())
    return theWire;

  TopoDS_Wire NewWire;
  //BRep_Builder BB;
  BB.MakeWire(NewWire);

  BB.Add(NewWire, StartEdge);

  //Define the direction of loop: forward or reversed
  TopAbs_Orientation Direction;
  Standard_Integer IndOr;
  //Here and further orientation of edge is taken into account
  TopoDS_Vertex V1 = TopExp::FirstVertex(StartEdge, Standard_True);
  if (V1.IsSame(StartVertex))
  {
    Direction = TopAbs_FORWARD;
    IndOr = 0;
  }
  else
  {
    Direction = TopAbs_REVERSED;
    IndOr = 1;
  }

  BRepAdaptor_Curve2d StartBAcurve(StartEdge, theFace);
  Standard_Real StartParam  = BRep_Tool::Parameter(StartVertex,  StartEdge);
  gp_Pnt2d StartPoint  = StartBAcurve.Value(StartParam);

  //Find second edge;
  TopTools_SequenceOfShape Candidates; //, DeletedEdges;
  TopoDS_Vertex VV [2];

  //Main loop
  TopoDS_Edge CurEdge = StartEdge, PrevEdge;
  TopoDS_Vertex CurVertex = (Direction == TopAbs_FORWARD)?
    TopExp::LastVertex(CurEdge, Standard_True) :
    TopExp::FirstVertex(CurEdge, Standard_True);
  BRepAdaptor_Curve2d CurCurve(CurEdge, theFace);
  Standard_Real CurParam = BRep_Tool::Parameter(CurVertex, CurEdge);
  gp_Pnt2d CurPoint = CurCurve.Value(CurParam);
  Standard_Boolean IsPrevSeamJustBuilt = Standard_False;
  for (;;)
  {
    const TopTools_ListOfShape& Elist = theVEmap.FindFromKey(CurVertex);
    Candidates.Clear();
    Standard_Boolean ToAdd = Standard_True;
    //Standard_Boolean IsPrevEdgeCorrect = Standard_True;
    
    //Candidates are the edges close to <CurPoint> in 2d
    for (itl.Initialize(Elist); itl.More(); itl.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      if (anEdge.IsSame(CurEdge))
        continue;
      BRepAdaptor_Curve2d BAcurve(anEdge, theFace);
      gp_Pnt2d aPoint = BAcurve.Value(BAcurve.FirstParameter());
      Standard_Real aDist = CurPoint.SquareDistance(aPoint);
      if (aDist < MinDist)
        Candidates.Append(anEdge);
      else
      {
        aPoint = BAcurve.Value(BAcurve.LastParameter());
        aDist = CurPoint.SquareDistance(aPoint);
        if (aDist < MinDist)
          Candidates.Append(anEdge);
      }
    }

    TopoDS_Edge NextEdge, aCandidate;
    if (Candidates.IsEmpty()) //hanging edge
    {
      //need to build additional edges
      Standard_Real fpar, lpar;
      if (BRep_Tool::Degenerated(CurEdge) &&
          !BRep_Tool::Degenerated(PrevEdge))
      {
        //enlarge current degenerated edge
        TopoDS_Vertex OtherVertex = CurVertex;
        OtherVertex.Reverse();
        Standard_Real OtherParam = BRep_Tool::Parameter(OtherVertex, CurEdge);
        gp_Pnt2d OtherPoint = CurCurve.Value(OtherParam);
        
        if (Uclosed)
        {
          Standard_Real DirOfCurEdge = CurPoint.X() - OtherPoint.X();
          Standard_Boolean FromBelow =  (DirOfCurEdge < 0.);
          Standard_Real NewX = NearestWholePeriod( CurPoint.X(), Uperiod, FromBelow );
          CurPoint.SetX(NewX);
        }
        Handle(Geom2d_Line) aLine;
        if (CurEdge.Orientation() == TopAbs_FORWARD)
          aLine = GCE2d_MakeLine(OtherPoint, CurPoint);
        else
          aLine = GCE2d_MakeLine(CurPoint, OtherPoint);
        BB.UpdateEdge(CurEdge, aLine, theFace, 0.);
        BB.Range(CurEdge, 0., CurPoint.Distance(OtherPoint));
        NextEdge = CurEdge;
        IsPrevSeamJustBuilt = Standard_False;
        ToAdd = Standard_False;
      }
      else if (IsPrevSeamJustBuilt)
      {
        //Find degenerated edge and enlarge it
        for (itl.Initialize(Elist); itl.More(); itl.Next())
        {
          TopoDS_Edge anEdge = TopoDS::Edge(itl.Value());
          if (anEdge.IsSame(CurEdge))
            continue;
          if (BRep_Tool::Degenerated(anEdge))
          {
            //enlarge found degenerated edge
            TopoDS_Vertex OtherVertex = CurVertex;
            if (anEdge.Orientation() == TopAbs_FORWARD)
              OtherVertex.Reverse();
            Standard_Real OtherParam = BRep_Tool::Parameter(OtherVertex, anEdge);
            BRepAdaptor_Curve2d aCurve(anEdge, theFace);
            gp_Pnt2d OtherPoint = aCurve.Value(OtherParam);
        
            Handle(Geom2d_Line) aLine;
            if (anEdge.Orientation() == TopAbs_FORWARD)
              aLine = GCE2d_MakeLine(OtherPoint, CurPoint);
            else
              aLine = GCE2d_MakeLine(CurPoint, OtherPoint);
            BB.UpdateEdge(anEdge, aLine, theFace, 0.);
            BB.Range(anEdge, 0., CurPoint.Distance(OtherPoint));
            NextEdge = anEdge;
            IsPrevSeamJustBuilt = Standard_False;
          }
        }
      }
      else
      {
        //build additional seam edge from existing edge
        for (itl.Initialize(Elist); itl.More(); itl.Next())
        {
          TopoDS_Edge anEdge = TopoDS::Edge(itl.Value());
          if (anEdge.IsSame(CurEdge))
            continue;
          TopExp::Vertices(anEdge, VV[0], VV[1], Standard_True);
          if (VV[1-IndOr].IsSame(CurVertex))
          {
            if (!BRep_Tool::IsClosed(anEdge, theFace))
            {            
              Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(anEdge, theFace, fpar, lpar);
              Standard_Real aParam = BRep_Tool::Parameter(CurVertex, anEdge);
              gp_Pnt2d aPoint = aPCurve->Value(aParam);
              gp_Vec2d Offset(0.,0.);
              if (Uclosed)
                Offset.SetX( CurPoint.X() - aPoint.X() );
              else
                Offset.SetY( CurPoint.Y() - aPoint.Y() );
              Handle(Geom2d_Curve) aPCurve2 = Handle(Geom2d_Curve)::DownCast(aPCurve->Translated(Offset));
              BB.UpdateEdge(anEdge, aPCurve, aPCurve2, theFace, 0.);
            }
            NextEdge = TopoDS::Edge(anEdge.Reversed());
            IsPrevSeamJustBuilt = Standard_True;
            break;
          }
        }
      }
    }

    for (Standard_Integer i = 1; i <= Candidates.Length(); i++)
    {
      IsPrevSeamJustBuilt = Standard_False;
      const TopoDS_Edge& anEdge = TopoDS::Edge(Candidates(i));
      if (theNewEdges.Contains(anEdge))
      {
        TopExp::Vertices(anEdge, VV[0], VV[1], Standard_True);
        if (VV[IndOr].IsSame(CurVertex))
        {
          BRepAdaptor_Curve2d BAcurve(anEdge, theFace);
          Standard_Real aParam = BRep_Tool::Parameter(CurVertex, anEdge);
          gp_Pnt2d aPoint = BAcurve.Value(aParam);
          Standard_Real aDist = CurPoint.SquareDistance(aPoint);
          if (aDist < MinDist)
          {
            NextEdge = anEdge;
            break;
          }
        }
        else //previous edge is incorrect
        {
          //error ?
          
          //remove previous edge from wire
          //build additional edges
          //NextEdge = anEdge;
        }
      }
      else if (aCandidate.IsNull())
      {
        TopExp::Vertices(anEdge, VV[0], VV[1], Standard_True);
        if (VV[IndOr].IsSame(CurVertex))
        {
          BRepAdaptor_Curve2d BAcurve(anEdge, theFace);
          Standard_Real aParam = BRep_Tool::Parameter(VV[IndOr], anEdge);
          gp_Pnt2d aPoint = BAcurve.Value(aParam);
          Standard_Real aDist = CurPoint.SquareDistance(aPoint);
          if (aDist < MinDist)
            aCandidate = anEdge;
        }
      }
    }
    if (NextEdge.IsNull())
      NextEdge = aCandidate;

    DeleteEdgesFromMap(theVEmap, Candidates, NextEdge, CurVertex, 1-IndOr);

    PrevEdge = CurEdge;
    CurEdge = NextEdge;
    CurVertex = (Direction == TopAbs_FORWARD)?
      TopExp::LastVertex(CurEdge, Standard_True) :
      TopExp::FirstVertex(CurEdge, Standard_True);
    CurCurve.Initialize(CurEdge, theFace);
    CurParam = BRep_Tool::Parameter(CurVertex, CurEdge);
    CurPoint = CurCurve.Value(CurParam);
    
    if (ToAdd)
      BB.Add(NewWire, CurEdge);

    if (CurVertex.IsSame(StartVertex) &&
        CurPoint.SquareDistance(StartPoint) < MinDist)
      break;
  }

  return NewWire;
}

//=======================================================================
//function : CompleteDS
//purpose  : 
//=======================================================================

static void CompleteDS(TopOpeBRepDS_DataStructure& DStr,
		       const TopoDS_Shape& S)
{
  ChFiDS_Map MapEW,MapFS;
  MapEW.Fill(S,TopAbs_EDGE,TopAbs_WIRE);
  MapFS.Fill(S,TopAbs_FACE,TopAbs_SHELL);
  
  TopExp_Explorer ExpE;
  for (ExpE.Init(S,TopAbs_EDGE); ExpE.More(); ExpE.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(ExpE.Current());
    Standard_Boolean hasgeom = DStr.HasGeometry(E);
    if (hasgeom) {
      const TopTools_ListOfShape& WireListAnc = MapEW(E);
      TopTools_ListIteratorOfListOfShape itaW(WireListAnc);
      while (itaW.More()) {
	const TopoDS_Shape& WireAnc = itaW.Value();
	DStr.AddShape(WireAnc);
	itaW.Next();
      }
    }
  }
  
  TopExp_Explorer ExpF;
  for (ExpF.Init(S,TopAbs_FACE); ExpF.More(); ExpF.Next()) {
    const TopoDS_Face& F = TopoDS::Face(ExpF.Current());
    Standard_Boolean hasgeom = DStr.HasGeometry(F);
    if (hasgeom) {
      const TopTools_ListOfShape& ShellListAnc = MapFS(F);
      TopTools_ListIteratorOfListOfShape itaS(ShellListAnc);
      while (itaS.More()) {
	const TopoDS_Shape& ShellAnc = itaS.Value();
	DStr.AddShape(ShellAnc);
	itaS.Next();
      }
    }
  }
  
  // set the range on the DS Curves
  for (Standard_Integer ic = 1; ic <= DStr.NbCurves(); ic++) {
    Standard_Real parmin = RealLast(), parmax = RealFirst();
    const TopOpeBRepDS_ListOfInterference& LI = DStr.CurveInterferences(ic);
    for (TopOpeBRepDS_PointIterator it(LI);
	 it.More();
	 it.Next() ) {
      Standard_Real par = it.Parameter();
      parmin = Min (parmin,par); parmax = Max (parmax,par);
    }
    DStr.ChangeCurve(ic).SetRange(parmin,parmax);
  }
}

//=======================================================================
//function : ~ChFi3d_Builder
//purpose  : 
//=======================================================================

ChFi3d_Builder::~ChFi3d_Builder()
{}

//=======================================================================
//function : ExtentAnalyse
//purpose  : 
//=======================================================================

void ChFi3d_Builder::ExtentAnalyse ()
{
  Standard_Integer nbedges, nbs;
  for (Standard_Integer iv = 1; iv <= myVDataMap.Extent(); iv++) {
    nbs = myVDataMap(iv).Extent();
    const TopoDS_Vertex& Vtx = myVDataMap.FindKey(iv);
    //nbedges = ChFi3d_NumberOfEdges(Vtx, myVEMap); 
    nbedges = ChFi3d_NumberOfSharpEdges(Vtx, myVEMap, myEFMap); 
    switch (nbs) {
    case 1 :
      ExtentOneCorner(Vtx, myVDataMap.FindFromIndex(iv).First());
      break;
    case 2 :
      if (nbedges <= 3)
	ExtentTwoCorner(Vtx, myVDataMap.FindFromIndex(iv));
      break;
    case 3 :
      if (nbedges <= 3)
	ExtentThreeCorner(Vtx, myVDataMap.FindFromIndex(iv));
      break;
    default : 
      break;
    }
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::Compute()
{
  
#ifdef OCCT_DEBUG   //perf 
  t_total=0;t_extent=0; t_perfsetofsurf=0;t_perffilletonvertex=0;
  t_filds=0;t_reconstruction=0;t_setregul=0;
  t_perfsetofkpart=0; t_perfsetofkgen=0;t_makextremities=0;
  t_performsurf=0;t_startsol=0; t_perform1corner=0;t_perform2corner=0;
  t_perform3corner=0;t_performmore3corner=0;t_inter=0;t_same=0;t_sameinter=0;
  t_plate=0;t_approxplate=0; t_batten=0;t_remplissage=0;t_t3cornerinit=0;
  t_spherique=0;t_torique=0;t_notfilling=0;t_filling=0;t_performatend=0;
  t_t2cornerinit=0; t_perf2cornerbyinter=0;t_chfikpartcompdata=0;
  t_cheminement=0; t_sameparam=0; t_computedata=0;t_completedata=0;
  t_t2cornerDS=0;t_t3cornerDS=0;
  ChFi3d_InitChron(cl_total);
  ChFi3d_InitChron(cl_extent);
#endif 
  
  if (myListStripe.IsEmpty())
    throw Standard_Failure("There are no suitable edges for chamfer or fillet");
  
  Reset();
  myDS = new TopOpeBRepDS_HDataStructure();
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  done = Standard_True;
  hasresult=Standard_False;
  
  // filling of myVDatatMap
  ChFiDS_ListIteratorOfListOfStripe itel;
  
  for (itel.Initialize(myListStripe);itel.More(); itel.Next()) {
    if ((itel.Value()->Spine()->FirstStatus() <= ChFiDS_BreakPoint))
      myVDataMap.Add(itel.Value()->Spine()->FirstVertex(),itel.Value());
    else if (itel.Value()->Spine()->FirstStatus() == ChFiDS_FreeBoundary)
      ExtentOneCorner(itel.Value()->Spine()->FirstVertex(),itel.Value());
    if ((itel.Value()->Spine()->LastStatus() <= ChFiDS_BreakPoint))
      myVDataMap.Add(itel.Value()->Spine()->LastVertex() ,itel.Value());
    else if (itel.Value()->Spine()->LastStatus() == ChFiDS_FreeBoundary)
      ExtentOneCorner(itel.Value()->Spine()->LastVertex(),itel.Value());
  }
  // preanalysis to evaluate the extensions.
  ExtentAnalyse();
  
  
#ifdef OCCT_DEBUG //perf 
  ChFi3d_ResultChron(cl_extent,t_extent);
  ChFi3d_InitChron(cl_perfsetofsurf);
#endif
  
  // Construction of the stripe of fillet on each stripe.
  for (itel.Initialize(myListStripe);itel.More(); itel.Next()) {
    itel.Value()->Spine()->SetErrorStatus(ChFiDS_Ok);
    try {
      OCC_CATCH_SIGNALS
      PerformSetOfSurf(itel.Value());
    }
    catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      std::cout <<"EXCEPTION Stripe compute " << anException << std::endl;
#endif
      (void)anException;
      badstripes.Append(itel.Value());
      done = Standard_True;
      if (itel.Value()->Spine()->ErrorStatus()==ChFiDS_Ok) 
      itel.Value()->Spine()->SetErrorStatus(ChFiDS_Error);
    }
    if (!done) badstripes.Append(itel.Value());
    done = Standard_True;
  }
  done = (badstripes.IsEmpty());
  
#ifdef OCCT_DEBUG //perf 
  ChFi3d_ResultChron(cl_perfsetofsurf,t_perfsetofsurf);
  ChFi3d_InitChron(cl_perffilletonvertex);
#endif 
  
  //construct fillets on each vertex + feed the Ds
  if (done) {
    Standard_Integer j;
    for (j=1;j<=myVDataMap.Extent();j++)
    {
      try
      {
        OCC_CATCH_SIGNALS
        PerformFilletOnVertex(j);
      }
      catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
        std::cout <<"EXCEPTION Corner compute " << anException << std::endl;
#endif
        (void)anException;
        badvertices.Append(myVDataMap.FindKey(j));
        hasresult=Standard_False;
        done = Standard_True;
      }
      if (!done) badvertices.Append(myVDataMap.FindKey(j));
      done = Standard_True;
    }
    if (!hasresult) done = badvertices.IsEmpty();
  }
  

#ifdef OCCT_DEBUG //perf 
  ChFi3d_ResultChron(cl_perffilletonvertex,t_perffilletonvertex);
  ChFi3d_InitChron(cl_filds);
#endif
  
  TColStd_MapOfInteger MapIndSo;
  TopExp_Explorer expso(myShape,TopAbs_SOLID);
  for(; expso.More(); expso.Next()){
    const TopoDS_Shape& cursol = expso.Current();
    Standard_Integer indcursol = DStr.AddShape(cursol);
    MapIndSo.Add(indcursol);
  }
  TopExp_Explorer expsh(myShape,TopAbs_SHELL,TopAbs_SOLID);
  for(; expsh.More(); expsh.Next()){
    const TopoDS_Shape& cursh = expsh.Current();
    Standard_Integer indcursh = DStr.AddShape(cursh);
    MapIndSo.Add(indcursh);
  }
  if (done) {
    Standard_Integer i1;
    for (itel.Initialize(myListStripe), i1=0;
	 itel.More(); 
	 itel.Next(), i1++) {
      const Handle(ChFiDS_Stripe)& st = itel.Value();
      // 05/02/02 akm vvv : (OCC119) First we'll check ain't there 
      //                    intersections between fillets
      ChFiDS_ListIteratorOfListOfStripe itel1;
      Standard_Integer i2;
      for (itel1.Initialize(myListStripe), i2=0;
	   itel1.More(); 
	   itel1.Next(), i2++) {
	if (i2 <= i1)
	  // Do not twice intersect the stripes
	  continue;
	Handle(ChFiDS_Stripe) aCheckStripe = itel1.Value();
	try {
	  OCC_CATCH_SIGNALS
	  ChFi3d_StripeEdgeInter (st, aCheckStripe, DStr, tol2d);
	}
	catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	  std::cout <<"EXCEPTION Fillets compute " << anException << std::endl;
#endif
	  (void)anException;
	  badstripes.Append(itel.Value());
	  hasresult=Standard_False;
	  done = Standard_False;
	  break;
	}
      }
      // 05/02/02 akm ^^^
      Standard_Integer solidindex = st->SolidIndex();
      ChFi3d_FilDS(solidindex,st,DStr,myRegul,tolesp,tol2d);
      if (!done) break;
    }
    
#ifdef OCCT_DEBUG //perf 
    ChFi3d_ResultChron(cl_filds,t_filds);
    ChFi3d_InitChron(cl_reconstruction);
#endif
    
    
    if (done) {
      BRep_Builder BB;
      CompleteDS(DStr,myShape);
      //Update tolerances on vertex to max adjacent edges or
      //Update tolerances on degenerated edge to max of adjacent vertexes.
      TopOpeBRepDS_CurveExplorer cex(DStr);
      for(;cex.More();cex.Next()){
	TopOpeBRepDS_Curve& c = *((TopOpeBRepDS_Curve*)(void*)&(cex.Curve()));
	Standard_Real tolc = 0.;
	Standard_Boolean degen = c.Curve().IsNull();
	if(!degen) tolc = c.Tolerance();
	Standard_Integer ic = cex.Index();
	TopOpeBRepDS_PointIterator It(myDS->CurvePoints(ic));
	for(;It.More();It.Next()){
	  Handle(TopOpeBRepDS_CurvePointInterference) II;
	  II = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(It.Value());
	  if (II.IsNull()) continue;
	  TopOpeBRepDS_Kind gk = II->GeometryType();
	  Standard_Integer gi = II->Geometry();
	  if(gk == TopOpeBRepDS_VERTEX){
	    const TopoDS_Vertex& v = TopoDS::Vertex(myDS->Shape(gi));
	    Standard_Real tolv = BRep_Tool::Tolerance(v);
	    if( tolv > 0.0001 ) {
	      tolv += 0.0003;
	      if( tolc < tolv ) tolc = tolv + 0.00001;
	    }
	    if(degen && tolc < tolv) tolc = tolv;
	    else if(tolc>tolv) BB.UpdateVertex(v,tolc);
	  }
	  else if(gk == TopOpeBRepDS_POINT){
	    TopOpeBRepDS_Point& p = DStr.ChangePoint(gi);
	    Standard_Real tolp = p.Tolerance();
	    if(degen && tolc < tolp) tolc = tolp;
	    else if(tolc>tolp) p.Tolerance(tolc);
	  }
	}
	if(degen) c.Tolerance(tolc);
      }
      
      //jgv
      
      //for (on modified faces)
      //compound of wires from each face
      //compound of new edges for this face
      //general fuse (compound of wires from a face, compound of new edges for this face)
      //method building new face from old and new edges
      //assembling of resulting shape from modified and unmodified faces.

      TopTools_ListOfShape aChFiFaces;
      TopTools_IndexedDataMapOfShapeShape aFacesModifiedFaces;

      TopTools_IndexedMapOfShape MapNewEdges;
      //TopTools_DataMapOfShapeListOfShape FaceListEdges;
      NCollection_DataMap<TopoDS_Shape, TopTools_IndexedMapOfShape> FaceMapEdges;
      
      BOPAlgo_Builder GenFuse;
      
      for (Standard_Integer i = 1; i <= myNewFaces.Extent(); i++)
      {
        TopoDS_Face aFace = TopoDS::Face(myNewFaces(i));
        //TopAbs_Orientation anOrOfFace = aFace.Orientation();
        aFace.Orientation(TopAbs_FORWARD);

        TopTools_IndexedMapOfShape* aEmap = FaceMapEdges.Bound(aFace, TopTools_IndexedMapOfShape());

        //TopoDS_Compound aNewEdges;
        //BB.MakeCompound(aNewEdges);
        //TopTools_ListOfShape ListNewEdges;
        
        //ChFi3d_ListIteratorOfListOfQualifiedEdge itl(myFaceNewEdges.FindFromKey(i));
        TColStd_ListIteratorOfListOfInteger itl(myFaceNewEdges.FindFromKey(i));
        for (; itl.More(); itl.Next())
        {
          Standard_Integer aSignedIndex = itl.Value();
          Standard_Integer anIndex = Abs(aSignedIndex);
          TopoDS_Shape aNewEdge = myNewEdges(anIndex);
          TopAbs_Orientation anOr = (aSignedIndex > 0)?
            TopAbs_FORWARD : TopAbs_REVERSED;
          aNewEdge.Orientation(anOr);
          //BB.Add(aNewEdges, aNewEdge);
          //ListNewEdges.Append(aNewEdge);
          aEmap->Add(aNewEdge);
        }

        if (myIndsChFiFaces.Contains(i)) //absolutely new face
        {
        }
        else //a modified old face
        {
          GenFuse.AddArgument(aFace);
          //TopTools_ListIteratorOfListOfShape itl(ListNewEdges);
          //for (; itl.More(); itl.Next())
          //MapNewEdges.Add(itl.Value());
          for (Standard_Integer iedge = 1; iedge <= aEmap->Extent(); iedge++)
            MapNewEdges.Add(aEmap->FindKey(iedge));
        }
      }

      //Fusing
      //GenFuse.AddArgument(aNewEdges);
      for (Standard_Integer i = 1; i <= MapNewEdges.Extent(); i++)
        GenFuse.AddArgument(MapNewEdges(i));
      GenFuse.Perform();
      //Temporary
      const TopoDS_Shape& aResFuse = GenFuse.Shape();
      ///////////
      const TopTools_DataMapOfShapeListOfShape& ModifiedShapes = GenFuse.Images();
      
      for (Standard_Integer i = 1; i <= myNewFaces.Extent(); i++)
      {
        TopoDS_Face aFace = TopoDS::Face(myNewFaces(i));
        TopAbs_Orientation anOrOfFace = aFace.Orientation();
        aFace.Orientation(TopAbs_FORWARD);

        if (myIndsChFiFaces.Contains(i)) //absolutely new face
        {
          TopoDS_Wire aWire;
          BB.MakeWire(aWire);
          for (Standard_Integer iedge = 1; iedge <= FaceMapEdges(aFace).Extent(); iedge++)
          {
            const TopoDS_Shape& anEdge = FaceMapEdges(aFace).FindKey(iedge);
            TopAbs_Orientation anOr = anEdge.Orientation();
            if (ModifiedShapes.IsBound(anEdge))
            {
              const TopTools_ListOfShape& aListOfSplits = ModifiedShapes(anEdge);
              TopTools_ListIteratorOfListOfShape itl(aListOfSplits);
              for (; itl.More(); itl.Next())
                BB.Add(aWire, itl.Value().Oriented(anOr));
            }
            else
              BB.Add(aWire, anEdge);
          }
          BB.Add(aFace, aWire);
          aFace.Orientation(anOrOfFace);
          aChFiFaces.Append(aFace);
        }
        else //modify old face
        {
          //Build compound for construction of new face
          const TopTools_ListOfShape& aListOfModifiedFromFace = ModifiedShapes(aFace);
          TopoDS_Compound ResFuseForFace;
          BB.MakeCompound(ResFuseForFace);
          TopTools_ListIteratorOfListOfShape itl(aListOfModifiedFromFace);
          for (; itl.More(); itl.Next())
            BB.Add(ResFuseForFace, itl.Value());
          
          //Update FaceMapEdges(aFace)
          TopTools_ListOfShape ToDelete, ToAdd;
          for (Standard_Integer ind = 1; ind <= FaceMapEdges(aFace).Extent(); ind++)
          {
            const TopoDS_Shape& aNewEdgeOfFace = FaceMapEdges(aFace)(ind);
            TopAbs_Orientation anOr = aNewEdgeOfFace.Orientation();
            if (ModifiedShapes.IsBound(aNewEdgeOfFace))
            {
              const TopTools_ListOfShape& aListOfSplits = ModifiedShapes(aNewEdgeOfFace);
              for (itl.Initialize(aListOfSplits); itl.More(); itl.Next())
                ToAdd.Append(itl.Value().Oriented(anOr));
              ToDelete.Append(aNewEdgeOfFace);
            }
          }
          for (itl.Initialize(ToDelete); itl.More(); itl.Next())
            FaceMapEdges(aFace).RemoveKey(itl.Value());
          for (itl.Initialize(ToAdd); itl.More(); itl.Next())
            FaceMapEdges(aFace).Add(itl.Value());
          
          //Build the map of splits for edges of old face
          TopTools_MapOfShape Splits;
          TopExp_Explorer Explo(aFace, TopAbs_EDGE);
          for (; Explo.More(); Explo.Next())
          {
            const TopoDS_Shape& anEdge = Explo.Current();
            if (ModifiedShapes.IsBound(anEdge))
            {
              const TopTools_ListOfShape& aListOfSplits = ModifiedShapes(anEdge);
              for (itl.Initialize(aListOfSplits); itl.More(); itl.Next())
                Splits.Add(itl.Value());
            }
          }
          
          ToDelete.Clear();
          for (Standard_Integer ind = 1; ind <= FaceMapEdges(aFace).Extent(); ind++)
          {
            const TopoDS_Shape& aNewEdgeOfFace = FaceMapEdges(aFace)(ind);
            if (Splits.Contains(aNewEdgeOfFace))
            {
              ToDelete.Append(aNewEdgeOfFace);
              continue;
            }
            TopAbs_Orientation anOr = aNewEdgeOfFace.Orientation();
            if (ModifiedShapes.IsBound(aNewEdgeOfFace))
            {
              const TopTools_ListOfShape& aListOfModifiedFromEdge = ModifiedShapes(aNewEdgeOfFace);
              for (itl.Initialize(aListOfModifiedFromEdge); itl.More(); itl.Next())
              {
                const TopoDS_Shape& aModifiedNewEdgeOfFace = itl.Value();
                if (!ContainsEdge(ResFuseForFace, aModifiedNewEdgeOfFace))
                  BB.Add(ResFuseForFace, aModifiedNewEdgeOfFace.Oriented(anOr));
              }
            }
            else
              BB.Add(ResFuseForFace, aNewEdgeOfFace);
          }
          for (itl.Initialize(ToDelete); itl.More(); itl.Next())
            FaceMapEdges(aFace).RemoveKey(itl.Value());
          
          TopTools_IndexedDataMapOfShapeListOfShape VEmapOfNewFace;
          TopExp::MapShapesAndUniqueAncestors(ResFuseForFace, TopAbs_VERTEX, TopAbs_EDGE,
                                              VEmapOfNewFace, Standard_True); //with orientation
          
          TopoDS_Shape aNewFace = aFace.EmptyCopied();
          TopoDS_Iterator itw(aFace);
          for (; itw.More(); itw.Next())
          {
            const TopoDS_Shape& aWire = itw.Value();
            TopTools_ListOfShape ListOfWires;
            if (ModifiedShapes.IsBound(aWire))
            {
              const TopTools_ListOfShape& aListOfModified = ModifiedShapes(aWire);
              for (itl.Initialize(aListOfModified); itl.More(); itl.Next())
                ListOfWires.Append(itl.Value());
            }
            else
              ListOfWires.Append(aWire);
            for (itl.Initialize(ListOfWires); itl.More(); itl.Next())
            {
              const TopoDS_Wire& aModifiedWire = TopoDS::Wire(itl.Value());
#ifdef OCCT_DEBUG
              std::cout << "a Modified Wire ..." << std::endl;
#endif
              //Temporary
              TopoDS_Face TmpFace = TopoDS::Face(aFace.EmptyCopied());
              BB.Add(TmpFace, aModifiedWire);
              ///////////
              //TopoDS_Wire aNewWire = BuildNewWire(aModifiedWire, VEmapOfNewFace, aNewEdges, aFace);
              TopoDS_Wire aNewWire = BuildNewWire(aModifiedWire, VEmapOfNewFace, FaceMapEdges(aFace), aFace);
#ifdef OCCT_DEBUG
              std::cout << "a New Wire ..." << std::endl;
#endif
              BB.Add(aNewFace, aNewWire);
#ifdef OCCT_DEBUG
              std::cout << "a New Face ..." << std::endl;
#endif
              aFacesModifiedFaces.Add(aFace, aNewFace);
            }
          }
        }
      }      

      //Update ChFiFaces
      /*
      TopTools_ListOfShape ModifiedChFiFaces;
      TopTools_ListIteratorOfListOfShape itl(aChFiFaces);
      while (itl.More())
      {
        TopoDS_Shape aChFiFace = itl.Value();
        TopAbs_Orientation anOrOfFace = aChFiFace.Orientation();
        aChFiFace.Orientation(TopAbs_FORWARD);
        BRepTools_Substitution aSubstitutor;
        TopExp_Explorer Explo(aChFiFace, TopAbs_EDGE);
        for (; Explo.More(); Explo.Next())
        {
          const TopoDS_Shape& anEdge = Explo.Current();
          if (!ModifiedShapes.IsBound(anEdge))
            continue;
          const TopTools_ListOfShape& aListOfSplits = ModifiedShapes(anEdge);
          TopTools_ListIteratorOfListOfShape itspl(aListOfSplits);
          for (; itspl.More(); itspl.Next())
          {
            const TopoDS_Shape& aSplit = itspl.Value();
            cout<<"a Split..."<<endl;
          }
          aSubstitutor.Substitute(anEdge.Oriented(TopAbs_FORWARD), aListOfSplits);
        }
        aSubstitutor.Build(aChFiFace);
        if (aSubstitutor.IsCopied(aChFiFace))
        {
          const TopTools_ListOfShape& listSh = aSubstitutor.Copy(aChFiFace);
          TopoDS_Shape aNewChFiFace = listSh.First();
          aNewChFiFace.Orientation(anOrOfFace);
          ModifiedChFiFaces.Append(aNewChFiFace);
          aChFiFaces.Remove(itl);
        }
        else
          itl.Next();
      }
      aChFiFaces.Append(ModifiedChFiFaces);
      */

      /*
        else //a modified old face
        {
          //BRepAlgoAPI_Fuse aFuse(aWires, aNewEdges);
          
          //BOPAlgo_Builder GenFuse;
          //GenFuse.AddArgument(aFace);
          GenFuse.AddArgument(aNewEdges);
          GenFuse.Perform();
          TopoDS_Shape aNewFace = aFace.EmptyCopied();
          const TopoDS_Shape& aResFuse = GenFuse.Shape();
          //const BOPCol_DataMapOfShapeListOfShape& ModifiedShapes = GenFuse.Images();
          const TopTools_DataMapOfShapeListOfShape& ModifiedShapes = GenFuse.Images();
          TopTools_IndexedDataMapOfShapeListOfShape VEmapOfNewFace;
          TopExp::MapShapesAndAncestors(aResFuse, TopAbs_VERTEX, TopAbs_EDGE, VEmapOfNewFace);
          TopoDS_Iterator itw(aFace);
          for (; itw.More(); itw.Next())
          {
            const TopoDS_Shape& aWire = itw.Value();
            if (!ModifiedShapes.IsBound(aWire))
              continue;
            const TopTools_ListOfShape& aListOfModified = ModifiedShapes(aWire);
            TopTools_ListIteratorOfListOfShape itwm(aListOfModified);
            for (; itwm.More(); itwm.Next())
            {
              const TopoDS_Wire& aModifiedWire = TopoDS::Wire(itwm.Value());
              cout<<"a Modified Wire ..."<<endl;
              TopoDS_Wire aNewWire = BuildNewWire(aModifiedWire, VEmapOfNewFace, aNewEdges, aFace);
              cout<<"a New Wire ..."<<endl;
              BB.Add(aNewFace, aNewWire);
              cout<<"a New Face ..."<<endl;
              aFacesModifiedFaces.Add(aFace, aNewFace);
              //Temporary
              BB.Add(aShell, aNewFace);
            }
          }
        }
      */


      //Modify the original shape using ReShape
      BRepTools_ReShape aReshape;
      for (Standard_Integer i = 1; i <= aFacesModifiedFaces.Extent(); i++)
      {
        const TopoDS_Shape& aFace = aFacesModifiedFaces.FindKey(i);
        const TopoDS_Shape& aNewFace = aFacesModifiedFaces(i);
        aReshape.Replace(aFace, aNewFace);
      }
      myShapeResult = aReshape.Apply(myShape);
      //Add ChFiFaces
      while (!aChFiFaces.IsEmpty())
      {
        TopTools_ListIteratorOfListOfShape itl(aChFiFaces);
        while (itl.More())
        {
          const TopoDS_Shape& aChFiFace = itl.Value();
          TopoDS_Shape aTargetShape = FindProperSubShape(aChFiFace, myShapeResult); //recursive method
          if (!aTargetShape.IsNull())
          {
            aTargetShape.Free(Standard_True);
            aTargetShape.Orientation(TopAbs_FORWARD);
            BB.Add(aTargetShape, aChFiFace);
            aChFiFaces.Remove(itl);
          }
          else
            itl.Next();
        }
      }
      /////////////////////////////////////////

      /*
      myCoup->Perform(myDS);
      //jgv//
      
      TColStd_MapIteratorOfMapOfInteger It(MapIndSo);
      for(; It.More(); It.Next()){
	Standard_Integer indsol = It.Key();
	const TopoDS_Shape& curshape = DStr.Shape(indsol);
	myCoup->MergeSolid(curshape,TopAbs_IN);
      }
      
      Standard_Integer i=1,n=DStr.NbShapes();
      for (;i<=n;i++) {
	const TopoDS_Shape S = DStr.Shape(i);
	if (S.ShapeType() != TopAbs_EDGE) continue;
	Standard_Boolean issplitIN = myCoup->IsSplit(S,TopAbs_IN);
	if ( !issplitIN ) continue;
	TopTools_ListIteratorOfListOfShape it(myCoup->Splits(S,TopAbs_IN));
	for (; it.More(); it.Next() ) {
	  const TopoDS_Edge& newE = TopoDS::Edge(it.Value());
	  Standard_Real tole = BRep_Tool::Tolerance(newE);
	  TopExp_Explorer exv(newE,TopAbs_VERTEX);
	  for (; exv.More(); exv.Next() ) {
	    const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
	    Standard_Real tolv = BRep_Tool::Tolerance(v);
	    if (tole>tolv) BB.UpdateVertex(v,tole);
	  }
	}
      }
      if (!hasresult) {
      BB.MakeCompound(TopoDS::Compound(myShapeResult));
      for(It.Reset(); It.More(); It.Next()){
	Standard_Integer indsol = It.Key();
	const TopoDS_Shape& curshape = DStr.Shape(indsol);
	TopTools_ListIteratorOfListOfShape 
	  its = myCoup->Merged(curshape,TopAbs_IN);
	if(!its.More()) BB.Add(myShapeResult,curshape);
	else {
	  //If the old type of Shape is Shell, Shell is placed instead of Solid, 
          //However there is a problem for compound of open Shell.
	  while (its.More()) {
	    const TopAbs_ShapeEnum letype = curshape.ShapeType();
	    if (letype == TopAbs_SHELL){
	      TopExp_Explorer expsh2(its.Value(),TopAbs_SHELL);
	      const TopoDS_Shape& cursh = expsh2.Current();
	      TopoDS_Shape tt = cursh;
	      BB.Add(myShapeResult,cursh);
	      its.Next();
	    }
	    else {
	      BB.Add(myShapeResult,its.Value());
	      its.Next();
	    }
	  }
	}
      }
      }
      else {
       done=Standard_False;
       BB.MakeCompound(TopoDS::Compound(badShape));
      for(It.Reset(); It.More(); It.Next()){
	Standard_Integer indsol = It.Key();
	const TopoDS_Shape& curshape = DStr.Shape(indsol);
	TopTools_ListIteratorOfListOfShape 
	  its = myCoup->Merged(curshape,TopAbs_IN);
	if(!its.More()) BB.Add(badShape,curshape);
	else {
	  while (its.More()) { 
	    BB.Add(badShape,its.Value());
	    its.Next();
	  }
	}
      }
      }
      */
      
#ifdef OCCT_DEBUG //perf 
      ChFi3d_ResultChron(cl_reconstruction ,t_reconstruction);
      ChFi3d_InitChron(cl_setregul);
#endif
      
      // Regularities are coded after cutting.
      SetRegul();
      
      
#ifdef OCCT_DEBUG //perf 
 ChFi3d_ResultChron(cl_setregul ,t_setregul);
#endif
    }
  }
#ifdef OCCT_DEBUG //perf 
  ChFi3d_ResultChron(cl_total,t_total);
#endif
      
  
  // display of time for perfs 
  
#ifdef OCCT_DEBUG
  if(ChFi3d_GettraceCHRON()){
    std::cout<<std::endl; 
    std::cout<<"COMPUTE: temps total "<<t_total<<"s  dont :"<<std::endl;
    std::cout<<"- Init + ExtentAnalyse "<<t_extent<<"s"<<std::endl;  
    std::cout<<"- PerformSetOfSurf "<<t_perfsetofsurf<<"s"<<std::endl;
    std::cout<<"- PerformFilletOnVertex "<<t_perffilletonvertex<<"s"<<std::endl; 
    std::cout<<"- FilDS "<<t_filds<<"s"<<std::endl; 
    std::cout<<"- Reconstruction "<<t_reconstruction<<"s"<<std::endl;
    std::cout<<"- SetRegul "<<t_setregul<<"s"<<std::endl<<std::endl;

    std::cout<<std::endl;
    std::cout <<"temps PERFORMSETOFSURF "<<t_perfsetofsurf <<"s  dont : "<<std::endl;
    std::cout <<"- SetofKPart "<<t_perfsetofkpart<<"s"<<std::endl;
    std::cout <<"- SetofKGen "<< t_perfsetofkgen <<"s"<<std::endl;
    std::cout <<"- MakeExtremities "<<t_makextremities<<"s"<<std::endl<<std::endl;
    
    
    std::cout <<"temps SETOFKGEN "<< t_perfsetofkgen<<"s dont : "<<std::endl;
    std::cout<<"- PerformSurf "<<t_performsurf<<"s"<<std::endl;
    std::cout<<"- starsol "<< t_startsol <<"s"<<std::endl<<std::endl;
    
    std::cout<<"temps PERFORMSURF "<<t_performsurf<<"s  dont : "  << std::endl;
    std::cout<<"- computedata "<<t_computedata<<"s"<<std::endl;
    std::cout<<"- completedata "<<t_completedata<<"s"<<std::endl<<std::endl;
    
    
    std::cout<<"temps PERFORMFILLETVERTEX "<<t_perffilletonvertex <<"s dont : "  << std::endl;
    std::cout<<"- PerformOneCorner "<<t_perform1corner<<"s"<<std::endl;
    std::cout<<"- PerformIntersectionAtEnd "<<t_performatend<<"s"<<std::endl;
    std::cout<<"- PerformTwoCorner "<<t_perform2corner<<"s"<<std::endl;
    std::cout<<"- PerformThreeCorner "<<t_perform3corner<<"s"<<std::endl;
    std::cout<<"- PerformMoreThreeCorner "<<t_performmore3corner<<"s"<<std::endl<<std::endl;
    
    
    std::cout<<"temps PerformOneCorner "<<t_perform1corner<<"s dont:"<<std::endl;
    std::cout<<"- temps condition if (same) "<<t_same << "s "<<std::endl; 
    std::cout<<"- temps condition if (inter) "<<t_inter<<"s " <<std::endl;
    std::cout<<"- temps condition if (same inter) "<<t_sameinter<<"s " <<std::endl<<std::endl;
    
    std::cout<<"temps PerformTwocorner "<<t_perform2corner<<"s  dont:"<<std::endl;
    std::cout<<"- temps initialisation "<< t_t2cornerinit<<"s"<<std::endl;
    std::cout<<"- temps PerformTwoCornerbyInter "<<t_perf2cornerbyinter<<"s"<<std::endl;
    std::cout<<"- temps ChFiKPart_ComputeData "<<t_chfikpartcompdata <<"s"<<std::endl;
    std::cout<<"- temps cheminement "<<t_cheminement<<"s"<<std::endl;
    std::cout<<"- temps remplissage "<<t_remplissage<<"s"<<std::endl;
    std::cout<<"- temps mise a jour stripes  "<<t_t2cornerDS<<"s"<<std::endl<<std::endl;
    
    std::cout<<" temps PerformThreecorner "<<t_perform3corner<<"s  dont:"<<std::endl;
    std::cout<<"- temps initialisation "<< t_t3cornerinit<<"s"<<std::endl;
    std::cout<<"- temps cas spherique  "<<t_spherique<<"s"<<std::endl;
    std::cout<<"- temps cas torique  "<<t_torique<<"s"<<std::endl;
    std::cout<<"- temps notfilling "<<t_notfilling<<"s"<<std::endl;
    std::cout<<"- temps filling "<<t_filling<<"s"<<std::endl;
    std::cout<<"- temps mise a jour stripes  "<<t_t3cornerDS<<"s"<<std::endl<<std::endl;
    
    std::cout<<"temps PerformMore3Corner "<<t_performmore3corner<<"s dont:"<<std::endl;
    std::cout<<"-temps plate "<<t_plate << "s "<<std::endl; 
    std::cout<<"-temps approxplate "<<t_approxplate<<"s " <<std::endl;
    std::cout<<"-temps batten "<< t_batten<<"s " <<std::endl<<std::endl;
    
    std::cout <<"TEMPS DIVERS "<<std::endl;
    std::cout<<"-temps ChFi3d_sameparameter "<<t_sameparam<<"s"<<std::endl<<std::endl;
  }
#endif
  //
  // Inspect the new faces to provide sameparameter 
  // if it is necessary
  if (IsDone())
  {
    //Temporary
    /*
    Standard_Real SameParTol = Precision::Confusion();
    Standard_Integer aNbSurfaces, iF;
    TopTools_ListIteratorOfListOfShape aIt;
    //
    aNbSurfaces=myDS->NbSurfaces();
    
    for (iF=1; iF<=aNbSurfaces; ++iF) {
      const TopTools_ListOfShape& aLF=myCoup->NewFaces(iF);
      aIt.Initialize(aLF);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aF=aIt.Value();
	BRepLib::SameParameter(aF, SameParTol, Standard_True);
	ShapeFix::SameParameter(aF, Standard_False, SameParTol);
      }
    }
    */
  }
}

//=======================================================================
//function : PerformSingularCorner
//purpose  : Load vertex and degenerated edges.
//=======================================================================

void ChFi3d_Builder::PerformSingularCorner
(const Standard_Integer Index){
  ChFiDS_ListIteratorOfListOfStripe It;
  Handle(ChFiDS_Stripe) stripe;
  TopOpeBRepDS_DataStructure&  DStr = myDS->ChangeDS();
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Index);
  
  Handle(ChFiDS_SurfData) Fd;
  Standard_Integer i, Icurv;
  Standard_Integer Ivtx = 0;
  for (It.Initialize(myVDataMap(Index)), i=0; It.More(); It.Next(),i++){
    stripe = It.Value(); 
    // SurfData concerned and its CommonPoints,
    Standard_Integer sens = 0;
    Standard_Integer num = ChFi3d_IndexOfSurfData(Vtx,stripe,sens);
    Standard_Boolean isfirst = (sens == 1);
    Fd =  stripe->SetOfSurfData()->Sequence().Value(num);
    const ChFiDS_CommonPoint& CV1 = Fd->Vertex(isfirst,1);
    const ChFiDS_CommonPoint& CV2 = Fd->Vertex(isfirst,2);
    // Is it always degenerated ?
    if ( CV1.Point().IsEqual( CV2.Point(), 0) ) { 
      // if yes the vertex is stored in the stripe
      // and the edge at end is created
      if (i==0) Ivtx = ChFi3d_IndexPointInDS(CV1, DStr);
      Standard_Real tolreached;
      Standard_Real Pardeb, Parfin; 
      gp_Pnt2d VOnS1, VOnS2;
      Handle(Geom_Curve) C3d;
      Handle(Geom2d_Curve) PCurv;
      TopOpeBRepDS_Curve Crv;
      if (isfirst) {
	VOnS1 = Fd->InterferenceOnS1().PCurveOnSurf()->
		    Value(Fd->InterferenceOnS1().FirstParameter());
	VOnS2 = Fd->InterferenceOnS2().PCurveOnSurf()->
		     Value(Fd->InterferenceOnS2().FirstParameter());
      }
      else {
	VOnS1 = Fd->InterferenceOnS1().PCurveOnSurf()->
		    Value(Fd->InterferenceOnS1().LastParameter());
	VOnS2 = Fd->InterferenceOnS2().PCurveOnSurf()->
		     Value(Fd->InterferenceOnS2().LastParameter());
      }
      
      ChFi3d_ComputeArete(CV1, VOnS1,
			  CV2, VOnS2,
			  DStr.Surface(Fd->Surf()).Surface(),
			  C3d, PCurv,
			  Pardeb,Parfin,tolapp3d,tolapp2d,tolreached,0);
      Crv = TopOpeBRepDS_Curve(C3d,tolreached);
      Icurv = DStr.AddCurve(Crv);

      stripe->SetCurve(Icurv, isfirst);
      stripe->SetParameters(isfirst, Pardeb,Parfin);
      stripe->ChangePCurve(isfirst) = PCurv;
      stripe->SetIndexPoint(Ivtx, isfirst, 1);
      stripe->SetIndexPoint(Ivtx, isfirst, 2);	  
    }
  }  
}

//=======================================================================
//function : PerformFilletOnVertex
//purpose  : 
//=======================================================================

void ChFi3d_Builder::PerformFilletOnVertex
(const Standard_Integer Index){
  
  ChFiDS_ListIteratorOfListOfStripe It;
  Handle(ChFiDS_Stripe) stripe;
  Handle(ChFiDS_Spine) sp;
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Index);
  
  Handle(ChFiDS_SurfData) Fd;
  Standard_Integer i;
  Standard_Boolean nondegenere = Standard_True;
  Standard_Boolean toujoursdegenere = Standard_True; 
  Standard_Boolean isfirst = Standard_False;
  for (It.Initialize(myVDataMap(Index)), i=0; It.More(); It.Next(),i++){
    stripe = It.Value(); 
    sp = stripe->Spine();
    // SurfData and its CommonPoints,
    Standard_Integer sens = 0;
    Standard_Integer num = ChFi3d_IndexOfSurfData(Vtx,stripe,sens);
    isfirst = (sens == 1);
    Fd =  stripe->SetOfSurfData()->Sequence().Value(num);
    const ChFiDS_CommonPoint& CV1 = Fd->Vertex(isfirst,1);
    const ChFiDS_CommonPoint& CV2 = Fd->Vertex(isfirst,2);
    // Is it always degenerated ?
    if ( CV1.Point().IsEqual( CV2.Point(), 0) )  
      nondegenere = Standard_False;
    else  toujoursdegenere = Standard_False;
  }
  
  // calcul du nombre de faces = nombre d'aretes
/*  TopTools_ListIteratorOfListOfShape ItF,JtF,ItE;
  Standard_Integer nbf = 0, jf = 0;
  for (ItF.Initialize(myVFMap(Vtx)); ItF.More(); ItF.Next()){
    jf++;
    Standard_Integer kf = 1;
    const TopoDS_Shape& cur = ItF.Value();
    for (JtF.Initialize(myVFMap(Vtx)); JtF.More() && (kf < jf); JtF.Next(), kf++){
      if(cur.IsSame(JtF.Value())) break;
    }
    if(kf == jf) nbf++;
  }
  Standard_Integer nba=myVEMap(Vtx).Extent();
  for (ItE.Initialize(myVEMap(Vtx)); ItE.More(); ItE.Next()){
    const TopoDS_Edge& cur = TopoDS::Edge(ItE.Value());
    if (BRep_Tool::Degenerated(cur)) nba--;
  }
  nba=nba/2;*/
  Standard_Integer nba = ChFi3d_NumberOfSharpEdges(Vtx, myVEMap, myEFMap);

  if (nondegenere) { // Normal processing
    switch (i) {
    case 1 : 
      {
	if(sp->Status(isfirst) == ChFiDS_FreeBoundary) return; 
	if(nba>3) {
#ifdef OCCT_DEBUG //perf    
	  ChFi3d_InitChron(cl_performatend);
#endif
	  PerformIntersectionAtEnd(Index);
#ifdef OCCT_DEBUG
	  ChFi3d_ResultChron(cl_performatend,t_performatend);
#endif 
	}
	else { 
#ifdef OCCT_DEBUG //perf    
	  ChFi3d_InitChron(cl_perform1corner);
#endif
          if (MoreSurfdata(Index))
             PerformMoreSurfdata(Index);
	  else PerformOneCorner(Index);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_perform1corner,t_perform1corner);
#endif  
	}
      }
      break;
    case 2 : 
      {
	if(nba>3){
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_InitChron(cl_performmore3corner);
#endif
	  PerformMoreThreeCorner(Index, i);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_performmore3corner,t_performmore3corner);
#endif
	}
	else { 
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_InitChron(cl_perform2corner);
#endif
	  PerformTwoCorner(Index);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_perform2corner,t_perform2corner);
#endif
	}
      }
      break;
    case 3 : 
      {
	if(nba>3){
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_InitChron(cl_performmore3corner);
#endif
	  PerformMoreThreeCorner(Index, i);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_performmore3corner,t_performmore3corner);
#endif
	}
	else {
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_InitChron(cl_perform3corner);
#endif
	  PerformThreeCorner(Index);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_perform3corner,t_perform3corner);
#endif
	}
      }
      break;
      default : {
#ifdef OCCT_DEBUG //perf 
	ChFi3d_InitChron(cl_performmore3corner);
#endif
	PerformMoreThreeCorner(Index, i);
#ifdef OCCT_DEBUG //perf 
	ChFi3d_ResultChron(cl_performmore3corner,t_performmore3corner);
#endif
      }
    }
  }
  else { // Single case processing
    if (toujoursdegenere) PerformSingularCorner(Index);
    else                  PerformMoreThreeCorner(Index, i);//Last chance...
  }              
}


//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::Reset()
{
  done = Standard_False;
  myVDataMap.Clear();
  myRegul.Clear();
  myEVIMap.Clear();
  badstripes.Clear();
  badvertices.Clear();

  ChFiDS_ListIteratorOfListOfStripe itel;
  for (itel.Initialize(myListStripe); itel.More(); ){
    if(!itel.Value()->Spine().IsNull()){
      itel.Value()->Reset();
      itel.Next();
    }
    else myListStripe.Remove(itel);
  }
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& ChFi3d_Builder::Generated(const TopoDS_Shape& EouV)
{
  myGenerated.Clear();
  if(EouV.IsNull()) return myGenerated;
  if(EouV.ShapeType() != TopAbs_EDGE &&
     EouV.ShapeType() != TopAbs_VERTEX) return myGenerated;
  if(myEVIMap.IsBound(EouV)) {
    const TColStd_ListOfInteger& L = myEVIMap.Find(EouV);
    TColStd_ListIteratorOfListOfInteger IL;
    for(IL.Initialize(L); IL.More(); IL.Next()){
      Standard_Integer I = IL.Value();
      //const TopTools_ListOfShape& LS =  myCoup->NewFaces(I);
      const TopoDS_Face& aNewFace = TopoDS::Face(myNewFaces(I));
      /*
      TopTools_ListIteratorOfListOfShape ILS;
      for(ILS.Initialize(LS); ILS.More(); ILS.Next())
	myGenerated.Append(ILS.Value());
      */
      myGenerated.Append(aNewFace);
    }
  }
  return myGenerated;
}



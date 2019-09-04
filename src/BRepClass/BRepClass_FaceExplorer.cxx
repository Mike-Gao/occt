// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

//  Modified by skv - Thu Jul 13 17:42:58 2006 OCC12627
//  Total rewriting of the method Segment; add the method OtherSegment.

#include <BRep_Tool.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>

static const Standard_Real Probing_Start = 0.123;
static const Standard_Real Probing_End = 0.7;
static const Standard_Real Probing_Step = 0.2111;

//=======================================================================
//function : BRepClass_FExp_DistanceTool
//purpose  : Tool for checking the ON status of a point for the face
//           using the real tolerances of sub-shapes of the latter.
//=======================================================================
#include <BVH_BoxSet.hxx>
#include <BVH_Distance.hxx>
#include <BVH_Tools.hxx>
#include <BVH_LinearBuilder.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRepBndLib.hxx>
#include <TopExp.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_Tools.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BndLib_Add3dCurve.hxx>

class BRepClass_FExp_DistanceTool:
  public BVH_Distance <Standard_Real, 3, NCollection_Vec3<Standard_Real>,
                       BVH_BoxSet<Standard_Real, 3, TopoDS_Shape> >
{
public:

  //! Empty constructor
  BRepClass_FExp_DistanceTool()
    : BVH_Distance <Standard_Real, 3, NCollection_Vec3<Standard_Real>, BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>>()
  {
  }

public: //! @name Setters

  void SetPoint (const gp_Pnt& thePoint)
  {
    SetObject (NCollection_Vec3<Standard_Real> (thePoint.X(), thePoint.Y(), thePoint.Z()));
  }

public: //! @name Setters

  const TopoDS_Shape& GetOnShape() const
  {
    return myOnShape;
  }

public: //! @name Clearing the results

    //! Returns the flag controlling the tree descend
  virtual void Clear() Standard_OVERRIDE
  {
    myDistance = 0.0;
    myIsDone = Standard_False;
    myOnShape.Nullify();
  }

public: //! @name Definition of the rules for tree descend

  // Computes the distance from the point to bounding box 
  virtual Standard_Boolean RejectNode (const BVH_Vec3d& theCMin,
                                       const BVH_Vec3d& theCMax,
                                       Standard_Real& theDistance) const Standard_OVERRIDE
  {
    theDistance = BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance (myObject, theCMin, theCMax);
    return RejectMetric (theDistance);
  }

  // Computes the distance from the point to triangle
  virtual Standard_Boolean Accept (const Standard_Integer theIndex,
                                   const Standard_Real&) Standard_OVERRIDE
  {
    if (myBVHSet->Box (theIndex).IsOut (myObject))
      return Standard_False;

    // Get the shape
    const TopoDS_Shape& aS = myBVHSet->Element (theIndex);

    if (aS.ShapeType() == TopAbs_VERTEX)
    {
      const TopoDS_Vertex& aV = TopoDS::Vertex (aS);
      gp_XYZ aP = BRep_Tool::Pnt (aV).XYZ();
      Standard_Real aTolV = BRep_Tool::Tolerance (aV);
      Standard_Real aSqTol = aTolV * aTolV;

      Standard_Real aSqDist = (aP - gp_XYZ (myObject.x(), myObject.y(), myObject.z())).SquareModulus();
      if (aSqDist <= aSqTol)
      {
        myOnShape = aS;
        return Standard_True;
      }
    }
    else if (aS.ShapeType() == TopAbs_EDGE)
    {
      const TopoDS_Edge& aE = TopoDS::Edge (aS);
      Standard_Real aFirst, aLast;
      const Handle (Geom_Curve)& aC = BRep_Tool::Curve (aE, aFirst, aLast);
      if (aC.IsNull())
        return Standard_False;

      GeomAPI_ProjectPointOnCurve aProjPC
        (gp_Pnt (myObject.x(), myObject.y(), myObject.z()), aC, aFirst, aLast);
      if (aProjPC.NbPoints() > 0)
      {
        if (aProjPC.LowerDistance() < BRep_Tool::Tolerance (aE))
        {
          myOnShape = aS;
          return Standard_True;
        }
      }
    }
    return Standard_False;
  }

  //! Returns the flag controlling the tree descend
  virtual Standard_Boolean Stop() const Standard_OVERRIDE
  {
    return !myOnShape.IsNull();
  }

private: //! @name Fields

  TopoDS_Shape myOnShape;
};

//=======================================================================
//function : BRepClass_FaceExplorer
//purpose  : 
//=======================================================================

BRepClass_FaceExplorer::BRepClass_FaceExplorer(const TopoDS_Face& F) :
       myFace(F),
       myCurEdgeInd(1),
       myCurEdgePar(Probing_Start),
       myUMin (Precision::Infinite()),
       myUMax (-Precision::Infinite()),
       myVMin (Precision::Infinite()),
       myVMax (-Precision::Infinite())
{
  myFace.Orientation(TopAbs_FORWARD);
}

//=======================================================================
//function : ComputeFaceBounds
//purpose  : 
//=======================================================================
void BRepClass_FaceExplorer::ComputeFaceBounds()
{
  TopLoc_Location aLocation;
  const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface (myFace, aLocation);
  aSurface->Bounds (myUMin, myUMax, myVMin, myVMax);
  if (Precision::IsInfinite (myUMin) || Precision::IsInfinite (myUMax) ||
      Precision::IsInfinite (myVMin) || Precision::IsInfinite (myVMax))
  {
    BRepTools::UVBounds(myFace, myUMin, myUMax, myVMin, myVMax);
  }
}

//=======================================================================
//function : CheckPoint
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass_FaceExplorer::CheckPoint(gp_Pnt2d& thePoint)
{
  if (myUMin > myUMax)
  {
    ComputeFaceBounds();
  }

  if (Precision::IsInfinite(myUMin) || Precision::IsInfinite(myUMax) ||
      Precision::IsInfinite(myVMin) || Precision::IsInfinite(myVMax))
  {
    return Standard_True;
  }

  gp_Pnt2d aCenterPnt(( myUMin + myUMax ) / 2, ( myVMin + myVMax ) / 2);
  Standard_Real aDistance = aCenterPnt.Distance(thePoint);
  if (Precision::IsInfinite(aDistance))
  {
    thePoint.SetCoord (myUMin - (myUMax - myUMin ),
                       myVMin - (myVMax - myVMin ));
    return Standard_False;
  }
  else
  {
    Standard_Real anEpsilon = Epsilon(aDistance);
    if (anEpsilon > Max (myUMax - myUMin, myVMax - myVMin))
    {
      gp_Vec2d aLinVec(aCenterPnt, thePoint);
      gp_Dir2d aLinDir(aLinVec);
      thePoint = aCenterPnt.XY() + aLinDir.XY() * ( 2. * anEpsilon );
      return Standard_False;
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================

Standard_Boolean  BRepClass_FaceExplorer::Reject(const gp_Pnt2d&)const 
{
  return Standard_False;
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass_FaceExplorer::Segment(const gp_Pnt2d& P, 
						 gp_Lin2d& L,
						 Standard_Real& Par)
{
  myCurEdgeInd = 1;
  myCurEdgePar = Probing_Start;

  return OtherSegment(P, L, Par);
}

//=======================================================================
//function : OtherSegment
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass_FaceExplorer::OtherSegment(const gp_Pnt2d& P, 
						      gp_Lin2d& L,
						      Standard_Real& Par)
{
  TopExp_Explorer      anExpF(myFace,TopAbs_EDGE);
  Standard_Integer     i;
  Standard_Real        aFPar;
  Standard_Real        aLPar;
  Handle(Geom2d_Curve) aC2d;
  Standard_Real        aTolParConf2 = Precision::PConfusion() * Precision::PConfusion();
  gp_Pnt2d             aPOnC;
  Standard_Real        aParamIn;

  for (i = 1; anExpF.More(); anExpF.Next(), i++) {
    if (i != myCurEdgeInd)
      continue;

    const TopoDS_Shape       &aLocalShape   = anExpF.Current();
    const TopAbs_Orientation  anOrientation = aLocalShape.Orientation();

    if (anOrientation == TopAbs_FORWARD || anOrientation == TopAbs_REVERSED) {
      const TopoDS_Edge &anEdge = TopoDS::Edge(aLocalShape);

      aC2d = BRep_Tool::CurveOnSurface(anEdge, myFace, aFPar, aLPar);

      if (!aC2d.IsNull()) {
	// Treatment of infinite cases.
	if (Precision::IsNegativeInfinite(aFPar)) {
	  if (Precision::IsPositiveInfinite(aLPar)) {
	    aFPar = -1.;
	    aLPar =  1.;
	  } else {
	    aFPar = aLPar - 1.;
	  }
	} else if (Precision::IsPositiveInfinite(aLPar))
	  aLPar = aFPar + 1.;

	for (; myCurEdgePar < Probing_End ;myCurEdgePar += Probing_Step) {
	  aParamIn = myCurEdgePar*aFPar + (1. - myCurEdgePar)*aLPar;

          gp_Vec2d aTanVec;
	  aC2d->D1(aParamIn, aPOnC, aTanVec);
	  Par = aPOnC.SquareDistance(P);

	  if (Par > aTolParConf2) {
	    gp_Vec2d aLinVec(P, aPOnC);
	    gp_Dir2d aLinDir(aLinVec);

            Standard_Real aTanMod = aTanVec.SquareMagnitude();
            if (aTanMod < aTolParConf2)
              continue;
            aTanVec /= Sqrt(aTanMod);
            Standard_Real aSinA = aTanVec.Crossed(aLinDir.XY());
            const Standard_Real SmallAngle = 0.001;
            if (Abs(aSinA) < SmallAngle)
            {
              // The line from the input point P to the current point on edge
              // is tangent to the edge curve. This condition is bad for classification.
              // Therefore try to go to another point in the hope that there will be 
              // no tangent. If there tangent is preserved then leave the last point in 
              // order to get this edge chanse to participate in classification.
              if (myCurEdgePar + Probing_Step < Probing_End)
                continue;
            }

	    L = gp_Lin2d(P, aLinDir);

	    // Check if ends of a curve lie on a line.
	    aC2d->D0(aFPar, aPOnC);

	    if (L.SquareDistance(aPOnC) > aTolParConf2) {
	      aC2d->D0(aLPar, aPOnC);

	      if (L.SquareDistance(aPOnC) > aTolParConf2) {
		myCurEdgePar += Probing_Step;

		if (myCurEdgePar >= Probing_End) {
		  myCurEdgeInd++;
		  myCurEdgePar = Probing_Start;
		}

		Par = Sqrt(Par);
		return Standard_True;
	      }
	    }
	  }
	}
      } // if (!aC2d.IsNull()) {
    } // if (anOrientation == TopAbs_FORWARD ...

    // This curve is not valid for line construction. Go to another edge.
    myCurEdgeInd++;
    myCurEdgePar = Probing_Start;
  }

  // nothing found, return an horizontal line
  Par = RealLast();
  L   = gp_Lin2d(P,gp_Dir2d(1,0));

  return Standard_False;
}

//=======================================================================
//function : InitWires
//purpose  : 
//=======================================================================

void  BRepClass_FaceExplorer::InitWires()
{
  myWExplorer.Init(myFace,TopAbs_WIRE);
}

//=======================================================================
//function : RejectWire NYI
//purpose  : 
//=======================================================================

Standard_Boolean  BRepClass_FaceExplorer::RejectWire
  (const gp_Lin2d& , 
   const Standard_Real)const 
{
  return Standard_False;
}

//=======================================================================
//function : InitEdges
//purpose  : 
//=======================================================================

void  BRepClass_FaceExplorer::InitEdges()
{
  myEExplorer.Init(myWExplorer.Current(),TopAbs_EDGE);
}

//=======================================================================
//function : RejectEdge NYI
//purpose  : 
//=======================================================================

Standard_Boolean  BRepClass_FaceExplorer::RejectEdge
  (const gp_Lin2d& , 
   const Standard_Real )const 
{
  return Standard_False;
}


//=======================================================================
//function : CurrentEdge
//purpose  : 
//=======================================================================

void  BRepClass_FaceExplorer::CurrentEdge(BRepClass_Edge& E, 
					  TopAbs_Orientation& Or) const 
{
  E.Edge() = TopoDS::Edge(myEExplorer.Current());
  E.Face() = myFace;
  Or = E.Edge().Orientation();
}

//=======================================================================
//function : IsPointOnFace
//purpose  : 
//=======================================================================
const opencascade::handle<BVH_BoxSet<Standard_Real, 3, TopoDS_Shape>>& 
  BRepClass_FaceExplorer::BVHBoxSet (Standard_Boolean toBuild) const
{
  if (toBuild && myBVHSet.IsNull())
  {
    myBVHSet = new BVH_BoxSet<Standard_Real, 3, TopoDS_Shape> (new BVH_LinearBuilder<Standard_Real, 3>());

    for (TopExp_Explorer anExpV (myFace, TopAbs_VERTEX); anExpV.More(); anExpV.Next())
    {
      const TopoDS_Vertex& aV = TopoDS::Vertex (anExpV.Current());
      const TopAbs_Orientation aVOri = aV.Orientation();
      if (aVOri == TopAbs_FORWARD || aVOri == TopAbs_REVERSED)
      {
        Bnd_Box aBox;
        aBox.Add (BRep_Tool::Pnt (aV));
        aBox.Enlarge (BRep_Tool::Tolerance (aV));
        myBVHSet->Add (anExpV.Current(), Bnd_Tools::Bnd2BVH (aBox));
      }
    }

    for (TopExp_Explorer anExpE (myFace, TopAbs_EDGE); anExpE.More(); anExpE.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge (anExpE.Current());
      const TopAbs_Orientation aEOri = aE.Orientation();
      if (aEOri == TopAbs_FORWARD || aEOri == TopAbs_REVERSED)
      {
        BRepAdaptor_Curve aBAC (aE);
        Bnd_Box aBox;
        BndLib_Add3dCurve::Add (aBAC, aBAC.Tolerance(), aBox);
        myBVHSet->Add (anExpE.Current(), Bnd_Tools::Bnd2BVH (aBox));
      }
    }
    myBVHSet->Build();
  }
  return myBVHSet;
}


//=======================================================================
//function : IsPointOnFace
//purpose  : 
//=======================================================================
Standard_Boolean BRepClass_FaceExplorer::IsPointOnFace (const gp_Pnt2d& thePoint) const
{
  const Handle (Geom_Surface)& aSurface = BRep_Tool::Surface (myFace);
  gp_Pnt aP3d = aSurface->Value (thePoint.X(), thePoint.Y());
  return IsPointOnFace (aP3d);
}

//=======================================================================
//function : IsPointOnFace
//purpose  : 
//=======================================================================
Standard_Boolean BRepClass_FaceExplorer::IsPointOnFace (const gp_Pnt& thePoint) const
{
  BRepClass_FExp_DistanceTool aDistTool;
  aDistTool.SetBVHSet (BVHBoxSet().get());
  aDistTool.SetPoint (thePoint);
  aDistTool.ComputeDistance();
  return aDistTool.IsDone();
}

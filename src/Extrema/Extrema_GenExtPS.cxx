// Created on: 1995-07-18
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

#include <Extrema_GenExtPS.hxx>

#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_Tools.hxx>
#include <BndLib_AddSurface.hxx>
#include <BVH_LinearBuilder.hxx>
#include <BVH_Tools.hxx>
#include <GCPnts.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <math_FunctionSetRoot.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT (Extrema_GenExtPS, Standard_Transient)

// Static methods definition
namespace
{
  //=======================================================================
  //function : fillParams
  //purpose  : 
  //=======================================================================
  static void fillParams (const TColStd_Array1OfReal& theKnots,
                          const Standard_Integer theDegree,
                          const Standard_Real theParMin,
                          const Standard_Real theParMax,
                          Standard_Integer theNbSamples,
                          Handle (TColStd_HArray1OfReal)& theParams)
  {
    NCollection_Vector<Standard_Real> aParams;
    GCPnts::FillParams (theKnots, theDegree, theParMin, theParMax, aParams);
    Standard_Integer nbPar = aParams.Length();

    // In case of an insufficient number of points the grid will be built later 
    if (nbPar < theNbSamples)
      return;

    theParams = new TColStd_HArray1OfReal (1, nbPar);
    for (Standard_Integer i = 0; i < nbPar; i++)
      theParams->SetValue (i + 1, aParams (i));
  }

  //=======================================================================
  //function : fillParams
  //purpose  : 
  //=======================================================================
  static void fillParams (Standard_Real theParMin,
                          Standard_Real theParMax,
                          Handle (TColStd_HArray1OfReal)& theParams,
                          Standard_Integer theNbSamples)
  {
    Standard_Real PasU = theParMax - theParMin;
    Standard_Real U0 = PasU / theNbSamples / 100.;
    PasU = (PasU - U0) / (theNbSamples - 1);
    U0 = U0 / 2. + theParMin;
    theParams = new TColStd_HArray1OfReal (1, theNbSamples);
    Standard_Real U = U0;
    for (int NoU = 1; NoU <= theNbSamples; NoU++, U += PasU)
      theParams->SetValue (NoU, U);
  }
}

//=======================================================================
//function : Extrema_GenExtPS
//purpose  : 
//=======================================================================
Extrema_GenExtPS::Extrema_GenExtPS()
  :
  myUMin (RealLast()),
  myUMax (RealFirst()),
  myVMin (RealLast()),
  myVMax (RealFirst()),
  myNbUSamples (0),
  myNbVSamples (0),
  myTolU (Precision::PConfusion()),
  myTolV (Precision::PConfusion()),
  myLocUMin (RealLast()),
  myLocUMax (RealFirst()),
  myLocVMin (RealLast()),
  myLocVMax (RealFirst()),
  myTarget (Extrema_ExtFlag_MINMAX),
  mySqDistance (-1),
  myIsDone (Standard_False)
{
}

//=======================================================================
//function : Extrema_GenExtPS
//purpose  : 
//=======================================================================
Extrema_GenExtPS::Extrema_GenExtPS (const gp_Pnt&            theP,
                                    const Adaptor3d_Surface& theS,
                                    const Standard_Integer   theNbU,
                                    const Standard_Integer   theNbV,
                                    const Standard_Real      theTolU,
                                    const Standard_Real      theTolV,
                                    const Extrema_ExtFlag    theTarget)
  :
  myNbUSamples (theNbU),
  myNbVSamples (theNbV),
  myTolU (theTolU),
  myTolV (theTolV),
  myTarget (theTarget),
  mySqDistance (-1),
  myIsDone (Standard_False)
{
  Initialize (theS, theNbU, theNbV, theTolU, theTolV);
  Perform (theP);
}

//=======================================================================
//function : Extrema_GenExtPS
//purpose  : 
//=======================================================================
Extrema_GenExtPS::Extrema_GenExtPS (const gp_Pnt&            theP,
                                    const Adaptor3d_Surface& theS,
                                    const Standard_Integer   theNbU,
                                    const Standard_Integer   theNbV,
                                    const Standard_Real      theUMin,
                                    const Standard_Real      theUMax,
                                    const Standard_Real      theVMin,
                                    const Standard_Real      theVMax,
                                    const Standard_Real      theTolU,
                                    const Standard_Real      theTolV,
                                    const Extrema_ExtFlag    theTarget)
  :
  myUMin (theUMin),
  myUMax (theUMax),
  myVMin (theVMin),
  myVMax (theVMax),
  myNbUSamples (theNbU),
  myNbVSamples (theNbV),
  myTolU (theTolU),
  myTolV (theTolV),
  myTarget (theTarget),
  mySqDistance (-1),
  myIsDone (Standard_False)
{
  Initialize (theS, theNbU, theNbV, theUMin, theUMax, theVMin, theVMax, theTolU, theTolV);
  Perform (theP);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void Extrema_GenExtPS::Initialize (const Adaptor3d_Surface& theS,
                                   const Standard_Integer   theNbU,
                                   const Standard_Integer   theNbV,
                                   const Standard_Real      theTolU,
                                   const Standard_Real      theTolV)
{
  Initialize (theS,
              theNbU, theNbV,
              theS.FirstUParameter(), theS.LastUParameter(),
              theS.FirstVParameter(), theS.LastVParameter(),
              theTolU, theTolV);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void Extrema_GenExtPS::Initialize (const Adaptor3d_Surface& theS,
                                   const Standard_Integer   theNbU,
                                   const Standard_Integer   theNbV,
                                   const Standard_Real      theUMin,
                                   const Standard_Real      theUMax,
                                   const Standard_Real      theVMin,
                                   const Standard_Real      theVMax,
                                   const Standard_Real      theTolU,
                                   const Standard_Real      theTolV)
{
  myS = (Adaptor3d_SurfacePtr)&theS;
  myUMin = theUMin;
  myUMax = theUMax;
  myVMin = theVMin;
  myVMax = theVMax;
  myNbUSamples = theNbU;
  myNbVSamples = theNbV;
  myTolU = theTolU;
  myTolV = theTolV;

  myLocUMin = myUMin;
  myLocUMax = myUMax;
  myLocVMin = myVMin;
  myLocVMax = myVMax;

  if (myNbUSamples < 2 || myNbVSamples < 2)
  {
    throw Standard_OutOfRange ("Extrema_GenExtPS::Initialize - number of samples is too small");
  }

  myF.Initialize (theS);

  myBVHBoxSet.Nullify();
  myUParams.Nullify();
  myVParams.Nullify();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void Extrema_GenExtPS::Perform (const gp_Pnt& thePoint)
{
  Perform (thePoint, myUMin, myUMax, myVMin, myVMax);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void Extrema_GenExtPS::Perform (const gp_Pnt& thePoint,
                                const Standard_Real theUMin,
                                const Standard_Real theUMax,
                                const Standard_Real theVMin,
                                const Standard_Real theVMax)
{
  myIsDone = Standard_False;
  myPoint = thePoint;
  myF.SetPoint (thePoint, myTarget);
  mySolutions.clear();

  myLocUMin = Max (theUMin, myUMin);
  myLocUMax = Min (theUMax, myUMax);
  myLocVMin = Max (theVMin, myVMin);
  myLocVMax = Min (theVMax, myVMax);

  if (myUParams.IsNull() || myVParams.IsNull())
  {
    // Prepare surface sampling
    BuildGrid();
  }

  if (myBVHBoxSet.IsNull() || myBVHBoxSet->IsDirty())
  {
    BuildTree();
  }

  if (myTarget == Extrema_ExtFlag_MIN)
    mySqDistance = RealLast();
  else if (myTarget == Extrema_ExtFlag_MAX)
    mySqDistance = -1;

  // Fill Square distances with default values
  for (int iU = 0; iU <= myNbUSamples + 1; ++iU)
  {
    for (int iV = 0; iV <= myNbVSamples + 1; ++iV)
    {
      myPoints->ChangeValue (iU, iV).SetSqrDistance (-1.);

      if (iU <= myNbUSamples && iV <= myNbVSamples)
        myFacePntParams->ChangeValue (iU, iV).SetSqrDistance (-1.);

      if (iU > 0 && iU <= myNbUSamples && iV > 0 && iV <= myNbVSamples)
      {
        if (iU < myNbUSamples)
          myUEdgePntParams->ChangeValue (iU, iV).SetSqrDistance (-1.);
        if (iV < myNbVSamples)
          myVEdgePntParams->ChangeValue (iU, iV).SetSqrDistance (-1.);
      }
    }
  }

  if (myTarget == Extrema_ExtFlag_MIN || myTarget == Extrema_ExtFlag_MINMAX)
  {
    // Fill boundary with RealLast square distance.
    for (int iV = 0; iV <= myNbVSamples; ++iV)
    {
      myFacePntParams->ChangeValue (0, iV).SetSqrDistance (RealLast());
      myFacePntParams->ChangeValue (myNbUSamples, iV).SetSqrDistance (RealLast());
    }

    for (int iU = 1; iU < myNbUSamples; ++iU)
    {
      myFacePntParams->ChangeValue (iU, 0).SetSqrDistance (RealLast());
      myFacePntParams->ChangeValue (iU, myNbVSamples).SetSqrDistance (RealLast());
    }
  }

  if (myTarget == Extrema_ExtFlag_MINMAX)
  {
    for (int iU = 1; iU <= myNbUSamples; ++iU)
    {
      for (int iV = 1; iV <= myNbVSamples; ++iV)
      {
        Extrema_POnSurfParams& aParam = myPoints->ChangeValue (iU, iV);
        aParam.SetSqrDistance (aParam.Value().SquareDistance (thePoint));
      }
    }

    // Perform standard solution search (no tree)
    for (Standard_Integer iTarget = 0; iTarget < 2; ++iTarget)
    {
      const Extrema_ExtFlag aTarget = static_cast<Extrema_ExtFlag> (iTarget);
      for (Standard_Integer iU = 1; iU <= myNbUSamples; iU++)
      {
        for (Standard_Integer iV = 1; iV <= myNbVSamples; iV++)
        {
          FindSolution (iU, iV, aTarget);
        }
      }
    }
    // Copy solutions
    for (Standard_Integer i = 1; i <= myF.NbExt(); ++i)
    {
      mySolutions.push_back (Extrema_GenExtPS_ExtPSResult (myF.Point (i), myF.SquareDistance (i)));
    }
  }
  else
  {
    // Indicator for high-level BVH traverse
    SetBVHSet (NULL);
    Select (myBVHBoxSet->BVH());

    // Copy solutions
    for (Standard_Integer i = 1; i <= myF.NbExt(); ++i)
    {
      if (Abs (mySqDistance - myF.SquareDistance (i)) < Precision::SquareConfusion())
        mySolutions.push_back (Extrema_GenExtPS_ExtPSResult (myF.Point (i), myF.SquareDistance (i)));
    }
  }

  // Sort solutions
  std::sort (mySolutions.begin(), mySolutions.end());
}

//=======================================================================
//function : BuildGrid
//purpose  : 
//=======================================================================
void Extrema_GenExtPS::BuildGrid()
{
  GetGridPoints (*myS);

  //build grid in other cases 
  if (myUParams.IsNull())
  {
    fillParams (myUMin, myUMax, myUParams, myNbUSamples);
  }

  if (myVParams.IsNull())
  {
    fillParams (myVMin, myVMax, myVParams, myNbVSamples);
  }

  myPoints = new Extrema_HArray2OfPOnSurfParams (0, myNbUSamples + 1, 0, myNbVSamples + 1);
  for (int iU = 1; iU <= myNbUSamples; iU++)
  {
    Standard_Real U = myUParams->Value (iU);
    for (int iV = 1; iV <= myNbVSamples; iV++)
    {
      Standard_Real V = myVParams->Value (iV);
      gp_Pnt aP = myS->Value (U, V);
      Extrema_POnSurfParams aParam (U, V, aP);
      aParam.SetElementType (Extrema_Node);
      aParam.SetIndices (iU, iV);
      myPoints->SetValue (iU, iV, aParam);
    }
  }

  myFacePntParams = new Extrema_HArray2OfPOnSurfParams (0, myNbUSamples, 0, myNbVSamples);

  myUEdgePntParams = new Extrema_HArray2OfPOnSurfParams (1, myNbUSamples - 1, 1, myNbVSamples);
  myVEdgePntParams = new Extrema_HArray2OfPOnSurfParams (1, myNbUSamples, 1, myNbVSamples - 1);
}

//=======================================================================
//function : BuildTree
//purpose  : 
//=======================================================================
void Extrema_GenExtPS::BuildTree()
{
  // The tree is not required for MINMAX mode
  if (myTarget == Extrema_ExtFlag_MINMAX)
    return;

  // so fill the tree with elements with empty boxes
  if (myBVHBoxSet.IsNull())
  {
    // Builder for low-level BVH sets
    opencascade::handle<BVH_LinearBuilder<Standard_Real, 3> > aLBuilder = new BVH_LinearBuilder<Standard_Real, 3>();

    myBVHBoxSet = new BVH_IndexedBoxSet<Standard_Real, 3, Extrema_GenExtPS_LocalizedGrid> (
      new BVH_LinearBuilder<Standard_Real, 3> (BVH_Constants_LeafNodeSizeSingle));

    // create hierarchy of BVH trees
    const Standard_Integer aCoeff = static_cast<Standard_Integer>(Sqrt (Min (myNbUSamples, myNbVSamples)));
    const Standard_Integer aNbUT = myNbUSamples / aCoeff;
    const Standard_Integer aNbVT = myNbVSamples / aCoeff;
    const Standard_Integer aNbU = myNbUSamples / aNbUT;
    const Standard_Integer aNbV = myNbVSamples / aNbVT;
    const Standard_Integer aSize = aNbU * aNbV;

    myBVHBoxSet->SetSize (aNbUT * aNbVT);

    Standard_Integer U1 = 1, V1 = 1, U2 = 1, V2 = 1;
    for (Standard_Integer iUT = 1; iUT <= aNbUT; ++iUT)
    {
      U2 = (iUT == aNbUT) ? myNbUSamples : iUT * aNbU;
      for (Standard_Integer iVT = 1; iVT <= aNbVT; ++iVT)
      {
        Handle (Extrema_GenExtPS_GridCellBoxSet) aGridSet = new Extrema_GenExtPS_GridCellBoxSet (aLBuilder);
        aGridSet->SetSize (aSize);

        V2 = (iVT == aNbVT) ? myNbVSamples : iVT * aNbV;
        for (Standard_Integer iU = U1; iU <= U2; ++iU)
        {
          for (Standard_Integer iV = V1; iV <= V2; ++iV)
          {
            aGridSet->Add (Extrema_GenExtPS_GridCell (iU, iV), BVH_Box<Standard_Real, 3>());
          }
        }
        myBVHBoxSet->Add (Extrema_GenExtPS_LocalizedGrid (U1, U2, V1, V2, aGridSet), BVH_Box<Standard_Real, 3>());
        V1 = V2 + 1;
      }
      U1 = U2 + 1;
      V1 = 1;
    }
  }

  if (myBVHBoxSet->IsDirty())
  {
    // Fill the tree with the real boxes
    const Standard_Integer aNbSets = myBVHBoxSet->Size();
    for (Standard_Integer iSet = 0; iSet < aNbSets; ++iSet)
    {
      Extrema_GenExtPS_LocalizedGrid aGridSet = myBVHBoxSet->Element (iSet);

      // Box of the set
      Bnd_Box aSetBox;
      const Standard_Integer aNbCells = aGridSet.CellBoxSet->Size();
      for (Standard_Integer iCell = 0; iCell < aNbCells; ++iCell)
      {
        const Extrema_GenExtPS_GridCell& aCell = aGridSet.CellBoxSet->Element (iCell);
        Standard_Integer iU = aCell.UIndex, iV = aCell.VIndex;

        Standard_Integer aUCoeff = (iU < myNbUSamples) ? 1 : 0;
        Standard_Integer aVCoeff = (iV < myNbVSamples) ? 1 : 0;

        // Build box for the cell
        Bnd_Box aGridBox;
        for (int i = 0; i < 2; ++i)
          for (int j = 0; j < 2; ++j)
            aGridBox.Add (myPoints->Value (iU + i * aUCoeff, iV + j * aVCoeff).Value());
        aGridBox.Enlarge (Precision::Confusion());

        const Extrema_POnSurf& aPMin = myPoints->Value (iU, iV);
        const Extrema_POnSurf& aPMax = myPoints->Value (iU + aUCoeff, iV + aVCoeff);

        Standard_Real U1, V1, U2, V2;
        aPMin.Parameter (U1, V1);
        aPMax.Parameter (U2, V2);

        // Enlarge box to make sure the whole cell is covered
        if (U1 != U2 || V1 != V2)
        {
          gp_Pnt aPMid = myS->Value ((U1 + U2) * 0.5, (V1 + V2) * 0.5);

          gp_Vec aDir (aPMin.Value(), aPMax.Value());
          Standard_Real diag = aDir.SquareMagnitude();
          if (diag > gp::Resolution())
          {
            //aDir /= Sqrt (diag);
            //gp_XYZ aPL = aPMin.Value().XYZ() + ((aPMid.XYZ() - aPMin.Value().XYZ()).Dot (aDir.XYZ())) * aDir.XYZ();
            //gp_XYZ aVMid (aPL - aPMid.XYZ());
            //aGridBox.Add (gp_Pnt (aPL - 1.1 * aVMid));
            //aGridBox.Add (gp_Pnt (aPL + 1.1 * aVMid));
            aGridBox.Enlarge (gp_Lin (aPMin.Value(), aDir).Distance (aPMid));
          }
          else
          {
            aGridBox.Add (aPMid);
          }
        }
        aGridSet.CellBoxSet->UpdateBox (iCell, Bnd_Tools::Bnd2BVH (aGridBox));

        aSetBox.Add (aGridBox);
      }
      myBVHBoxSet->UpdateBox (iSet, Bnd_Tools::Bnd2BVH (aSetBox));
    }

    // Build only high level BVH tree
    myBVHBoxSet->Build();
  }
}

//=======================================================================
//function : GetGridPoints
//purpose  : 
//=======================================================================
void Extrema_GenExtPS::GetGridPoints (const Adaptor3d_Surface& theSurf)
{
  // Creation parametric points for BSpline and Bezier surfaces
  // taking into account of Degree and NbKnots of BSpline or Bezier geometry
  if (theSurf.GetType() == GeomAbs_OffsetSurface)
    GetGridPoints (theSurf.BasisSurface()->Surface());

  // Parametric points for BSpline surfaces
  else if (theSurf.GetType() == GeomAbs_BSplineSurface)
  {
    Handle (Geom_BSplineSurface) aBspl = theSurf.BSpline();
    if (!aBspl.IsNull())
    {
      TColStd_Array1OfReal aUKnots (1, aBspl->NbUKnots());
      aBspl->UKnots (aUKnots);
      TColStd_Array1OfReal aVKnots (1, aBspl->NbVKnots());
      aBspl->VKnots (aVKnots);
      fillParams (aUKnots, aBspl->UDegree(), myUMin, myUMax, myNbUSamples, myUParams);
      fillParams (aVKnots, aBspl->VDegree(), myVMin, myVMax, myNbVSamples, myVParams);
    }
  }

  // Calculation parametric points for Bezier surfaces
  else if (theSurf.GetType() == GeomAbs_BezierSurface)
  {
    Handle (Geom_BezierSurface) aBezier = theSurf.Bezier();
    if (!aBezier.IsNull())
    {
      TColStd_Array1OfReal aUKnots (1, 2);
      TColStd_Array1OfReal aVKnots (1, 2);
      aBezier->Bounds (aUKnots (1), aUKnots (2), aVKnots (1), aVKnots (2));
      fillParams (aUKnots, aBezier->UDegree(), myUMin, myUMax, myNbUSamples, myUParams);
      fillParams (aVKnots, aBezier->VDegree(), myVMin, myVMax, myNbVSamples, myVParams);
    }
  }

  // creation points for surfaces based on BSpline or Bezier curves
  else if (theSurf.GetType() == GeomAbs_SurfaceOfRevolution ||
           theSurf.GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    Handle (TColStd_HArray1OfReal) anArrKnots;
    Standard_Integer aDegree = 0;
    if (theSurf.BasisCurve()->Curve().GetType() == GeomAbs_BSplineCurve)
    {
      Handle (Geom_BSplineCurve) aBspl = theSurf.BasisCurve()->Curve().BSpline();
      if (!aBspl.IsNull())
      {
        anArrKnots = new TColStd_HArray1OfReal (1, aBspl->NbKnots());
        aBspl->Knots (anArrKnots->ChangeArray1());
        aDegree = aBspl->Degree();
      }
    }
    if (theSurf.BasisCurve()->Curve().GetType() == GeomAbs_BezierCurve)
    {
      Handle (Geom_BezierCurve) aBez = theSurf.BasisCurve()->Curve().Bezier();
      if (!aBez.IsNull())
      {
        anArrKnots = new TColStd_HArray1OfReal (1, 2);
        anArrKnots->SetValue (1, aBez->FirstParameter());
        anArrKnots->SetValue (2, aBez->LastParameter());
        aDegree = aBez->Degree();

      }
    }
    if (anArrKnots.IsNull())
      return;
    if (theSurf.GetType() == GeomAbs_SurfaceOfRevolution)
      fillParams (anArrKnots->Array1(), aDegree, myVMin, myVMax, myNbVSamples, myVParams);
    else
      fillParams (anArrKnots->Array1(), aDegree, myUMin, myUMax, myNbUSamples, myUParams);
  }

  // Update the number of points in sample
  if (!myUParams.IsNull())
    myNbUSamples = myUParams->Length();
  if (!myVParams.IsNull())
    myNbVSamples = myVParams->Length();
}

//=======================================================================
//function : RejectNode
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GenExtPS::RejectNode (const BVH_Vec3d& theCMin,
                                               const BVH_Vec3d& theCMax,
                                               Standard_Real& theDistance) const
{
  if (myTarget == Extrema_ExtFlag_MINMAX)
    return Standard_False;

  theDistance = (myTarget == Extrema_ExtFlag_MIN) ?
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance (BVH_Vec3d (myPoint.X(), myPoint.Y(), myPoint.Z()), theCMin, theCMax) :
    BVH_Tools<Standard_Real, 3>::PointBoxMaxSquareDistance (BVH_Vec3d (myPoint.X(), myPoint.Y(), myPoint.Z()), theCMin, theCMax);

  return RejectMetric (theDistance);
}

//=======================================================================
//function : RejectMetric
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GenExtPS::RejectMetric (const Standard_Real& theDistance) const
{
  switch (myTarget)
  {
  case Extrema_ExtFlag_MIN:
    return theDistance > (mySqDistance + Precision::SquareConfusion());
  case Extrema_ExtFlag_MAX:
    return theDistance < (mySqDistance - Precision::SquareConfusion());
  default:
    return Standard_False;
  }
}

//=======================================================================
//function : IsMetricBetter
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GenExtPS::IsMetricBetter (const Standard_Real& theLeft,
                                                   const Standard_Real& theRight) const
{
  switch (myTarget)
  {
  case Extrema_ExtFlag_MIN:
    return theLeft <= theRight;
  case Extrema_ExtFlag_MAX:
    return theLeft >= theRight;
  default:
    return Standard_True;
  }
}

//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GenExtPS::Accept (const Standard_Integer theIndex,
                                           const Standard_Real&)
{
  if (myBVHSet == NULL)
  {
    Extrema_GenExtPS_LocalizedGrid aGridSet = myBVHBoxSet->Element (theIndex);

    Standard_Integer aUCoeff = (aGridSet.IdUMax < myNbUSamples) ? 1 : 0;
    Standard_Integer aVCoeff = (aGridSet.IdVMax < myNbVSamples) ? 1 : 0;

    const Extrema_POnSurf& aPMin = myPoints->Value (aGridSet.IdUMin, aGridSet.IdVMin);
    const Extrema_POnSurf& aPMax = myPoints->Value (aGridSet.IdUMax + aUCoeff, aGridSet.IdVMax + aVCoeff);

    Standard_Real aUMin, aUMax, aVMin, aVMax;
    aPMin.Parameter (aUMin, aVMin);
    aPMax.Parameter (aUMax, aVMax);

    if ((aUMax < myLocUMin || aUMin > myLocUMax) &&
        (aVMax < myLocVMin || aVMin > myLocVMax))
      return 0;

    Standard_Integer idUMin = Max (1, aGridSet.IdUMin - 1), idUMax = Min (myNbUSamples, aGridSet.IdUMax + 1);
    Standard_Integer idVMin = Max (1, aGridSet.IdVMin - 1), idVMax = Min (myNbVSamples, aGridSet.IdVMax + 1);
    for (Standard_Integer iU = idUMin; iU <= idUMax; ++iU)
    {
      for (Standard_Integer iV = idVMin; iV <= idVMax; ++iV)
      {
        Extrema_POnSurfParams& aParam = myPoints->ChangeValue (iU, iV);
        aParam.SetSqrDistance (aParam.Value().SquareDistance (myPoint));
      }
    }

    aGridSet.CellBoxSet->Build();
    // Set low-level BVH set for inner selection
    SetBVHSet (aGridSet.CellBoxSet.get());
    Standard_Integer aNb = Select();
    // Unset the inner set and continue with high level BVH traverse
    SetBVHSet (NULL);
    return aNb > 0;
  }
  else
  {
    Extrema_GenExtPS_GridCell aCell = myBVHSet->Element (theIndex);
    return FindSolution (aCell.UIndex, aCell.VIndex, myTarget);
  }
}

//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GenExtPS::FindSolution (const Standard_Integer theNU,
                                                 const Standard_Integer theNV,
                                                 const Extrema_ExtFlag theTarget)
{
  // Fill corner points with square distance to myPoint
  const Extrema_POnSurfParams& aParam00 = myPoints->Value (theNU, theNV);
  const Extrema_POnSurfParams& aParam11 = myPoints->Value (theNU + 1, theNV + 1);

  {
    Standard_Real U1, U2, V1, V2;
    aParam00.Parameter (U1, V1);
    aParam11.Parameter (U2, V2);
    if ((U2 < myLocUMin || U1 > myLocUMax) &&
        (V2 < myLocVMin || V1 > myLocVMax))
      return Standard_False;
  }

  Standard_Boolean isFound = Standard_False;
  if (theTarget == Extrema_ExtFlag_MIN && 
      theNU != myNbUSamples && theNV != myNbVSamples)
  {
    // Find minimum

    const Extrema_POnSurfParams& aParam = ComputeFaceParameters (theNU, theNV);

    Standard_Boolean isMin = Standard_False;
    Extrema_ElementType anElemType = aParam.GetElementType();

    if (anElemType == Extrema_Face)
    {
      isMin = Standard_True;
    }
    else
    {
      // Check if it is a boundary edge or corner vertex.
      Standard_Integer iU, iV;
      aParam.GetIndices (iU, iV);

      if (anElemType == Extrema_UIsoEdge)
      {
        isMin = (iV == 1 || iV == myNbVSamples);
      }
      else if (anElemType == Extrema_VIsoEdge)
      {
        isMin = (iU == 1 || iU == myNbUSamples);
      }
      else if (anElemType == Extrema_Node)
      {
        isMin = (iU == 1 || iU == myNbUSamples) &&
          (iV == 1 || iV == myNbVSamples);
      }

      if (!isMin)
      {
        // This is a middle element.
        if (anElemType == Extrema_UIsoEdge ||
          (anElemType == Extrema_Node && (iU == 1 || iU == myNbUSamples)))
        {
          // Check the down face.
          const Extrema_POnSurfParams &aDownParam = ComputeFaceParameters (theNU, theNV - 1);

          if (aDownParam.GetElementType() == anElemType)
          {
            Standard_Integer iU2, iV2;
            aDownParam.GetIndices (iU2, iV2);
            isMin = (iU == iU2 && iV == iV2);
          }
        }
        else if (anElemType == Extrema_VIsoEdge ||
          (anElemType == Extrema_Node && (iV == 1 || iV == myNbVSamples)))
        {
          // Check the right face.
          const Extrema_POnSurfParams &aRightParam = ComputeFaceParameters (theNU - 1, theNV);

          if (aRightParam.GetElementType() == anElemType)
          {
            Standard_Integer iU2, iV2;
            aRightParam.GetIndices (iU2, iV2);
            isMin = (iU == iU2 && iV == iV2);
          }
        }
        else if (iU == theNU && iV == theNV)
        {
          // Check the lower-left node. For this purpose it is necessary
          // to check lower-left, lower and left faces.
          isMin = Standard_True;

          const Extrema_POnSurfParams *anOtherParam[3] = {
            &ComputeFaceParameters (theNU,     theNV - 1), // Down
            &ComputeFaceParameters (theNU - 1, theNV - 1), // Lower-left
            &ComputeFaceParameters (theNU - 1, theNV) };   // Left

          for (int i = 0; i < 3 && isMin; i++)
          {
            if (anOtherParam[i]->GetElementType() == Extrema_Node)
            {
              Standard_Integer iU2, iV2;
              anOtherParam[i]->GetIndices (iU2, iV2);
              isMin = (iU == iU2 && iV == iV2);
            }
            else
            {
              isMin = Standard_False;
            }
          }
        }
      }
    }

    if (isMin)
    {
      if (FindSolution (aParam))
        isFound = Standard_True;
    }
  }

  if (theTarget == Extrema_ExtFlag_MAX)
  {
    // Find maximum
    const Extrema_POnSurfParams &aParam1 = myPoints->Value (theNU - 1, theNV - 1);
    const Extrema_POnSurfParams &aParam2 = myPoints->Value (theNU - 1, theNV);
    const Extrema_POnSurfParams &aParam3 = myPoints->Value (theNU - 1, theNV + 1);
    const Extrema_POnSurfParams &aParam4 = myPoints->Value (theNU, theNV - 1);
    const Extrema_POnSurfParams &aParam5 = myPoints->Value (theNU, theNV + 1);
    const Extrema_POnSurfParams &aParam6 = myPoints->Value (theNU + 1, theNV - 1);
    const Extrema_POnSurfParams &aParam7 = myPoints->Value (theNU + 1, theNV);
    const Extrema_POnSurfParams &aParam8 = myPoints->Value (theNU + 1, theNV + 1);

    Standard_Real aDist = aParam00.GetSqrDistance();

    if ((aParam1.GetSqrDistance() <= aDist) &&
        (aParam2.GetSqrDistance() <= aDist) &&
        (aParam3.GetSqrDistance() <= aDist) &&
        (aParam4.GetSqrDistance() <= aDist) &&
        (aParam5.GetSqrDistance() <= aDist) &&
        (aParam6.GetSqrDistance() <= aDist) &&
        (aParam7.GetSqrDistance() <= aDist) &&
        (aParam8.GetSqrDistance() <= aDist))
    {
      // Find maximum.
      if (FindSolution (aParam00))
        isFound = Standard_True;
    }
  }

  return isFound;
}

//=======================================================================
//function : FindSolution
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_GenExtPS::FindSolution (const Extrema_POnSurfParams &theParams)
{
  math_Vector Tol (1, 2);
  Tol (1) = myTolU;
  Tol (2) = myTolV;

  math_Vector UV (1, 2);
  theParams.Parameter (UV (1), UV (2));

  math_Vector UVinf (1, 2), UVsup (1, 2);
  UVinf (1) = myUMin;
  UVinf (2) = myVMin;
  UVsup (1) = myUMax;
  UVsup (2) = myVMax;

  Standard_Integer aNbFuncSol = myF.NbExt();

  math_FunctionSetRoot S (myF, Tol);
  S.Perform (myF, UV, UVinf, UVsup);

  myIsDone = Standard_True;

  Standard_Boolean isFound = Standard_False;
  if (myTarget != Extrema_ExtFlag_MINMAX)
  {
    isFound = (mySqDistance != myF.BestSquareDistance());
    mySqDistance = myF.BestSquareDistance();
  }
  else
  {
    isFound = myF.NbExt() > aNbFuncSol;
  }
  return isFound;
}

//=======================================================================
//function : ComputeEdgeParameters
//purpose  : This function computes the point on surface parameters on edge.
//           if it coincides with theParam0 or theParam1, it is returned.
//=======================================================================
const Extrema_POnSurfParams& Extrema_GenExtPS::
  ComputeEdgeParameters (const Standard_Boolean IsUEdge,
                         const Extrema_POnSurfParams& theParam0,
                         const Extrema_POnSurfParams& theParam1,
                         const Standard_Real theDiffTol)
{
  const Handle (Extrema_HArray2OfPOnSurfParams)& anEdgeParamsArr = IsUEdge ? myUEdgePntParams : myVEdgePntParams;
  Standard_Integer iU, iV;
  theParam0.GetIndices (iU, iV);
  Extrema_POnSurfParams& anEdgeParams = anEdgeParamsArr->ChangeValue (iU, iV);
  if (anEdgeParams.GetSqrDistance() < 0.0)
  {
    const Standard_Real aSqrDist01 =
      theParam0.Value().SquareDistance (theParam1.Value());

    if (aSqrDist01 <= theDiffTol)
    {
      // The points are confused. Get the first point and change its type.
      anEdgeParams = theParam0;
    }
    else
    {
      const Standard_Real aDiffDist =
        Abs (theParam0.GetSqrDistance() - theParam1.GetSqrDistance());

      if (aDiffDist >= aSqrDist01 - theDiffTol)
      {
        // The shortest distance is one of the nodes.
        if (theParam0.GetSqrDistance() > theParam1.GetSqrDistance())
        {
          // The shortest distance is the point 1.
          anEdgeParams = theParam1;
        }
        else
        {
          // The shortest distance is the point 0.
          anEdgeParams = theParam0;
        }
      }
      else
      {
        // The shortest distance is inside the edge.
        gp_XYZ aPoP (myPoint.XYZ().Subtracted (theParam0.Value().XYZ()));
        gp_XYZ aPoP1 (theParam1.Value().XYZ().Subtracted (theParam0.Value().XYZ()));
        Standard_Real aRatio = aPoP.Dot (aPoP1) / aSqrDist01;
        Standard_Real aU[2];
        Standard_Real aV[2];

        theParam0.Parameter (aU[0], aV[0]);
        theParam1.Parameter (aU[1], aV[1]);

        Standard_Real aUPar = aU[0];
        Standard_Real aVPar = aV[0];

        if (IsUEdge)
        {
          aUPar += aRatio*(aU[1] - aU[0]);
        }
        else
        {
          aVPar += aRatio*(aV[1] - aV[0]);
        }

        anEdgeParams.SetParameters (aUPar, aVPar, myS->Value (aUPar, aVPar));
        anEdgeParams.SetElementType (IsUEdge ? Extrema_UIsoEdge : Extrema_VIsoEdge);
        anEdgeParams.SetSqrDistance (anEdgeParams.Value().SquareDistance (myPoint));
        anEdgeParams.SetIndices (iU, iV);
      }
    }
  }

  return anEdgeParams;
}

//=======================================================================
//function : ComputeFaceParameters
//purpose  : This function computes the point on surface parameters on cell
//=======================================================================
const Extrema_POnSurfParams& Extrema_GenExtPS::
  ComputeFaceParameters (const Standard_Integer theU,
                         const Standard_Integer theV)
{
  Extrema_POnSurfParams& aFaceParams = myFacePntParams->ChangeValue (theU, theV);
  if (aFaceParams.GetSqrDistance() < 0.0)
  {
    // This is the tolerance of difference of squared values.
    // No need to set it too small.
    const Standard_Real aDiffTol = myTolU + myTolV;

    const Extrema_POnSurfParams& aParam00 = myPoints->Value (theU, theV);
    const Extrema_POnSurfParams& aParam01 = myPoints->Value (theU, theV + 1);
    const Extrema_POnSurfParams& aParam10 = myPoints->Value (theU + 1, theV);
    const Extrema_POnSurfParams& aParam11 = myPoints->Value (theU + 1, theV + 1);

    const Extrema_POnSurfParams &aUE0 =
      ComputeEdgeParameters (Standard_True, aParam00, aParam10, aDiffTol);
    const Extrema_POnSurfParams &aUE1 =
      ComputeEdgeParameters (Standard_True, aParam01, aParam11, aDiffTol);

    const Extrema_POnSurfParams &aVE0 =
      ComputeEdgeParameters (Standard_False, aParam00, aParam01, aDiffTol);
    const Extrema_POnSurfParams &aVE1 =
      ComputeEdgeParameters (Standard_False, aParam10, aParam11, aDiffTol);

    Standard_Real aSqrDist01 = aUE0.Value().SquareDistance (aUE1.Value());
    Standard_Real aDiffDist = Abs (aUE0.GetSqrDistance() - aUE1.GetSqrDistance());
    Standard_Boolean isOut = Standard_False;

    if (aDiffDist >= aSqrDist01 - aDiffTol)
    {
      // The projection is outside the face.
      isOut = Standard_True;
    }
    else
    {
      aSqrDist01 = aVE0.Value().SquareDistance (aVE1.Value());
      aDiffDist = Abs (aVE0.GetSqrDistance() - aVE1.GetSqrDistance());

      if (aDiffDist >= aSqrDist01 - aDiffTol)
      {
        // The projection is outside the face.
        isOut = Standard_True;
      }
    }

    if (isOut)
    {
      // Get the closest point on an edge.
      const Extrema_POnSurfParams &aUEMin =
        aUE0.GetSqrDistance() < aUE1.GetSqrDistance() ? aUE0 : aUE1;
      const Extrema_POnSurfParams &aVEMin =
        aVE0.GetSqrDistance() < aVE1.GetSqrDistance() ? aVE0 : aVE1;
      const Extrema_POnSurfParams &aEMin =
        aUEMin.GetSqrDistance() < aVEMin.GetSqrDistance() ? aUEMin : aVEMin;

      aFaceParams = aEMin;
    }
    else
    {
      // Find closest point inside the face.
      Standard_Real aU[2];
      Standard_Real aV[2];
      Standard_Real aUPar;
      Standard_Real aVPar;

      // Compute U parameter.
      aUE0.Parameter (aU[0], aV[0]);
      aUE1.Parameter (aU[1], aV[1]);
      aUPar = 0.5*(aU[0] + aU[1]);
      // Compute V parameter.
      aVE0.Parameter (aU[0], aV[0]);
      aVE1.Parameter (aU[1], aV[1]);
      aVPar = 0.5*(aV[0] + aV[1]);

      aFaceParams.SetParameters (aUPar, aVPar, myS->Value (aUPar, aVPar));
      aFaceParams.SetElementType (Extrema_Face);
      aFaceParams.SetSqrDistance (aFaceParams.Value().SquareDistance (myPoint));
      aFaceParams.SetIndices (theU, theV);
    }
  }

  return aFaceParams;
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================
Standard_Integer Extrema_GenExtPS::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return static_cast<Standard_Integer> (mySolutions.size());
}

//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================
Standard_Real Extrema_GenExtPS::SquareDistance (const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }

  return mySolutions[N - 1].SqDistance;
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================
const Extrema_POnSurf& Extrema_GenExtPS::Point (const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }

  return mySolutions[N - 1].UV;
}

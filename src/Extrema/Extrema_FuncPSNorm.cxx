// Created on: 1995-07-18
// Created by: Modelistation
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


#include <Extrema_FuncPSNorm.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAbs_IsoType.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <Standard_TypeMismatch.hxx>

Extrema_FuncPSNorm::Extrema_FuncPSNorm()
  :
  myS(NULL),
  myU(0.0),
  myV(0.0),
  myPinit (Standard_False),
  mySinit (Standard_False),
  myTarget (Extrema_ExtFlag_MINMAX),
  myBestSqDistance (-1)
{
}

//=============================================================================
Extrema_FuncPSNorm::Extrema_FuncPSNorm (const gp_Pnt& P,
                                        const Adaptor3d_Surface& S)
  :
  myP (P),
  myS ((Adaptor3d_SurfacePtr)&S),
  myU(0.0),
  myV(0.0),
  myPinit (Standard_True),
  mySinit (Standard_True),
  myTarget (Extrema_ExtFlag_MINMAX),
  myBestSqDistance (-1)
{
}

//=============================================================================
void Extrema_FuncPSNorm::Initialize(const Adaptor3d_Surface& S)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  mySinit = Standard_True;
  myPoints.clear();
  mySqDistances.clear();
  myTarget = Extrema_ExtFlag_MINMAX;
  myBestSqDistance = -1;
}

//=============================================================================

void Extrema_FuncPSNorm::SetPoint (const gp_Pnt& P,
                                   const Extrema_ExtFlag theTarget)
{
  myP = P;
  myPinit = Standard_True;
  myTarget = theTarget;
  myBestSqDistance = (myTarget == Extrema_ExtFlag_MIN ? RealLast() : RealFirst());
  myPoints.clear();
  mySqDistances.clear();
}

//=============================================================================

//=============================================================================

Standard_Integer Extrema_FuncPSNorm::NbVariables () const { return 2;}
//=============================================================================

Standard_Integer Extrema_FuncPSNorm::NbEquations () const { return 2;}
//=============================================================================

Standard_Boolean Extrema_FuncPSNorm::Value (const math_Vector& UV, 
                                           math_Vector& F)
{
  if (!myPinit || !mySinit) throw Standard_TypeMismatch();
  myU = UV(1);
  myV = UV(2);
  gp_Vec Dus, Dvs;
  myS->D1(myU,myV,myPs,Dus,Dvs);

  gp_Vec PPs (myP,myPs);

  F(1) = PPs.Dot(Dus);
  F(2) = PPs.Dot(Dvs);

  return Standard_True;
}
//=============================================================================

Standard_Boolean Extrema_FuncPSNorm::Derivatives (const math_Vector& UV, 
                                                 math_Matrix& Df)
{
  math_Vector F(1,2);
  return Values(UV,F,Df);
}
//=============================================================================

Standard_Boolean Extrema_FuncPSNorm::Values (const math_Vector& UV, 
                                            math_Vector& F,
                                            math_Matrix& Df)
{
  if (!myPinit || !mySinit) throw Standard_TypeMismatch();
  myU = UV(1);
  myV = UV(2);
  gp_Vec Dus, Dvs, Duus, Dvvs, Duvs;
  myS->D2(myU,myV,myPs,Dus,Dvs,Duus,Dvvs,Duvs);

  gp_Vec PPs (myP,myPs);

  Df(1,1) = Dus.SquareMagnitude() + PPs.Dot(Duus);
  Df(1,2) = Dvs.Dot(Dus)          + PPs.Dot(Duvs);
  Df(2,1) = Df(1,2);
  Df(2,2) = Dvs.SquareMagnitude() + PPs.Dot(Dvvs);

  // 3. Value
  F(1) = PPs.Dot(Dus);
  F(2) = PPs.Dot(Dvs);

  return Standard_True;
}

//=============================================================================
Standard_Integer Extrema_FuncPSNorm::GetStateNumber()
{
  if (!myPinit || !mySinit) throw Standard_TypeMismatch();

  Standard_Real aNewSqDist  = myPs.SquareDistance (myP);
  if ((myTarget == Extrema_ExtFlag_MIN && aNewSqDist > myBestSqDistance) ||
      (myTarget == Extrema_ExtFlag_MAX && aNewSqDist < myBestSqDistance))
    return 0;
  myBestSqDistance = aNewSqDist;

  // Comparison of solution with previous solutions
  Standard_Real tol2d = Precision::SquarePConfusion();
  std::size_t i = 0, nbSol = mySqDistances.size();
  for (; i < nbSol; i++)
  {
    Standard_Real aU, aV;
    Extrema_POnSurf& aPOnSurf = myPoints[i];
    aPOnSurf.Parameter (aU, aV);
    if (((myU - aU) * (myU - aU) + (myV - aV) * (myV - aV)) <= tol2d)
    {
      // The points are almost the same in the parametric space.
      if (myTarget != Extrema_ExtFlag_MINMAX)
      {
        // Check if new solution gives better distance than the existing solution.
        Standard_Real& anOldSqDist = mySqDistances[i];
        if ((myTarget == Extrema_ExtFlag_MIN && aNewSqDist < anOldSqDist) ||
            (myTarget == Extrema_ExtFlag_MAX && aNewSqDist > anOldSqDist))
        {
          aPOnSurf.SetParameters (myU, myV, myPs);
          anOldSqDist = aNewSqDist;
        }
      }
      break;
    }
  }
  if (i < nbSol)
    return 0;
  mySqDistances.push_back (aNewSqDist);
  myPoints.push_back (Extrema_POnSurf (myU, myV, myPs));
  return 0;
}
//=============================================================================

Standard_Integer Extrema_FuncPSNorm::NbExt () const
{
  return static_cast<Standard_Integer>(mySqDistances.size());
}
//=============================================================================

Standard_Real Extrema_FuncPSNorm::SquareDistance (const Standard_Integer N) const
{
  if (!myPinit || !mySinit) throw Standard_TypeMismatch();
  return mySqDistances [N - 1];
}
//=============================================================================

const Extrema_POnSurf& Extrema_FuncPSNorm::Point (const Standard_Integer N) const
{
  if (!myPinit || !mySinit) throw Standard_TypeMismatch();
  return myPoints [N - 1];
}

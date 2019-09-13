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

#ifndef _Extrema_GenExtPS_HeaderFile
#define _Extrema_GenExtPS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Adaptor3d_SurfacePtr.hxx>
#include <BVH_BoxSet.hxx>
#include <BVH_Traverse.hxx>
#include <Extrema_HArray2OfPOnSurfParams.hxx>
#include <Extrema_FuncPSNorm.hxx>
#include <Extrema_POnSurfParams.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_HArray1OfReal.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_TypeMismatch;
class gp_Pnt;
class Adaptor3d_Surface;
class Extrema_POnSurf;
class Extrema_POnSurfParams;


//! It calculates the extreme distances between a point and a surface.
//! These distances can be minimum or maximum.
//! 
//! The function F(u,v) = distance (P, S(u,v)) has an extrema when
//! gradient(F) = 0. The algorithm searches all the zeros inside
//! the definition ranges of the surface.
//!
//! When defining the surface, the number of samples in U and V direction
//! should be specified. These numbers should be great enough such 
//! that if there exist N extreme distances between the point and the surface,
//! so there also exist N extrema between the point and the grid. 
//!
//! It is possible to look for extrema distances in the whole parametric
//! space of the surface or limit it with the specified range which can be
//! useful when it is needed to look for local extrema distances.
//!
//! Parametric tolerances are used to determine the conditions to stop the
//! iterations - at the iteration number n:
//! (Un - Un-1) < TolU and (Vn - Vn-1) < TolV 
//!
//! It is possible to look for only Minimal or Maximal distances,
//! as well as for all solutions.
//!
//! The class is BVH enhanced - the grid cells are stored into BVH-organized
//! structure. Depending on the Extrema target the traverse of the BVH tree
//! is different.
class Extrema_GenExtPS: protected BVH_Traverse <Standard_Real, 3>,
                        public Standard_Transient
{
public:

  DEFINE_STANDARD_RTTIEXT (Extrema_GenExtPS, Standard_Transient)

public: //! @name public types

  //! Grid cell is defined just by (U, V) indices of the minimal
  //! corner of the cell.
  struct GridCell
  {
    GridCell (Standard_Integer theUInd = -1, Standard_Integer theVInd = -1)
      : myUInd (theUInd), myVInd (theVInd)
    {}

    Standard_Integer myUInd; //!< U index of the minimal corner
    Standard_Integer myVInd; //!< V index of the minimal corner
  };


public: //! @name Constructors computing the distances
  
  //! Constructor for computation of the distances between specified point and surface.
  //! The whole parametric space of the surfaces is taken into account.
  //!
  //! Constructor is mostly used for one time projection of the point on the surface,
  //! but still the instances of extrema can be used for projecting other points on the surface
  //! with *Perform()* method.
  //!
  //! @param theP point
  //! @param theS Surface
  //! @param theNbU Number of samples in U direction
  //! @param theNbV Number of samples in V direction
  //! @param theTolU U Parametric tolerance
  //! @param theTolV V Parametric tolerance
  //! @param theTarget defines what solutions are required
  Standard_EXPORT Extrema_GenExtPS (const gp_Pnt&            theP,
                                    const Adaptor3d_Surface& theS,
                                    const Standard_Integer   theNbU,
                                    const Standard_Integer   theNbV,
                                    const Standard_Real      theTolU,
                                    const Standard_Real      theTolV,
                                    const Extrema_ExtFlag    theTarget = Extrema_ExtFlag_MINMAX);
  
  //! Constructor for computation of the distances between specified point and surface.
  //! Only the specified parametric range of the surface is taken into account.
  //!
  //! Constructor is mostly used for one time projection of the point on the surface,
  //! but still the instances of extrema can be used for projecting other points on the surface
  //! with *Perform()* method.
  //!
  //! @param theP point
  //! @param theS Surface
  //! @param theNbU Number of samples in U direction
  //! @param theNbV Number of samples in V direction
  //! @param theUMin Lower U bound
  //! @param theUMax Upper U bound
  //! @param theVMin Lower V bound
  //! @param theVMax Upper V bound
  //! @param theTolU U Parametric tolerance
  //! @param theTolV V Parametric tolerance
  //! @param theTarget defines what solutions are required
  Standard_EXPORT Extrema_GenExtPS (const gp_Pnt&            theP,
                                    const Adaptor3d_Surface& theS,
                                    const Standard_Integer   theNbU,
                                    const Standard_Integer   theNbV,
                                    const Standard_Real      theUMin,
                                    const Standard_Real      theUMax,
                                    const Standard_Real      theVMin,
                                    const Standard_Real      theVMax,
                                    const Standard_Real      theTolU,
                                    const Standard_Real      theTolV,
                                    const Extrema_ExtFlag    theTarget = Extrema_ExtFlag_MINMAX);

public: //! @name Empty constructor + Initialization step

  //! Empty constructor
  Standard_EXPORT Extrema_GenExtPS();

  //! Initializes Extrema algorithm with the surfaces.
  //! Search is performed in whole parametric range of the surface.
  //! @param theS Surface
  //! @param theNbU Number of samples in U direction
  //! @param theNbV Number of samples in V direction
  //! @param theTolU U Parametric tolerance
  //! @param theTolV V Parametric tolerance
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& theS,
                                   const Standard_Integer   theNbU,
                                   const Standard_Integer   theNbV,
                                   const Standard_Real      theTolU,
                                   const Standard_Real      theTolV);

  //! Initializes Extrema algorithm with the surfaces.
  //! Search is performed in the given parametric range.
  //! @param S Surface
  //! @param theNbU Number of samples in U direction
  //! @param theNbV Number of samples in V direction
  //! @param theUMin Lower U bound
  //! @param theUMax Upper U bound
  //! @param theVMin Lower V bound
  //! @param theVMax Upper V bound
  //! @param theTolU U Parametric tolerance
  //! @param theTolV V Parametric tolerance
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& theS,
                                   const Standard_Integer   theNbU,
                                   const Standard_Integer   theNbV,
                                   const Standard_Real      theUMin,
                                   const Standard_Real      theUMax,
                                   const Standard_Real      theVMin,
                                   const Standard_Real      theVMax,
                                   const Standard_Real      theTolU,
                                   const Standard_Real      theTolV);

public: //! @name Specifying the search options

  //! Specifies what solutions are necessary:
  //! - *Extrema_ExtFlag_MIN* - only minimal solutions
  //! - *Extrema_ExtFlag_MAX* - only maximal solutions
  //! - *Extrema_ExtFlag_MINMAX - all solutions.
  void SetTarget (const Extrema_ExtFlag theTarget)
  {
    myTarget = theTarget;
  }

  //! Returns the Extrema target type
  Extrema_ExtFlag Target() const { return myTarget; }

  //! Sets the tolerance for the search.
  //! These tolerances are used for projection of the point,
  //! and not used for surface initialization, so can be changed
  //! from point to point.
  void SetTolerance (const Standard_Real theTolU,
                     const Standard_Real theTolV)
  {
    myTolU = theTolU;
    myTolV = theTolV;
  }


public: //! @name Performing projection

  //! Performs projection of the point on the surface.
  //! Extrema must already be initialized with the surface.
  //! Allows multiple points be projected on the same surface.
  Standard_EXPORT void Perform (const gp_Pnt& theP);


public: //! @name Rules for BVH traverse

  //! Rejection of the node by bounding box.
  //! Metric is computed to choose the best branch.
  //! Returns true if the node should be rejected, false otherwise.
  Standard_EXPORT virtual Standard_Boolean
    RejectNode (const BVH_Vec3d& theCornerMin,
                const BVH_Vec3d& theCornerMax,
                Standard_Real& theMetric) const Standard_OVERRIDE;

  //! Rejects the node by the metric
  Standard_EXPORT virtual Standard_Boolean
    RejectMetric (const Standard_Real& theMetric) const Standard_OVERRIDE;

  //! Compares the two metrics and chooses the best one.
  //! Returns true if the first metric is better than the second,
  //! false otherwise.
  Standard_EXPORT virtual Standard_Boolean
    IsMetricBetter (const Standard_Real& theLeft,
                    const Standard_Real& theRight) const Standard_OVERRIDE;

  //! Leaf element acceptance.
  //! Metric of the parent leaf-node is passed to avoid the check on the
  //! element and accept it unconditionally.
  //! Returns true if the element has been accepted, false otherwise.
  Standard_EXPORT virtual Standard_Boolean
    Accept (const Standard_Integer theIndex,
            const Standard_Real& theMetric) Standard_OVERRIDE;


public: //! @name Getting the results

  //! Returns True if the distances are found.
  Standard_Boolean IsDone() const { return myIsDone; }

  //! Returns the number of extrema distances found.
  Standard_EXPORT Standard_Integer NbExt() const;

  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer theN) const;

  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& Point (const Standard_Integer theN) const;


private: //! @name Private methods performing the job

  //! Creation of grid of parametric points (sampling of the surface)
  Standard_EXPORT void BuildGrid();

  //! Selection of points to build grid, depending on the type of surface
  Standard_EXPORT void GetGridPoints (const Adaptor3d_Surface& theSurf);

  //! Builds the BVH tree with bounding boxes of the cells of the grid
  Standard_EXPORT void BuildTree();

  //! Looks for the solution starting at given point
  Standard_EXPORT void FindSolution (const Extrema_POnSurfParams& theParams);

  //! Compute new edge parameters.
  Standard_EXPORT const Extrema_POnSurfParams&
    ComputeFaceParameters (const Standard_Integer theU,
                           const Standard_Integer theV,
                           const gp_Pnt& thePoint);

  //! Compute new edge parameters.
  Standard_EXPORT const Extrema_POnSurfParams&
    ComputeEdgeParameters (const Standard_Boolean IsUEdge,
                           Extrema_POnSurfParams& theParam0,
                           Extrema_POnSurfParams& theParam1,
                           const gp_Pnt& thePoint,
                           const Standard_Real theDiffTol);

  //! Looks for the Min or Max Solution (depending on the given target).
  Standard_EXPORT Standard_Boolean FindSolution (const Standard_Integer theIndex,
                                                 const Standard_Real& theMetric,
                                                 const Extrema_ExtFlag theTarget);

  //! structure to keep the results
  struct ExtPSResult
  {
    ExtPSResult ()
      : mySqDistance (-1)
    {}

    ExtPSResult (const Extrema_POnSurf& theUV,
                 const Standard_Real theSqDist)
      : myUV (theUV),
        mySqDistance (theSqDist)
    {}

    Standard_Boolean operator< (const ExtPSResult& Other) const
    {
      if (mySqDistance != Other.mySqDistance)
        return mySqDistance < Other.mySqDistance;

      Standard_Real U1, U2, V1, V2;
      myUV.Parameter (U1, V1);
      Other.myUV.Parameter (U2, V2);
      return (U1 < U2 || (U1 == U2 && V1 < V2));
    }

    Extrema_POnSurf myUV;
    Standard_Real mySqDistance;
  };

private: //! @name Fields

  // Inputs
  NCollection_Vec3<Standard_Real> myPoint; //!< Point
  Adaptor3d_SurfacePtr myS; //!< Surface
  Extrema_FuncPSNorm myF;   //!< Function

  Standard_Real myUMin;     //!< Surface parametric range: UMin
  Standard_Real myUMax;     //!< Surface parametric range: UMax
  Standard_Real myVMin;     //!< Surface parametric range: VMin
  Standard_Real myVMax;     //!< Surface parametric range: VMax

  Standard_Integer myNbUSamples; //!< Number of samples in U parametric direction
  Standard_Integer myNbVSamples; //!< Number of samples in V parametric direction

  Standard_Real myTolU;     //!< U parametric tolerance
  Standard_Real myTolV;     //!< V parametric tolerance

  Extrema_ExtFlag myTarget; //!< Extrema objective

  // Intermediate data

  Handle(Extrema_HArray2OfPOnSurfParams) myPoints; //!< Grid points
  Handle(TColStd_HArray1OfReal) myUParams;         //!< Grid parameters in U parametric direction
  Handle(TColStd_HArray1OfReal) myVParams;         //!< Grid parameters in V parametric direction

  Handle(Extrema_HArray2OfPOnSurfParams) myFacePntParams;
  Handle(Extrema_HArray2OfPOnSurfParams) myUEdgePntParams;
  Handle(Extrema_HArray2OfPOnSurfParams) myVEdgePntParams;

  Standard_Real mySqDistance; //!< Min/Max found square distance used in BVH tree traverse
  opencascade::handle <BVH_BoxSet <Standard_Real, 3, GridCell>> myGridBoxSet; //!< BVH-organized grid

  // Results
  std::vector <ExtPSResult> mySolutions;
  Standard_Boolean myIsDone; //!< IsDone flag
};

DEFINE_STANDARD_HANDLE (Extrema_GenExtPS, Standard_Transient)

#endif // _Extrema_GenExtPS_HeaderFile

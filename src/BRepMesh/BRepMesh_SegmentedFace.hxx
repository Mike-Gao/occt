// Created on: 2016-07-04
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_SegmentedFace_HeaderFile
#define _BRepMesh_SegmentedFace_HeaderFile

#include <IMeshTools_Parameters.hxx>
#include <Standard_Transient.hxx>
#include <IMeshData_Face.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Shared.hxx>
#include <BRepMesh_GeomTool.hxx>

//! Auxiliary class providing common functionality for exploding wires of 
//! discrete face on sets of segments using tessellation data stored in model.
class BRepMesh_SegmentedFace : public Standard_Transient
{
public: //! @name mesher API

  //! Identifies segment inside face.
  //! Uses explicit pointers to points instead of accessing using indices.
  struct Segment
  {
    IMeshData::IEdgePtr      EdgePtr;
    IMeshData::IPCurvePtr    PCurvePtr;
    gp_Pnt2d*                Point1;
    Standard_Integer         Param1; 
    gp_Pnt2d*                Point2;
    Standard_Integer         Param2;

    Segment()
      : EdgePtr  (NULL)
      , PCurvePtr(NULL)
      , Point1   (NULL)
      , Param1   (0)
      , Point2   (NULL)
      , Param2   (0)
    {
    }

    Segment(const IMeshData::IEdgePtr&   theEdgePtr,
            const IMeshData::IPCurvePtr& thePCurvePtr,
            gp_Pnt2d*                    thePoint1,
            const Standard_Integer       theParam1,
            gp_Pnt2d*                    thePoint2,
            const Standard_Integer       theParam2)
      : EdgePtr  (theEdgePtr)
      , PCurvePtr(thePCurvePtr)
      , Point1   (thePoint1)
      , Param1   (theParam1)
      , Point2   (thePoint2)
      , Param2   (theParam2)
    {
    }
  };

  //! Keeps parameters of intersection of two segments.
  //! Params are in range relative to distance between points of corresponding segment.
  struct IntersectionParams
  {
    BRepMesh_GeomTool::IntFlag Type;
    gp_Pnt2d                   Point;
    Standard_Real              Params[2];
  };

  typedef NCollection_Vector<IntersectionParams>                                    VectorOfIntersectionParams;
  typedef NCollection_Shared<NCollection_Vector<Segment> >                          Segments;
  typedef NCollection_Shared<NCollection_Array1<Handle(Segments)> >                 ArrayOfSegments;
  typedef NCollection_Shared<NCollection_Array1<Handle(IMeshData::BndBox2dTree)> >  ArrayOfBndBoxTree;


  //! Default constructor
  Standard_EXPORT BRepMesh_SegmentedFace(const IMeshData::IFaceHandle& theFace,
                                         const IMeshTools_Parameters&  theParameters);

  //! Destructor
  Standard_EXPORT virtual ~BRepMesh_SegmentedFace();

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_SegmentedFace, Standard_Transient)

protected:

  //! Returns True in case if check can be performed in parallel mode.
  inline Standard_Boolean isParallel() const
  {
    return (myParameters.InParallel && myDFace->WiresNb() > 1);
  }

  //! Collects face segments.
  void collectSegments();

private:

  BRepMesh_SegmentedFace (const BRepMesh_SegmentedFace& theOther);

  void operator=(const BRepMesh_SegmentedFace& theOther);

protected:

  //! Selector.
  //! Used to identify segments with overlapped bounding boxes.
  class BndBox2dTreeSelector : public IMeshData::BndBox2dTree::Selector
  {
  public:
    //! Constructor.
    BndBox2dTreeSelector(const Standard_Boolean isCollectIntersectionParams);

    //! Sets working set of segments.
    inline void SetSegments(const Handle(BRepMesh_SegmentedFace::Segments)& theSegments)
    {
      mySegments = theSegments;
    }

    //! Returns indices of intersecting segments.
    inline const IMeshData::VectorOfInteger& Indices() const
    {
      return myIndices;
    }

    //! Returns intersection parameters.
    inline const BRepMesh_SegmentedFace::VectorOfIntersectionParams& IntParams() const
    {
      return myIntParams;
    }

    //! Resets current selector.
    void Reset(const BRepMesh_SegmentedFace::Segment* theSegment,
               const Standard_Integer                 theSelfSegmentIndex);

    //! Indicates should the given box be rejected or not.
    virtual Standard_Boolean Reject(const Bnd_Box2d& theBox) const;

    //! Accepts segment with the given index in case if it fits conditions.
    virtual Standard_Boolean Accept(const Standard_Integer& theSegmentIndex);

  private:

    Standard_Boolean                                   myCollectIntersectionParams;
    Standard_Integer                                   mySelfSegmentIndex;
    Handle(BRepMesh_SegmentedFace::Segments)           mySegments;
    const  BRepMesh_SegmentedFace::Segment*            mySegment;
    Bnd_Box2d                                          myBox;
    IMeshData::VectorOfInteger                         myIndices;
    BRepMesh_SegmentedFace::VectorOfIntersectionParams myIntParams;              
  };

protected:

  IMeshData::IFaceHandle            myDFace;
  const IMeshTools_Parameters&      myParameters;

  Handle(ArrayOfSegments)           myWiresSegments;
  Handle(ArrayOfBndBoxTree)         myWiresBndBoxTree;

};

#endif

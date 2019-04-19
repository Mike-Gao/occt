// Created on: 2019-04-11
// Created by: Timur Izmaylov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <StdPrs_SectionLines.hxx>

#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pln.hxx>

#include <vector>

namespace
{

  //! Pair of points
  typedef std::pair<gp_Pnt, gp_Pnt> PointPair;

  //! Segment of line
  typedef PointPair Segment;

  //! Array of segments
  typedef std::vector<Segment> Segments;

  //! Triple of points
  typedef gp_Pnt PointTriple[3];

  //! Triangle (a set of 3 points)
  typedef PointTriple Triangle;

  //! Array of triangles
  typedef std::vector<Triangle> Triangles;

  //! Adds the segment to the array of segments
  //! @param thePoint1 the first end of segment
  //! @param thePoint2 the second end of segment
  //! @param theSegments the array of segments
  static void addSegment (const gp_Pnt& thePoint1, const gp_Pnt& thePoint2, Segments& theSegments)
  {
    if (thePoint1.Distance (thePoint2) <= Precision::Confusion())
    {
      return;
    }
    const PointPair anEdge (thePoint1, thePoint2);
    theSegments.push_back (anEdge);
  }

  //! Adds the point to the section
  //! @param theVertex1 the first vertex of the crossed edge
  //! @param theDistanceToPlane1 the signed distance from the first vertex of the crossed edge to the plane
  //! @param theVertex2 the second vertex of the crossed edge
  //! @param theDistanceToPlane2 the signed distance from the second vertex of the crossed edge to the plane
  //! @param theSectionPoints the container of section points where found point should be added
  //! @param theNumberOfSectionPoints the number of points which are already in theSectionPoints container
  static void addSectionPoint (const gp_Pnt&       theVertex1,
                               const Standard_Real theDistanceToPlane1,
                               const gp_Pnt&       theVertex2,
                               const Standard_Real theDistanceToPlane2,
                               PointTriple&        theSectionPoints,
                               std::size_t&        theNumberOfSectionPoints)
  {
    if (theDistanceToPlane1 * theDistanceToPlane2 > 0.0)
    {
      return;
    }
    const Standard_Real aDistanceParameter     = theDistanceToPlane1 / (theDistanceToPlane1 - theDistanceToPlane2);
    theSectionPoints[theNumberOfSectionPoints] = theVertex1.Coord()
                                                 + aDistanceParameter * (theVertex2.Coord() - theVertex1.Coord());
    ++theNumberOfSectionPoints;
  }

  //! Add points found as the result of crossing the triangle with the plane to the section
  //! @param theTriangle the triangle that is crossed with the plane
  //! @param areVerticesOnSectionPlane the array of flags that shows whether the triangle vertex
  //! with corresponding index is on the section plane
  //! @param theDistancesToPlane the array of signed distances from the plane to the triangle vertex with corresponding
  //! index
  //! @param theSectionPoints the container of section points where found point should be added
  //! @param theNumberOfSectionPoints the number of points which are already in theSectionPoints container
  static void addSectionPoints (const Triangle& theTriangle,
                                const bool (&areVerticesOnSectionPlane)[3],
                                const Standard_Real (&theDistancesToPlane)[3],
                                PointTriple& theSectionPoints,
                                std::size_t& theNumberOfSectionPoints)
  {
    for (std::size_t aVertexIndex1 = 0; aVertexIndex1 < 2; ++aVertexIndex1)
    {
      const bool isVertex1OnSectionPlane = areVerticesOnSectionPlane[aVertexIndex1];
      if (isVertex1OnSectionPlane)
      {
        continue;
      }
      for (std::size_t aVertexIndex2 = aVertexIndex1 + 1; aVertexIndex2 < 3; ++aVertexIndex2)
      {
        const bool isVertex2OnSectionPlane = areVerticesOnSectionPlane[aVertexIndex2];
        if (isVertex2OnSectionPlane)
        {
          continue;
        }
        addSectionPoint (theTriangle[aVertexIndex1],
                         theDistancesToPlane[aVertexIndex1],
                         theTriangle[aVertexIndex2],
                         theDistancesToPlane[aVertexIndex2],
                         theSectionPoints,
                         theNumberOfSectionPoints);
      }
    }
  }

  //! Add all segments between points (2 or 3) from point triple to the section
  //! @param theSectionPoints the points to be added
  //! @param theNumberOfSectionPoints the number of section points to be added
  //! @param theSectionSegments the container of section segments where the new segments should be added
  static void addAllSectionPoints (const PointTriple& theSectionPoints,
                                   const std::size_t  theNumberOfSectionPoints,
                                   Segments&          theSectionSegments)
  {
    if (theNumberOfSectionPoints < 2)
    {
      return;
    }
    for (std::size_t aPointIndex1 = 0; aPointIndex1 < theNumberOfSectionPoints - 1; ++aPointIndex1)
    {
      for (std::size_t aPointIndex2 = aPointIndex1 + 1; aPointIndex2 < theNumberOfSectionPoints; ++aPointIndex2)
      {
        addSegment (theSectionPoints[aPointIndex1], theSectionPoints[aPointIndex2], theSectionSegments);
      }
    }
  }

  //! Add all segments between triangle vertices that are on the section plane
  //! @param theTriangle the triangle which vertices should be added
  //! @param areVerticesOnSectionPlane the array of flags that shows whether the triangle vertex
  //! with corresponding index is on the section plane
  //! @param theSectionSegments the container of section segments where the new segments should be added
  static void addAllTriangleVerticesOnSectionPlane (const Triangle& theTriangle,
                                                    const bool (&areVerticesOnSectionPlane)[3],
                                                    Segments& theSectionSegments)
  {
    for (std::size_t aVertexIndex1 = 0; aVertexIndex1 < 2; ++aVertexIndex1)
    {
      const bool isVertex1OnSectionPlane = areVerticesOnSectionPlane[aVertexIndex1];
      if (!isVertex1OnSectionPlane)
      {
        continue;
      }
      for (std::size_t aVertexIndex2 = aVertexIndex1 + 1; aVertexIndex2 < 3; ++aVertexIndex2)
      {
        const bool isVertex2OnSectionPlane = areVerticesOnSectionPlane[aVertexIndex2];
        if (!isVertex2OnSectionPlane)
        {
          continue;
        }
        addSegment (theTriangle[aVertexIndex1], theTriangle[aVertexIndex2], theSectionSegments);
      }
    }
  }

  //! Computes a cross section of the triangle with the plane
  //! @param theTriangle the triangle to be sectioned
  //! @param theSectionPlane the section plane
  //! @param theSectionSegments the container of section segments where the new segments should be added
  static void crossSection (const Triangle& theTriangle, const gp_Pln& theSectionPlane, Segments& theSectionSegments)
  {
    Standard_Real aDistancesToPlane[3];
    bool          areVerticesOnSectionPlane[3];
    std::size_t   aNumberOfVerticesOnSectionPlane = 0;
    for (std::size_t aVertexIndex = 0; aVertexIndex < 3; ++aVertexIndex)
    {
      const gp_Pnt   aNode            = theTriangle[aVertexIndex];
      Standard_Real& aDistanceToPlane = aDistancesToPlane[aVertexIndex];
      aDistanceToPlane                = theSectionPlane.SignedDistance (aNode);
      bool& isVertexOnSectionPlane    = areVerticesOnSectionPlane[aVertexIndex];
      isVertexOnSectionPlane          = (Abs (aDistanceToPlane) <= Precision::Confusion());
      if (isVertexOnSectionPlane)
      {
        ++aNumberOfVerticesOnSectionPlane;
      }
    }
    if (aNumberOfVerticesOnSectionPlane <= 1)
    {
      PointTriple aSectionPoints;
      std::size_t aNumberOfSectionPoints = 0;
      for (std::size_t aVertexIndex = 0; aVertexIndex < 3; ++aVertexIndex)
      {
        if (areVerticesOnSectionPlane[aVertexIndex])
        {
          aSectionPoints[aNumberOfSectionPoints] = theTriangle[aVertexIndex];
          ++aNumberOfSectionPoints;
        }
      }
      addSectionPoints (
        theTriangle, areVerticesOnSectionPlane, aDistancesToPlane, aSectionPoints, aNumberOfSectionPoints);
      addAllSectionPoints (aSectionPoints, aNumberOfSectionPoints, theSectionSegments);
    }
    else
    {
      addAllTriangleVerticesOnSectionPlane (theTriangle, areVerticesOnSectionPlane, theSectionSegments);
    }
  }

  //! Gets a triangle of the triangulated face
  //! @param theNodes the nodes of the face
  //! @param thePolyTriangle the indices of the triangle nodes
  //! @param theFaceLocation the location of the face
  //! @param theTriangle a triangle that should be extracted from the triangulated face
  static void getTriangleOnFace (const TColgp_Array1OfPnt& theNodes,
                                 const Poly_Triangle&      thePolyTriangle,
                                 const TopLoc_Location&    theFaceLocation,
                                 Triangle&                 theTriangle)
  {
    Standard_Integer aVertexNumbers[3];
    thePolyTriangle.Get (aVertexNumbers[0], aVertexNumbers[1], aVertexNumbers[2]);
    for (std::size_t aVertexIndex = 0; aVertexIndex < 3; ++aVertexIndex)
    {
      const Standard_Integer aNodeNumber      = aVertexNumbers[aVertexIndex];
      const gp_Pnt           aNode            = theNodes (aNodeNumber);
      const gp_Pnt           aTransformedNode = aNode.Transformed (theFaceLocation);
      theTriangle[aVertexIndex]               = aTransformedNode;
    }
  }

  //! Computes a cross section of the face triangulation with the plane
  //! @param theFaceTriangulation the triangulation of the face to be sectioned
  //! @param theFaceLocation the location of the face
  //! @param theSectionPlane the section plane
  //! @param theSectionSegments the container of section segments where the new segments should be added
  static void crossSection (const Handle (Poly_Triangulation) & theFaceTriangulation,
                            const TopLoc_Location& theFaceLocation,
                            const gp_Pln&          theSectionPlane,
                            Segments&              theSectionSegments)
  {
    const Poly_Array1OfTriangle& aTriangles = theFaceTriangulation->Triangles();
    const TColgp_Array1OfPnt&    aFaceNodes = theFaceTriangulation->Nodes();
    for (Standard_Integer aTriangleIndex = 1; aTriangleIndex <= aTriangles.Size(); ++aTriangleIndex)
    {
      const Poly_Triangle& aPolyTriangle = aTriangles (aTriangleIndex);
      Triangle             aTriangle;
      getTriangleOnFace (aFaceNodes, aPolyTriangle, theFaceLocation, aTriangle);
      crossSection (aTriangle, theSectionPlane, theSectionSegments);
    }
  }

  //! Checks whether the shape's bounding box intersects with the plane
  //! @param theShape the shape which bounding box takes part in checking on intersection with the given plane
  //! @param thePlane the plane that takes part in checking on intersection with the given shape's bounding box
  //! @return true if the shape's bounding box intersects with the plane, or false otherwise
  static bool intersects (const TopoDS_Shape& theShape, const gp_Pln& thePlane)
  {
    Bnd_Box aBoundingBox;
    BRepBndLib::Add (theShape, aBoundingBox);
    return !aBoundingBox.IsOut (thePlane);
  }

  //! Computes a cross section of the face triangulation with the plane
  //! @param theFaceTriangulation the triangulation of the face to be sectioned
  //! @param theFaceLocation the location of the face
  //! @param theSectionPlane the section plane
  //! @param theSectionSegments the container of section segments where the new segments should be added
  static void crossSection (const TopoDS_Face& theFace, const gp_Pln& theSectionPlane, Segments& theSectionSegments)
  {
    if (!intersects (theFace, theSectionPlane))
    {
      return;
    }
    TopLoc_Location aFaceLocation;
    const Handle (Poly_Triangulation)& aFaceTriangulation = BRep_Tool::Triangulation (theFace, aFaceLocation);
    if (aFaceTriangulation.IsNull())
    {
      return;
    }
    crossSection (aFaceTriangulation, aFaceLocation, theSectionPlane, theSectionSegments);
  }

  //! Computes a cross section of the shape with the plane
  //! @param theShape the shape consisting of triangulated faces
  //! @param theSectionPlane the plane that intersects the shape
  //! @param theSectionSegments the container of section segments where the section segments should be added
  static void crossSection (const TopoDS_Shape& theShape, const gp_Pln& theSectionPlane, Segments& theSectionSegments)
  {
    if (!intersects (theShape, theSectionPlane))
    {
      return;
    }
    for (TopExp_Explorer aFaceIterator (theShape, TopAbs_FACE); aFaceIterator.More(); aFaceIterator.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face (aFaceIterator.Current());
      crossSection (aFace, theSectionPlane, theSectionSegments);
    }
  }

} // namespace

//=======================================================================
// function : CrossSectionAsShape
// purpose  :
//=======================================================================
TopoDS_Compound StdPrs_SectionLines::CrossSectionAsShape (const TopoDS_Shape& theShape, const gp_Pln& theSectionPlane)
{
  Segments aSectionSegments;
  crossSection (theShape, theSectionPlane, aSectionSegments);
  TopoDS_Builder  aShapeBuilder;
  TopoDS_Compound aSectionShape;
  aShapeBuilder.MakeCompound (aSectionShape);
  for (Segments::const_iterator aSectionSegmentIterator = aSectionSegments.begin();
       aSectionSegmentIterator != aSectionSegments.end();
       ++aSectionSegmentIterator)
  {
    const Segment&    aSectionSegment = *aSectionSegmentIterator;
    const TopoDS_Edge aSectionEdge    = BRepBuilderAPI_MakeEdge (aSectionSegment.first, aSectionSegment.second);
    aShapeBuilder.Add (aSectionShape, aSectionEdge);
  }
  return aSectionShape;
}

//=======================================================================
// function : CrossSectionAsArraysOfSegments
// purpose  :
//=======================================================================
Handle (Graphic3d_ArrayOfPrimitives) StdPrs_SectionLines::CrossSectionAsArrayOfSegments (const TopoDS_Shape& theShape,
                                                                                         const gp_Pln& theSectionPlane)
{
  Segments aSectionSegments;
  crossSection (theShape, theSectionPlane, aSectionSegments);
  const Standard_Integer aNumberOfSectionEdges = static_cast<Standard_Integer> (2 * aSectionSegments.size());
  const Handle (Graphic3d_ArrayOfSegments)
    anArrayOfSectionEdges = new Graphic3d_ArrayOfSegments (aNumberOfSectionEdges);
  for (Segments::const_iterator aSectionSegmentIterator = aSectionSegments.begin();
       aSectionSegmentIterator != aSectionSegments.end();
       ++aSectionSegmentIterator)
  {
    const Segment& aSectionSegment = *aSectionSegmentIterator;
    anArrayOfSectionEdges->AddVertex (aSectionSegment.first);
    anArrayOfSectionEdges->AddVertex (aSectionSegment.second);
  }
  return anArrayOfSectionEdges;
}

//=======================================================================
// function : CrossSectionAsArraysOfTrianglesAndEdges
// purpose  :
//=======================================================================
void StdPrs_SectionLines::CrossSectionAsArraysOfTrianglesAndSegments (const TopoDS_Shape& theShape,
                                                                      const gp_Pln&       theSectionPlane,
                                                                      Handle (Graphic3d_ArrayOfPrimitives)
                                                                        & theArrayOfSectionTriangles,
                                                                      Handle (Graphic3d_ArrayOfPrimitives)
                                                                        & theArrayOfSectionSegments)
{
  (void)theShape;
  (void)theSectionPlane;
  (void)theArrayOfSectionTriangles;
  (void)theArrayOfSectionSegments;
}

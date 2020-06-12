#include "GeometrySamples.h"

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx> 
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Parab.hxx>
#include <gp_Hypr.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>



#include <Geom_CartesianPoint.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

#include "AdaptorPnt2d_AIS.h"

#include <Geom2d_CartesianPoint.hxx>


#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <AIS_Point.hxx>
#include <AIS_TextLabel.hxx>
#include <AIS_Axis.hxx>
#include <AIS_Circle.hxx>
#include <AIS_Plane.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ColoredShape.hxx>

//#include <AIS_Ellipse.hxx>
//#include <AIS_Parabola.hxx>
//#include <AIS_Hyperbola.hxx>


void GeometrySamples::ExecuteSample(TCollection_AsciiString theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "ZeroDimensionObjects3dSample")
    ZeroDimensionObjects3dSample();
  else if (theSampleName == "Vectors3dSample")
    Vectors3dSample();
  else if (theSampleName == "InfinityLines3dSample")
    InfinityLines3dSample();
  else if (theSampleName == "SecondOrderCurves3dSample")
    SecondOrderCurves3dSample();
  else if (theSampleName == "PlaneSurfaces3dSample")
    PlaneSurfaces3dSample();
  else if (theSampleName == "SecondOrderSurfaces3dSample")
    SecondOrderSurfaces3dSample();
  else if (theSampleName == "ZeroDimensionObjects2dSample")
    ZeroDimensionObjects2dSample();
  else if (theSampleName == "Vectors2dSample")
    Vectors2dSample();
  else if (theSampleName == "InfinityLines2dSample")
    InfinityLines2dSample();
  else if (theSampleName == "SecondOrderCurves2dSample")
    SecondOrderCurves2dSample();
  else if (theSampleName == "BarycenterPoint3dSample")
    BarycenterPoint3dSample();
  else if (theSampleName == "RotatedVector3dSample")
    RotatedVector3dSample();
  else if (theSampleName == "MirroredLine3dSample")
    MirroredLine3dSample();
  else if (theSampleName == "ScaledEllipse3dSample")
    ScaledEllipse3dSample();
  else if (theSampleName == "TransformedCylinder3dSample")
    TransformedCylinder3dSample();
  else if (theSampleName == "TranslatedTorus3dSample")
    TranslatedTorus3dSample();
  else if (theSampleName == "ConjugateObjects3dSample")
    ConjugateObjects3dSample();
  else if (theSampleName == "TangentAndNormal3dSample")
    TangentAndNormal3dSample();
  else if (theSampleName == "ProjectionOfPoint3dSample")
    ProjectionOfPoint3dSample();
  else if (theSampleName == "MinimalDistance3dSample")
    MinimalDistance3dSample();
  else if (theSampleName == "MirroredAxis2dSample")
    MirroredAxis2dSample();
  else if (theSampleName == "TranslatedPoint2dSample")
    TranslatedPoint2dSample();
  else if (theSampleName == "RotatedDirection2dSample")
    RotatedDirection2dSample();
  else if (theSampleName == "TangentAndNormal2dSample")
    TangentAndNormal2dSample();
  else if (theSampleName == "ProjectionOfPoint2dSample")
    ProjectionOfPoint2dSample();
  else if (theSampleName == "MinimalDistance2dSample")
    MinimalDistance2dSample();
  else if (theSampleName == "PointInfo3dSample")
    PointInfo3dSample();
  else if (theSampleName == "EllipseInfo3dSample")
    EllipseInfo3dSample();
  else if (theSampleName == "PointInfo2dSample")
    PointInfo2dSample();
  else if (theSampleName == "CircleInfo2dSample")
    CircleInfo2dSample();
  else if (theSampleName == "SecondOrderCurves3dSample")
    SecondOrderCurves3dSample();
  else if (theSampleName == "FreeStyleCurves3dSample")
    FreeStyleCurves3dSample();
  else if (theSampleName == "AnalyticalSurfaces3dSample")
    AnalyticalSurfaces3dSample();
  else if (theSampleName == "FreeStyleSurfaces3dSample")
    FreeStyleSurfaces3dSample();
  else if (theSampleName == "SecondOrderCurves2dSample")
    SecondOrderCurves2dSample();
  else if (theSampleName == "FreeStyleCurves2dSample")
    FreeStyleCurves2dSample();
  else if (theSampleName == "OffsetCurve3dSample")
    OffsetCurve3dSample();
  else if (theSampleName == "BSplineFromCircle3dSample")
    BSplineFromCircle3dSample();
  else if (theSampleName == "TrimmedSurface3dSample")
    TrimmedSurface3dSample();
  else if (theSampleName == "ExtrusionSurface3dSample")
    ExtrusionSurface3dSample();
  else if (theSampleName == "RevolutionSurface3dSample")
    RevolutionSurface3dSample();
  else if (theSampleName == "OffsetCurve3dSample")
    OffsetCurve3dSample();
  else if (theSampleName == "BoundingBoxOfSurface3dSample")
    BoundingBoxOfSurface3dSample();
  else if (theSampleName == "BoundingBoxOfCurves3dSample")
    BoundingBoxOfCurves3dSample();
  else if (theSampleName == "BoundingBoxOfCurves2dSample")
    BoundingBoxOfCurves2dSample();
  else if (theSampleName == "JsonWithCircleRadiusSample")
    JsonWithCircleRadiusSample();
  else if (theSampleName == "JsonWithBSplineCurveInfoSample")
    JsonWithBSplineCurveInfoSample();
  else
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}

void GeometrySamples::ZeroDimensionObjects3dSample()
{
  // gp_Pnt describes a point in 3D space. A Geom_CartesianPoint is defined by 
  // a gp_Pnt point, with its three Cartesian coordinates X, Y and Z.
  gp_Pnt aCoordPnt(10.0, 20.0, 30.0);
  Handle(Geom_CartesianPoint) aCoordGeomPoint = new Geom_CartesianPoint(aCoordPnt);
  Handle(AIS_Point) aCoordAisPoint = new AIS_Point(aCoordGeomPoint);
  myObject3d.Append(aCoordAisPoint);

  Handle(AIS_TextLabel) aPntLabel = new AIS_TextLabel();
  aPntLabel->SetText("gp_Pnt");
  aPntLabel->SetPosition(gp_Pnt(aCoordPnt.X(), aCoordPnt.Y(), aCoordPnt.Z() + 5.0));
  myObject3d.Append(aPntLabel);
  myResult << "gp_Pnt was created" << std::endl;

  // gp_XYZ class describes a cartesian coordinate entity in 3D space (X,Y,Z).
  // This entity is used for algebraic calculation.
  // This entity can be transformed with a "Trsf" or a "GTrsf" from package "gp".
  // It is used in vectorial computations or for holding this type of information in data structures.
  gp_XYZ aXyz1(10.0, 20.0, 30.0);
  gp_XYZ aXyz2(20.0, 10.0, 30.0);
  gp_XYZ aXyzSum = aXyz1 + aXyz2;
  gp_Pnt aSumPnt(aXyzSum);
  Handle(Geom_CartesianPoint) aXyzGeomPoint = new Geom_CartesianPoint(aSumPnt);
  Handle(AIS_Point) aSumAisPoint = new AIS_Point(aXyzGeomPoint);
  myObject3d.Append(aSumAisPoint);

  Handle(AIS_TextLabel) aXyzLabel = new AIS_TextLabel();
  aXyzLabel->SetText("gp_XYZ");
  aXyzLabel->SetPosition(gp_Pnt(aXyzSum.X(), aXyzSum.Y(), aXyzSum.Z() + 5));
  myObject3d.Append(aXyzLabel);
  myResult << "gp_XYZ was created" << std::endl;

}

void GeometrySamples::Vectors3dSample()
{
  gp_Pnt aPnt1(0.0, 0.0, 0.0);
  gp_Pnt aPnt2(5.0, 0.0, 0.0);

  // gp_Vec defines a non-persistent vector in 3D space.
  gp_Vec aVec(aPnt1, aPnt2);
  myResult << "gp_Vec magnitude: " << aVec.Magnitude() << std::endl;
  Handle(Geom_VectorWithMagnitude) aGeomVector = new Geom_VectorWithMagnitude(aVec);

  // Describes a unit vector in 3D space.
  // This unit vector is also called "Direction".
  // See Also gce_MakeDir which provides functions for more complex unit vector 
  // constructions Geom_Direction which provides additional functions 
  // for constructing unit vectors and works, in particular, 
  // with the parametric equations of unit vectors.
  gp_Dir aDir(aVec);
  myResult << "gp_Dir coorditates: X: " << aDir.X()
           << ", Y: " << aDir.Y() << ", Z: " << aDir.Z() << std::endl;
}

void GeometrySamples::InfinityLines3dSample()
{
  gp_Pnt aBasePoint(0.0, 0.0, 0.0);
  gp_Dir aZ_Direction(0.0, 0.0, 1.0);

  // Describes an axis in 3D space.
  gp_Ax1 anAxis1(aBasePoint, aZ_Direction);
  
  Handle(Geom_Axis1Placement) anAxis1Placement = new Geom_Axis1Placement(anAxis1);
  Handle(AIS_Axis) anAisAxis1 = new AIS_Axis(anAxis1Placement);
  myObject3d.Append(anAisAxis1);
  myResult << "gp_Ax1 was created" << std::endl << std::endl;


  //  Describes a right - handed coordinate system in 3D space.
  aBasePoint.SetXYZ(gp_XYZ(0.0, 0.0, 10.0));
  gp_Ax2 anAxis2(aBasePoint, aZ_Direction);
  Handle(Geom_Axis2Placement) anAxis2Placement = new Geom_Axis2Placement(anAxis2);
  Handle(AIS_Axis) anAisAxis2 = new AIS_Axis(anAxis2Placement, AIS_TOAX_XAxis);
  myObject3d.Append(anAisAxis2);
  myResult << "gp_Ax2 was created" << std::endl;

  const gp_Dir& anAxis2_xDir = anAxis2.XDirection();
  const gp_Dir& anAxis2_yDir = anAxis2.YDirection();
  myResult << "gp_Ax2 X direction: " << anAxis2_xDir.X() <<  " " 
           << anAxis2_xDir.Y() <<  " " << anAxis2_xDir.Z() << std::endl;
  myResult << "gp_Ax2 Y direction: " << anAxis2_yDir.X() << " "
    << anAxis2_yDir.Y() << " " << anAxis2_yDir.Z() << std::endl << std::endl;

  // Describes a coordinate system in 3D space.Unlike a gp_Ax2 coordinate system, 
  // a gp_Ax3 can be right - handed("direct sense") or left - handed("indirect sense").
  gp_Dir aX_Direction(1.0, 0.0, 0.0);
  gp_Ax3 anAxis3(gp_XYZ(0.0, 0.0, 20.0), aZ_Direction, aX_Direction);
  anAxis3.YReverse();
  myResult << "gp_Ax3 was created" << std::endl;
  const gp_Dir& anAxis3_xDir = anAxis3.XDirection();
  const gp_Dir& anAxis3_yDir = anAxis3.YDirection();
  myResult << "gp_Ax3 X direction: " << anAxis3_xDir.X() << " "
    << anAxis3_xDir.Y() << " " << anAxis3_xDir.Z() << std::endl;
  myResult << "gp_Ax3 Y direction: " << anAxis3_yDir.X() << " "
    << anAxis3_yDir.Y() << " " << anAxis3_yDir.Z() << std::endl;
  TCollection_AsciiString aDirectionDescribtion;
  if (anAxis3.Direct())
    aDirectionDescribtion = "anAxis3 is a right-handed axis system";
  else
    aDirectionDescribtion = "anAxis3 is a left-handed axis system";
  myResult << aDirectionDescribtion << std::endl << std::endl;

  // Describes a line in 3D space. A line is positioned in space with an axis
  // (a gp_Ax1 object) which gives it an origin and a unit vector. 
  gp_Lin aLine(gp_Pnt(0.0, 0.0, 30.0), gp_Dir(0.0, 1.0, 0.0));
  myResult << "gp_Lin was created" << std::endl << std::endl;
}

void GeometrySamples::SecondOrderCurves3dSample()
{
  gp_Ax2 anAxis2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 2.0, 0.0));
  gp_Circ aCirc(anAxis2, 10.0);
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(aCirc);
  Handle(AIS_Circle) aAisCircle = new AIS_Circle(aGeomCircle);
  myObject3d.Append(aAisCircle);

  // Describes an ellipse in 3D space. An ellipse is defined by its major and minor 
  // radii and positioned in space with a coordinate system (a gp_Ax2 object) 
//  gp_Elips anElips(anAxis2.Translated(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 0.0, 10.0)),
//                   10.0, 20.0);
//  Handle(Geom_Ellipse) aEllipse = new Geom_Ellipse(anElips);
//  Handle(AIS_Circle) aAisCircle = new AIS_Circle(aGeomCircle);
//  myObject3d.Append(aAisCircle);

//#include <Geom_Ellipse.hxx>
//#include <Geom_Parabola.hxx>
//#include <Geom_Hyperbola.hxx>
  //  , gp_Ellips, gp_Parab, gp_Hypr
}

void GeometrySamples::PlaneSurfaces3dSample()
{

}

void GeometrySamples::SecondOrderSurfaces3dSample()
{

}

void GeometrySamples::ZeroDimensionObjects2dSample()
{
  // Defines a non-persistent 2D cartesian point.
  gp_Pnt2d aCoordPnt(10.0, 20.0);
  Handle(Geom2d_CartesianPoint) aCoordGeomPoint = new Geom2d_CartesianPoint(aCoordPnt);
  Handle(AdaptorPnt2d_AIS) aCoordAisPoint = new AdaptorPnt2d_AIS(aCoordGeomPoint);
  myObject3d.Append(aCoordAisPoint);

  Handle(AIS_TextLabel) aPntLabel = new AIS_TextLabel();
  aPntLabel->SetText("gp_Pnt2d");
  aPntLabel->SetPosition(gp_Pnt(aCoordPnt.X(), aCoordPnt.Y(), 0.0));
  myObject3d.Append(aPntLabel);
  myResult << "gp_Pnt was created" << std::endl;

  //// gp_XYZ class describes a cartesian coordinate entity in 3D space (X,Y,Z).
  //// This entity is used for algebraic calculation.
  //// This entity can be transformed with a "Trsf" or a "GTrsf" from package "gp".
  //// It is used in vectorial computations or for holding this type of information in data structures.
  //gp_XYZ aXyz1(10.0, 20.0, 30.0);
  //gp_XYZ aXyz2(20.0, 10.0, 30.0);
  //gp_XYZ aXyzSum = aXyz1 + aXyz2;
  //gp_Pnt aSumPnt(aXyzSum);
  //Handle(Geom_CartesianPoint) aXyzGeomPoint = new Geom_CartesianPoint(aSumPnt);
  //Handle(AIS_Point) aSumAisPoint = new AIS_Point(aXyzGeomPoint);
  //myObject3d.Append(aSumAisPoint);

  //Handle(AIS_TextLabel) aXyzLabel = new AIS_TextLabel();
  //aXyzLabel->SetText("gp_XYZ");
  //aXyzLabel->SetPosition(gp_Pnt(aXyzSum.X(), aXyzSum.Y(), aXyzSum.Z() + 5));
  //myObject3d.Append(aXyzLabel);
  //myResult << "gp_XYZ was created" << std::endl;
}

void GeometrySamples::Vectors2dSample()
{

}

void GeometrySamples::InfinityLines2dSample()
{

}

void GeometrySamples::SecondOrderCurves2dSample()
{

}

void GeometrySamples::BarycenterPoint3dSample()
{

}

void GeometrySamples::RotatedVector3dSample()
{

}

void GeometrySamples::MirroredLine3dSample()
{

}

void GeometrySamples::ScaledEllipse3dSample()
{

}

void GeometrySamples::TransformedCylinder3dSample()
{

}

void GeometrySamples::TranslatedTorus3dSample()
{

}

void GeometrySamples::ConjugateObjects3dSample()
{

}

void GeometrySamples::TangentAndNormal3dSample()
{

}

void GeometrySamples::ProjectionOfPoint3dSample()
{

}

void GeometrySamples::MinimalDistance3dSample()
{

}

void GeometrySamples::Intersection3dSample()
{

}

void GeometrySamples::TranslatedPoint2dSample()
{

}

void GeometrySamples::RotatedDirection2dSample()
{

}

void GeometrySamples::MirroredAxis2dSample()
{

}

void GeometrySamples::TransformedEllipse2dSample()
{

}

void GeometrySamples::TangentAndNormal2dSample()
{

}

void GeometrySamples::ProjectionOfPoint2dSample()
{

}

void GeometrySamples::MinimalDistance2dSample()
{

}

void GeometrySamples::Intersection2dSample()
{

}

void GeometrySamples::PointInfo3dSample()
{

}

void GeometrySamples::EllipseInfo3dSample()
{

}

void GeometrySamples::PointInfo2dSample()
{

}

void GeometrySamples::CircleInfo2dSample()
{

}

void GeometrySamples::ParamSecondOrderCurves3dSample()
{

}

void GeometrySamples::FreeStyleCurves3dSample()
{
  // Define points.
  gp_Pnt aPnt1(0.0,  0.0, 0.0);
  gp_Pnt aPnt2(5.0,  5.0, 0.0);
  gp_Pnt aPnt3(10.0, 5.0, 0.0);
  gp_Pnt aPnt4(15.0, 0.0, 0.0);

  // Add points to the curve poles array.
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles.SetValue(1, aPnt1);
  aPoles.SetValue(2, aPnt2);
  aPoles.SetValue(3, aPnt3);
  aPoles.SetValue(4, aPnt4);

  // Define BSpline weights.
  TColStd_Array1OfReal aBSplineWeights(1, 4);
  aBSplineWeights.SetValue(1, 1.0);
  aBSplineWeights.SetValue(2, 0.5);
  aBSplineWeights.SetValue(3, 0.5);
  aBSplineWeights.SetValue(4, 1.0);

  // Define knots.
  TColStd_Array1OfReal aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);

  // Define multiplicities.
  TColStd_Array1OfInteger aMults(1, 2);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  // Define BSpline degree and periodicity.
  Standard_Integer aDegree = 3;
  Standard_Boolean aPeriodic = Standard_False;

  // Create a BSpline curve.
  Handle(Geom_BSplineCurve) aBSplineCurve = new Geom_BSplineCurve(aPoles, aBSplineWeights, aKnots, aMults, aDegree, aPeriodic);
  myResult << "Geom_BSplineCurve was created in red" << std::endl;

  // Define Bezier weights.
  TColStd_Array1OfReal aBezierWeights(1, 4);
  aBezierWeights.SetValue(1, 0.5);
  aBezierWeights.SetValue(2, 1.5);
  aBezierWeights.SetValue(3, 1.5);
  aBezierWeights.SetValue(4, 0.5);

  // Create Bezier curve.
  Handle(Geom_BezierCurve) aBezierCurve = new Geom_BezierCurve(aPoles, aBezierWeights);
  myResult << "Geom_BezierCurve was created in green" << std::endl;

  Handle(AIS_ColoredShape) anAisBSplineCurve = new AIS_ColoredShape(BRepBuilderAPI_MakeEdge(aBSplineCurve).Shape());
  Handle(AIS_ColoredShape) anAisBezierCurve = new AIS_ColoredShape(BRepBuilderAPI_MakeEdge(aBezierCurve).Shape());
  anAisBSplineCurve->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisBezierCurve->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisBSplineCurve);
  myObject3d.Append(anAisBezierCurve);
  myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt1)));
  myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt2)));
  myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt3)));
  myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(aPnt4)));
}

void GeometrySamples::AnalyticalSurfaces3dSample()
{
  // Define a XY plane.
  gp_Pln aPln(gp::Origin(), gp::DZ());
  // Create plane geometry.
  Handle(Geom_Plane) aPlaneSurf = new Geom_Plane(aPln);
  myResult << "Geom_Plane was created in red" << std::endl;

  // Define a cylinder.
  gp_Cylinder aCyl(gp::XOY(), 2.5);
  // Create cylindrical surface.
  Handle(Geom_CylindricalSurface) aCylSurf = new Geom_CylindricalSurface(aCyl);
  myResult << "Geom_CylindricalSurface was created in green" << std::endl;

  // Define a cone.
  gp_Cone aCone(gp::XOY(), M_PI_4, 2.5);
  // Create conical surface.
  Handle(Geom_ConicalSurface) aConeSurf = new Geom_ConicalSurface(aCone);
  myResult << "Geom_ConicalSurface was created in blue" << std::endl;

  // Define a sphere.
  gp_Pnt aSphereCenter(15.0, 15.0, 15.0);
  gp_Sphere aSphere(gp_Ax3(aSphereCenter, gp::DZ()), 8.0);
  // Create conical surface.
  Handle(Geom_SphericalSurface) aSphereSurf = new Geom_SphericalSurface(aSphere);
  myResult << "Geom_SphericalSurface was created in cyan" << std::endl;

  // Define a sphere.
  gp_Pnt aTorusCenter(-15.0, -15.0, 25.0);
  gp_Torus aTorus(gp_Ax3(aTorusCenter, gp::DZ()), 15.0, 5.0);
  // Create toroidal surface.
  Handle(Geom_ToroidalSurface) aTorusSurf = new Geom_ToroidalSurface(aTorus);
  myResult << "Geom_ToroidalSurface was created in yellow" << std::endl;

  Handle(AIS_ColoredShape) anAisPlane = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(aPlaneSurf, 0.0, 20.0, 0.0, 20.0, 0.001).Shape());
  Handle(AIS_ColoredShape) anAisCylinder = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(aCylSurf, 0.0, 2.0 * M_PI, 5.0, 15.0, 0.001).Shape());
  Handle(AIS_ColoredShape) anAisCone = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(aConeSurf, 0.0, 2.0 * M_PI, 0.0, 15.0, 0.001).Shape());
  Handle(AIS_ColoredShape) anAisSphere = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(aSphereSurf, 0.001).Shape());
  Handle(AIS_ColoredShape) anAisTorus = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(aTorusSurf, 0.001).Shape());
  anAisPlane->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisCylinder->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  anAisCone->SetColor(Quantity_Color(Quantity_NOC_BLUE1));
  anAisSphere->SetColor(Quantity_Color(Quantity_NOC_CYAN1));
  anAisTorus->SetColor(Quantity_Color(Quantity_NOC_YELLOW));
  myObject3d.Append(anAisPlane);
  myObject3d.Append(anAisCylinder);
  myObject3d.Append(anAisCone);
  myObject3d.Append(anAisSphere);
  myObject3d.Append(anAisTorus);
}

void GeometrySamples::FreeStyleSurfaces3dSample()
{
  // Define a 4x4 grid of points for BSpline surface.
  TColgp_Array2OfPnt aBSplinePnts(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    gp_Pnt aPnt;
    aPnt.SetX(5.0 * i);
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      aPnt.SetY(5.0 * j);
      if (1 < i && i < 4 && 1 < j && j < 4)
        aPnt.SetZ(5.0);
      else
        aPnt.SetZ(0.0);
      aBSplinePnts.SetValue(i, j, aPnt);
    }
  }

  // Define a 4x4 grid of points for Bezier surface.
  TColgp_Array2OfPnt aBezierPnts(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    gp_Pnt aPnt;
    aPnt.SetX(20.0 + 5.0 * i);
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      aPnt.SetY(20.0 + 5.0 * j);
      if (1 < i && i < 4 && 1 < j && j < 4)
        aPnt.SetZ(5.0);
      else
        aPnt.SetZ(0.0);
      aBezierPnts.SetValue(i, j, aPnt);
    }
  }

  // Define BSpline weights.
  TColStd_Array2OfReal aBSplineWeights(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      if (1 < i && i < 4 && 1 < j && j < 4)
        aBSplineWeights.SetValue(i, j, 0.5);
      else
        aBSplineWeights.SetValue(i, j, 1.0);
    }
  }

  // Define knots.
  TColStd_Array1OfReal aUKnots(1, 2), aVKnots(1, 2);
  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);

  // Define multiplicities.
  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUMults.SetValue(1, 4);
  aUMults.SetValue(2, 4);
  aVMults.SetValue(1, 4);
  aVMults.SetValue(2, 4);

  // Define BSpline degree and periodicity.
  Standard_Integer aUDegree = 3;
  Standard_Integer aVDegree = 3;
  Standard_Boolean aUPeriodic = Standard_False;
  Standard_Boolean aVPeriodic = Standard_False;

  // Create a BSpline surface.
  Handle(Geom_BSplineSurface) aBSplineSurf = new Geom_BSplineSurface(aBSplinePnts, aBSplineWeights, aUKnots, aVKnots,
    aUMults, aVMults, aUDegree, aVDegree, aUPeriodic, aVPeriodic);
  myResult << "Geom_BSplineSurface was created: red" << std::endl;

  // Define BSpline weights.
  TColStd_Array2OfReal aBezierWeights(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      if (1 < i && i < 4 && 1 < j && j < 4)
        aBezierWeights.SetValue(i, j, 1.5);
      else
        aBezierWeights.SetValue(i, j, 0.5);
    }
  }

  // Create a Bezier surface.
  Handle(Geom_BezierSurface) aBezierSurf = new Geom_BezierSurface(aBezierPnts, aBezierWeights);
  myResult << "Geom_BezierSurface was created: green" << std::endl;

  Handle(AIS_ColoredShape) anAisBSplineSurf = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(aBSplineSurf, 0.001).Shape());
  Handle(AIS_ColoredShape) anAisBezierSurf = new AIS_ColoredShape(BRepBuilderAPI_MakeFace(aBezierSurf, 0.001).Shape());
  anAisBSplineSurf->SetColor(Quantity_Color(Quantity_NOC_RED));
  anAisBezierSurf->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  myObject3d.Append(anAisBSplineSurf);
  myObject3d.Append(anAisBezierSurf);
  for (TColgp_Array2OfPnt::Iterator anIt(aBSplinePnts); anIt.More(); anIt.Next())
  {
    myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(anIt.Value())));
  }
  for (TColgp_Array2OfPnt::Iterator anIt(aBezierPnts); anIt.More(); anIt.Next())
  {
    myObject3d.Append(new AIS_Point(new Geom_CartesianPoint(anIt.Value())));
  }
}

void GeometrySamples::ParamSecondOrderCurves2dSample()
{

}

void GeometrySamples::FreeStyleCurves2dSample()
{

}

void GeometrySamples::TrimmedCurve3dSample()
{

}

void GeometrySamples::OffsetCurve3dSample()
{

}

void GeometrySamples::BSplineFromCircle3dSample()
{

}

void GeometrySamples::TrimmedSurface3dSample()
{

}

void GeometrySamples::OffsetSurface3dSample()
{

}

void GeometrySamples::ExtrusionSurface3dSample()
{

}

void GeometrySamples::RevolutionSurface3dSample()
{

}

void GeometrySamples::TrimmedCurve2dSample()
{

}

void GeometrySamples::ParamOffsetCurve3dSample()
{

}

void GeometrySamples::BoundingBoxOfSurface3dSample()
{

}

void GeometrySamples::BoundingBoxOfCurves3dSample()
{

}

void GeometrySamples::BoundingBoxOfCurves2dSample()
{

}

void GeometrySamples::JsonWithCircleRadiusSample()
{

}

void GeometrySamples::JsonWithBSplineCurveInfoSample()
{

}

#ifndef GEOMETRYSAMPLES_H
#define GEOMETRYSAMPLES_H


#include "BaseSample.h"


class GeometrySamples;
DEFINE_STANDARD_HANDLE(GeometrySamples, BaseSample)

class GeometrySamples : public BaseSample
{
public:
  DEFINE_STANDARD_RTTI_INLINE(GeometrySamples, BaseSample)

  virtual ~GeometrySamples() {}
protected:
  void ExecuteSample(TCollection_AsciiString theSampleName) override;

private:
  void DisplayPnt(gp_Pnt thePnt, TCollection_AsciiString theText,
                  Aspect_TypeOfMarker theMarker = Aspect_TOM_PLUS);
  void DisplayPnt(gp_Pnt2d thePnt2d, TCollection_AsciiString theText,
                  Aspect_TypeOfMarker theMarker = Aspect_TOM_PLUS);

  // One function for every sample
  void ZeroDimensionObjects3dSample();
  void Vectors3dSample();
  void InfinityLines3dSample();
  void SecondOrderCurves3dSample();
  void PlaneSurfaces3dSample();
  void SecondOrderSurfaces3dSample();
  void ZeroDimensionObjects2dSample();
  void Vectors2dSample();
  void InfinityLines2dSample();
  void SecondOrderCurves2dSample();
  void BarycenterPoint3dSample();
  void RotatedVector3dSample();
  void MirroredLine3dSample();
  void ScaledEllipse3dSample();
  void TransformedCylinder3dSample();
  void TranslatedTorus3dSample();
  void ConjugateObjects3dSample();
  void ProjectionOfPoint3dSample();
  void MinimalDistance3dSample();
  void Intersection3dSample();
  void TranslatedPoint2dSample();
  void RotatedDirection2dSample();
  void MirroredAxis2dSample();
  void TransformedEllipse2dSample();
  void ConjugateObjects2dSample();
  void Tangent2dSample();
  void ProjectionOfPoint2dSample();
  void MinimalDistance2dSample();
  void Intersection2dSample();
  void PointInfo3dSample();
  void EllipseInfo3dSample();
  void PointInfo2dSample();
  void CircleInfo2dSample();
  void ParamSecondOrderCurves3dSample();
  void FreeStyleCurves3dSample();
  void AnalyticalSurfaces3dSample();
  void FreeStyleSurfaces3dSample();
  void ParamSecondOrderCurves2dSample();
  void FreeStyleCurves2dSample();
  void TrimmedCurve3dSample();
  void OffsetCurve3dSample();
  void BSplineFromCircle3dSample();
  void TrimmedSurface3dSample();
  void OffsetSurface3dSample();
  void ExtrusionSurface3dSample();
  void RevolutionSurface3dSample();
  void TrimmedCurve2dSample();
  void BoundingBoxOfSurface3dSample();
  void BoundingBoxOfCurves3dSample();
  void BoundingBoxOfCurves2dSample();
  void DumpCircleInfoSample();
  void DumpBSplineCurveInfoSample();

};

#endif  //GEOMETRYSAMPLES_H

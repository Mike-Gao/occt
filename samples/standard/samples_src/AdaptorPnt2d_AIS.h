#ifndef ADAPTOR_PNT2D_AIS_H
#define ADAPTOR_PNT2D_AIS_H

#include <Geom2d_Point.hxx>
#include <AIS_InteractiveObject.hxx>


class AdaptorPnt2d_AIS;
DEFINE_STANDARD_HANDLE(AdaptorPnt2d_AIS, AIS_InteractiveObject)
class AdaptorPnt2d_AIS : public AIS_InteractiveObject
{
public:
  AdaptorPnt2d_AIS(const Handle(Geom2d_Point) thePoint, Standard_Real theElevation = 0);
  virtual ~AdaptorPnt2d_AIS();

  DEFINE_STANDARD_RTTI_INLINE(AdaptorPnt2d_AIS, AIS_InteractiveObject)

private:

  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
    const Handle(Prs3d_Presentation)& aPresentation,
    const Standard_Integer aMode);
  void Compute(const Handle(Prs3d_Projector)& aProjector,
    const Handle(Prs3d_Presentation)& aPresentation);

  void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
    const Standard_Integer unMode);

  gp_Pnt myPoint;

};

#endif // ADAPTOR_PNT2D_AIS_H

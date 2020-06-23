#ifndef ADAPTOR_VEC_AIS_H
#define ADAPTOR_VEC_AIS_H

#include <AIS_InteractiveObject.hxx>




class AdaptorVec_AIS;
DEFINE_STANDARD_HANDLE(AdaptorVec_AIS, AIS_InteractiveObject)
class AdaptorVec_AIS : public AIS_InteractiveObject
{
public:

  void SetText(const TCollection_AsciiString& aText);
  void SetLineAspect(const Handle(Prs3d_LineAspect)& theAspect);
  AdaptorVec_AIS();
  AdaptorVec_AIS(const gp_Pnt& aPnt, const gp_Dir& aDir, Standard_Real aLength = 1, Standard_Real anArrowLength = 1);
  AdaptorVec_AIS(const gp_Pnt& aPnt, const gp_Vec& aVec, Standard_Real anArrowLength = 1);

  AdaptorVec_AIS(const gp_Pnt2d& aPnt2d, const gp_Dir2d& aDir2d, Standard_Real aLength = 1, Standard_Real anArrowLength = 1);
  AdaptorVec_AIS(const gp_Pnt2d& aPnt2d, const gp_Vec2d& aVec2d, Standard_Real anArrowLength = 1);
  AdaptorVec_AIS(const gp_Pnt2d& aPnt1, const gp_Pnt2d& aPnt2, Standard_Real anArrowLength = 1);

  virtual ~AdaptorVec_AIS();
  DEFINE_STANDARD_RTTI_INLINE(AdaptorVec_AIS, AIS_InteractiveObject)

private:

  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
    const Handle(Prs3d_Presentation)& aPresentation,
    const Standard_Integer aMode);
  void Compute(const Handle(Prs3d_Projector)& aProjector,
    const Handle(Prs3d_Presentation)& aPresentation);
  void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode);

  gp_Pnt myPnt;
  gp_Dir myDir;
  Standard_Real myLength;
  Standard_Real myArrowLength;
  TCollection_AsciiString myText;
};

#endif // ADAPTOR_VEC2D_AIS_H

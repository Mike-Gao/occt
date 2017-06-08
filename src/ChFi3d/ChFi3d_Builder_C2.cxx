// Created on: 1996-08-20
// Created by: Stagiaire Xuan Tang PHAMPHU
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <AppBlend_Approx.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <Extrema_ExtPC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>

static void Reduce(const Standard_Real& p1,
		   const Standard_Real& p2,
		   Handle(GeomAdaptor_HSurface)& hs1,
		   Handle(GeomAdaptor_HSurface)& hs2)
{
  GeomAdaptor_Surface& s1 = hs1->ChangeSurface();
  GeomAdaptor_Surface& s2 = hs2->ChangeSurface();
  const Handle(Geom_Surface)& surf = s1.Surface();
  Standard_Real ud,uf,vd,vf;
  surf->Bounds(ud,uf,vd,vf);
  Standard_Real milmoins = 0.51*vd+0.49*vf, milplus = 0.49*vd+0.51*vf;
  if(p1 < p2) {
    s1.Load(surf,ud,uf,vd,milmoins);
    s2.Load(surf,ud,uf,milplus,vf);
  }
  else{
    s1.Load(surf,ud,uf,milplus,vf);
    s2.Load(surf,ud,uf,vd,milmoins);
  }  
}

static void Reduce(const Standard_Real& p1,
		   const Standard_Real& p2,
		   Handle(GeomAdaptor_HCurve)& hc)
{
  GeomAdaptor_Curve& c = hc->ChangeCurve();
  Standard_Real f = c.FirstParameter();
  Standard_Real l = c.LastParameter();
  Standard_Real milmoins = 0.51*f+0.49*l, milplus = 0.49*f+0.51*l;
  if(p1 < p2) {
    c.Load(c.Curve(),f,milmoins);
  }
  else{
    c.Load(c.Curve(),milplus,l);
  }  
}


//=======================================================================
//function : PerformTwoCornerbyInter
//purpose  : Performs PerformTwoCorner by intersection.
//           In case of Biseau for all cases the 
//           path is used; 3D curve and 2 pcurves are approximated.
//=======================================================================

Standard_Boolean ChFi3d_Builder::PerformTwoCornerbyInter(const Standard_Integer Index)

{
  done = 0;
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Index);
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  BRep_Builder BB;
  TopoDS_Vertex Vertices [3];

  //Information on fillets is extracted 
  //------------------------------------------------------

  //the first
  //----------
  ChFiDS_ListIteratorOfListOfStripe It;
  It.Initialize(myVDataMap(Index));
  Handle(ChFiDS_Stripe)& Corner1 = It.Value(); 
  Standard_Integer Sens1;
  Standard_Integer IFd1 = 
    ChFi3d_IndexOfSurfData(Vtx,Corner1,Sens1);
  ChFiDS_SequenceOfSurfData& SeqFil1 =    
    Corner1->ChangeSetOfSurfData()->ChangeSequence();
  Handle(ChFiDS_SurfData)& Fd1 = SeqFil1.ChangeValue(IFd1);
  //jgv
  Standard_Integer IndexOfNewFace1 = Fd1->IndexOfFace();
  /////

  //the second
  //----------
  It.Next();
  Handle(ChFiDS_Stripe)& Corner2 = It.Value(); 
  Standard_Integer Sens2;
  Standard_Integer IFd2;
  if(Corner2 == Corner1) {
    Sens2 = -1;
    IFd2 = Corner2->SetOfSurfData()->Length();
  }
  else{ IFd2 = ChFi3d_IndexOfSurfData(Vtx,Corner2,Sens2); }
  ChFiDS_SequenceOfSurfData& SeqFil2 =    
    Corner2->ChangeSetOfSurfData()->ChangeSequence();
  Handle(ChFiDS_SurfData)& Fd2 = SeqFil2.ChangeValue(IFd2);
  //jgv
  Standard_Integer IndexOfNewFace2 = Fd2->IndexOfFace();
  /////

  // The concavities are analysed in case of differents concavities, 
  // preview an evolutionary connection of type ThreeCorner of R to 0.
  // Otherwise the opposite face  
  // and the eventual intersection of 2 pcurves on this face are found.

  Standard_Boolean isfirst1 = (Sens1 == 1);
  Standard_Boolean isfirst2 = (Sens2 == 1);
  Standard_Boolean  OkinterCC,Okvisavis,SameSide;
  Standard_Integer IFaCo1,IFaCo2;
  Standard_Real UIntPC1,UIntPC2;
  TopoDS_Face FaCo;
  OkinterCC = ChFi3d_IsInFront(DStr,Corner1,Corner2,IFd1,IFd2,Sens1,Sens2,
			       UIntPC1,UIntPC2,FaCo,SameSide,
			       IFaCo1,IFaCo2,Okvisavis,Vtx,Standard_True);
  if (!Okvisavis) {
#ifdef OCCT_DEBUG
    cout<<"TwoCorner : pas de face commune"<<endl;
#endif
    done=Standard_False;
    return done;
  }
  if (!OkinterCC) {
     // The intersection of pcurves is calculated without restricting them by  
    // common points.
    OkinterCC= ChFi3d_IsInFront(DStr,Corner1,Corner2,IFd1,IFd2,Sens1,Sens2,
				 UIntPC1,UIntPC2,FaCo,SameSide,
				 IFaCo1,IFaCo2,Okvisavis,Vtx,Standard_True,1);
  }
  
  if (!Okvisavis) {
#ifdef OCCT_DEBUG
    cout<<"TwoCorner : no common face"<<endl;
#endif
    done=Standard_False;
    return done;
  }   
  if (!OkinterCC) {
#ifdef OCCT_DEBUG
    cout<<"biseau : failed intersection of tangency lines on common face"<<endl;
#endif
    done=Standard_False;
    return done;
  }
  Standard_Integer IFaArc1 = 3-IFaCo1, IFaArc2 = 3-IFaCo2;
  
  // It is checked if the fillets have a commonpoint on a common arc.
  // This edge is the pivot of the bevel or of the kneecap.
  
  ChFiDS_CommonPoint& CP1 = Fd1->ChangeVertex(isfirst1,IFaArc1);
  ChFiDS_CommonPoint& CP2 = Fd2->ChangeVertex(isfirst2,IFaArc2);

  if (!CP1.IsOnArc() || !CP2.IsOnArc()) {
#ifdef OCCT_DEBUG
    cout<<"fail 1 of 2 fillets are not on arc"<<endl;
#endif
    done=Standard_False;
    return done;
  }
  if ( ! CP1.Arc().IsSame( CP2.Arc()) ) {
    // look like OnSame + OnDiff case (eap, Arp 9 2002, occ266)
#ifdef OCCT_DEBUG
    cout<<"PerformTwoCornerbyInter(): fillets are not on the same arc"<<endl;
#endif
    done = Standard_True;
    PerformMoreThreeCorner(Index, 2);
    return done;
  }
    
  TopoDS_Edge pivot;
  pivot = CP1.Arc();
  Standard_Real parCP1 = CP1.ParameterOnArc();
  Standard_Real parCP2 = CP2.ParameterOnArc();
  Handle(BRepAdaptor_HCurve) Hpivot = new BRepAdaptor_HCurve(pivot);
  if (!pivot.IsSame(CP2.Arc())){ 
    Handle(Geom_Curve) csau;
    Standard_Real ubid,vbid;
    csau=BRep_Tool::Curve(pivot,ubid,vbid );
    Handle(Geom_BoundedCurve) C1= Handle(Geom_BoundedCurve)::DownCast(csau);
    if (! C1.IsNull()) {
      GeomLib::ExtendCurveToPoint(C1,CP2.Point(),1,Standard_False);
      GeomAdaptor_Curve cad;
      cad.Load(C1);
      Extrema_ExtPC ext(CP2.Point(),cad,1.e-4);   
      parCP2 = ext.Point(1).Parameter();
    }
  }
  gp_Pnt psp1 = Hpivot->Value(parCP1);
  gp_Pnt psp2 = Hpivot->Value(parCP2);
  Standard_Boolean sameparam = (psp1.Distance(psp2) < 10 * tolesp);
   
  TopoDS_Face FF1 = TopoDS::Face(DStr.Shape(Fd1->Index(IFaArc1)));
  TopoDS_Face FF2 = TopoDS::Face(DStr.Shape(Fd2->Index(IFaArc2)));
  TopTools_ListIteratorOfListOfShape Kt;
  Standard_Boolean ok1 = Standard_False, ok2 = Standard_False;
  for (Kt.Initialize(myEFMap(pivot)); Kt.More(); Kt.Next()){
    TopoDS_Face F = TopoDS::Face(Kt.Value());
    if(!ok1 && FF1.IsSame(F)){
      ok1 = Standard_True;
    }
    if(!ok2 && FF2.IsSame(F)){
      ok2 = Standard_True;
    }
  }
  if(!ok1 || !ok2){
#ifdef OCCT_DEBUG
    cout<<"fail one of surfaces has no common base face with the pivot edge"<<endl;
#endif
    done=Standard_False;
    return done;
  }

  Handle(GeomAdaptor_HSurface) HS1, HS2;
  HS1 = ChFi3d_BoundSurf (DStr,Fd1,IFaCo1,IFaArc1);
  HS2 = ChFi3d_BoundSurf (DStr,Fd2,IFaCo2,IFaArc2);
  
  TColStd_Array1OfReal Pardeb(1,4),Parfin(1,4);
  
  Handle(Geom2d_Curve) PGc1,PGc2;
  Handle(Geom_Curve) Gc;
  
  if(sameparam) {
    // Side common face, calculation of Pardeb.
    ChFi3d_ComputesIntPC (Fd1->Interference(IFaCo1),
			  Fd2->Interference(IFaCo2),
			  HS1,HS2,UIntPC1,UIntPC2);
    gp_Pnt2d UV;
    UV = Fd1->Interference(IFaCo1).PCurveOnSurf()->Value(UIntPC1);
    Pardeb(1)= UV.X(); Pardeb(2)=UV.Y();
    UV = Fd2->Interference(IFaCo2).PCurveOnSurf()->Value(UIntPC2);
    Pardeb(3)= UV.X(); Pardeb(4)=UV.Y();
    gp_Pnt PFaCo = HS1->Surface().Value(Pardeb(1),Pardeb(2));
    
    // Side arc, calculation of Parfin.
    Standard_Real UIntArc1 = Fd1->Interference(IFaArc1).Parameter(isfirst1);
    Standard_Real UIntArc2 = Fd2->Interference(IFaArc2).Parameter(isfirst2);
    
    ChFi3d_ComputesIntPC (Fd1->Interference(IFaArc1),Fd2->Interference(IFaArc2),
			  HS1,HS2,UIntArc1,UIntArc2);
    UV = Fd1->Interference(IFaArc1).PCurveOnSurf()->Value(UIntArc1);
    Parfin(1)= UV.X(); Parfin(2)=UV.Y();
    UV = Fd2->Interference(IFaArc2).PCurveOnSurf()->Value(UIntArc2);
    Parfin(3)= UV.X(); Parfin(4)=UV.Y();
    
    if(Fd1->Surf() == Fd2->Surf()){
      Reduce(UIntPC1,UIntPC2,HS1,HS2);
    }
    
    Standard_Real tolreached = tolesp;
    if (IFaCo1 == 1 && 
	!ChFi3d_ComputeCurves(HS1,HS2,Pardeb,Parfin,Gc,
			      PGc1,PGc2,tolesp,tol2d,tolreached)) {
#ifdef OCCT_DEBUG
      cout<<"failed to calculate bevel error interSS"<<endl;
#endif
      done=Standard_False;
      return done;
    }
    else if (IFaCo1 == 2 && 
	     !ChFi3d_ComputeCurves(HS1,HS2,Parfin,Pardeb,Gc,
				   PGc1,PGc2,tolesp,tol2d,tolreached)) {
#ifdef OCCT_DEBUG
      cout<<"failed to calculate bevel error interSS"<<endl;
#endif
      done=Standard_False;
      return done;	
    }
    // CornerData are updated with results of the intersection. 
    Standard_Real WFirst = Gc->FirstParameter();
    Standard_Real WLast = Gc->LastParameter();
    
    //jgv
    ChFiDS_FaceInterference& InterfArc1 = Fd1->ChangeInterference(IFaArc1);
    Standard_Integer IndEarc1 = Fd1->IndexOfEdge(IFaArc1);
    TopoDS_Edge EdgeArc1 = TopoDS::Edge(myNewEdges(IndEarc1));
    EdgeArc1.Orientation(TopAbs_FORWARD);
    Standard_Real fpar, lpar;
    Handle(Geom_Curve) CurveEdgeArc1 = BRep_Tool::Curve(EdgeArc1, fpar, lpar);
    if (isfirst1)
      fpar = InterfArc1.FirstParameter();
    else
      lpar = InterfArc1.LastParameter();
    BB.Range(EdgeArc1, fpar, lpar);
    Vertices[1] = (isfirst1)? TopExp::FirstVertex(EdgeArc1)
      : TopExp::LastVertex(EdgeArc1);
    gp_Pnt aPnt = CurveEdgeArc1->Value((isfirst1)? fpar : lpar);
    if (Vertices[1].IsNull())
    {
      Vertices[1] = BRepLib_MakeVertex(aPnt);
      TopAbs_Orientation OrOfVer = TopAbs_FORWARD;
      if (!isfirst1)
        OrOfVer = TopAbs_REVERSED;
      BB.Add(EdgeArc1, Vertices[1].Oriented(OrOfVer));
    }
    else
      BB.UpdateVertex(Vertices[1], aPnt, 0.);
    
    ChFiDS_FaceInterference& InterfArc2 = Fd2->ChangeInterference(IFaArc2);
    Standard_Integer IndEarc2 = Fd2->IndexOfEdge(IFaArc2);
    TopoDS_Edge EdgeArc2 = TopoDS::Edge(myNewEdges(IndEarc2));
    EdgeArc2.Orientation(TopAbs_FORWARD);
    Handle(Geom_Curve) CurveEdgeArc2 = BRep_Tool::Curve(EdgeArc2, fpar, lpar);
    if (isfirst2)
      fpar = InterfArc2.FirstParameter();
    else
      lpar = InterfArc2.LastParameter();
    BB.Range(EdgeArc2, fpar, lpar);
    TopoDS_Vertex aVertexArc2 = (isfirst2)? TopExp::FirstVertex(EdgeArc2)
      : TopExp::LastVertex(EdgeArc2);
    if (!aVertexArc2.IsSame(Vertices[1]))
    {
      BB.Remove(EdgeArc2, aVertexArc2);
      TopAbs_Orientation OrOfVer = TopAbs_FORWARD;
      if (!isfirst2)
        OrOfVer = TopAbs_REVERSED;
      BB.Add(EdgeArc2, Vertices[1].Oriented(OrOfVer));
    }
    /////
    Standard_Integer Ipoin1;
    Standard_Integer Ipoin2;
    ChFiDS_CommonPoint& cpco1 = Fd1->ChangeVertex(isfirst1,IFaCo1);
    ChFiDS_CommonPoint& cpco2 = Fd2->ChangeVertex(isfirst2,IFaCo2);
    Standard_Real tolpco = Max(cpco1.Tolerance(),cpco2.Tolerance());
    ChFiDS_CommonPoint& cparc1 = Fd1->ChangeVertex(isfirst1,IFaArc1);
    ChFiDS_CommonPoint& cparc2 = Fd2->ChangeVertex(isfirst2,IFaArc2);
    Standard_Real tolparc = Max(cparc1.Tolerance(),cparc2.Tolerance());
    Standard_Integer ICurv = DStr.AddCurve(TopOpeBRepDS_Curve(Gc,tolreached));
    //Corner1
    Corner1->SetParameters(isfirst1,WFirst,WLast);
    Corner1->SetCurve(ICurv,isfirst1);
    Corner1->ChangePCurve(isfirst1) = PGc1;
    cpco1.Reset();
    cpco1.SetPoint(PFaCo);
    cpco1.SetTolerance(Max(tolreached,tolpco));
    Fd1->ChangeInterference(IFaCo1).SetParameter(UIntPC1,isfirst1);
    //jgv
    ChFiDS_FaceInterference& Interf1 = Fd1->ChangeInterference(IFaCo1);
    Standard_Integer IndEsurf1 = Fd1->IndexOfEdge(IFaCo1);
    TopoDS_Edge EdgeSurf1 = TopoDS::Edge(myNewEdges(IndEsurf1));
    EdgeSurf1.Orientation(TopAbs_FORWARD);
    Handle(Geom_Curve) CurveEdgeSurf1 = BRep_Tool::Curve(EdgeSurf1, fpar, lpar);
    //BRep_Tool::Range(EdgeSurf, fpar, lpar);
    if (isfirst1)
      fpar = Interf1.FirstParameter();
    else
      lpar = Interf1.LastParameter();
    BB.Range(EdgeSurf1, fpar, lpar);
    Vertices[2] = (isfirst1)? TopExp::FirstVertex(EdgeSurf1)
      : TopExp::LastVertex(EdgeSurf1);
    aPnt = CurveEdgeSurf1->Value((isfirst1)? fpar : lpar);
    if (Vertices[2].IsNull())
    {
      Vertices[2] = BRepLib_MakeVertex(aPnt);
      TopAbs_Orientation OrOfVer = TopAbs_FORWARD;
      if (!isfirst1)
        OrOfVer = TopAbs_REVERSED;
      BB.Add(EdgeSurf1, Vertices[2].Oriented(OrOfVer));
    }
    else
      BB.UpdateVertex(Vertices[2], aPnt, 0.);
    /////
    tolparc = Max(tolparc,tolreached);
    cparc1.SetTolerance(Max(tolparc,tolreached));
    Ipoin1 = ChFi3d_IndexPointInDS(Fd1->Vertex(isfirst1,1),DStr);
    Corner1->SetIndexPoint(Ipoin1,isfirst1,1);
    Ipoin2 = ChFi3d_IndexPointInDS(Fd1->Vertex(isfirst1,2),DStr);
    Corner1->SetIndexPoint(Ipoin2,isfirst1,2);
    //Corner2
    Corner2->SetParameters(isfirst2,WFirst,WLast);
    Corner2->SetCurve(ICurv,isfirst2);
    Corner2->ChangePCurve(isfirst2) = PGc2;
    Fd2->ChangeInterference(IFaCo2).SetParameter(UIntPC2,isfirst2);
    //jgv
    ChFiDS_FaceInterference& Interf2 = Fd2->ChangeInterference(IFaCo2);
    Standard_Integer IndEsurf2 = Fd2->IndexOfEdge(IFaCo2);
    TopoDS_Edge EdgeSurf2 = TopoDS::Edge(myNewEdges(IndEsurf2));
    EdgeSurf2.Orientation(TopAbs_FORWARD);
    Handle(Geom_Curve) CurveEdgeSurf2 = BRep_Tool::Curve(EdgeSurf2, fpar, lpar);
    //BRep_Tool::Range(EdgeSurf, fpar, lpar);
    if (isfirst2)
      fpar = Interf2.FirstParameter();
    else
      lpar = Interf2.LastParameter();
    BB.Range(EdgeSurf2, fpar, lpar);
    TopoDS_Vertex aVertex2 = (isfirst2)? TopExp::FirstVertex(EdgeSurf2)
      : TopExp::LastVertex(EdgeSurf2);
    if (!aVertex2.IsSame(Vertices[2]))
    {
      BB.Remove(EdgeSurf2, aVertex2);
      TopAbs_Orientation OrOfVer = TopAbs_FORWARD;
      if (!isfirst2)
        OrOfVer = TopAbs_REVERSED;
      BB.Add(EdgeSurf2, Vertices[2].Oriented(OrOfVer));
    }

    //Create new edge
    gp_Pnt aPntVer = BRep_Tool::Pnt(Vertices[IFaArc1]);
    gp_Pnt aPntFromCurve = Gc->Value(WFirst);
    Standard_Real aDist = aPntVer.Distance(aPntFromCurve);
    BB.UpdateVertex(Vertices[IFaArc1], 1.01*aDist);
    TopoDS_Edge aNewEdge = BRepLib_MakeEdge(Gc,
                                            Vertices[IFaArc1], Vertices[IFaCo1],
                                            WFirst, WLast);
    BB.UpdateEdge(aNewEdge, tolreached);
    TopLoc_Location aLoc;
    BB.UpdateEdge(aNewEdge, PGc1, DStr.Surface(Fd1->Surf()).Surface(), aLoc, 0.);
    BB.UpdateEdge(aNewEdge, PGc2, DStr.Surface(Fd2->Surf()).Surface(), aLoc, 0.);
    myNewEdges.Add(aNewEdge);
    //Temporary
    //TopAbs_Orientation Or1 = Fd1->Orientation();
    //TopAbs_Orientation Or2 = Fd2->Orientation();
    ///////////
    Standard_Integer IndE = myNewEdges.FindIndex(aNewEdge);
    Standard_Boolean IsFirstArcForward = Standard_True;
    const TColStd_ListOfInteger& Elist = myFaceNewEdges.FindFromKey(IndexOfNewFace1);
    TColStd_ListIteratorOfListOfInteger itl(Elist);
    for (; itl.More(); itl.Next())
    {
      Standard_Integer anIndexOfEdge = itl.Value();
      if (Abs(anIndexOfEdge) == IndEarc1)
      {
        IsFirstArcForward = (anIndexOfEdge > 0);
        break;
      }
    }
    if ((isfirst1  && IFaArc1 == 1 && IsFirstArcForward) ||
        (!isfirst1 && IFaArc1 == 2 && IsFirstArcForward))
      IndE *= -1;
    //QualifiedEdge aQE(IndE, Et);
    myFaceNewEdges.ChangeFromKey(IndexOfNewFace1).Append(IndE);
    Standard_Integer IndE_forNewFace2 = IndE;
    if (Fd1->Orientation() == Fd2->Orientation())
      IndE_forNewFace2 *= -1;
    myFaceNewEdges.ChangeFromKey(IndexOfNewFace2).Append(IndE_forNewFace2);
    /////
    Fd2->ChangeVertex(isfirst2,IFaCo2) = Fd1->Vertex(isfirst1,IFaCo1);
    Fd2->ChangeVertex(isfirst2,IFaArc2) = Fd1->Vertex(isfirst1,IFaArc1);
    if (IFaCo1!=IFaCo2) Corner2->SetOrientation(TopAbs_REVERSED,isfirst2);
    Corner2->SetIndexPoint(Corner1->IndexPoint(isfirst1,IFaCo1),
			   isfirst2,IFaCo2);
    Corner2->SetIndexPoint(Corner1->IndexPoint(isfirst1,IFaArc1),
			   isfirst2,IFaArc2);
    //The tolerances of points are updated.
    Bnd_Box bco,barc;
    if(IFaCo1 == 1) ChFi3d_EnlargeBox(DStr,Corner1,Fd1,bco,barc,isfirst1);
    else ChFi3d_EnlargeBox(DStr,Corner1,Fd1,barc,bco,isfirst1);
    if(IFaCo2 == 1) ChFi3d_EnlargeBox(DStr,Corner2,Fd2,bco,barc,isfirst2);
    else ChFi3d_EnlargeBox(DStr,Corner2,Fd2,barc,bco,isfirst2);
    const ChFiDS_CommonPoint& cparc = Fd1->Vertex(isfirst1,IFaArc1);
    ChFi3d_EnlargeBox(cparc.Arc(),myEFMap(cparc.Arc()),cparc.ParameterOnArc(),barc);
    ChFi3d_SetPointTolerance(DStr,barc,Corner1->IndexPoint(isfirst1,IFaArc1));
    ChFi3d_SetPointTolerance(DStr,bco,Corner1->IndexPoint(isfirst1,IFaCo1));
  }
  else {
    // It is necessary to identify the border surface,
    // find the end point of the intersection Surf/Surf 
    // by the intersection of the tangency line of the small
    // on the opposing face with the surface of the big,
    // and finally intersect the big with the face at end 
    // between this point and the point on arc.
    Standard_Boolean parcrois = Standard_False ;
    TopExp_Explorer Expl;
    for(Expl.Init(pivot.Oriented(TopAbs_FORWARD),TopAbs_VERTEX); 
	Expl.More(); Expl.Next()){
      if(Expl.Current().IsSame(Vtx)){
	parcrois = (Expl.Current().Orientation() == TopAbs_FORWARD);
	break;
      }
    }
    Handle(ChFiDS_Stripe) BigCD, SmaCD;
    Handle(ChFiDS_SurfData) BigFD, SmaFD;
    Handle(GeomAdaptor_HSurface) BigHS, SmaHS;
    Standard_Integer IFaCoBig, IFaCoSma, IFaArcBig, IFaArcSma;
    Standard_Boolean isfirstBig, isfirstSma;
    Standard_Real UIntPCBig, UIntPCSma;
    Standard_Integer IndexOfSmaNewFace, IndexOfBigNewFace;
    
    if((parcrois && parCP2 > parCP1) || (!parcrois && parCP2 < parCP1)){
      UIntPCBig = UIntPC2; UIntPCSma = UIntPC1; 
      BigHS = HS2; SmaHS = HS1;
      BigCD = Corner2; SmaCD = Corner1; 
      BigFD = Fd2; SmaFD = Fd1;
      IFaCoBig = IFaCo2; IFaCoSma = IFaCo1;
      IFaArcBig = IFaArc2; IFaArcSma = IFaArc1;
      isfirstBig = isfirst2; isfirstSma = isfirst1;
      IndexOfSmaNewFace = IndexOfNewFace1; IndexOfBigNewFace = IndexOfNewFace2;
    }
    else{
      UIntPCBig = UIntPC1, UIntPCSma = UIntPC2; 
      BigHS = HS1; SmaHS = HS2;
      BigCD = Corner1; SmaCD = Corner2; 
      BigFD = Fd1; SmaFD = Fd2;
      IFaCoBig = IFaCo1; IFaCoSma = IFaCo2;
      IFaArcBig = IFaArc1; IFaArcSma = IFaArc2;
      isfirstBig = isfirst1; isfirstSma = isfirst2;
      IndexOfSmaNewFace = IndexOfNewFace2; IndexOfBigNewFace = IndexOfNewFace1;
    }
    
    //Intersection of the big with the small :
    //------------------------------------

    // Pardeb (parameters of point PFaCo)
    // the intersection is checked
    ChFi3d_ComputesIntPC (SmaFD->Interference(IFaCoSma),
			  BigFD->Interference(IFaCoBig),
			  SmaHS,BigHS,UIntPCSma,UIntPCBig);
    gp_Pnt2d UVi;
    UVi = BigFD->Interference(IFaCoBig).PCurveOnSurf()->Value(UIntPCBig);
    Pardeb(3)= UVi.X(); Pardeb(4)=UVi.Y();
    UVi = SmaFD->Interference(IFaCoSma).PCurveOnSurf()->Value(UIntPCSma);
    Pardeb(1)= UVi.X(); Pardeb(2)=UVi.Y();
    gp_Pnt PFaCo = SmaHS->Value(UVi.X(),UVi.Y());

      // Parfin (parameters of point PMil)
    const ChFiDS_FaceInterference& FiArcSma = SmaFD->Interference(IFaArcSma);
    Handle(Geom_Curve) ctg = DStr.Curve(FiArcSma.LineIndex()).Curve();
    Handle(GeomAdaptor_HCurve) Hctg = new GeomAdaptor_HCurve();
    GeomAdaptor_Curve& bid = Hctg->ChangeCurve();
    Standard_Real temp,wi;

    if (isfirstSma) {
      wi = temp =  FiArcSma.FirstParameter();
      if (UIntPCSma < temp)
	temp = UIntPCSma;
      bid.Load(ctg,temp,FiArcSma.LastParameter());
    }
    else {
      wi = temp =  FiArcSma.LastParameter();
      if (UIntPCSma > temp)
	temp = UIntPCSma;
      bid.Load(ctg,FiArcSma.FirstParameter(),temp);
    }
    if(SmaFD->Surf() == BigFD->Surf()){
      Reduce(UIntPCSma,UIntPCBig,SmaHS,BigHS);
      Reduce(UIntPCSma,UIntPCBig,Hctg);
    }
    if(!ChFi3d_IntCS(BigHS,Hctg,UVi,wi)){
#ifdef OCCT_DEBUG
      cout<<"bevel : failed inter C S"<<endl;
#endif
      done=Standard_False;
      return done;
    }
    Parfin(3) = UVi.X(); Parfin(4) = UVi.Y();
    UVi = FiArcSma.PCurveOnSurf()->Value(wi);
    Parfin(1) = UVi.X(); Parfin(2) = UVi.Y();
    gp_Pnt PMil = SmaHS->Value(Parfin(1),Parfin(2));
    
    Standard_Real tolreached;
    if (!ChFi3d_ComputeCurves(SmaHS,BigHS,Pardeb,Parfin,Gc,
			      PGc1,PGc2,tolesp,tol2d,tolreached)) {
#ifdef OCCT_DEBUG
      cout<<"failed to calculate bevel failed interSS"<<endl;
#endif
      done=Standard_False;
      return done;
    }
      // SmaCD is updated, for it this is all. 
    Standard_Real WFirst = Gc->FirstParameter();
    Standard_Real WLast = Gc->LastParameter();
    Standard_Integer IpointCo, IpointMil, IpointArc;
    ChFiDS_CommonPoint& psmaco = SmaFD->ChangeVertex(isfirstSma,IFaCoSma);
    ChFiDS_CommonPoint& pbigco = BigFD->ChangeVertex(isfirstBig,IFaCoBig);
    Standard_Real tolpco = Max(psmaco.Tolerance(),pbigco.Tolerance());
    ChFiDS_CommonPoint& psmamil = SmaFD->ChangeVertex(isfirstSma,IFaArcSma);
    Standard_Real tolpmil = psmamil.Tolerance();
    Standard_Integer ICurv = DStr.AddCurve(TopOpeBRepDS_Curve(Gc,tolreached));
    
    SmaCD->SetParameters(isfirstSma,WFirst,WLast);
    SmaCD->SetCurve(ICurv,isfirstSma);
    SmaCD->ChangePCurve(isfirstSma) = PGc1;
    psmaco.Reset();
    psmaco.SetPoint(PFaCo);
    psmaco.SetTolerance(Max(tolpco,tolreached));
    SmaFD->ChangeInterference(IFaCoSma).SetParameter(UIntPCSma,isfirstSma);
    //jgv
    ChFiDS_FaceInterference& SmaInterf = SmaFD->ChangeInterference(IFaCoSma);
    Standard_Integer IndEsmall = SmaFD->IndexOfEdge(IFaCoSma);
    TopoDS_Edge EdgeSmall = TopoDS::Edge(myNewEdges(IndEsmall));
    EdgeSmall.Orientation(TopAbs_FORWARD);
    Standard_Real fpar, lpar;
    Handle(Geom_Curve) CurveEdgeSma = BRep_Tool::Curve(EdgeSmall, fpar, lpar);
    //BRep_Tool::Range(EdgeSmall, fpar, lpar);
    if (isfirstSma)
      fpar = SmaInterf.FirstParameter();
    else
      lpar = SmaInterf.LastParameter();
    BB.Range(EdgeSmall, fpar, lpar);
    Vertices[2] = (isfirstSma)? TopExp::FirstVertex(EdgeSmall)
      : TopExp::LastVertex(EdgeSmall);
    gp_Pnt aPnt = CurveEdgeSma->Value((isfirstSma)? fpar : lpar);
    if (Vertices[2].IsNull())
    {
      Vertices[2] = BRepLib_MakeVertex(aPnt);
      TopAbs_Orientation OrOfVer = TopAbs_FORWARD;
      if (!isfirstSma)
        OrOfVer = TopAbs_REVERSED;
      BB.Add(EdgeSmall, Vertices[2].Oriented(OrOfVer));
    }
    else
      BB.UpdateVertex(Vertices[2], aPnt, 0.);
    /////
    psmamil.Reset();
    psmamil.SetPoint(PMil);
    psmamil.SetTolerance(Max(tolpmil,tolreached));
    SmaFD->ChangeInterference(IFaArcSma).SetParameter(wi,isfirstSma);
    //jgv
    ChFiDS_FaceInterference& SmaArcInterf = SmaFD->ChangeInterference(IFaArcSma);
    Standard_Integer IndEarcsmall = SmaFD->IndexOfEdge(IFaArcSma);
    TopoDS_Edge EdgeArcSmall = TopoDS::Edge(myNewEdges(IndEarcsmall));
    EdgeArcSmall.Orientation(TopAbs_FORWARD);
    Handle(Geom_Curve) CurveEdgeArcSma = BRep_Tool::Curve(EdgeArcSmall, fpar, lpar);
    if (isfirstSma)
      fpar = SmaArcInterf.FirstParameter();
    else
      lpar = SmaArcInterf.LastParameter();
    BB.Range(EdgeArcSmall, fpar, lpar);
    Vertices[1] = (isfirstSma)? TopExp::FirstVertex(EdgeArcSmall)
      : TopExp::LastVertex(EdgeArcSmall);
    aPnt = CurveEdgeArcSma->Value((isfirstSma)? fpar : lpar);
    if (Vertices[1].IsNull())
    {
      Vertices[1] = BRepLib_MakeVertex(aPnt);
      TopAbs_Orientation OrOfVer = TopAbs_FORWARD;
      if (!isfirstSma)
        OrOfVer = TopAbs_REVERSED;
      BB.Add(EdgeArcSmall, Vertices[1].Oriented(OrOfVer));
    }
    else
      BB.UpdateVertex(Vertices[1], aPnt, 0.);
    /////
    IpointCo = ChFi3d_IndexPointInDS(psmaco,DStr);
    SmaCD->SetIndexPoint(IpointCo,isfirstSma,IFaCoSma);
    IpointMil = ChFi3d_IndexPointInDS(psmamil,DStr);
    SmaCD->SetIndexPoint(IpointMil,isfirstSma,IFaArcSma);
    if (IFaCoSma == 2) SmaCD->SetOrientation(TopAbs_REVERSED,isfirstSma);

    // For BigCD the first results are met in the DS.
    BigCD->SetIndexPoint(IpointCo,isfirstBig,IFaCoBig);
    BigFD->ChangeVertex(isfirstBig,IFaCoBig) = psmaco;
    BigFD->ChangeInterference(IFaCoBig).SetParameter(UIntPCBig,isfirstBig);
    //jgv
    ChFiDS_FaceInterference& BigInterf = BigFD->ChangeInterference(IFaCoBig);
    Standard_Integer IndEbig = BigFD->IndexOfEdge(IFaCoBig);
    TopoDS_Edge EdgeBig = TopoDS::Edge(myNewEdges(IndEbig));
    EdgeBig.Orientation(TopAbs_FORWARD);

    //???
    TopoDS_Vertex BigVer [3];
    Handle(Geom_Curve) CurveEdgeBig = BRep_Tool::Curve(EdgeBig, fpar, lpar);
    //BRep_Tool::Range(EdgeBig, fpar, lpar);
    if (isfirstBig)
      fpar = BigInterf.FirstParameter();
    else
      lpar = BigInterf.LastParameter();
    BB.Range(EdgeBig, fpar, lpar);
    BigVer[2] = (isfirstBig)? TopExp::FirstVertex(EdgeBig)
      : TopExp::LastVertex(EdgeBig);
    aPnt = CurveEdgeBig->Value((isfirstBig)? fpar : lpar);
    //if (BigVer[2].IsNull())
    if (!BigVer[2].IsSame(Vertices[2]))
    {
      //BigVer[2] = BRepLib_MakeVertex(aPnt);
      BB.Remove(EdgeBig, BigVer[2]);
      TopAbs_Orientation OrOfVer = TopAbs_FORWARD;
      if (!isfirstBig)
        OrOfVer = TopAbs_REVERSED;
      BB.Add(EdgeBig, Vertices[2].Oriented(OrOfVer));
    }
    /////
    
    ChFiDS_FaceInterference& BigArcInterf = BigFD->ChangeInterference(IFaArcBig);
    Standard_Integer IndEarcbig = BigFD->IndexOfEdge(IFaArcBig);
    TopoDS_Edge EdgeArcBig = TopoDS::Edge(myNewEdges(IndEarcbig));
    EdgeArcBig.Orientation(TopAbs_FORWARD);

    //???
    Handle(Geom_Curve) CurveEdgeArcBig = BRep_Tool::Curve(EdgeArcBig, fpar, lpar);
    if (isfirstBig)
      fpar = BigArcInterf.FirstParameter();
    else
      lpar = BigArcInterf.LastParameter();
    BB.Range(EdgeArcBig, fpar, lpar);
    BigVer[1] = (isfirstBig)? TopExp::FirstVertex(EdgeArcBig)
      : TopExp::LastVertex(EdgeArcBig);
    aPnt = CurveEdgeArcBig->Value((isfirstBig)? fpar : lpar);
    if (BigVer[1].IsNull())
    {
      BigVer[1] = BRepLib_MakeVertex(aPnt);
      TopAbs_Orientation OrOfVer = TopAbs_FORWARD;
      if (!isfirstBig)
        OrOfVer = TopAbs_REVERSED;
      BB.Add(EdgeArcBig, BigVer[1].Oriented(OrOfVer));
    }
    else
      BB.UpdateVertex(BigVer[1], aPnt, 0.);
    /////
    
    //Create new edge
    gp_Pnt aPntVer = BRep_Tool::Pnt(Vertices[2]);
    gp_Pnt aPntFromCurve = Gc->Value(WFirst);
    Standard_Real aDist = aPntVer.Distance(aPntFromCurve);
    BB.UpdateVertex(Vertices[2], 1.01*aDist);
    TopoDS_Edge aNewEdge = BRepLib_MakeEdge(Gc,
                                            Vertices[2], Vertices[1],
                                            WFirst, WLast);
    BB.UpdateEdge(aNewEdge, tolreached);
    TopLoc_Location aLoc;
    BB.UpdateEdge(aNewEdge, PGc1, DStr.Surface(SmaFD->Surf()).Surface(), aLoc, 0.);
    BB.UpdateEdge(aNewEdge, PGc2, DStr.Surface(BigFD->Surf()).Surface(), aLoc, 0.);
    myNewEdges.Add(aNewEdge);
    Standard_Integer IndE = myNewEdges.FindIndex(aNewEdge);
    if (isfirstSma  && IFaArcSma == 1 ||
        !isfirstSma && IFaArcSma == 2)
      IndE *= -1;
    //QualifiedEdge aQE(IndE, Et);
    myFaceNewEdges.ChangeFromKey(IndexOfSmaNewFace).Append(IndE);
    Standard_Integer IndE_forBigNewFace = IndE;
    if (SmaFD->Orientation() == BigFD->Orientation())
      IndE_forBigNewFace *= -1;
    myFaceNewEdges.ChangeFromKey(IndexOfBigNewFace).Append(IndE_forBigNewFace);
    /////
    
    TopOpeBRepDS_ListOfInterference& Li = DStr.ChangeCurveInterferences(ICurv);
    Handle(TopOpeBRepDS_CurvePointInterference) Interfp;
    Interfp = ChFi3d_FilPointInDS(TopAbs_FORWARD,ICurv,IpointCo,WFirst);
    Li.Append(Interfp);
    Interfp = ChFi3d_FilPointInDS(TopAbs_REVERSED,ICurv,IpointMil,WLast);
    Li.Append(Interfp);
    
    // the transition of curves of intersection on the Big
    TopAbs_Orientation tra = BigFD->InterferenceOnS1().Transition();
    TopAbs_Orientation ofac = DStr.Shape(BigFD->IndexOfS1()).Orientation();
    TopAbs_Orientation ofil = BigFD->Orientation();
    TopAbs_Orientation tracurv = TopAbs::Compose(ofac,ofil);
    tracurv = TopAbs::Compose(tracurv,tra);
    if(!isfirstBig) tracurv = TopAbs::Reverse(tracurv);
    if(IFaCoBig != 1) tracurv = TopAbs::Reverse(tracurv);
    
    Handle(TopOpeBRepDS_SurfaceCurveInterference) Interfc;
    Standard_Integer ISurf = BigFD->Surf();
    Interfc = ChFi3d_FilCurveInDS (ICurv,ISurf,PGc2,tracurv);
    DStr.ChangeSurfaceInterferences(ISurf).Append(Interfc);
    
    //The tolerances of points are updated (beginning).
    Bnd_Box bco,bmil,barc;
    if(IFaCoSma == 1) ChFi3d_EnlargeBox(DStr,SmaCD,SmaFD,bco,bmil,isfirstSma);
    else ChFi3d_EnlargeBox(DStr,SmaCD,SmaFD,bmil,bco,isfirstSma);
    ChFi3d_EnlargeBox(BigHS,PGc2,WFirst,WLast,bco,bmil);
    
    // Intersection of the big with the face at end :
    // -------------------------------------------

    // Pardeb (parameters of PMil)
    // The intersection curve surface is tried again, now with representation
    // pcurve on face of the curve to be sure.
    TopoDS_Face F = TopoDS::Face(DStr.Shape(SmaFD->Index(IFaArcSma)));
    Handle(BRepAdaptor_HSurface) HF = new BRepAdaptor_HSurface(F);
    Standard_Real fsma = FiArcSma.FirstParameter();
    Standard_Real lsma = FiArcSma.LastParameter();
    Standard_Real deltSma = 0.05 * (lsma - fsma);
    Handle(Geom2d_Curve) pcpc = SmaFD->Interference(IFaArcSma).PCurveOnFace();
    fsma = Max(pcpc->FirstParameter(),wi-deltSma);
    lsma = Min(pcpc->LastParameter(),wi+deltSma);
    if ( lsma<fsma ) {
       done=Standard_False;
       return done;
    }
    Handle(Geom2dAdaptor_HCurve) c2df = 
      new Geom2dAdaptor_HCurve(SmaFD->Interference(IFaArcSma).PCurveOnFace(),fsma,lsma);
    Adaptor3d_CurveOnSurface consf(c2df,HF);
    Handle(Adaptor3d_HCurveOnSurface) Hconsf = new Adaptor3d_HCurveOnSurface(consf);
    if(!ChFi3d_IntCS(BigHS,Hconsf,UVi,wi)) {
#ifdef OCCT_DEBUG
      cout<<"bevel : failed inter C S"<<endl;
#endif
      done=Standard_False;
      return done;
    }
    Pardeb(3) = UVi.X(); Pardeb(4) = UVi.Y();
    UVi = SmaFD->Interference(IFaArcSma).PCurveOnFace()->Value(wi);
    Pardeb(1) = UVi.X(); Pardeb(2) = UVi.Y(); 
    gp_Pnt2d ppff1 = UVi;

      // Parfin (parameters of the point cpend)
    Standard_Real ptg = BigFD->Interference(IFaArcBig).Parameter(isfirstBig);
    UVi = BigFD->Interference(IFaArcBig).PCurveOnSurf()->Value(ptg);
    Parfin(3) = UVi.X(); Parfin(4) = UVi.Y();
    ChFiDS_CommonPoint& cpend = BigFD->ChangeVertex(isfirstBig,IFaArcBig);
    TopoDS_Edge etest = cpend.Arc();
    if(BRep_Tool::IsClosed(etest,F)) etest.Reverse();
    BRepAdaptor_Curve2d arc(etest,F);
    UVi = arc.Value(cpend.ParameterOnArc());
    Parfin(1) = UVi.X(); Parfin(2) = UVi.Y();
    gp_Pnt2d ppff2 = UVi;
    
      // Intersection. 
    Standard_Real uu1,uu2,vv1,vv2;
    ChFi3d_Boite(ppff1,ppff2,uu1,uu2,vv1,vv2);
    // for the case when two chamfers are on two edges OnSame,
    // it is necessary to extend the surface carrying F, or at least
    // not to limit it.
    ChFi3d_BoundFac(HF->ChangeSurface(),uu1,uu2,vv1,vv2,Standard_True);

    if (!ChFi3d_ComputeCurves(HF,BigHS,Pardeb,Parfin,Gc,
			      PGc1,PGc2,tolesp,tol2d,tolreached)) {
#ifdef OCCT_DEBUG
      cout<<"fail calculation bevel fail interSS"<<endl;
#endif
      done=Standard_False;
      return done;
    }
    
      // End of update of the BigCD and the DS.
    WFirst = Gc->FirstParameter();
    WLast = Gc->LastParameter();

    //Create additional new edge
    TopoDS_Edge anAdditionalNewEdge = BRepLib_MakeEdge(Gc,
                                                       Vertices[1], BigVer[1],
                                                       WFirst, WLast);
    BB.UpdateEdge(anAdditionalNewEdge, tolreached);
    BB.UpdateEdge(anAdditionalNewEdge, PGc1, F, 0.);
    BB.UpdateEdge(anAdditionalNewEdge, PGc2, DStr.Surface(BigFD->Surf()).Surface(), aLoc, 0.);
    myNewEdges.Add(anAdditionalNewEdge);
    IndE = myNewEdges.FindIndex(anAdditionalNewEdge);
    if (IndE_forBigNewFace < 0)
      IndE *= -1;
    //QualifiedEdge aQE(IndE, Et);
    myFaceNewEdges.ChangeFromKey(IndexOfBigNewFace).Append(IndE);
    Standard_Integer IndE_forOldFace = IndE;
    if (BigFD->Orientation() == F.Orientation())
      IndE_forOldFace *= -1;
    
    Standard_Integer IndF;
    if (!myNewFaces.Contains(F))
      myNewFaces.Add(F);
    IndF = myNewFaces.FindIndex(F);
    if (!myFaceNewEdges.Contains(IndF))
    {
      //ChFi3d_ListOfQualifiedEdge aList;
      TColStd_ListOfInteger aList;
      myFaceNewEdges.Add(IndF, aList);
    }
    myFaceNewEdges.ChangeFromKey(IndF).Append(IndE_forOldFace);
    /////
    
    ICurv = DStr.AddCurve(TopOpeBRepDS_Curve(Gc,tolreached));
    cpend.SetTolerance(Max(cpend.Tolerance(),tolreached));
    IpointArc = ChFi3d_IndexPointInDS(cpend,DStr);
    BigCD->SetIndexPoint(IpointArc,isfirstBig,IFaArcBig);
    
    TopOpeBRepDS_ListOfInterference& Li7 = DStr.ChangeCurveInterferences(ICurv);
    Interfp = ChFi3d_FilPointInDS(TopAbs_FORWARD,ICurv,IpointMil,WFirst);
    Li7.Append(Interfp);
    Interfp = ChFi3d_FilPointInDS(TopAbs_REVERSED,ICurv,IpointArc,WLast);
    Li7.Append(Interfp);
    Interfc = ChFi3d_FilCurveInDS (ICurv,ISurf,PGc2,tracurv);
    DStr.ChangeSurfaceInterferences(ISurf).Append(Interfc);
    BigCD->InDS(isfirstBig);
    
    // Finally the information on faces is placed in the DS.
    Standard_Integer IShape = DStr.AddShape(F);
    if(SmaFD->Surf() == BigFD->Surf()){
      tracurv = TopAbs::Compose(etest.Orientation(),
				cpend.TransitionOnArc());
    }
    else {
      TopExp_Explorer Exp;
      for (Exp.Init(F.Oriented(TopAbs_FORWARD),
		    TopAbs_EDGE);Exp.More();Exp.Next()) {
	if (Exp.Current().IsSame(etest)) {
	  tracurv = TopAbs::Compose(Exp.Current().Orientation(),
				    cpend.TransitionOnArc());
	  break;
	}
      }
    }
    Interfc = ChFi3d_FilCurveInDS(ICurv,IShape,PGc1,tracurv);
    DStr.ChangeShapeInterferences(IShape).Append(Interfc);

    //The tolerances of points are updated (end).
    Handle(ChFiDS_Stripe) bidst;
    if(IFaCoBig == 1) ChFi3d_EnlargeBox(DStr,bidst,BigFD,bco,barc,isfirstBig);
    else ChFi3d_EnlargeBox(DStr,bidst,BigFD,barc,bco,isfirstBig);
    ChFi3d_EnlargeBox(BigHS,PGc2,WFirst,WLast,bmil,barc);
    ChFi3d_EnlargeBox(HF,PGc1,WFirst,WLast,bmil,barc);
    ChFi3d_EnlargeBox(Gc,WFirst,WLast,bmil,barc);
    const ChFiDS_CommonPoint& cparc = BigFD->Vertex(isfirstBig,IFaArcBig);
    ChFi3d_EnlargeBox(cparc.Arc(),myEFMap(cparc.Arc()),cparc.ParameterOnArc(),barc);

    ChFi3d_SetPointTolerance(DStr,bco,SmaCD->IndexPoint(isfirstSma,IFaCoSma));
    ChFi3d_SetPointTolerance(DStr,bmil,SmaCD->IndexPoint(isfirstSma,IFaArcSma));
    ChFi3d_SetPointTolerance(DStr,barc,BigCD->IndexPoint(isfirstBig,IFaArcBig));
  }
  done = 1;
  
  return done;
}







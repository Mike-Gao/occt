// Created on: 1991-06-24
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _DrawTrSurf_HeaderFile
#define _DrawTrSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Draw_SaveAndRestore.hxx>

class gp_Pnt;
class gp_Pnt2d;
class Poly_Triangulation;
class Poly_Polygon3D;
class Poly_Polygon2D;
class Geom_Curve;
class Geom_BezierCurve;
class Geom_BSplineCurve;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;
class Geom_BezierSurface;
class Geom_BSplineSurface;
class DrawTrSurf_Drawable;
class DrawTrSurf_Point;
class DrawTrSurf_Curve;
class DrawTrSurf_BSplineCurve;
class DrawTrSurf_BezierCurve;
class DrawTrSurf_Curve2d;
class DrawTrSurf_BSplineCurve2d;
class DrawTrSurf_BezierCurve2d;
class DrawTrSurf_Triangulation2D;
class DrawTrSurf_Surface;
class DrawTrSurf_BSplineSurface;
class DrawTrSurf_BezierSurface;
class DrawTrSurf_Triangulation;
class DrawTrSurf_Polygon3D;
class DrawTrSurf_Polygon2D;


//! This  package supports the   display of parametric
//! curves and surfaces.
//!
//! The  Drawable deferred  classes is  inherited from
//! the Drawable3D  class  from  the package Draw,  it
//! adds methods to  draw 3D Curves  and  Curves on 3D
//! Surfaces.
//!
//! The classes Curve Curve2d and Surface are drawable
//! and  can be  used  to  draw a   single  curve from
//! packages Geom or Geom2d or a surface from Geom.
//!
//! The  Triangulation  and Polygon  from the  package
//! Poly are also supported.
class DrawTrSurf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Sets <G> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const gp_Pnt& G);
  
  //! Sets <G> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const gp_Pnt2d& G);
  
  //! Sets <G> in the variable <Name>.  Overwrite the
  //! variable if already set.
  //! isSenseMarker indicates whether to render the
  //! sense glyph (arrow) for curves or not
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Geom_Geometry)& G, const Standard_Boolean isSenseMarker = Standard_True);
template <class T> static void Set (const Standard_CString Name, const Handle(T)& Arg, typename opencascade::std::enable_if<opencascade::std::is_base_of<Geom_Geometry, T>::value>::type * = 0) { Set (Name, (const Handle(Geom_Geometry)&)Arg); }
  
  //! Sets <C> in the variable <Name>.  Overwrite the
  //! variable if already set.
  //! isSenseMarker indicates whether to render the
  //! sense glyph (arrow) for curves or not
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Geom2d_Curve)& C, const Standard_Boolean isSenseMarker = Standard_True);
template <class T> static void Set (const Standard_CString Name, const Handle(T)& Arg, typename opencascade::std::enable_if<opencascade::std::is_base_of<Geom2d_Curve, T>::value>::type * = 0) { Set (Name, (const Handle(Geom2d_Curve)&)Arg); }
  
  //! Sets <T> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Poly_Triangulation)& T);
  
  //! Sets <P> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Poly_Polygon3D)& P);
  
  //! Sets <P> in the variable <Name>.  Overwrite the
  //! variable if already set.
  Standard_EXPORT static void Set (const Standard_CString Name, const Handle(Poly_Polygon2D)& P);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_Geometry) Get (Standard_CString& Name);
  
  //! Gets the variable. Returns False if none and print
  //! a warning message.
  Standard_EXPORT static Standard_Boolean GetPoint (Standard_CString& Name, gp_Pnt& P);
  
  //! Gets the variable. Returns False if none and print
  //! a warning message.
  Standard_EXPORT static Standard_Boolean GetPoint2d (Standard_CString& Name, gp_Pnt2d& P);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_Curve) GetCurve (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_BezierCurve) GetBezierCurve (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_BSplineCurve) GetBSplineCurve (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom2d_Curve) GetCurve2d (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom2d_BezierCurve) GetBezierCurve2d (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) GetBSplineCurve2d (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_Surface) GetSurface (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_BezierSurface) GetBezierSurface (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Geom_BSplineSurface) GetBSplineSurface (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Poly_Triangulation) GetTriangulation (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Poly_Polygon3D) GetPolygon3D (Standard_CString& Name);
  
  //! Get  the variable <S>.  Returns a  null  handle if
  //! none, and print a warning message.
  Standard_EXPORT static Handle(Poly_Polygon2D) GetPolygon2D (Standard_CString& Name);
  
  //! defines display commands.
  Standard_EXPORT static void BasicCommands (Draw_Interpretor& I);




protected:





private:




friend class DrawTrSurf_Drawable;
friend class DrawTrSurf_Point;
friend class DrawTrSurf_Curve;
friend class DrawTrSurf_BSplineCurve;
friend class DrawTrSurf_BezierCurve;
friend class DrawTrSurf_Curve2d;
friend class DrawTrSurf_BSplineCurve2d;
friend class DrawTrSurf_BezierCurve2d;
friend class DrawTrSurf_Triangulation2D;
friend class DrawTrSurf_Surface;
friend class DrawTrSurf_BSplineSurface;
friend class DrawTrSurf_BezierSurface;
friend class DrawTrSurf_Triangulation;
friend class DrawTrSurf_Polygon3D;
friend class DrawTrSurf_Polygon2D;

};

class DrawTrSurf_SaveAndRestoreCurve : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreCurve()
    :Draw_SaveAndRestoreBase("DrawTrSurf_Curve") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreBezierCurve : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreBezierCurve()
    :Draw_SaveAndRestoreBase("DrawTrSurf_BezierCurve") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreBSplineCurve : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreBSplineCurve()
    :Draw_SaveAndRestoreBase("DrawTrSurf_BSplineCurve") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreCurve2d : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreCurve2d()
    :Draw_SaveAndRestoreBase("DrawTrSurf_Curve2d") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreBezierCurve2d : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreBezierCurve2d()
    :Draw_SaveAndRestoreBase("DrawTrSurf_BezierCurve2d") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreBSplineCurve2d : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreBSplineCurve2d()
    :Draw_SaveAndRestoreBase("DrawTrSurf_BSplineCurve2d") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreSurface : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreSurface()
    :Draw_SaveAndRestoreBase("DrawTrSurf_Surface") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreBezierSurface : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreBezierSurface()
    :Draw_SaveAndRestoreBase("DrawTrSurf_BezierSurface") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreBSplineSurface : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreBSplineSurface()
    :Draw_SaveAndRestoreBase("DrawTrSurf_BSplineSurface") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& os, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestorePoint : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestorePoint()
    :Draw_SaveAndRestoreBase("DrawTrSurf_Point") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& OS, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestoreTriangulation : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestoreTriangulation()
    :Draw_SaveAndRestoreBase("DrawTrSurf_Triangulation") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& OS, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestorePolygon3D : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestorePolygon3D()
    :Draw_SaveAndRestoreBase("DrawTrSurf_Polygon3D") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& OS, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

class DrawTrSurf_SaveAndRestorePolygon2D : public Draw_SaveAndRestoreBase
{
public:
  DrawTrSurf_SaveAndRestorePolygon2D()
    :Draw_SaveAndRestoreBase("DrawTrSurf_Polygon2D") {}

  void                    Save(const Handle(Draw_Drawable3D)& theDrawable3D, std::ostream& OS, TopTools_FormatVersion theVersion) const  override;
  Handle(Draw_Drawable3D) Restore(std::istream& is) const override;
  Standard_Boolean        Test(const Handle(Draw_Drawable3D)& theDrawable3D) const override;
private:

};

#endif // _DrawTrSurf_HeaderFile

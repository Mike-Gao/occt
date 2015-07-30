// Created on: 1995-12-07
// Created by: Jacques GOUSSARD
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

#ifndef _BRepCheck_Vertex_HeaderFile
#define _BRepCheck_Vertex_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepCheck_Result.hxx>
#include <Standard_Real.hxx>
class TopoDS_Vertex;
class TopoDS_Shape;


class BRepCheck_Vertex;
DEFINE_STANDARD_HANDLE(BRepCheck_Vertex, BRepCheck_Result)


class BRepCheck_Vertex : public BRepCheck_Result
{

public:

  
  Standard_EXPORT BRepCheck_Vertex(const TopoDS_Vertex& V);
  
  Standard_EXPORT void InContext (const TopoDS_Shape& ContextShape);
  
  Standard_EXPORT void Minimum();
  
  Standard_EXPORT void Blind();
  
  Standard_EXPORT Standard_Real Tolerance();

  //! Sets status of the Vertex
  Standard_EXPORT void SetStatus(const BRepCheck_Status theStatus);



  DEFINE_STANDARD_RTTI(BRepCheck_Vertex,BRepCheck_Result)

protected:




private:




};







#endif // _BRepCheck_Vertex_HeaderFile

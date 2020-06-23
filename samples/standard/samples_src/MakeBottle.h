#ifndef MAKEBOTTLE_H
#define MAKEBOTTLE_H

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>


TopoDS_Shape MakeBottle(const Standard_Real myWidth,
                        const Standard_Real myHeight,
                        const Standard_Real myThickness);

#endif // MAKEBOTTLE_H
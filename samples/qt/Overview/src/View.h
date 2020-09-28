// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef VIEW_H
#define VIEW_H

#include <functional>

#include <Standard_WarningsDisable.hxx>
#include <QWidget>
#include <QAction>
#include <QToolBar>
#include <QList>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_ViewController.hxx>

class TopoDS_Shape;
class QRubberBand;

enum class CurrentAction3d { Nothing, DynamicZooming, WindowZooming, 
                             DynamicPanning, GlobalPanning, DynamicRotation, ObjectDececting };
enum class ViewAction { FitAll, FitArea, Zoom, Pan, GlobalPan, Front, Back, Top, Bottom, 
                        Left, Right, Axo, Rotation, Reset, HlrOff, HlrOn, Shading, Wireframe, Transparency };
enum class RaytraceAction { ToolRaytracing, ToolShadows, ToolReflections, ToolAntialiasing };

class View: public QWidget, protected AIS_ViewController
{
    Q_OBJECT
public:
    View( Handle(AIS_InteractiveContext) theContext, bool is3dView, QWidget* parent );

    ~View();

    virtual void    init();
    QList<QAction*> getViewActions();
    QAction*        getViewAction(ViewAction theAction);
    QList<QAction*> getRaytraceActions();
    QAction*        getRaytraceAction(RaytraceAction theAction);

    void EnableRaytracing();
    void DisableRaytracing();

    void SetRaytracedShadows (bool theState);
    void SetRaytracedReflections (bool theState);
    void SetRaytracedAntialiasing (bool theState);

    bool IsRaytracingMode() const { return myIsRaytracing; }
    bool IsShadowsEnabled() const { return myIsShadowsEnabled; }
    bool IsReflectionsEnabled() const { return myIsReflectionsEnabled; }
    bool IsAntialiasingEnabled() const { return myIsAntialiasingEnabled; }

    static QString GetMessages(int type,TopAbs_ShapeEnum aSubShapeType, TopAbs_ShapeEnum aShapeType);
    static QString GetShapeType(TopAbs_ShapeEnum aShapeType);

    Standard_EXPORT static void OnButtonuseraction(int ExerciceSTEP, Handle(AIS_InteractiveContext)& );
    Standard_EXPORT static void DoSelection(int Id, Handle(AIS_InteractiveContext)& );
    Standard_EXPORT static void OnSetSelectionMode(Handle(AIS_InteractiveContext)&,
                                                   Standard_Integer&,
                                                   TopAbs_ShapeEnum& SelectionMode,
                                                   Standard_Boolean& );
    virtual QPaintEngine* paintEngine() const;
    Handle(V3d_View)      getView();
signals:
    void selectionChanged();

public slots:
    void fitAll();
    void fitArea();
    void zoom();
    void pan();
    void globalPan();
    void front();
    void back();
    void top();
    void bottom();
    void left();
    void right();
    void axo();
    void rotation();
    void reset();
    void hlrOn();
    void hlrOff();
    void shading();
    void wireframe();
    void onTransparency();

    void updateToggled( bool );
    void onBackground();
    void onEnvironmentMap();
    void onRaytraceAction();

private slots:
  void onTransparencyChanged(int theVal);

protected:
    virtual void paintEvent( QPaintEvent* ) override;
    virtual void resizeEvent( QResizeEvent* ) override;
    virtual void mousePressEvent( QMouseEvent* ) override;
    virtual void mouseReleaseEvent(QMouseEvent* ) override;
    virtual void mouseMoveEvent( QMouseEvent* ) override;
    virtual void wheelEvent(QWheelEvent*) override;

    virtual void addItemInPopup( QMenu* );

    Handle(AIS_InteractiveContext)& getContext();
    void                            activateCursor( const CurrentAction3d );
    CurrentAction3d                 getCurrentMode();

private:
    void initCursors();
    void initViewActions();
    void initRaytraceActions();

    QAction* RegisterAction(QString theIconPath, QString thePromt, void (View::*theSlot)(void));

private:
    bool myIsRaytracing;
    bool myIsShadowsEnabled;
    bool myIsReflectionsEnabled;
    bool myIsAntialiasingEnabled;

    bool myIs3dView;

    Handle(V3d_View)                myV3dView;
    Handle(AIS_InteractiveContext)  myContext;
    AIS_MouseGestureMap             myDefaultGestures;
    Graphic3d_Vec2i                 myClickPos;

    void                            updateView();

    //! Setup mouse gestures.
    void defineMouseGestures();

    //! Set current action.
    void setCurrentAction(CurrentAction3d theAction)
    {
      myCurrentMode = theAction;
      defineMouseGestures();
    }

    //! Handle selection changed event.
    void OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx,
                            const Handle(V3d_View)& theView) Standard_OVERRIDE;    
    CurrentAction3d                 myCurrentMode;
    Standard_Real                   myCurZoom;
    QMap<ViewAction, QAction*>      myViewActions;
    QMap<RaytraceAction, QAction*>  myRaytraceActions;
    QMenu*                          myBackMenu;
    QToolBar*        myViewBar;
};

#endif



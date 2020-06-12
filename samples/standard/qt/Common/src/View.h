#ifndef VIEW_H
#define VIEW_H


#include <QWidget>
#include <QAction>
#include <QToolBar>
#include <QList>

#include <Standard_WarningsRestore.hxx>
#include <Standard_WarningsDisable.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

class TopoDS_Shape;
class QRubberBand;

enum class CurrentAction3d { CurAction3d_Nothing, CurAction3d_DynamicZooming,
                             CurAction3d_WindowZooming, CurAction3d_DynamicPanning,
                             CurAction3d_GlobalPanning, CurAction3d_DynamicRotation };
enum class ViewAction { ViewFitAll, ViewFitArea, ViewZoom, ViewPan, ViewGlobalPan,
                        ViewFront, ViewBack, ViewTop, ViewBottom, ViewLeft, ViewRight,
                        ViewAxo, ViewRotation, ViewReset, ViewHlrOff, ViewHlrOn };
enum class RaytraceActionId { ToolRaytracing, ToolShadows, ToolReflections, ToolAntialiasing };

//class COMMONSAMPLE_EXPORT View: public QWidget
class View: public QWidget
{
    Q_OBJECT
public:
    View( Handle(AIS_InteractiveContext) theContext, QWidget* parent );

    ~View();

    virtual void                  init();
    bool                          dump( Standard_CString theFile );
    QAction*              getViewAction(ViewAction theAction);
    QList<QAction*>       getViewActions();
    QAction*              getRaytraceAction(RaytraceActionId theAction);
    void                          noActiveActions();
    bool                          isShadingMode();

    void                          EnableRaytracing();
    void                          DisableRaytracing();

    void                          SetRaytracedShadows (bool theState);
    void                          SetRaytracedReflections (bool theState);
    void                          SetRaytracedAntialiasing (bool theState);

    bool                          IsRaytracingMode() const { return myIsRaytracing; }
    bool                          IsShadowsEnabled() const { return myIsShadowsEnabled; }
    bool                          IsReflectionsEnabled() const { return myIsReflectionsEnabled; }
    bool                          IsAntialiasingEnabled() const { return myIsAntialiasingEnabled; }

    static QString                GetMessages( int type,TopAbs_ShapeEnum aSubShapeType,
                                               TopAbs_ShapeEnum aShapeType );
    static QString                GetShapeType( TopAbs_ShapeEnum aShapeType );

    Standard_EXPORT static void   OnButtonuseraction( int ExerciceSTEP,
                                                      Handle(AIS_InteractiveContext)& );
    Standard_EXPORT static void   DoSelection( int Id,
                                               Handle(AIS_InteractiveContext)& );
    Standard_EXPORT static void   OnSetSelectionMode( Handle(AIS_InteractiveContext)&,
                                                      Standard_Integer&,
                                                      TopAbs_ShapeEnum& SelectionMode,
                                                      Standard_Boolean& );
    virtual QPaintEngine*         paintEngine() const;
signals:
    void                          selectionChanged();

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
    void updateToggled( bool );
    void onBackground();
    void onEnvironmentMap();
    void onRaytraceAction();

protected:
    virtual void paintEvent( QPaintEvent* ) override;
    virtual void resizeEvent( QResizeEvent* ) override;
    virtual void mousePressEvent( QMouseEvent* ) override;
    virtual void mouseReleaseEvent(QMouseEvent* ) override;
    virtual void mouseMoveEvent( QMouseEvent* ) override;

    virtual void                  addItemInPopup( QMenu* );

//    Handle(V3d_View)&                     getView();
    Handle(AIS_InteractiveContext)&       getContext();
    void                                  activateCursor( const CurrentAction3d );
    void                                  Popup( const int x, const int y );
    CurrentAction3d                       getCurrentMode();

    virtual void onLButtonDown( const int nFlags, const QPoint point );
    virtual void onMButtonDown( const int nFlags, const QPoint point );
    virtual void onRButtonDown( const int nFlags, const QPoint point );
    virtual void onLButtonUp( Qt::MouseButtons nFlags, const QPoint point );
    virtual void onMButtonUp( Qt::MouseButtons nFlags, const QPoint point );
    virtual void onRButtonUp( Qt::MouseButtons nFlags, const QPoint point );
    virtual void onMouseMove( Qt::MouseButtons nFlags, const QPoint point );

private:
    void initCursors();
    void initViewActions();
    void initRaytraceActions();
    void DragEvent( const int x, const int y, const int TheState );
    void InputEvent( const int x, const int y );
    void MoveEvent( const int x, const int y );
    void MultiMoveEvent( const int x, const int y );
    void MultiDragEvent( const int x, const int y, const int TheState );
    void MultiInputEvent( const int x, const int y );
    void DrawRectangle( const int MinX, const int MinY,
                                                 const int MaxX, const int MaxY, const bool Draw );

private:
    bool                            myIsRaytracing;
    bool                            myIsShadowsEnabled;
    bool                            myIsReflectionsEnabled;
    bool                            myIsAntialiasingEnabled;

    bool                            myDrawRect;           // set when a rect is used for selection or magnify 
    Handle(V3d_View)                myV3dView;
    Handle(AIS_InteractiveContext)  myContext;
    CurrentAction3d                 myCurrentMode;
    Standard_Integer                myXmin;
    Standard_Integer                myYmin;
    Standard_Integer                myXmax;
    Standard_Integer                myYmax;
    Standard_Real                   myCurZoom;
    Standard_Boolean                myHlrModeIsOn;
    QMap<ViewAction, QAction*>      myViewActions;
    QMap<RaytraceActionId, QAction*>  myRaytraceActions;
    QMenu*                          myBackMenu;
    QToolBar*        myViewBar;
    QRubberBand*                    myRectBand; //!< selection rectangle rubber band
};

#endif



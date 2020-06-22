#include "OcctHighlighter.h"



OcctHighlighter::OcctHighlighter(QTextDocument *parent): QSyntaxHighlighter(parent)
{
  HighlightingRule aRule;

  myOcctFormat.setForeground(Qt::darkCyan);

  const QString anOcctPatterns[] = 
  { 
    QStringLiteral("gp_Pnt"), QStringLiteral("gp_XYZ"),  QStringLiteral("gp_Vec"), 
    QStringLiteral("gp_Dir"), QStringLiteral("gp_Ax1"), QStringLiteral("gp_Ax2"),
    QStringLiteral("gp_Ax3"), QStringLiteral("gp_Lin"), QStringLiteral("gp_Circ"),
    QStringLiteral("gp_Elips"), QStringLiteral("gp_Parab"), QStringLiteral("gp_Hypr"),
    QStringLiteral("gp_Cylinder."), QStringLiteral("gp_Cone"), QStringLiteral("gp_Sphere"),
    QStringLiteral("gp_Torus"), QStringLiteral("gp_Pnt2d"), QStringLiteral("gp_XY"),
    QStringLiteral("gp_Ax2d"), QStringLiteral("gp_Ax22d"), QStringLiteral("gp_Lin2d"),
    QStringLiteral("gp_Circ2d."), QStringLiteral("gp_Elips2d"), QStringLiteral("gp_Parab2d"),
    QStringLiteral("gp_Hypr2d"), QStringLiteral("Geom2d_BSplineCurve"), QStringLiteral("Geom2d_BezierCurve"),
    QStringLiteral("Geom2d_OffsetCurve"), QStringLiteral("ProjLib"), QStringLiteral("ElSLib"), 
    QStringLiteral("Extrema_ExtElCS"), QStringLiteral("Extrema_POnCurv"), QStringLiteral("IntAna_Quadric"),
    QStringLiteral("IntAna_IntConicQuad"), QStringLiteral("GccAna_Lin2d2Tan"), QStringLiteral("GccEnt_QualifiedCirc"),
    QStringLiteral("Geom2dAPI_ProjectPointOnCurve"), QStringLiteral("Geom2dAPI_ExtremaCurveCurve"),
    QStringLiteral("Geom2dAPI_InterCurveCurve"), QStringLiteral("Geom2dAPI_PointsToBSpline"), 
    QStringLiteral("Geom_CartesianPoint"), QStringLiteral("Geom_VectorWithMagnitude"), QStringLiteral("Geom_Axis1Placement"),
    QStringLiteral("Geom_Axis2Placement"), QStringLiteral("Geom_Line"),  QStringLiteral("Geom_Circle"), 
    QStringLiteral("Geom_Ellipse"),  QStringLiteral("Geom_Parabola"),  QStringLiteral("Geom_Hyperbola"), 
    QStringLiteral("Geom_BSplineCurve"), QStringLiteral("Geom_BezierCurve"),  QStringLiteral("Geom_TrimmedCurve"), 
    QStringLiteral("Geom_OffsetCurve"), QStringLiteral("Geom_BSplineSurface"), QStringLiteral("Geom_BezierSurface"), 
    QStringLiteral("Geom_Plane"), QStringLiteral("Geom_CylindricalSurface"), QStringLiteral("Geom_ConicalSurface"), 
    QStringLiteral("Geom_SphericalSurface"), QStringLiteral("Geom_ToroidalSurface"), QStringLiteral("Geom_RectangularTrimmedSurface"), 
    QStringLiteral("Geom_OffsetSurface"), QStringLiteral("Geom_SurfaceOfLinearExtrusion"), QStringLiteral("Geom_SurfaceOfRevolution"), 
    QStringLiteral("BndLib_Add3dCurve"), QStringLiteral("BndLib_AddSurface"), QStringLiteral("GeomAdaptor_Curve"), 
    QStringLiteral("GeomAdaptor_Surface"), QStringLiteral("GeomAPI_PointsToBSpline"), QStringLiteral("GeomAPI_PointsToBSplineSurface"), 
    QStringLiteral("GeomConvert"), QStringLiteral("Geom2d_CartesianPoint"), QStringLiteral("Geom2d_VectorWithMagnitude"), 
    QStringLiteral("Geom2d_Line"), QStringLiteral("Geom2d_Circle"), QStringLiteral("Geom2d_Ellipse"), 
    QStringLiteral("Geom2d_Parabola"), QStringLiteral("Geom2d_Hyperbola"), QStringLiteral("Geom2d_TrimmedCurve"), 
    QStringLiteral("Geom2dAdaptor_Curve"), QStringLiteral("Bnd_Box2d"), QStringLiteral("BndLib_Add2dCurve"), 
    QStringLiteral("Adaptor2d_Curve2d"), QStringLiteral("BRepBuilderAPI_MakeEdge"), QStringLiteral("BRepBuilderAPI_MakeFace"), 
    QStringLiteral("BRepPrimAPI_MakeBox"), QStringLiteral("AIS_Point"), QStringLiteral("AIS_TextLabel"), QStringLiteral("AIS_Axis"), 
    QStringLiteral("AIS_Circle"), QStringLiteral("AIS_Plane"), QStringLiteral("AIS_Shape"), QStringLiteral("AIS_ColoredShape"), 
    QStringLiteral("GProp_PEquation"), QStringLiteral("Extrema_ExtCS"), QStringLiteral("GCPnts_QuasiUniformDeflection"), 
    QStringLiteral("GProp_GProps"), QStringLiteral("GProp_PrincipalProps"), QStringLiteral("TopoDS"), 
    QStringLiteral("TopoDS_Iterator"), QStringLiteral("TopoDS_Compound"), QStringLiteral("TopoDS_Edge"), QStringLiteral("TopoDS_Face"), 
    QStringLiteral("TopoDS_Shell"), QStringLiteral("TopoDS_Solid"), QStringLiteral("TopoDS_Vertex"), 
    QStringLiteral("TopoDS_Wire"), QStringLiteral("TopExp"), QStringLiteral("TopExp_Explorer"), 
    QStringLiteral("TColgp_Array2OfPnt"), QStringLiteral("BRep_Builder"), QStringLiteral("BRepGProp"), QStringLiteral("BRep_Tool"), 
    QStringLiteral("BRepTools"), QStringLiteral("BRepTools_ReShape"), QStringLiteral("BRepAdaptor_Curve"), 
    QStringLiteral("BRepAdaptor_CompCurve"), QStringLiteral("BRepAdaptor_Surface"), QStringLiteral("BRepAlgoAPI_Common"), 
    QStringLiteral("BRepAlgoAPI_Cut"), QStringLiteral("BRepAlgoAPI_Fuse"), QStringLiteral("BRepAlgoAPI_Section"), 
    QStringLiteral("BRepAlgoAPI_Splitter"), QStringLiteral("BRepAlgoAPI_Defeaturing"), QStringLiteral("BRepBuilderAPI_Copy"), 
    QStringLiteral("BRepBuilderAPI_MakeVertex"), QStringLiteral("BRepBuilderAPI_MakeEdge"), QStringLiteral("BRepBuilderAPI_MakeFace"), 
    QStringLiteral("BRepBuilderAPI_MakePolygon"), QStringLiteral("BRepBuilderAPI_MakeShell"), QStringLiteral("BRepBuilderAPI_MakeSolid"), 
    QStringLiteral("BRepBuilderAPI_MakeWire"), QStringLiteral("BRepBuilderAPI_NurbsConvert"), QStringLiteral("BRepBuilderAPI_Sewing"), 
    QStringLiteral("BRepBuilderAPI_Transform"), QStringLiteral("BRepCheck_Analyzer"), QStringLiteral("BRepPrimAPI_MakeBox"), 
    QStringLiteral("BRepPrimAPI_MakeCylinder"), QStringLiteral("BRepPrimAPI_MakeRevol"), QStringLiteral("BRepFilletAPI_MakeChamfer"), 
    QStringLiteral("BRepFilletAPI_MakeFillet"), QStringLiteral("BRepOffsetAPI_MakeOffset"), QStringLiteral("BRepOffsetAPI_MakeEvolved.hxx"),
    QStringLiteral("Standard_Integer"), QStringLiteral("Standard_Real"), QStringLiteral("Standard_Boolean"), QStringLiteral("Standard_ShortReal"), 
    QStringLiteral("Standard_Character"), QStringLiteral("Standard_Byte"), QStringLiteral("Standard_Address"), QStringLiteral("Standard_Size"),
    QStringLiteral("Standard_Time"), QStringLiteral("Standard_Utf8Char"), QStringLiteral("Standard_Utf8UChar"), 
    QStringLiteral("Standard_ExtCharacter"), QStringLiteral("Standard_Utf16Char"), QStringLiteral("Standard_Utf32Char"), 
    QStringLiteral("Standard_WideChar"), QStringLiteral("Standard_CString"), QStringLiteral("Standard_ExtString"), 
    QStringLiteral("NCollection_Vector"), QStringLiteral("TCollection_AsciiString"), QStringLiteral("TCollection_BaseSequence"), 
    QStringLiteral("TCollection_BasicMap"), QStringLiteral("TCollection_BasicMapIterator"), QStringLiteral("TCollection_ExtendedString"), 
    QStringLiteral("TCollection_HAsciiString"), QStringLiteral("TCollection_HExtendedString"), QStringLiteral("TCollection_MapNode"), 
    QStringLiteral("TCollection_MapNodePtr"), QStringLiteral("TCollection_SeqNode"), QStringLiteral("TCollection_SeqNodePtr"), 
    QStringLiteral("TCollection_Side"), QStringLiteral("Standard_False"), QStringLiteral("Standard_True"), 
    QStringLiteral("TCollection"), QStringLiteral("NCollection"), QStringLiteral("gp_Trsf"), QStringLiteral("Handle"),
    QStringLiteral("Aspect_TOL_DASH"),  QStringLiteral("Aspect_TOM_O_STAR"), QStringLiteral("Aspect_TOL_SOLID"),
    QStringLiteral("Aspect_TOM_O_STAR")
  };
  for (const QString &aPattern : anOcctPatterns)
  {
    aRule.myPattern = QRegularExpression(aPattern);
    aRule.myFormat = myOcctFormat;
    myHighlightingRules.append(aRule);
  }

  myMemberFormat.setFontWeight(QFont::Black);
  const QString aMemberPatterns[] =
  {
    QStringLiteral("myIsProcessed"), QStringLiteral("myObject2d"), QStringLiteral("myObject3d"),
    QStringLiteral("myResult"), QStringLiteral("myCode"),  QStringLiteral("myCodePath")
  };
  for (const QString &aPattern : aMemberPatterns)
  {
    aRule.myPattern = QRegularExpression(aPattern);
    aRule.myFormat = myMemberFormat;
    myHighlightingRules.append(aRule);
  }
  
  myHelperFormat.setForeground(Qt::gray);
  const QString aHelperPatterns[] =
  {
    QStringLiteral("AdaptorCurve_AIS"), QStringLiteral("AdaptorVec_AIS"), QStringLiteral("AdaptorCurve2d_AIS"), QStringLiteral("AdaptorPnt2d_AIS")
  };
  for (const QString &aPattern : aHelperPatterns)
  {
    aRule.myPattern = QRegularExpression(aPattern);
    aRule.myFormat = myHelperFormat;
    myHighlightingRules.append(aRule);
  }


  myKeywordFormat.setForeground(Qt::darkBlue);
  myKeywordFormat.setFontWeight(QFont::Bold);
  const QString aKeywordPatterns[] = 
  { 
    QStringLiteral("\\balignas\\b"), QStringLiteral("\\balignof\\b"), QStringLiteral("\\band\\b"),
    QStringLiteral("\\band_eq\\b"), QStringLiteral("\\basm\\b"), QStringLiteral("\\bauto\\b"),
    QStringLiteral("\\bbitand\\b"), QStringLiteral("\\bbitor\\b"), QStringLiteral("\\bbool\\b"),
    QStringLiteral("\\bbreak\\b"), QStringLiteral("\\bcase\\b"), QStringLiteral("\\bcatch\\b"),
    QStringLiteral("\\bchar\\b"), QStringLiteral("\\bchar16_t\\b"), QStringLiteral("\\bchar32_t\\b"),
    QStringLiteral("\\bclass\\b"), QStringLiteral("\\bcompl\\b"), QStringLiteral("\\bconst\\b"),
    QStringLiteral("\\bconstexpr\\b"), QStringLiteral("\\bconst_cast\\b"), QStringLiteral("\\bcontinue\\b"),
    QStringLiteral("\\bdecltype\\b"), QStringLiteral("\\bdefault\\b"), QStringLiteral("\\bdelete\\b"),
    QStringLiteral("\\bdo\\b"), QStringLiteral("\\bdouble\\b"), QStringLiteral("\\bdynamic_cast\\b"),
    QStringLiteral("\\belse\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
    QStringLiteral("\\bexport\\b"), QStringLiteral("\\bextern\\b"), QStringLiteral("\\bfalse\\b"),
    QStringLiteral("\\bfloat\\b"), QStringLiteral("\\bfor\\b"), QStringLiteral("\\bfriend\\b"),
    QStringLiteral("\\bgoto\\b"), QStringLiteral("\\bif\\b"), QStringLiteral("\\binline\\b"),
    QStringLiteral("\\bint\\b"), QStringLiteral("\\blong\\b"), QStringLiteral("\\bmutable\\b"),
    QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\bnew\\b"), QStringLiteral("\\bnoexcept\\b"),
    QStringLiteral("\\bnot\\b"), QStringLiteral("\\bnot_eq\\b"), QStringLiteral("\\bnullptr\\b"),
    QStringLiteral("\\boperator\\b"), QStringLiteral("\\bor\\b"), QStringLiteral("\\bor_eq\\b"),
    QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
    QStringLiteral("\\bregister\\b"), QStringLiteral("\\breinterpret_cast\\b"), QStringLiteral("\\breturn\\b"),
    QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsigned\\b"), QStringLiteral("\\bsizeof\\b"),
    QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstatic_assert\\b"), QStringLiteral("\\bstatic_cast\\b"),
    QStringLiteral("\\bstruct\\b"),QStringLiteral("\\bswitch\\b"), QStringLiteral("\\btemplate\\b"),
    QStringLiteral("\\bthis\\b"), QStringLiteral("\\bthread_local\\b"), QStringLiteral("\\bthrow\\b"),
    QStringLiteral("\\btrue\\b"), QStringLiteral("\\btry\\b"), QStringLiteral("\\btypedef\\b"),
    QStringLiteral("\\btypeid\\b"), QStringLiteral("\\btypename\\b"),QStringLiteral("\\bunion\\b"),
    QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\busing\\b"), QStringLiteral("\\bvirtual\\b"),
    QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bwchar_t\\b"),
    QStringLiteral("\\bwhile\\b"), QStringLiteral("\\bxor\\b"), QStringLiteral("\\bxor_eq\\b"),
    QStringLiteral("\\boverride\\b"), QStringLiteral("\\bfinal\\b")
  };
  for (const QString &aPattern : aKeywordPatterns) 
  {
    aRule.myPattern = QRegularExpression(aPattern);
    aRule.myFormat = myKeywordFormat;
    myHighlightingRules.append(aRule);
  }

  myClassFormat.setFontWeight(QFont::Bold);
  myClassFormat.setForeground(Qt::darkMagenta);
  aRule.myPattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
  aRule.myFormat = myClassFormat;
  myHighlightingRules.append(aRule);

  myQuotationFormat.setForeground(Qt::darkRed);
  aRule.myPattern = QRegularExpression(QStringLiteral("\".*\""));
  aRule.myFormat = myQuotationFormat;
  myHighlightingRules.append(aRule);

  //myFunctionFormat.setFontItalic(true);
  //myFunctionFormat.setForeground(Qt::blue);
  //aRule.myPattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
  //aRule.myFormat = myFunctionFormat;
  //myHighlightingRules.append(aRule);

  mySingleLineCommentFormat.setForeground(Qt::darkGreen);
  aRule.myPattern = QRegularExpression(QStringLiteral("//[^\n]*"));
  aRule.myFormat = mySingleLineCommentFormat;
  myHighlightingRules.append(aRule);

  myMultiLineCommentFormat.setForeground(Qt::darkGreen);

  myCommentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
  myCommentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void OcctHighlighter::highlightBlock(const QString &theText)
{
  for (const HighlightingRule &rule : qAsConst(myHighlightingRules)) 
  {
    QRegularExpressionMatchIterator matchIterator = rule.myPattern.globalMatch(theText);
    while (matchIterator.hasNext()) 
    {
      QRegularExpressionMatch match = matchIterator.next();
      setFormat(match.capturedStart(), match.capturedLength(), rule.myFormat);
    }
  }

  setCurrentBlockState(0);

  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = theText.indexOf(myCommentStartExpression);

  while (startIndex >= 0) 
  {
    QRegularExpressionMatch match = myCommentEndExpression.match(theText, startIndex);
    int endIndex = match.capturedStart();
    int commentLength = 0;
    if (endIndex == -1) 
    {
      setCurrentBlockState(1);
      commentLength = theText.length() - startIndex;
    }
    else 
    {
      commentLength = endIndex - startIndex
        + match.capturedLength();
    }
    setFormat(startIndex, commentLength, myMultiLineCommentFormat);
    startIndex = theText.indexOf(myCommentStartExpression, startIndex + commentLength);
  }
}


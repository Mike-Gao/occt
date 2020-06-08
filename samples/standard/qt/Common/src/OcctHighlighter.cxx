#include "OcctHighlighter.h"



OcctHighlighter::OcctHighlighter(QTextDocument *parent): QSyntaxHighlighter(parent)
{
  HighlightingRule aRule;

  myKeywordFormat.setForeground(Qt::darkBlue);
  myKeywordFormat.setFontWeight(QFont::Bold);
  const QString aKeywordPatterns[] = 
  {
    QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
    QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
    QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
    QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
    QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
    QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
    QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
    QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
    QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
    QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b")
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

  myQuotationFormat.setForeground(Qt::darkGreen);
  aRule.myPattern = QRegularExpression(QStringLiteral("\".*\""));
  aRule.myFormat = myQuotationFormat;
  myHighlightingRules.append(aRule);

  myFunctionFormat.setFontItalic(true);
  myFunctionFormat.setForeground(Qt::blue);
  aRule.myPattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
  aRule.myFormat = myFunctionFormat;
  myHighlightingRules.append(aRule);

  mySingleLineCommentFormat.setForeground(Qt::red);
  aRule.myPattern = QRegularExpression(QStringLiteral("//[^\n]*"));
  aRule.myFormat = mySingleLineCommentFormat;
  myHighlightingRules.append(aRule);

  myMultiLineCommentFormat.setForeground(Qt::red);

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


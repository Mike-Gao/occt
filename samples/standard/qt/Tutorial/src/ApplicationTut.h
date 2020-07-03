#ifndef APPLICATIONTUT_H
#define APPLICATIONTUT_H

#include "DocumentTut.h"
#include "ApplicationCommon.h"

class ApplicationTut: public ApplicationCommonWindow
{
    Q_OBJECT

public:
	
  ApplicationTut(QString theSampleType);
  ~ApplicationTut();

  static QString    getTutResourceDir();

public slots:

private:
	void              createMakeBottleOperation();

private:
	QToolBar*					myMakeBottleBar;
};

#endif

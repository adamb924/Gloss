#ifndef ORTHOGRAPHICWORDDISPLAYWIDGET_H
#define ORTHOGRAPHICWORDDISPLAYWIDGET_H

#include "abstractworddisplaywidget.h"

class OrthographicWordDisplayWidget : public AbstractWordDisplayWidget
{
public:
    OrthographicWordDisplayWidget(TextBit *bit, Project *project);

    void guessInterpretation();

};

#endif // ORTHOGRAPHICWORDDISPLAYWIDGET_H

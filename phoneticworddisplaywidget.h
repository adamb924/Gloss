#ifndef PHONETICWORDDISPLAYWIDGET_H
#define PHONETICWORDDISPLAYWIDGET_H

#include "abstractworddisplaywidget.h"

class PhoneticWordDisplayWidget : public AbstractWordDisplayWidget
{
public:
    PhoneticWordDisplayWidget(TextBit *bit, Project *project);

    void guessInterpretation();
};

#endif // PHONETICWORDDISPLAYWIDGET_H

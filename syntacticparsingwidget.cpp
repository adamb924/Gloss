#include "syntacticparsingwidget.h"
#include "ui_syntacticparsingwidget.h"

SyntacticParsingWidget::SyntacticParsingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyntacticParsingWidget)
{
    ui->setupUi(this);
}

SyntacticParsingWidget::~SyntacticParsingWidget()
{
    delete ui;
}

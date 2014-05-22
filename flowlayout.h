/*!
  \class FlowLayout
  \ingroup Interlinear
  \brief A QLayout subclass, providing a layout which arranges widgets as if it they were text. This is instantiated in InterlinearDisplayWidget.

  This is Qt example code, modified to allow the layout to add widgets in left-to-right order.

  This is under the BSD license. I removed the header because it was interfering with my Doxygen comments.
*/

#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>
#include <QStyle>

class FlowLayout : public QLayout
{
public:
    FlowLayout(Qt::LayoutDirection layoutDir, QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    FlowLayout(Qt::LayoutDirection layoutDir, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout();

    void addItem(QLayoutItem *item);
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int) const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt(int index);

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    Qt::LayoutDirection mDir;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;
};

#endif

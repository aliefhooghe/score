#pragma once
#include <iscore/selection/Selection.hpp>
#include <qwidget.h>

namespace iscore{
    class SelectionStack;
}

class QToolButton;

class SelectionStackWidget : public QWidget
{
    public:
        SelectionStackWidget(iscore::SelectionStack& s, QWidget* parent);

    public slots:
        void selectionChanged(const Selection& s);

    private:
        QToolButton* m_prev{};
        QToolButton* m_next{};
        iscore::SelectionStack& m_stack;
};

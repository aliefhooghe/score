#pragma once
#include <Scenario/Inspector/ExpressionValidator.hpp>
#include <boost/core/explicit_operator_bool.hpp>
#include <qstring.h>
#include <qvector.h>
#include <qwidget.h>

#include "State/Expression.hpp"

class QVBoxLayout;
class SimpleExpressionEditorWidget;

class ExpressionEditorWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit ExpressionEditorWidget(QWidget *parent = 0);

    iscore::Expression expression();

    public slots:
    void setExpression(iscore::Expression e);

    signals:
    void editingFinished();

    private slots:
    void on_editFinished();
//	void on_operatorChanged(int i);
// TODO on_modelChanged()

    QString currentExpr();
    void addNewRelation();

    private:
    QVector<SimpleExpressionEditorWidget*> m_relations;

    QVBoxLayout* m_mainLayout{};

    ExpressionValidator<iscore::Expression> m_validator;
};


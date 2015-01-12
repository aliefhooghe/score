#include "ConstraintInspectorWidget.hpp"

#include "Widgets/AddSharedProcessWidget.hpp"
#include "Widgets/BoxWidget.hpp"
#include "Widgets/DurationSectionWidget.hpp"
#include "Widgets/Box/BoxInspectorSection.hpp"

#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Constraint/Temporal/TemporalConstraintViewModel.hpp"
#include "Document/Constraint/Box/BoxModel.hpp"
#include "Document/Constraint/Box/Deck/DeckModel.hpp"
#include "Commands/Constraint/AddProcessToConstraint.hpp"
#include "Commands/Constraint/AddBoxToConstraint.hpp"
#include "Commands/Constraint/SetMinDuration.hpp"
#include "Commands/Constraint/SetMaxDuration.hpp"
#include "Commands/Constraint/SetRigidity.hpp"
#include "Commands/Scenario/ShowBoxInViewModel.hpp"
#include "Commands/Scenario/HideBoxInViewModel.hpp"
#include "ProcessInterface/ProcessSharedModelInterface.hpp"

#include <InspectorInterface/InspectorSectionWidget.hpp>

#include <tools/ObjectPath.hpp>

#include <QFrame>
#include <QLineEdit>
#include <QLayout>
#include <QFormLayout>
#include <QWidget>
#include <QToolButton>
#include <QPushButton>
#include <QApplication>

using namespace Scenario::Command;


class Separator : public QFrame
{
	public:
		Separator(QWidget* parent):
			QFrame{parent}
		{
			setFrameShape(QFrame::HLine);
			setLineWidth(2);
		}
};

ConstraintInspectorWidget::ConstraintInspectorWidget (TemporalConstraintViewModel* object, QWidget* parent) :
	InspectorWidgetBase (parent)
{
	setObjectName ("Constraint");
	m_currentConstraint = object;

	m_durationSection = new DurationSectionWidget{this};
	m_properties.push_back(m_durationSection);

	// Separator
	m_properties.push_back(new Separator{this});

	// Processes
	m_processSection = new InspectorSectionWidget ("Processes", this);
	m_processSection->setObjectName("Processes");

	m_properties.push_back(m_processSection);
	m_properties.push_back(new AddSharedProcessWidget{this});

	// Separator
	m_properties.push_back(new Separator{this});

	// Boxes
	m_boxSection = new InspectorSectionWidget{"Boxes", this};
	m_boxSection->setObjectName("Boxes");

	m_boxWidget = new BoxWidget{this};

	m_properties.push_back(m_boxSection);
	m_properties.push_back(m_boxWidget);

	// Display data
	updateSectionsView (areaLayout(), m_properties);
	areaLayout()->addStretch(1);

	updateDisplayedValues(object);
}

TemporalConstraintViewModel*ConstraintInspectorWidget::viewModel() const
{ return m_currentConstraint; }

ConstraintModel* ConstraintInspectorWidget::model() const
{ return m_currentConstraint? m_currentConstraint->model() : nullptr; }

void ConstraintInspectorWidget::updateDisplayedValues (TemporalConstraintViewModel* constraint)
{
	// Cleanup the widgets
	for(auto& process : m_processesSectionWidgets)
	{
		m_processSection->removeContent(process);
	}
	m_processesSectionWidgets.clear();

	for(auto& box : m_boxesSectionWidgets)
	{
		m_boxSection->removeContent(box);
	}
	m_boxesSectionWidgets.clear();

	// Cleanup the connections
	for(auto& connection : m_connections)
		QObject::disconnect(connection);
	m_connections.clear();


	if (constraint != nullptr)
	{
		m_currentConstraint = constraint;

		// Constraint settings
		setName (model()->metadata.name() );
		setColor (model()->metadata.color() );
		setComments (model()->metadata.comment() );
		setInspectedObject (m_currentConstraint);
		changeLabelType ("Constraint");

		// Constraint interface
		m_connections.push_back(
					connect(model(),	&ConstraintModel::processCreated,
							this,		&ConstraintInspectorWidget::on_processCreated));
		m_connections.push_back(
					connect(model(),	&ConstraintModel::processRemoved,
							this,		&ConstraintInspectorWidget::on_processRemoved));
		m_connections.push_back(
					connect(model(),	&ConstraintModel::boxCreated,
							this,		&ConstraintInspectorWidget::on_boxCreated));
		m_connections.push_back(
					connect(model(),	&ConstraintModel::boxRemoved,
							this,		&ConstraintInspectorWidget::on_boxRemoved));

		// Processes
		for(ProcessSharedModelInterface* process : model()->processes())
		{
			displaySharedProcess(process);
		}

		// Box
		m_boxWidget->setModel(model());
		m_boxWidget->updateComboBox();

		for(BoxModel* box: model()->boxes())
		{
			setupBox(box);
		}
	}
	else
	{
		m_currentConstraint = nullptr;
		m_boxWidget->setModel(nullptr);
	}
}

void ConstraintInspectorWidget::createProcess(QString processName)
{
	auto cmd = new AddProcessToConstraint{
			   ObjectPath::pathFromObject("BaseConstraintModel",
										  model()),
			   processName};
	emit submitCommand(cmd);
}

void ConstraintInspectorWidget::createBox()
{
	auto cmd = new AddBoxToConstraint(
				   ObjectPath::pathFromObject(
					   "BaseConstraintModel",
					   model()));
	emit submitCommand(cmd);
}

void ConstraintInspectorWidget::activeBoxChanged(QString box)
{
	// TODO mettre à jour l'inspecteur si la box affichée change (i.e. via une commande réseau).
	if(box == m_boxWidget->hiddenText)
	{
		if(viewModel()->isBoxShown())
		{
			auto cmd = new HideBoxInViewModel(viewModel());
			emit submitCommand(cmd);
		}
	}
	else
	{
		bool ok{};
		int id = box.toInt(&ok);

		if(ok)
		{
			auto cmd = new ShowBoxInViewModel(viewModel(), id);
			emit submitCommand(cmd);
		}
	}
}

void ConstraintInspectorWidget::minDurationSpinboxChanged(int val)
{
	auto cmd = new SetMinDuration(
				   ObjectPath::pathFromObject(
					   "BaseConstraintModel",
					   model()),
				   val);

	emit submitCommand(cmd);
}

void ConstraintInspectorWidget::maxDurationSpinboxChanged(int val)
{
	auto cmd = new SetMaxDuration(
				   ObjectPath::pathFromObject(
					   "BaseConstraintModel",
					   model()),
				   val);

	emit submitCommand(cmd);
}

void ConstraintInspectorWidget::rigidCheckboxToggled(bool b)
{
	auto cmd = new SetRigidity(
				   ObjectPath::pathFromObject(
					   "BaseConstraintModel",
					   model()),
				   b);

	emit submitCommand(cmd);
}

void ConstraintInspectorWidget::displaySharedProcess(ProcessSharedModelInterface* process)
{
	InspectorSectionWidget* newProc = new InspectorSectionWidget (process->processName());
	m_processesSectionWidgets.push_back (newProc);
	m_processSection->addContent (newProc);
}

void ConstraintInspectorWidget::setupBox(BoxModel* box)
{
	// Display the widget
	BoxInspectorSection* newBox = new BoxInspectorSection{QString{"Box.%1"}.arg((SettableIdentifier::identifier_type)box->id()),
								  box,
								  this};

	connect(newBox, &BoxInspectorSection::submitCommand,
			this,	&ConstraintInspectorWidget::submitCommand);

	m_boxesSectionWidgets[(SettableIdentifier::identifier_type)box->id()] = newBox;
	m_boxSection->addContent(newBox);
}


void ConstraintInspectorWidget::on_processCreated(QString processName, int processId)
{
	reloadDisplayedValues();
}

void ConstraintInspectorWidget::on_processRemoved(int processId)
{
	reloadDisplayedValues();
}


void ConstraintInspectorWidget::on_boxCreated(int boxId)
{
	setupBox(model()->box(boxId));
	m_boxWidget->updateComboBox();
}

void ConstraintInspectorWidget::on_boxRemoved(int boxId)
{
	auto ptr = m_boxesSectionWidgets[boxId];
	m_boxesSectionWidgets.remove(boxId);

	if(ptr)
	{
		ptr->deleteLater();
	}
	m_boxWidget->updateComboBox();
}


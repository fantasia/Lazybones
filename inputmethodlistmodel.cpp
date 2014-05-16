#include "inputmethodlistmodel.h"
#include <QQmlEngine>
#include "Lazybones.h"
#include "presetdata.h"

InputMethodListModel::InputMethodListModel(PresetData *preset) :
	QAbstractListModel(NULL), m_currentActionIndex(-1), m_running(false), m_repeat(true), m_preset(preset)
{
}

int InputMethodListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_inputMethods.size();
}

QVariant InputMethodListModel::data(const QModelIndex &index, int /*role*/) const
{
	InputAction *action = m_inputMethods.at(index.row()).data();
	QQmlEngine::setObjectOwnership(action, QQmlEngine::CppOwnership);
	return qVariantFromValue<InputAction *>(action);
}

QHash<int, QByteArray> InputMethodListModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[Qt::UserRole + 1] = "itemData";
	return roles;
}

void InputMethodListModel::add(QSharedPointer<InputAction> action)
{
	beginInsertRows(QModelIndex(), m_inputMethods.size(), m_inputMethods.size());
	m_inputMethods.push_back(action);
	endInsertRows();

	if(m_preset)
		emit m_preset->currentIndexChanged();

	action->setModel(this);
}

QSharedPointer<InputAction> InputMethodListModel::find(InputAction *rawPointer)
{
	for(int i = 0; i < m_inputMethods.size(); i++)
	{
		if(m_inputMethods[i].data() == rawPointer)
			return m_inputMethods[i];
	}
	return QSharedPointer<InputAction>();
}

void InputMethodListModel::remove(int index)
{
	beginRemoveRows(QModelIndex(), index, index);
	m_inputMethods.removeAt(index);
	endRemoveRows();

	if(m_preset)
		emit m_preset->currentIndexChanged();
}

void InputMethodListModel::stop(void)
{
	m_running = false;

	if(m_preset)
		emit m_preset->runningChanged();
}

void InputMethodListModel::start(int fromIndex)
{
	if(m_inputMethods.size() <= 0)
		return;

	Lazybones::setEmergencyStop(false);

	m_running = true;
	if(fromIndex < 0 || fromIndex >= m_inputMethods.size())
		m_currentActionIndex = -1;
	else
		m_currentActionIndex = fromIndex - 1;

	if(m_preset)
		emit m_preset->currentIndexChanged();

	doNextAction();
}

void InputMethodListModel::doNextAction()
{
	if(!m_running)
		return;

	if(Lazybones::isEmergencyStop())
	{
		stop();
		return;
	}

	++m_currentActionIndex;
	if(m_currentActionIndex >= m_inputMethods.size())
	{
		if(!m_repeat)
		{
			stop();
			return;
		}
		m_currentActionIndex = 0;
	}

	if(m_preset)
		emit m_preset->currentIndexChanged();

	m_inputMethods[m_currentActionIndex]->doit();
}

void InputMethodListModel::clear(void)
{
	beginResetModel();
	m_currentActionIndex = -1;
	m_inputMethods.clear();
	endResetModel();
}

#include "macropresetlistmodel.h"

MacroPresetListModel::MacroPresetListModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

int MacroPresetListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_model.size();
}

QVariant MacroPresetListModel::data(const QModelIndex &index, int /*role*/) const
{
	QSharedPointer<MacroPresetData> presetData = m_model.at(index.row());
	return presetData->title();
}

QHash<int, QByteArray> MacroPresetListModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[Qt::UserRole + 1] = "modelData";
	return roles;
}

void MacroPresetListModel::add(QSharedPointer<MacroPresetData> presetData)
{
	beginInsertRows(QModelIndex(), 0, 0);
	m_model.push_back(presetData);
	presetData->setMacroPresetListModel(this);
	endInsertRows();
}

void MacroPresetListModel::update(MacroPresetData *data)
{
	for(int i = 0; i < m_model.size(); i++)
	{
		if(m_model[i].data() == data)
		{
			QModelIndex modelIndex = createIndex(i, 0);
			emit dataChanged(modelIndex, modelIndex);
			break;
		}
	}
}

void MacroPresetListModel::resetModel(void)
{
	beginResetModel();
	endResetModel();
}

void MacroPresetListModel::clear(void)
{
	beginResetModel();
	m_model.clear();
	endResetModel();
}

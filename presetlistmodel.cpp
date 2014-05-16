#include "presetlistmodel.h"

PresetListModel::PresetListModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

int PresetListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_model.size();
}

QVariant PresetListModel::data(const QModelIndex &index, int /*role*/) const
{
	QSharedPointer<PresetData> presetData = m_model.at(index.row());
	return presetData->title();
}

QHash<int, QByteArray> PresetListModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[Qt::UserRole + 1] = "modelData";
	return roles;
}

void PresetListModel::add(QSharedPointer<PresetData> presetData)
{
	beginInsertRows(QModelIndex(), m_model.size(), m_model.size());
	m_model.push_back(presetData);
	presetData->setPresetListModel(this);
	endInsertRows();
}

void PresetListModel::update(PresetData *data)
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

void PresetListModel::resetModel(void)
{
	beginResetModel();
	endResetModel();
}

void PresetListModel::clear(void)
{
	beginResetModel();
	m_model.clear();
	endResetModel();
}

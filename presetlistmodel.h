#ifndef COMBOBOXLISTMODEL_H
#define COMBOBOXLISTMODEL_H

#include <QAbstractListModel>
#include <QSharedPointer>
#include "presetdata.h"

class PresetListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit PresetListModel(QObject *parent = 0);

	void add(QSharedPointer<PresetData> presetData);
	void update(PresetData *data);
	QSharedPointer<PresetData> at(int index) { return m_model.at(index); }
	void resetModel(void);
	void clear(void);
	int count(void) { return m_model.size(); }

protected:
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;

signals:

public slots:

private:
	QVector<QSharedPointer<PresetData> > m_model;
};

#endif // COMBOBOXLISTMODEL_H

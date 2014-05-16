#ifndef INPUTMETHODLISTMODEL_H
#define INPUTMETHODLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QSharedPointer>
#include "inputaction.h"

class PresetData;

class InputMethodListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit InputMethodListModel(PresetData *preset);

	void add(QSharedPointer<InputAction> action);
	void remove(int index);
	QSharedPointer<InputAction> find(InputAction *rawPointer);
	QSharedPointer<InputAction> at(int index) { return m_inputMethods.at(index); }
	Q_INVOKABLE void clear(void);

	int currentIndex(void) { return m_currentActionIndex; }
	Q_INVOKABLE int count(void) { return m_inputMethods.size(); }
	void stop(void);
	void start(int fromIndex = -1);
	void doNextAction(void);
	bool running(void) { return m_running; }
	void setRepeat(bool repeat) { m_repeat = repeat; }

protected:
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;

signals:

public slots:

private:
	QVector<QSharedPointer<InputAction> > m_inputMethods;
	int m_currentActionIndex;

	bool m_running;
	bool m_repeat;

	PresetData *m_preset;
};

#endif // INPUTMETHODLISTMODEL_H

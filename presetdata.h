#ifndef PRESETDATA_H
#define PRESETDATA_H

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include "inputmethodlistmodel.h"

class PresetListModel;

class PresetData : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
	Q_PROPERTY(bool recording READ recording WRITE setRecording NOTIFY recordingChanged)
	Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
	Q_PROPERTY(InputMethodListModel * model READ model NOTIFY modelChanged)

public:
	explicit PresetData(PresetListModel *model = 0);
	Q_INVOKABLE void addAction(InputAction *newAction);
	Q_INVOKABLE void removeAction(int index);
	Q_INVOKABLE void clear(void);
	Q_INVOKABLE int count(void) { return m_model.count(); }

	InputMethodListModel *model(void) { return &m_model; }

	void addAction(const QKeyEvent *event, qint64 delayMsec = -1);
	void addAction(const QMouseEvent *event, qint64 delayMsec = -1);

	int currentIndex(void) { return m_model.currentIndex(); }

	bool running(void) { return m_model.running(); }
	void setRunning(bool running);

	bool recording(void) { return m_recording; }
	void setRecording(bool recording);

	void setTitle(const QString &title);
	const QString & title(void) { return m_title; }

	void setPresetListModel(PresetListModel *model) { m_presetModel = model; }

private:
	qint64 __calculateDelayMsec(qint64 delayMsec);
	void __addAction(QSharedPointer<InputAction> newAction);
	void updatePresetModel(void);

signals:
	void titleChanged(void);
	void modelChanged(void);
	void runningChanged(void);
	void currentIndexChanged(void);
	void recordingChanged(void);

private:
	PresetListModel *m_presetModel;
	InputMethodListModel m_model;

	QString m_title;
	bool m_recording;

	qint64 m_prevDelayMsec;
};

#endif // PRESETDATA_H

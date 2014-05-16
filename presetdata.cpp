#include "presetdata.h"
#include <QQmlEngine>
#include <QDateTime>
#include "Lazybones.h"
#include "presetlistmodel.h"

PresetData::PresetData(PresetListModel *model) : QObject(NULL)
  , m_presetModel(model), m_model(this), m_recording(false), m_prevDelayMsec(-1)
{
	QQmlEngine::setObjectOwnership(&m_model, QQmlEngine::CppOwnership);
}

void PresetData::addAction(InputAction *newAction)
{
	QSharedPointer<InputAction> inputAction =
			QSharedPointer<InputAction>(
				new InputAction(
					(InputAction::ActionType)newAction->actionType(),
					newAction->keyCode(),
					newAction->mouseX(),
					newAction->mouseY(),
					newAction->delayedMsec()));

	__addAction(inputAction);
}

void PresetData::removeAction(int index)
{
	m_model.remove(index);
}

void PresetData::setTitle(const QString &title)
{
	m_title = title;
	emit titleChanged();
	updatePresetModel();
}

void PresetData::updatePresetModel(void)
{
	if(m_presetModel == NULL)
		return;

	m_presetModel->update(this);
}

void PresetData::setRunning(bool running)
{
	if(running)
	{
		m_model.start();
	}
	else
	{
		m_model.stop();
	}
	emit runningChanged();
}

void PresetData::clear(void)
{
	m_model.clear();
	emit currentIndexChanged();
}

void PresetData::addAction(const QKeyEvent *event, qint64 delayMsec)
{
	QSharedPointer<InputAction> inputAction = InputAction::fromKeyEvent(event, __calculateDelayMsec(delayMsec));
	if(inputAction)
		__addAction(inputAction);
}

void PresetData::addAction(const QMouseEvent *event, qint64 delayMsec)
{
	QSharedPointer<InputAction> inputAction = InputAction::fromMouseEvent(event, __calculateDelayMsec(delayMsec));
	if(inputAction)
		__addAction(inputAction);
}

void PresetData::__addAction(QSharedPointer<InputAction> newAction)
{
	m_model.add(newAction);
}

void PresetData::setRecording(bool recording)
{
	m_recording = recording;
	m_prevDelayMsec = QDateTime::currentMSecsSinceEpoch();
	emit recordingChanged();
	if(recording)
		Lazybones::inputEventManager()->startFiltering(InputEventManager::AllHook);
	else
		Lazybones::inputEventManager()->stopFiltering(InputEventManager::MouseHook);
}


qint64 PresetData::__calculateDelayMsec(qint64 delayMsec)
{
	if(delayMsec > 0)
	{
		m_prevDelayMsec = QDateTime::currentMSecsSinceEpoch();
		return delayMsec;
	}

	qint64 curr = QDateTime::currentMSecsSinceEpoch();

	delayMsec = curr - m_prevDelayMsec;
	m_prevDelayMsec = curr;
	return delayMsec;
}

#include "macroaction.h"
#include "macroactionlistmodel.h"
#include "inputaction.h"
#include "Lazybones.h"
#include <QQmlEngine>
#include <QThread>
#include "Logger.h"

#define kDefaultLegendaryColorOffset 3

MacroAction::MacroAction(MacroAction *action) : QObject(0)
  , m_depth(0), m_model(NULL), m_running(false), m_recording(false), m_actionType(MacroAction::None), m_sleepMsec(0)
  , m_inputActionChunk(NULL), m_prevDelayMsec(-1), m_startActionMsec(-1)
  , m_gotoIndex(-1)
  , m_active(false), m_randomSleep(false), m_currentSleepMsec(-1)
{
	copy(action);
	QQmlEngine::setObjectOwnership(&m_inputAction, QQmlEngine::CppOwnership);
	connect(&m_inputAction, SIGNAL(descriptionChanged()), this, SLOT(onInputActionDescriptionChanged()));

	m_inputActionChunk.setRepeat(false);
}

void MacroAction::init(void)
{
	m_startActionMsec = -1;
	m_currentSleepMsec = -1;
	m_currentSleepMsec = realSleepTime();

	emit descriptionChanged();
}

void MacroAction::copy(MacroAction *action)
{
	if(action == NULL)
		return;
	m_actionType = (MacroAction::ActionType)action->actionType();
	m_comment = action->comment();
	m_conditionScreenName = action->conditionScreen();
	m_inputAction.copy(action->inputAction());
	m_sleepMsec = action->sleepMsec();
	m_gotoIndex = action->gotoIndex();
	m_randomSleep = action->randomSleep();
	for(int i = 0; i < action->inputActionChunk()->count(); i++)
	{
		addInputActionChunk(action->inputActionChunk()->at(i).data());
	}
}

void MacroAction::setActive(bool active)
{
	if(active)
	{
		connect(Lazybones::inputEventManager(), SIGNAL(keyEvent(QKeyEvent)), this, SLOT(onKeyEvent(QKeyEvent)));
		connect(Lazybones::inputEventManager(), SIGNAL(mouseEvent(QMouseEvent)), this, SLOT(onMouseEvent(QMouseEvent)));
	}
	else
	{
		disconnect(Lazybones::inputEventManager(), SIGNAL(keyEvent(QKeyEvent)), this, SLOT(onKeyEvent(QKeyEvent)));
		disconnect(Lazybones::inputEventManager(), SIGNAL(mouseEvent(QMouseEvent)), this, SLOT(onMouseEvent(QMouseEvent)));
	}
	m_active = active;
	emit activeChanged();
}

MacroPresetData *MacroAction::presetData(void)
{
	if(m_model)
		return m_model->presetData();
	return NULL;
}

int MacroAction::realSleepTime(void)
{
	if(m_currentSleepMsec > 0)
		return m_currentSleepMsec;

	if(m_randomSleep)
		return Lazybones::randInt(m_sleepMsec, m_sleepMsec * 2);
	return m_sleepMsec;
}

bool MacroAction::doit()
{
	if(m_actionType != MacroAction::Sleep
			&& m_actionType != MacroAction::LoopIfNotElapsedTime
			&& m_actionType != MacroAction::WaitScreen)
	{
		if(m_sleepMsec > 0)
		{
			QThread::msleep(realSleepTime());
		}
	}

	switch(m_actionType)
	{
	case MacroAction::None:
	case MacroAction::LoopEnd:
	case MacroAction::IfEnd:
	case MacroAction::Else:
		return true;
	case MacroAction::DoInputAction:
		doInputAction();
		return true;
	case MacroAction::DoInputActionChunk:
	{
		doInputActionChunk();
		return true;
	}
	case MacroAction::Sleep:
		QThread::msleep(realSleepTime());
		return true;
	case MacroAction::LoopIfNotElapsedTime:
		if(m_startActionMsec < 0)
		{
			m_startActionMsec = QDateTime::currentMSecsSinceEpoch();
		}
		if(QDateTime::currentMSecsSinceEpoch() - m_startActionMsec >= m_sleepMsec )
		{
			// time over
			m_startActionMsec = -1; // init
			return false;
		}
		else
		{
			// not yet..
			return true;
		}
	case MacroAction::IfStart:
	case MacroAction::LoopIfStart:
		return Lazybones::gameScreenManager()->isCurrentScreen(m_conditionScreenName, true);
	case MacroAction::IfNotStart:
	case MacroAction::LoopIfNotStart:
		return !Lazybones::gameScreenManager()->isCurrentScreen(m_conditionScreenName, true);
	case MacroAction::WaitScreen:
	{
		qint64 prev = QDateTime::currentMSecsSinceEpoch();
		while(presetData()->running())
		{
			if(Lazybones::gameScreenManager()->isCurrentScreen(m_conditionScreenName, true))
				return true;

			if(m_sleepMsec > 0 && ((QDateTime::currentMSecsSinceEpoch() - prev) > m_sleepMsec))
			{
				break;
			}
			QThread::msleep(100);
		}
		break;
	}
	case MacroAction::Goto:
		presetData()->setCurrentIndex(m_gotoIndex);
		break;
	case MacroAction::FindLegendaryItem:
		doFindLegendaryItem();
		break;
	case MacroAction::DisassembleAllItemWithoutLegendary:
		doDisassembleAllItemWithoutLegendary();
		break;
	case MacroAction::LoopWaitScreenDoInputActionEnd:
		doLoopWaitScreenDoInputActionEnd();
		break;
	}

	return true;
}

void MacroAction::setRunning(bool running)
{
	m_running = running;
	emit runningChanged();
}

QString MacroAction::description(void)
{
	QString desc;
	QString name;
	bool needScreenName = false;

	switch(m_actionType)
	{
	case MacroAction::None:					name = "None";					break;
	case MacroAction::DoInputAction:		name = "DoInputAction";			break;
	case MacroAction::DoInputActionChunk:	name = "DoInputActionChunk";	break;
	case MacroAction::LoopIfNotElapsedTime:	name = "LoopIfNotElapsedTime";	break;
	case MacroAction::LoopIfStart:			name = "LoopIfStart";			needScreenName = true; break;
	case MacroAction::LoopIfNotStart:		name = "LoopIfNotStart";		needScreenName = true; break;
	case MacroAction::LoopEnd:				name = "LoopEnd";				break;
	case MacroAction::IfStart:				name = "IfStart";				needScreenName = true; break;
	case MacroAction::IfNotStart:			name = "IfNotStart";			needScreenName = true; break;
	case MacroAction::IfEnd:				name = "IfEnd";					break;
	case MacroAction::Else:					name = "Else";					break;
	case MacroAction::WaitScreen:			name = "WaitScreen";			needScreenName = true; break;
	case MacroAction::Sleep:				name = "Sleep";					break;
	case MacroAction::Goto:					name = "Goto";					break;
	case MacroAction::FindLegendaryItem:	name = "FindLegendaryItem";		break;
	case MacroAction::DisassembleAllItemWithoutLegendary:	name = "DisassembleAllItemWithoutLegendary";	break;
	case MacroAction::LoopWaitScreenDoInputActionEnd: name = "LoopWaitScreenDoInputActionEnd"; needScreenName = true; break;
	}

	int step = 0;
	desc += name;
	desc += " ";

	if(m_actionType == MacroAction::IfEnd
			|| m_actionType == MacroAction::Else
			|| m_actionType == MacroAction::FindLegendaryItem
			|| m_actionType == MacroAction::DisassembleAllItemWithoutLegendary
			|| m_actionType == MacroAction::LoopEnd)
	{
		// No info
		return desc;
	}

	if(needScreenName)
	{
		if(step++ > 0) desc += ", ";
		desc += m_conditionScreenName;
	}

	if(m_actionType == MacroAction::DoInputAction || m_actionType == MacroAction::LoopWaitScreenDoInputActionEnd)
	{
		if(step++ > 0) desc += ", ";
		desc += m_inputAction.description();
	}

	if(m_actionType == MacroAction::DoInputActionChunk)
	{
		if(step++ > 0) desc += ", ";
		desc += "<CHUNK> " + (m_inputActionChunk.count() > 0 ? (m_inputActionChunk.at(0)->description()) : "EMPTY");
	}

	if(m_sleepMsec > 0)
	{
		if(step++ > 0) desc += ", ";
		desc += QString::number(m_sleepMsec) + "msec";
		if(m_randomSleep)
		{
			desc += " Random : " + QString::number(m_currentSleepMsec) + "msec";
		}
	}

	if(m_actionType == MacroAction::Goto)
	{
		if(step++ > 0) desc += ", ";
		desc += "Macro Index " + QString::number(m_gotoIndex);
	}

	if(!m_comment.isEmpty())
	{
		desc += " : " + m_comment;
	}

	return desc;
}

void MacroAction::addInputActionChunk(InputAction* newAction)
{
	QSharedPointer<InputAction> inputAction =
			QSharedPointer<InputAction>(
				new InputAction(
					(InputAction::ActionType)newAction->actionType(),
					newAction->keyCode(),
					newAction->mouseX(),
					newAction->mouseY(),
					newAction->delayedMsec()));

	m_inputActionChunk.add(inputAction);
}

void MacroAction::setRecording(bool recording)
{
	m_recording = recording;
	m_prevDelayMsec = QDateTime::currentMSecsSinceEpoch();
	emit recordingChanged();
	if(recording)
		Lazybones::inputEventManager()->startFiltering(InputEventManager::AllHook);
	else
		Lazybones::inputEventManager()->stopFiltering(InputEventManager::MouseHook);
}

void MacroAction::test(void)
{
	switch(m_actionType)
	{
	case MacroAction::DoInputActionChunk:
		doInputActionChunk(true);
		break;
	case MacroAction::DoInputAction:
		doInputAction(true);
		break;
	case MacroAction::FindLegendaryItem:
		doFindLegendaryItem();
		break;
	case MacroAction::DisassembleAllItemWithoutLegendary:
		doDisassembleAllItemWithoutLegendary();
		break;
	case MacroAction::LoopWaitScreenDoInputActionEnd:
		doLoopWaitScreenDoInputActionEnd(true);
		break;
	default:
		break;
	}
}

qint64 MacroAction::__calculateDelayMsec(qint64 delayMsec)
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

void MacroAction::doInputAction(bool testMode)
{
	if(testMode)
	{
		m_inputAction.test();
		return;
	}
	if(m_inputAction.delayedMsec() > 0)
		QThread::msleep(m_inputAction.delayedMsec());
	m_inputAction.doitNow();
}

void MacroAction::doInputActionChunk(bool testMode)
{
	if(testMode)
	{
		m_startActionMsec = -1;
		m_inputActionChunk.setRepeat(false);
		m_inputActionChunk.start();
		return;
	}

	for(int i = 0; i < m_inputActionChunk.count(); i++)
	{
		if((!testMode && !presetData()->running()) || Lazybones::isEmergencyStop())
			break;

		QSharedPointer<InputAction> action = m_inputActionChunk.at(i);
		QThread::msleep(qMax(10, action->delayedMsec()));
		action->doitNow();
	}
}

void MacroAction::doLoopWaitScreenDoInputActionEnd(bool testMode)
{
	while((testMode || presetData()->running()) && !Lazybones::isEmergencyStop())
	{
		if(Lazybones::gameScreenManager()->isCurrentScreen(m_conditionScreenName, true))
			return;

		doInputAction();

		QThread::msleep(800);
	}
}

void MacroAction::doDisassembleAllItemWithoutLegendary(void)
{
	const int ITEM_WIDTH = 27;

	QString screenName = GameScreenData::InventoryItemYellowName;
	QSharedPointer<GameScreenData> screenData = Lazybones::gameScreenManager()->findScreenData(screenName);
	if(!screenData)
		return;

	int x = screenData->pixelX();
	int y = screenData->pixelY();
	for(int i = 0; i < 10; i++)
	{
		QColor clr = Lazybones::getPixelColor(x, y, false);
		qDebug() << clr.name();

		Lazybones::inputEventManager()->worker()->doMouseMove(x, y);
		x += ITEM_WIDTH;
		QThread::msleep(200);
	}
	// TODO
}

void MacroAction::doFindLegendaryItem(void)
{
	QRect windowRect = presetData()->windowRect();
	int winW = windowRect.width();
	int winH = windowRect.height();

	Lazybones::inputEventManager()->worker()->doMouseLeftClick(winW/2, winH/2);
	QThread::msleep(300);
	Lazybones::inputEventManager()->worker()->doKeyboardInput(Qt::Key_Alt);
	QThread::msleep(300);

	for(int i = 0; i < 6; i++)
	{
		int checked = 0;
		QPoint pos = Lazybones::gameScreenManager()->findPixel("#ff8003"
																 , kDefaultLegendaryColorOffset, kDefaultLegendaryColorOffset, kDefaultLegendaryColorOffset
																 , QPoint(windowRect.x() + winW/2, windowRect.y() + winH/2), QSize(winW, winH));
		LOG_INFO() << "FIND ITEM LEGENDARY RESULT : " << pos.x() << "x" << pos.y();
		if(!pos.isNull())
		{
			Lazybones::inputEventManager()->worker()->doMouseLeftClick(pos.x(), pos.y());
			presetData()->increaseLegendaryCount();
			SETTING_MANAGER->dropBox()->doScreenShot("Legendary_", false, false);
		}
		else
			checked++;
		QThread::msleep(1000);

		pos = Lazybones::gameScreenManager()->findPixel("#00ff00"
														  , kDefaultLegendaryColorOffset, kDefaultLegendaryColorOffset, kDefaultLegendaryColorOffset
														  , QPoint(winW/2, winH/2), QSize(winW, winH));
		LOG_INFO() << "FIND ITEM SET RESULT : " << pos.x() << "x" << pos.y();
		if(!pos.isNull())
		{
			Lazybones::inputEventManager()->worker()->doMouseLeftClick(pos.x(), pos.y());
			presetData()->increaseLegendaryCount();
			SETTING_MANAGER->dropBox()->doScreenShot("Legendary_", false, false);
		}
		else
			checked++;

		if(checked == 2)
			break;
	}
}


void MacroAction::onKeyEvent(QKeyEvent event)
{
	if(recording())
	{
		if(event.key() == Qt::Key_F8 || event.key() == Qt::Key_F10 || event.key() == Qt::Key_F12 || event.key() == Qt::Key_Pause)
		{
			setRecording(false);
			qDebug() << "EMERGENCY RECORD STOP";
		}
		else
		{
			qDebug() << "Add KeyEvent" << event.key() << recording();

			QSharedPointer<InputAction> inputAction = InputAction::fromKeyEvent(&event, __calculateDelayMsec(-1));
			m_inputActionChunk.add(inputAction);
		}
	}
	else
	{
		if(event.key() == Qt::Key_F7)
		{
			setRecording(true);
			qDebug() << "EMERGENCY RECORD START";
		}
	}
}

void MacroAction::onMouseEvent(QMouseEvent event)
{
	if(!recording())
		return;
	qDebug() << "MouseEvent" << m_actionType<< event.pos() << event.button();
	QSharedPointer<InputAction> inputAction = InputAction::fromMouseEvent(&event, __calculateDelayMsec(-1));
	m_inputActionChunk.add(inputAction);
}


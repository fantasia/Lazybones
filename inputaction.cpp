#include "inputaction.h"
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include "inputmethodlistmodel.h"
#include "Lazybones.h"

InputAction::InputAction(QObject *parent) :
	QObject(parent), m_action(InputAction::None), m_key(-1), m_xPos(-1), m_yPos(-1), m_delayedMsec(1000), m_running(false), m_model(NULL)
{
	__initTimer();
}

InputAction::InputAction(InputAction::ActionType action, int keyCode, int xPos, int yPos, int delayed)
	: QObject(NULL), m_action(action), m_key(keyCode), m_xPos(xPos), m_yPos(yPos), m_delayedMsec(delayed), m_running(false), m_model(NULL)
{
	__initTimer();
}

InputAction::~InputAction(void)
{
	stop();
}

void InputAction::copy(InputAction *action)
{
	if(action == NULL)
		return;

	m_action = (InputAction::ActionType)action->actionType();
	m_key = action->keyCode();
	m_xPos = action->mouseX();
	m_yPos = action->mouseY();
	m_delayedMsec = action->delayedMsec();
}

void InputAction::__perform(void)
{
	//qDebug() << "InputAction action : " << m_action << m_key << m_xPos << m_yPos << m_delayedMsec;

	switch(m_action)
	{
	case InputAction::MouseLeftClick:
		Lazybones::inputEventManager()->worker()->doMouseLeftClick(m_xPos, m_yPos);
		break;
	case InputAction::MouseLeftPress:
		Lazybones::inputEventManager()->worker()->doMouseLeftPress(m_xPos, m_yPos);
		break;
	case InputAction::MouseLeftRelease:
		Lazybones::inputEventManager()->worker()->doMouseLeftRelease(m_xPos, m_yPos);
		break;
	case InputAction::MouseRightClick:
		Lazybones::inputEventManager()->worker()->doMouseRightClick(m_xPos, m_yPos);
		break;
	case InputAction::MouseRightPress:
		Lazybones::inputEventManager()->worker()->doMouseRightPress(m_xPos, m_yPos);
		break;
	case InputAction::MouseRightRelease:
		Lazybones::inputEventManager()->worker()->doMouseRightRelease(m_xPos, m_yPos);
		break;
	case InputAction::MouseMove:
		Lazybones::inputEventManager()->worker()->doMouseMove(m_xPos, m_yPos);
		break;
	case InputAction::KeyPress:
		if(m_xPos >= 0 && m_yPos >= 0)
			Lazybones::inputEventManager()->worker()->doMouseMove(m_xPos, m_yPos);
		Lazybones::inputEventManager()->worker()->doKeyboardInput((Qt::Key)m_key);
		break;
	default:
		break;
	}
}

QString InputAction::description(void)
{
	QString str;
	switch(actionType())
	{
	case InputAction::KeyPress:
		if(m_xPos >= 0 && m_yPos >= 0)
			str = "Mouse Move & Key Press : " + keyName() + ", " + QString::number(m_xPos) + "x" + QString::number(m_yPos) + ", ";
		else
			str = "Key Press : " + keyName() + ", ";
		break;
	case InputAction::MouseLeftClick:
		str = "Mouse Left Click : " + QString::number(m_xPos) + "x" + QString::number(m_yPos) + ", ";
		break;
	case InputAction::MouseLeftPress:
		str = "Mouse Left Press : " + QString::number(m_xPos) + "x" + QString::number(m_yPos) + ", ";
		break;
	case InputAction::MouseLeftRelease:
		str = "Mouse Left Release : " + QString::number(m_xPos) + "x" + QString::number(m_yPos) + ", ";
		break;
	case InputAction::MouseRightClick:
		str = "Mouse Right Click : " + QString::number(m_xPos) + "x" + QString::number(m_yPos) + ", ";
		break;
	case InputAction::MouseRightPress:
		str = "Mouse Right Press : " + QString::number(m_xPos) + "x" + QString::number(m_yPos) + ", ";
		break;
	case InputAction::MouseRightRelease:
		str = "Mouse Right Release : " + QString::number(m_xPos) + "x" + QString::number(m_yPos) + ", ";
		break;
	case InputAction::MouseMove:
		str = "Mouse Move : " + QString::number(m_xPos) + "x" + QString::number(m_yPos) + ", ";
		break;
	default:
		str = "None : ";
		break;
	}

	str += ("Delay : " + QString::number(m_delayedMsec) + "msec");
	return str;
}

void InputAction::stop(void)
{
	m_timer.stop();
	setRunning(false);
}

void InputAction::test(void)
{
	m_doNextAction = false;

	__doit();
}

void InputAction::doit(void)
{
	m_doNextAction = true;

	__doit();
}

void InputAction::doitNow(void)
{
	__perform();
}

void InputAction::__doit(void)
{
	setRunning(true);

	if(m_delayedMsec > 10)
	{
		m_timer.setInterval(m_delayedMsec);
		m_timer.start();
	}
	else
	{
		onTriggered();
	}
}

void InputAction::setActionType(int actionType)
{
	m_action = (InputAction::ActionType)actionType;
	emit actionTypeChanged();
	emit descriptionChanged();
}

void InputAction::setKeyCode(int keyCode)
{
	m_key = keyCode;
	emit keyCodeChanged();
	emit descriptionChanged();
}

void InputAction::setMouseX(int mouseX)
{
	m_xPos = mouseX;
	emit mouseXChanged();
	emit descriptionChanged();
}

void InputAction::setMouseY(int mouseY)
{
	m_yPos = mouseY;
	emit mouseYChanged();
	emit descriptionChanged();
}

void InputAction::setDelayedMsec(int delayedMsec)
{
	m_delayedMsec = delayedMsec;
	emit delayedMsecChanged();
	emit descriptionChanged();
}

bool InputAction::running(void)
{
	return m_running;
}

void InputAction::setRunning(bool running)
{
	m_running = running;
	emit runningChanged();
}

void InputAction::onTriggered(void)
{
	__perform();

	setRunning(false);
	if(m_doNextAction)
	{
		m_doNextAction = false;
		if(m_model)
		{
			m_model->doNextAction();
		}
	}
}

void InputAction::setModel(InputMethodListModel *model)
{
	m_model = model;
}

QSharedPointer<InputAction> InputAction::fromKeyEvent(const QKeyEvent *event, qint64 delayMsec)
{
	QSharedPointer<InputAction> inputAction =
			QSharedPointer<InputAction>(
				new InputAction(
					InputAction::KeyPress,
					event->key(),
					-1,
					-1,
					delayMsec));
	return inputAction;
}

QSharedPointer<InputAction> InputAction::fromMouseEvent(const QMouseEvent *event, qint64 delayMsec)
{
	InputAction::ActionType type = InputAction::None;
	if(event->button() == Qt::LeftButton)
	{
		if(event->type() == QEvent::MouseButtonPress)
			type = InputAction::MouseLeftPress;
		if(event->type() == QEvent::MouseButtonRelease)
			type = InputAction::MouseLeftRelease;
	}
	else if(event->button() == Qt::RightButton)
	{
		if(event->type() == QEvent::MouseButtonPress)
			type = InputAction::MouseRightPress;
		if(event->type() == QEvent::MouseButtonRelease)
			type = InputAction::MouseRightRelease;
	}

	if(event->type() == QEvent::MouseMove)
	{
		if(event->type() == QEvent::MouseMove)
			type = InputAction::MouseMove;
	}

	if(type == InputAction::None)
		return QSharedPointer<InputAction>(); // this mouse event not supported

	QPoint pos = event->pos();

	QSharedPointer<InputAction> inputAction =
			QSharedPointer<InputAction>(
				new InputAction(
					type,
					0,
					pos.x(),
					pos.y(),
					delayMsec));
	return inputAction;
}



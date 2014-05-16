#ifndef INPUTACTION_H
#define INPUTACTION_H

#include <QObject>
#include <QTimer>
#include <QKeySequence>
#include <QKeyEvent>
#include <QMouseEvent>

class InputMethodListModel;

class InputAction : public QObject
{
	Q_OBJECT
	Q_ENUMS(ActionType)

	Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
	Q_PROPERTY(int actionType READ actionType WRITE setActionType NOTIFY actionTypeChanged)
	Q_PROPERTY(int keyCode READ keyCode WRITE setKeyCode NOTIFY keyCodeChanged)
	Q_PROPERTY(int mouseX READ mouseX WRITE setMouseX NOTIFY mouseXChanged)
	Q_PROPERTY(int mouseY READ mouseY WRITE setMouseY NOTIFY mouseYChanged)
	Q_PROPERTY(int delayedMsec READ delayedMsec WRITE setDelayedMsec NOTIFY delayedMsecChanged)
	Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
	enum ActionType {
		None,
		MouseLeftClick,
		MouseLeftPress,
		MouseLeftRelease,
		MouseRightClick,
		MouseRightPress,
		MouseRightRelease,
		MouseMove,
		KeyPress,
	};

	explicit InputAction(QObject *parent = 0);	
	InputAction(ActionType action, int keyCode, int xPos, int yPos, int delayed);
	virtual ~InputAction(void);

	void doit(void);
	void doitNow(void);
	Q_INVOKABLE void test(void);
	Q_INVOKABLE void stop(void);
	Q_INVOKABLE QString keyName()
	{
		QKeySequence seq(m_key);
		return seq.toString();
	}

	int actionType(void) { return (int)m_action; }
	void setActionType(int actionType);

	int keyCode(void) { return m_key; }
	void setKeyCode(int keyCode);

	int mouseX(void) { return m_xPos; }
	void setMouseX(int mouseX);

	int mouseY(void) { return m_yPos; }
	void setMouseY(int mouseY);

	int delayedMsec(void) { return m_delayedMsec; }
	void setDelayedMsec(int delayedMsec);

	bool running(void);
	void setRunning(bool running);

	void setModel(InputMethodListModel *model);

	QString description(void);

	Q_INVOKABLE void copy(InputAction *action);

	static QSharedPointer<InputAction> fromKeyEvent(const QKeyEvent *event, qint64 delayMsec);
	static QSharedPointer<InputAction> fromMouseEvent(const QMouseEvent *event, qint64 delayMsec);

private:
	qint32 nativeVirtualKey(Qt::Key key);
	void __initTimer(void)
	{
		m_timer.setSingleShot(true);
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTriggered()));
	}

	void __perform(void);
	void __doit(void);

signals:
	void actionTypeChanged(void);
	void keyCodeChanged(void);
	void mouseXChanged(void);
	void mouseYChanged(void);
	void delayedMsecChanged(void);
	void runningChanged(void);
	void descriptionChanged(void);

public slots:
	void onTriggered(void);

private:
	friend class InputMethodListModel;
	ActionType m_action;
	int m_key;
	int m_xPos;
	int m_yPos;
	int m_delayedMsec;
	bool m_running;
	bool m_doNextAction;

	QTimer m_timer;
	InputMethodListModel *m_model;
};

#endif // INPUTACTION_H

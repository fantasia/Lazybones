#ifndef MACROACTION_H
#define MACROACTION_H

#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include <QKeyEvent>
#include <QMouseEvent>
#include "inputmethodlistmodel.h"

class MacroActionListModel;
class MacroPresetData;

class MacroAction : public QObject
{
	Q_OBJECT
	Q_ENUMS(ActionType)
	Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
	Q_PROPERTY(QString conditionScreen READ conditionScreen WRITE setConditionScreen NOTIFY conditionScreenChanged)
	Q_PROPERTY(int actionType READ actionType WRITE setActionType NOTIFY actionTypeChanged)
	Q_PROPERTY(InputAction* inputAction READ inputAction NOTIFY inputActionChanged)
	Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged)
	Q_PROPERTY(int sleepMsec READ sleepMsec WRITE setSleepMsec NOTIFY sleepMsecChanged)
	Q_PROPERTY(int gotoIndex READ gotoIndex WRITE setGotoIndex NOTIFY gotoIndexChanged)
	Q_PROPERTY(bool running READ running NOTIFY runningChanged)
	Q_PROPERTY(bool recording READ recording WRITE setRecording NOTIFY recordingChanged)
	Q_PROPERTY(bool randomSleep READ randomSleep WRITE setRandomSleep NOTIFY randomSleepChanged)
	Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
	Q_PROPERTY(InputMethodListModel * inputActionChunk READ inputActionChunk NOTIFY inputActionChunkChanged)

public:
	enum ActionType {
		None,
		DoInputAction,
		LoopIfNotElapsedTime,
		LoopIfStart,
		LoopIfNotStart,
		LoopEnd,
		IfStart,
		IfNotStart,
		IfEnd,
		WaitScreen,
		Sleep,
		DoInputActionChunk,
		FindLegendaryItem,
		Else,
		Goto,
		DisassembleAllItemWithoutLegendary,
		LoopWaitScreenDoInputActionEnd,
	};

	explicit MacroAction(MacroAction *action = 0);

	bool doit();

	Q_INVOKABLE bool blockStartType(void)
	{
		return blockLoopStartType()
				|| blockIfType();
	}

	Q_INVOKABLE bool blockLoopStartType(void)
	{
		return m_actionType == LoopIfStart
				|| m_actionType == LoopIfNotElapsedTime
				|| m_actionType == LoopIfNotStart;
	}

	Q_INVOKABLE bool blockLoopEndType(void)
	{
		return m_actionType == LoopEnd;
	}

	Q_INVOKABLE bool blockEndType(void)
	{
		return m_actionType == LoopEnd
				|| m_actionType == IfEnd;
	}

	Q_INVOKABLE bool blockIfType(void)
	{
		return checkBlockIfType(m_actionType);
	}

	static bool checkBlockIfType(ActionType actionType)
	{
		return actionType == IfNotStart
				|| actionType == IfStart;
	}

	MacroPresetData *presetData(void);

	int syntaxDepth(void) { return m_depth; }
	void setSyntaxDepth(int depth) { m_depth = depth; emit descriptionChanged(); }

	void setModel(MacroActionListModel *model) { m_model = model; }

	int actionType(void) { return m_actionType; }
	void setActionType(int type) { m_actionType = (ActionType)type; emit actionTypeChanged(); emit descriptionChanged(); }

	const QString &conditionScreen(void) { return m_conditionScreenName; }
	void setConditionScreen(QString name) { m_conditionScreenName = name; emit conditionScreenChanged(); emit descriptionChanged(); }

	const QString &comment(void) { return m_comment; }
	void setComment(QString comment) { m_comment = comment; emit commentChanged(); emit descriptionChanged(); }

	int sleepMsec(void) { return m_sleepMsec; }
	void setSleepMsec(int msec) { m_sleepMsec = msec; emit sleepMsecChanged(); emit descriptionChanged(); }

	int gotoIndex(void) { return m_gotoIndex; }
	void setGotoIndex(int index) { m_gotoIndex = index; emit gotoIndexChanged(); emit descriptionChanged(); }

	bool running(void) { return m_running; }
	void setRunning(bool running);

	bool recording(void) { return m_recording; }
	void setRecording(bool recording);

	bool active(void) { return m_active; }
	void setActive(bool active);

	bool randomSleep(void) { return m_randomSleep; }
	void setRandomSleep(bool randomSleep) { m_randomSleep = randomSleep; emit randomSleepChanged(); }

	int realSleepTime(void);

	void init(void);

	Q_INVOKABLE void test(void);

	InputAction* inputAction(void)
	{
		return &m_inputAction;
	}
	InputMethodListModel *inputActionChunk(void) { return &m_inputActionChunk; }

	void addInputActionChunk(InputAction* inputAction);
	Q_INVOKABLE void removeInputActionChunkAt(int index)
	{
		m_inputActionChunk.remove(index);
	}

	QString description(void);

	void copy(MacroAction *action);

private:
	qint64 __calculateDelayMsec(qint64 delayMsec);
	void doFindLegendaryItem(void);
	void doDisassembleAllItemWithoutLegendary(void);
	void doLoopWaitScreenDoInputActionEnd(bool testMode = false);
	void doInputAction(bool testMode = false);
	void doInputActionChunk(bool testMode = false);

private slots:
	void onKeyEvent(QKeyEvent event);
	void onMouseEvent(QMouseEvent event);

signals:
	void conditionScreenChanged(void);
	void actionTypeChanged(void);
	void inputActionChanged(void);
	void commentChanged(void);
	void sleepMsecChanged(void);
	void gotoIndexChanged(void);
	void descriptionChanged(void);
	void runningChanged(void);
	void recordingChanged(void);
	void inputActionChunkChanged(void);
	void activeChanged(void);
	void randomSleepChanged(void);

public slots:
	void onInputActionDescriptionChanged(void)
	{
		emit descriptionChanged();
	}

private:
	int m_depth;
	MacroActionListModel *m_model;
	bool m_running;
	bool m_recording;
	ActionType m_actionType;
	QString m_conditionScreenName;
	int m_sleepMsec;
	QString m_comment;

	InputAction m_inputAction;
	InputMethodListModel m_inputActionChunk;
	qint64 m_prevDelayMsec;
	qint64 m_startActionMsec;
	int m_gotoIndex;
	bool m_active;
	bool m_randomSleep;
	int m_currentSleepMsec;
};

#endif // MACROACTION_H

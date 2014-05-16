#ifndef MACROPRESETDATA_H
#define MACROPRESETDATA_H

#include <QObject>
#include <QMutex>
#include <QRect>
#include "macroactionlistmodel.h"
#include <QTimer>

class MacroPresetListModel;
class MacroThreadWorker;

class MacroPresetData : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
	Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
	Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
	Q_PROPERTY(MacroActionListModel * model READ model NOTIFY modelChanged)

	Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle NOTIFY windowTitleChanged)
	Q_PROPERTY(QRect windowRect READ windowRect WRITE setWindowRect NOTIFY windowRectChanged)

	Q_PROPERTY(int statsLoopCount READ statsLoopCount NOTIFY statsLoopCountChanged)
	Q_PROPERTY(int statsHangCount READ statsHangCount NOTIFY statsHangCountChanged)
	Q_PROPERTY(int statsLegendaryCount READ statsLegendaryCount NOTIFY statsLegendaryCountChanged)

public:
	explicit MacroPresetData(MacroPresetListModel *model = 0);
	virtual ~MacroPresetData(void);

	Q_INVOKABLE void insertAction(int index, MacroAction *newAction);
	Q_INVOKABLE void addAction(MacroAction *newAction);
	Q_INVOKABLE void removeAction(int index);
	Q_INVOKABLE void moveAction(int index, MacroAction *action);
	Q_INVOKABLE void clear(void);
	Q_INVOKABLE int count(void) { return m_model.count(); }

	Q_INVOKABLE void applyWindowRect(void);

	MacroActionListModel *model(void) { return &m_model; }

	int currentIndex(void) { return m_currentIndex; }
	void setCurrentIndex(int index) { m_currentIndex = index; emit currentIndexChanged(); }

	bool running(void);
	void setRunning(bool running);

	bool repeat(void) { return m_repeat; }
	void setRepeat(bool repeat) { m_repeat = repeat; emit repeatChanged(); }

	void setTitle(const QString &title);
	const QString & title(void) { return m_title; }

	void setMacroPresetListModel(MacroPresetListModel *model) { m_presetModel = model; }

	const QRect & windowRect(void)
	{
		if(m_winRect.isEmpty())
			m_winRect.setRect(0, 0, 1024, 768);
		return m_winRect;
	}
	void setWindowRect(QRect rect) { m_winRect = rect; emit windowRectChanged(); }

	const QString & windowTitle(void) { return m_winTitle; }
	void setWindowTitle(QString title) { m_winTitle = title; emit windowTitleChanged(); }

	int statsLoopCount(void) { return m_statsLoopCount; }
	int statsHangCount(void) { return m_statsHangCount; }
	int statsLegendaryCount(void) { return m_statsLegendaryCount; }

	void lock(void) { m_mutex.lock(); }
	void unlock(void) { m_mutex.unlock(); }

	Q_INVOKABLE int verify(void);
	Q_INVOKABLE void clearStatistics(void)
	{
		m_mutex.lock();
		m_statsLoopCount = 0;
		m_statsHangCount = 0;
		m_statsLegendaryCount = 0;
		m_mutex.unlock();

		emit statsLoopCountChanged();
		emit statsHangCountChanged();
		emit statsLegendaryCountChanged();
	}

	void increaseLoopCount(void)
	{
		m_mutex.lock();
		m_statsLoopCount++;
		m_mutex.unlock();
		emit statsLoopCountChanged();
	}

	void increaseHangCount(void)
	{
		m_mutex.lock();
		m_statsHangCount++;
		m_mutex.unlock();
		emit statsHangCountChanged();
	}

	void increaseLegendaryCount(void)
	{
		m_mutex.lock();
		m_statsLegendaryCount++;
		m_mutex.unlock();
		emit statsLegendaryCountChanged();
	}

private:
	void __addAction(QSharedPointer<MacroAction> newAction);
	void __insertAction(int index, QSharedPointer<MacroAction> newAction);

	void updatePresetModel(void);
	void start(void);
	void stop(void);
	void goToMainScreen(void);

signals:
	void titleChanged(void);
	void modelChanged(void);
	void runningChanged(void);
	void currentIndexChanged(void);
	void windowRectChanged(void);
	void windowTitleChanged(void);
	void repeatChanged(void);

	void statsLoopCountChanged(void);
	void statsHangCountChanged(void);
	void statsLegendaryCountChanged(void);

private slots:
	void onHangupCheckTimer(void);

private:
	friend class MacroThreadWorker;
	QMutex m_mutex;
	MacroThreadWorker *m_threadWorker;
	MacroPresetListModel *m_presetModel;
	MacroActionListModel m_model;
	int m_currentIndex;

	QString m_title;
	QString m_winTitle;
	QRect m_winRect;
	bool m_repeat;
	QTimer m_hangupTimer;
	int m_maxIndex;
	int m_hangupTickCount;

	// statistics
	int m_statsLoopCount;
	int m_statsHangCount;
	int m_statsLegendaryCount;
};

#endif // MACROPRESETDATA_H

#ifndef AUTODIABLO3_H
#define AUTODIABLO3_H

#include <QObject>
#include <QQmlEngine>
#include <QStringListModel>
#include "settingmanager.h"
#include "gamescreenmanager.h"
#include "presetlistmodel.h"
#include "dropboxdata.h"
#include "macroaction.h"
#include "macropresetlistmodel.h"
#include <QDateTime>
#include <QDebug>
#include <QScreen>
#include "InputEvent/InputEventManager.h"

class Lazybones : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int currentPresetIndex READ currentPresetIndex WRITE setCurrentPresetIndex NOTIFY currentPresetIndexChanged)
	Q_PROPERTY(int currentMacroPresetIndex READ currentMacroPresetIndex WRITE setCurrentMacroPresetIndex NOTIFY currentMacroPresetIndexChanged)
	Q_PROPERTY(PresetListModel *presetList READ presetList NOTIFY presetListChanged)
	Q_PROPERTY(PresetData *currentPreset READ currentPreset NOTIFY currentPresetChanged)
	Q_PROPERTY(MacroPresetListModel *macroPresetList READ macroPresetList NOTIFY macroPresetListChanged)
	Q_PROPERTY(MacroPresetData *currentMacroPreset READ currentMacroPreset NOTIFY currentMacroPresetChanged)
	Q_PROPERTY(DropBoxData *dropBox READ dropBox NOTIFY dropBoxChanged())
	Q_PROPERTY(GameScreenManager *gameScreenManager READ gameScreenManagerPrivate NOTIFY gameScreenManagerChanged)

public:
	explicit Lazybones(QObject *parent = 0);

	static void initialize(void);

	Q_INVOKABLE InputAction * createInputAction(void)
	{
		InputAction *newAction = new InputAction();
		QQmlEngine::setObjectOwnership(newAction, QQmlEngine::JavaScriptOwnership);
		return newAction;
	}

	Q_INVOKABLE MacroAction * createMacroAction(void)
	{
		MacroAction *newAction = new MacroAction();
		QQmlEngine::setObjectOwnership(newAction, QQmlEngine::JavaScriptOwnership);
		return newAction;
	}

	Q_INVOKABLE GameScreenData * createGameScreenData(void)
	{
		GameScreenData *newData = new GameScreenData();
		QQmlEngine::setObjectOwnership(newData, QQmlEngine::JavaScriptOwnership);
		return newData;
	}

	Q_INVOKABLE qint64 currentMSecsSinceEpoch(void)
	{
		return QDateTime::currentMSecsSinceEpoch();
	}

	Q_INVOKABLE void addNewPreset(QString title)
	{
		QSharedPointer<PresetData> newData = QSharedPointer<PresetData>(new PresetData());
		newData->setTitle(title);
		presetList()->add(newData);
		qDebug() << "addNewPreset :" << title;
	}

	Q_INVOKABLE void addNewMacroPreset(QString title)
	{
		QSharedPointer<MacroPresetData> newData = QSharedPointer<MacroPresetData>(new MacroPresetData());
		newData->setTitle(title);
		macroPresetList()->add(newData);
		qDebug() << "addNewMacroPreset :" << title;
	}

	Q_INVOKABLE void save(void)
	{
		SETTING_MANAGER->save();
	}

	PresetListModel * presetList(void) { return SETTING_MANAGER->presetList(); }
	PresetData *currentPreset(void) { return m_currentPresetData.data(); }

	MacroPresetListModel * macroPresetList(void) { return SETTING_MANAGER->macroPresetList(); }
	MacroPresetData *currentMacroPreset(void) { return m_currentMacroPresetData.data(); }

	int currentPresetIndex(void) { return m_currentPresetIndex; }
	void setCurrentPresetIndex(int presetIndex);

	int currentMacroPresetIndex(void) { return m_currentMacroPresetIndex; }
	void setCurrentMacroPresetIndex(int presetIndex);

	GameScreenManager *gameScreenManagerPrivate(void) { return gameScreenManager(); }

	DropBoxData *dropBox(void) { return SETTING_MANAGER->dropBox(); }

	Q_INVOKABLE QColor pixelColor(int pixelX, int pixelY, bool useLastShot);

	static int randInt(int low, int high)
	{
		QTime time = QTime::currentTime();
		qsrand((uint)time.msec());

		// Random number between low and high
		return qrand() % ((high + 1) - low) + low;
	}

	static QPoint toRealPos(QImage *image, int imageX, int imageY);

	static QColor getPixelColor(int pixelX, int pixelY, bool useLastShot);
	static QColor getPixelColor(QScreen *screen, QImage *image, int pixelX, int pixelY);
	static InputEventManager *inputEventManager(void) { return s_inputEventManager; }
	static GameScreenManager *gameScreenManager(void) { return s_gameScreenManager; }
	static bool isEmergencyStop(void) { return s_forceStopFlag; }
	static void setEmergencyStop(bool stop) { s_forceStopFlag = stop; }

private slots:
	void onKeyEvent(QKeyEvent event);
	void onMouseEvent(QMouseEvent event);

signals:
	void currentMacroPresetIndexChanged(void);
	void currentPresetIndexChanged(void);
	void currentPresetChanged(void);
	void currentMacroPresetChanged(void);
	void presetListChanged(void);
	void macroPresetListChanged(void);
	void dropBoxChanged(void);
	void gameScreenManagerChanged(void);

public slots:
	void onPresetListChanged(void);
	void onMacroPresetListChanged(void);

private:
	bool m_running;

	QSharedPointer<PresetData> m_currentPresetData;
	QSharedPointer<MacroPresetData> m_currentMacroPresetData;
	int m_currentPresetIndex;
	int m_currentMacroPresetIndex;

	static bool s_forceStopFlag;
	static InputEventManager *s_inputEventManager;
	static GameScreenManager *s_gameScreenManager;
};

#endif // AUTODIABLO3_H

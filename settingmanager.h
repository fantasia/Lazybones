#ifndef SETTINGMANAGER_H
#define SETTINGMANAGER_H

#include <QObject>
#include <QString>
#include "presetlistmodel.h"
#include "macropresetlistmodel.h"
#include "gamescreenlistmodel.h"
#include "dropboxdata.h"

#ifndef Q_MOC_RUN
#include "Singleton.h"
#endif

class SettingManager : public QObject
{
	Q_OBJECT

public:
	SettingManager();

	void load(void);
	void save(void);

	GameScreenListModel* gameScreenModel(void) { return &m_gameScreenList; }
	PresetListModel* presetList(void) { return &m_presetList; }
	MacroPresetListModel* macroPresetList(void) { return &m_macroPresetList; }
	DropBoxData* dropBox(void) { return &m_dropBox; }

private:
	void __loadDropBox(void);
	void __loadPresetAction(void);
	void __loadMacroPresetAction(void);
	void __loadGameScreen(void);

	void __saveDropBox(void);
	void __savePresetAction(void);
	void __saveMacroPresetAction(void);
	void __saveGameScreen(void);

	QStringList __getPresetList(void);
	void __addDefaultPreset(void);
	void __addDefaultMacroPreset(void);
	void __makeDefaultDropBoxData(void);

signals:
	void presetListChanged(void);
	void macroPresetListChanged(void);

private:
	GameScreenListModel m_gameScreenList;
	PresetListModel m_presetList;
	MacroPresetListModel m_macroPresetList;
	DropBoxData m_dropBox;
	QString m_initPath;
};

#define SETTING_MANAGER CSingleton<SettingManager>::instance()

#endif // SETTINGMANAGER_H

#include "settingmanager.h"
#include <QSettings>
#include <QApplication>
#include <QQmlEngine>
#include <QDir>
#include "Lazybones.h"

const static QString Preset_GROUP_NAME = "PresetGroups";

SettingManager::SettingManager()
{
	m_initPath = qApp->applicationDirPath() + QDir::separator() + "Setting.ini";

	QQmlEngine::setObjectOwnership(&m_presetList, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_dropBox, QQmlEngine::CppOwnership);
}

void SettingManager::__addDefaultPreset(void)
{
	QSharedPointer<PresetData> presetData;

	presetData = QSharedPointer<PresetData>(new PresetData());
	presetData->setTitle("Empty Preset");

	m_presetList.add(presetData);
}

void SettingManager::__addDefaultMacroPreset(void)
{
	QSharedPointer<MacroPresetData> presetData;

	presetData = QSharedPointer<MacroPresetData>(new MacroPresetData());
	presetData->setTitle("Empty Macro");

	m_macroPresetList.add(presetData);
}

void SettingManager::__makeDefaultDropBoxData(void)
{
	m_dropBox.setDropBoxPath(m_dropBox.defaultDropBoxPath());
	m_dropBox.setScreenShotIntervalSec(60);
	m_dropBox.setScreenShotFileRotateCount(10);
}

void SettingManager::load(void)
{
	__loadDropBox();
	__loadPresetAction();
	__loadGameScreen();
	__loadMacroPresetAction();
}


void SettingManager::save(void)
{
	QFile::remove(m_initPath);

	__saveDropBox();
	__savePresetAction();
	__saveGameScreen();
	__saveMacroPresetAction();
}

void SettingManager::__loadDropBox(void)
{
	QSettings settings(m_initPath, QSettings::IniFormat );

	settings.beginGroup("DropBox");
	if(settings.contains("dropBoxPath"))
	{
		m_dropBox.setDropBoxPath(settings.value("dropBoxPath").toString());
		m_dropBox.setScreenShotIntervalSec((settings.value("screenShotInterval").toInt()));
		m_dropBox.setScreenShotFileRotateCount(settings.value("screenShotFileRotate").toInt());
	}
	else
	{
		__makeDefaultDropBoxData();
	}
	settings.endGroup();
}

void SettingManager::__saveDropBox(void)
{
	QSettings settings(m_initPath, QSettings::IniFormat );

	settings.beginGroup("DropBox");
	settings.setValue("dropBoxPath", m_dropBox.dropBoxPath());
	settings.setValue("screenShotInterval", m_dropBox.screenShotIntervalSec());
	settings.setValue("screenShotFileRotate", m_dropBox.screenShotFileRotateCount());
	settings.endGroup();
}

void SettingManager::__loadPresetAction(void)
{
	m_presetList.clear();

	QSettings settings(m_initPath, QSettings::IniFormat );

	int presetCount = settings.beginReadArray("Preset");

	for(int i = 0; i < presetCount; i++)
	{
		QSharedPointer<PresetData> presetData = QSharedPointer<PresetData>(new PresetData());

		settings.setArrayIndex(i);
		presetData->setTitle(settings.value("title").toString());

		int actionCount = settings.beginReadArray("ActionList");

		for(int j = 0; j < actionCount; j++)
		{
			InputAction* inputAction = new InputAction();
			settings.setArrayIndex(j);
			settings.beginGroup("Action");
			inputAction->setActionType(settings.value("actionType").toInt());
			inputAction->setKeyCode(settings.value("keyCode").toInt());
			inputAction->setMouseX(settings.value("mouseX").toInt());
			inputAction->setMouseY(settings.value("mouseY").toInt());
			inputAction->setDelayedMsec(settings.value("delayed").toInt());
			settings.endGroup();

			presetData->addAction(inputAction);

			delete inputAction;
		}

		settings.endArray();

		m_presetList.add(presetData);
	}
	settings.endArray();

	if(m_presetList.count() <= 0)
		__addDefaultPreset();
}

void SettingManager::__savePresetAction(void)
{
	QSettings settings(m_initPath, QSettings::IniFormat );

	settings.beginWriteArray("Preset");

	for(int i = 0; i < m_presetList.count(); i++)
	{
		QSharedPointer<PresetData> presetData = m_presetList.at(i);

		settings.setArrayIndex(i);
		settings.setValue("title", presetData->title());

		settings.beginWriteArray("ActionList");

		for(int j = 0; j < presetData->model()->count(); j++)
		{
			QSharedPointer<InputAction> inputAction = presetData->model()->at(j);
			settings.setArrayIndex(j);
			settings.beginGroup("Action");
			settings.setValue("actionType", (int)inputAction->actionType());
			settings.setValue("keyCode", (int)inputAction->keyCode());
			settings.setValue("mouseX", (int)inputAction->mouseX());
			settings.setValue("mouseY", (int)inputAction->mouseY());
			settings.setValue("delayed", (int)inputAction->delayedMsec());
			settings.endGroup();
		}

		settings.endArray();
	}
	settings.endArray();
}

void SettingManager::__loadGameScreen(void)
{
	m_gameScreenList.clear();

	QSettings settings(m_initPath, QSettings::IniFormat );

	int dataCount = settings.beginReadArray("GameScreen");

	for(int i = 0; i < dataCount; i++)
	{
		GameScreenData *data = new GameScreenData();

		settings.setArrayIndex(i);
		settings.beginGroup("ScreenData");
		data->setName(settings.value("name").toString());
		data->setPixelX(settings.value("pixelX").toInt());
		data->setPixelY(settings.value("pixelY").toInt());
		data->setColor(settings.value("color").toString());
		settings.endGroup();

		Lazybones::gameScreenManager()->addScreenData(data);
		delete data;
	}
	settings.endArray();
}

void SettingManager::__saveGameScreen(void)
{
	QSettings settings(m_initPath, QSettings::IniFormat );
	settings.beginWriteArray("GameScreen");

	for(int i = 0; i < m_gameScreenList.count(); i++)
	{
		QSharedPointer<GameScreenData> data = m_gameScreenList.at(i);
		settings.setArrayIndex(i);
		settings.beginGroup("ScreenData");
		settings.setValue("name", data->name());
		settings.setValue("pixelX", data->pixelX());
		settings.setValue("pixelY", data->pixelY());
		settings.setValue("color", data->color());
		settings.endGroup();
	}
	settings.endArray();
}

void SettingManager::__loadMacroPresetAction(void)
{
	m_macroPresetList.clear();

	QSettings settings(m_initPath, QSettings::IniFormat );

	int presetCount = settings.beginReadArray("MacroPreset");

	for(int i = 0; i < presetCount; i++)
	{
		QSharedPointer<MacroPresetData> presetData = QSharedPointer<MacroPresetData>(new MacroPresetData());

		settings.setArrayIndex(i);
		presetData->setTitle(settings.value("title").toString());
		presetData->setWindowTitle(settings.value("windowTitle").toString());
		presetData->setWindowRect(settings.value("windowRect").toRect());

		int actionCount = settings.beginReadArray("MacroActionList");

		for(int j = 0; j < actionCount; j++)
		{
			MacroAction* macroAction = new MacroAction();
			InputAction *inputAction = macroAction->inputAction();
			settings.setArrayIndex(j);
			settings.beginGroup("MacroAction");
			macroAction->setActionType(settings.value("macroActionType").toInt());
			macroAction->setComment(settings.value("comment").toString());
			macroAction->setSleepMsec(settings.value("sleepMsec").toInt());
			macroAction->setRandomSleep(settings.value("randomSleep").toBool());
			macroAction->setGotoIndex(settings.value("gotoIndex").toInt());
			macroAction->setConditionScreen(settings.value("conditionScreen").toString());
			inputAction->setActionType(settings.value("actionType").toInt());
			inputAction->setKeyCode(settings.value("keyCode").toInt());
			inputAction->setMouseX(settings.value("mouseX").toInt());
			inputAction->setMouseY(settings.value("mouseY").toInt());
			inputAction->setDelayedMsec(settings.value("delayed").toInt());

			int intpuActionCount = settings.beginReadArray("ActionList");

			for(int k = 0; k < intpuActionCount; k++)
			{
				InputAction* inputAction = new InputAction();
				settings.setArrayIndex(k);
				settings.beginGroup("Action");
				inputAction->setActionType(settings.value("actionType").toInt());
				inputAction->setKeyCode(settings.value("keyCode").toInt());
				inputAction->setMouseX(settings.value("mouseX").toInt());
				inputAction->setMouseY(settings.value("mouseY").toInt());
				inputAction->setDelayedMsec(settings.value("delayed").toInt());
				settings.endGroup();

				macroAction->addInputActionChunk(inputAction);

				delete inputAction;
			}
			settings.endArray();

			settings.endGroup();

			presetData->addAction(macroAction);

			delete macroAction;
		}

		settings.endArray();

		m_macroPresetList.add(presetData);
	}
	settings.endArray();

	if(m_macroPresetList.count() <= 0)
		__addDefaultMacroPreset();
}

void SettingManager::__saveMacroPresetAction(void)
{
	QSettings settings(m_initPath, QSettings::IniFormat );

	settings.beginWriteArray("MacroPreset");

	for(int i = 0; i < m_macroPresetList.count(); i++)
	{
		QSharedPointer<MacroPresetData> presetData = m_macroPresetList.at(i);

		settings.setArrayIndex(i);
		settings.setValue("title", presetData->title());
		settings.setValue("windowTitle", presetData->windowTitle());
		settings.setValue("windowRect", presetData->windowRect());

		settings.beginWriteArray("MacroActionList");

		for(int j = 0; j < presetData->model()->count(); j++)
		{
			QSharedPointer<MacroAction> macroAction = presetData->model()->at(j);
			InputAction *inputAction = macroAction->inputAction();
			settings.setArrayIndex(j);
			settings.beginGroup("MacroAction");
			settings.setValue("macroActionType", (int)macroAction->actionType());
			settings.setValue("comment", macroAction->comment());
			settings.setValue("sleepMsec", macroAction->sleepMsec());
			settings.setValue("randomSleep", macroAction->randomSleep());
			settings.setValue("gotoIndex", macroAction->gotoIndex());
			settings.setValue("conditionScreen", macroAction->conditionScreen());
			settings.setValue("actionType", (int)inputAction->actionType());
			settings.setValue("keyCode", (int)inputAction->keyCode());
			settings.setValue("mouseX", (int)inputAction->mouseX());
			settings.setValue("mouseY", (int)inputAction->mouseY());
			settings.setValue("delayed", (int)inputAction->delayedMsec());

			settings.beginWriteArray("ActionList");

			for(int k = 0; k < macroAction->inputActionChunk()->count(); k++)
			{
				QSharedPointer<InputAction> inputAction = macroAction->inputActionChunk()->at(k);
				settings.setArrayIndex(k);
				settings.beginGroup("Action");
				settings.setValue("actionType", (int)inputAction->actionType());
				settings.setValue("keyCode", (int)inputAction->keyCode());
				settings.setValue("mouseX", (int)inputAction->mouseX());
				settings.setValue("mouseY", (int)inputAction->mouseY());
				settings.setValue("delayed", (int)inputAction->delayedMsec());
				settings.endGroup();
			}

			settings.endArray();

			settings.endGroup();
		}

		settings.endArray();
	}
	settings.endArray();
}

QStringList SettingManager::__getPresetList()
{
	QStringList res;
	QSettings settings(m_initPath, QSettings::IniFormat );

	QStringList list = settings.childGroups();
	for(int i = 0; i < list.size(); i++)
	{
		QString group = list.at(i);
		res.append(group);
	}
	return res;
}


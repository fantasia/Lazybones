#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <qqml.h>

#include "Lazybones.h"
#include "inputaction.h"
#include "gamescreenlistmodel.h"
#include "gamescreenmanager.h"
#include "inputmethodlistmodel.h"
#include "screencaptureprovider.h"
#include "presetlistmodel.h"
#include "macropresetlistmodel.h"
#include "settingmanager.h"
#include "simplestringlistmodel.h"
#include "dropboxdata.h"
#include "ConsoleAppender.h"
#include "FileAppender.h"
#include "Logger.h"

// TODO
// 2. Throw up yellow items
// 3. Bug fix Hanup
// 4. Random time
// 5. LoopWaitScreenDoInputActionEnd

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QQmlApplicationEngine engine;

	Lazybones::initialize();

	FileAppender* fileAppender = new FileAppender("log.txt");
	fileAppender->setFormat("%t{yyyy-MM-dd HH:mm:ss.zzz} %m\n");

	ConsoleAppender* consoleAppender = new ConsoleAppender();
	consoleAppender->setFormat("%t{yyyy-MM-dd HH:mm:ss.zzz} %m\n");

	Logger::registerAppender(fileAppender);
	Logger::registerAppender(consoleAppender);

	LOG_INFO() << "Starting Application..";

	qmlRegisterType<Lazybones>("gunoodaddy", 1, 0, "Lazybones");
	qmlRegisterType<InputAction>("gunoodaddy", 1, 0, "InputAction");
	qmlRegisterType<GameScreenData>("gunoodaddy", 1, 0, "GameScreenData");
	qmlRegisterType<MacroAction>("gunoodaddy", 1, 0, "MacroAction");
	qmlRegisterUncreatableType<SimpleStringListModel>("gunoodaddy", 1, 0, "SimpleStringListModel", "Can only be created by C++.");
	qmlRegisterUncreatableType<GameScreenManager>("gunoodaddy", 1, 0, "GameScreenManager", "Can only be created by C++.");
	qmlRegisterUncreatableType<GameScreenListModel>("gunoodaddy", 1, 0, "GameScreenListModel", "Can only be created by C++.");
	qmlRegisterUncreatableType<InputMethodListModel>("gunoodaddy", 1, 0, "InputMethodListModel", "Can only be created by C++.");
	qmlRegisterUncreatableType<MacroActionListModel>("gunoodaddy", 1, 0, "MacroActionListModel", "Can only be created by C++.");
	qmlRegisterUncreatableType<MacroPresetListModel>("gunoodaddy", 1, 0, "MacroPresetListModel", "Can only be created by C++.");
	qmlRegisterUncreatableType<MacroPresetData>("gunoodaddy", 1, 0, "MacroPresetData", "Can only be created by C++.");
	qmlRegisterUncreatableType<PresetListModel>("gunoodaddy", 1, 0, "PresetListModel", "Can only be created by C++.");
	qmlRegisterUncreatableType<PresetData>("gunoodaddy", 1, 0, "PresetData", "Can only be created by C++.");
	qmlRegisterUncreatableType<DropBoxData>("gunoodaddy", 1, 0, "DropBoxData", "Can only be created by C++.");

	engine.addImageProvider("capture", new ScreenCaptureProvider());

	QString mainQmlFile = "/qml/main.qml";
	QString qmlRootPath = QDir::currentPath() + mainQmlFile;
	if(QFile::exists(qmlRootPath))
		engine.load(qmlRootPath);
	else
	{
		qmlRootPath = "qrc://" + mainQmlFile;
		engine.load(QUrl(qmlRootPath));
	}

	int ret = app.exec();

	SETTING_MANAGER->save();

	return ret;
}

TEMPLATE = app

QT += qml quick widgets gui

folder_01.source = qml
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

macx {
	LIBS += -framework Foundation -framework ApplicationServices -framework AppKit
}

CONFIG(debug,debug|release) {
	TARGET = LazybonesD
} else {
	TARGET = Lazybones
}

for(deploymentfolder, DEPLOYMENTFOLDERS) {
		item = item$${deploymentfolder}
		greaterThan(QT_MAJOR_VERSION, 4) {
				itemsources = $${item}.files
		} else {
				itemsources = $${item}.sources
		}
		$$itemsources = $$eval($${deploymentfolder}.source)
		itempath = $${item}.path
		$$itempath= $$eval($${deploymentfolder}.target)
		export($$itemsources)
		export($$itempath)
		DEPLOYMENT += $$item
}

SOURCES += main.cpp \
	inputmethodlistmodel.cpp \
    Lazybones.cpp \
    inputaction.cpp \
    screencaptureprovider.cpp \
    settingmanager.cpp \
    presetlistmodel.cpp \
    presetdata.cpp \
    dropboxdata.cpp \
	gamescreenmanager.cpp \
    gamescreenlistmodel.cpp \
    gamescreendata.cpp \
    macropresetdata.cpp \
    macroaction.cpp \
    macroactionlistmodel.cpp \
    macropresetlistmodel.cpp \
    simplestringlistmodel.cpp

HEADERS += inputmethodlistmodel.h \
    Lazybones.h \
    inputaction.h \
    screencaptureprovider.h \
    settingmanager.h \
    presetlistmodel.h \
    presetdata.h \
    dropboxdata.h \
	gamescreenmanager.h \
    gamescreenlistmodel.h \
    gamescreendata.h \
    macropresetdata.h \
    macroaction.h \
    macroactionlistmodel.h \
    macropresetlistmodel.h \
    simplestringlistmodel.h

RESOURCES += \
    Lazybones.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

include(InputEvent/InputEvent.pri)

include(CuteLogger/CuteLogger.pri)

OTHER_FILES += \
    qml/ActionPanel.qml \
    qml/MacroActionSelectWindow.qml \
    qml/InputMethodSelectWindow.qml \
    qml/GameScreenListWindow.qml

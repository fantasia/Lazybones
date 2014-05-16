#include "Lazybones.h"
#include "settingmanager.h"
#include <QQmlEngine>
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
#include "screencaptureprovider.h"

bool Lazybones::s_forceStopFlag = false;
InputEventManager* Lazybones::s_inputEventManager = NULL;
GameScreenManager* Lazybones::s_gameScreenManager = NULL;

Lazybones::Lazybones(QObject *parent) :
	QObject(parent), m_currentPresetIndex(0), m_currentMacroPresetIndex(0)
{
	connect(SETTING_MANAGER, SIGNAL(presetListChanged()), this, SLOT(onPresetListChanged()));
	connect(SETTING_MANAGER, SIGNAL(macroPresetListChanged()), this, SLOT(onMacroPresetListChanged()));

	connect(s_inputEventManager, SIGNAL(keyEvent(QKeyEvent)), this, SLOT(onKeyEvent(QKeyEvent)));
	connect(s_inputEventManager, SIGNAL(mouseEvent(QMouseEvent)), this, SLOT(onMouseEvent(QMouseEvent)));

	setCurrentPresetIndex(0);

	inputEventManager()->startFiltering(InputEventManager::KeyboardHook);	// for global key hooking
}

void Lazybones::initialize(void)
{
	if(s_inputEventManager == NULL)
		s_inputEventManager = new InputEventManager();

	if(s_gameScreenManager == NULL)
		s_gameScreenManager = new GameScreenManager();

	SETTING_MANAGER->load();
}

void Lazybones::setCurrentPresetIndex(int presetIndex)
{
	m_currentPresetIndex = presetIndex;

	if(presetIndex < 0 || presetIndex >= SETTING_MANAGER->presetList()->count())
	{
		m_currentPresetData.clear();
	}
	else
	{
		m_currentPresetData = SETTING_MANAGER->presetList()->at(presetIndex);
	}

	emit currentPresetChanged();
	emit currentPresetIndexChanged();
}

void Lazybones::setCurrentMacroPresetIndex(int presetIndex)
{
	m_currentMacroPresetIndex = presetIndex;

	if(presetIndex < 0 || presetIndex >= SETTING_MANAGER->macroPresetList()->count())
	{
		m_currentMacroPresetData.clear();
	}
	else
	{
		m_currentMacroPresetData = SETTING_MANAGER->macroPresetList()->at(presetIndex);
	}

	emit currentMacroPresetChanged();
	emit currentMacroPresetIndexChanged();
}

void Lazybones::onPresetListChanged(void)
{
	emit presetListChanged();
}

void Lazybones::onMacroPresetListChanged(void)
{
	emit macroPresetListChanged();
}

void Lazybones::onKeyEvent(QKeyEvent event)
{
	PresetData *preset = currentPreset();
	if(!preset)
		return;

	//qDebug() << "KeyEvent" << event.key() << preset->recording();

	if(preset->running())
	{
		if(event.key() == Qt::Key_F10 || event.key() == Qt::Key_F12 || event.key() == Qt::Key_Pause)
		{
			s_forceStopFlag = true;
			qDebug() << "EMERGENCY STOP";
		}
	}
	else if(preset->recording())
	{
		if(event.key() == Qt::Key_F10 || event.key() == Qt::Key_F12 || event.key() == Qt::Key_Pause)
		{
			preset->setRecording(false);
			qDebug() << "EMERGENCY RECORD STOP";
		}
		else
		{
			qDebug() << "Add KeyEvent" << event.key() << preset->recording();
			preset->addAction(&event);
		}
	}

	MacroPresetData *macroPreset = currentMacroPreset();
	if(!macroPreset)
		return;

	if(macroPreset->running())
	{
		if(event.key() == Qt::Key_F10 || event.key() == Qt::Key_F12 || event.key() == Qt::Key_Pause)
		{
			s_forceStopFlag = true;
			macroPreset->setRunning(false);
			qDebug() << "EMERGENCY STOP : MACRO";
		}
	}
}

void Lazybones::onMouseEvent(QMouseEvent event)
{
	PresetData *preset = currentPreset();
	if(!preset || !preset->recording())
		return;
	//qDebug() << "MouseEvent" << event.pos() << event.button() << preset->recording();
	preset->addAction(&event);
}

QColor Lazybones::pixelColor(int pixelX, int pixelY, bool useLastShot)
{
	return getPixelColor(pixelX, pixelY, useLastShot);
}

QPoint Lazybones::toRealPos(QImage *image, int imageX, int imageY)
{
	QScreen *screen = QGuiApplication::primaryScreen();
	qreal ratioH = image->size().width() / screen->size().width();
	qreal ratioV = image->size().height() / screen->size().height();

	return QPoint(imageX / ratioH, imageY / ratioV);
}

QColor Lazybones::getPixelColor(int pixelX, int pixelY, bool useLastShot)
{
	QImage image;
	QScreen *screen = QGuiApplication::primaryScreen();

	if(useLastShot)
	{
		const QPixmap *pixmap = ScreenCaptureProvider::lastPixmap();
		if(!pixmap->isNull())
		{
			image = pixmap->toImage();
		}
	}

	if(image.isNull())
	{
		QPixmap screenShotPixmap = screen->grabWindow(0);
		image = screenShotPixmap.toImage();
		ScreenCaptureProvider::setLastPixmap(screenShotPixmap);
	}

	return getPixelColor(screen, &image, pixelX, pixelY);
}

QColor Lazybones::getPixelColor(QScreen *screen, QImage *image, int pixelX, int pixelY)
{
	qreal ratioH = image->size().width() / screen->size().width();
	qreal ratioV = image->size().height() / screen->size().height();

	QColor clr = image->pixel(pixelX * ratioH, pixelY * ratioV);
	//qDebug() << "Pixel color:" << pixelX << pixelY << pixelX * ratioH << pixelY * ratioV << clr << image->size().width() / 2 << image->size().height() / 2 << ratioH << ratioV;

	return clr;
}

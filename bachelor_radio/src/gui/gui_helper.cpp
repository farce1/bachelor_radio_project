/*
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    This file is based on SDR-J
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *
 *
 */

#include <QDebug>
#include <QSettings>
#include <QQuickStyle>
#include <QQmlProperty>

#include "gui_helper.h"
#include "debug_output.h"
#include "input_factory.h"
#include "audio_output.h"
#include "dab-constants.h"
#include "msc-handler.h"
#include "version.h"

/**
  *	We use the creation function merely to set up the
  *	user interface and make the connections between the
  *	gui elements and the handling agents. All real action
  *	is embedded in actions, initiated by gui buttons
  */
CGUIHelper::CGUIHelper(CRadioController *RadioController, QObject *parent)
    : QObject(parent)
    , radioController(RadioController)
    , spectrumSeries(nullptr)
    , impulseResponseSeries(nullptr)
{
    // Add image provider for the MOT slide show
    motImageProvider = new CMOTImageProvider;

    QSettings settings;
    connect(RadioController, &CRadioController::motChanged, this, &CGUIHelper::motUpdate);
    connect(RadioController, &CRadioController::motReseted, this, &CGUIHelper::motReset);
    connect(RadioController, &CRadioController::showErrorMessage, this, &CGUIHelper::showErrorMessage);
    connect(RadioController, &CRadioController::showInfoMessage, this, &CGUIHelper::showInfoMessage);

#ifndef QT_NO_SYSTEMTRAYICON
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SIGNAL(minimizeWindow()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SIGNAL(maximizeWindow()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SIGNAL(restoreWindow()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIconMenu = new QMenu();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon();

    trayIcon->setContextMenu(trayIconMenu);

    trayIcon->setIcon(QIcon(":/icons/icon.png"));
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(showWindow(QSystemTrayIcon::ActivationReason)));

#endif

    CDebugOutput::setCGUI(this);
}

CGUIHelper::~CGUIHelper()
{
    // Avoid segmentation fault if a debug message should be displayed after deleting
    CDebugOutput::setCGUI(nullptr);

    qDebug() << "GUI:" <<  "Deleting CGUIHelper";
}

void CGUIHelper::close()
{
    qDebug() << "GUI:" <<  "Close application";
    QApplication::quit();
}

void CGUIHelper::deviceClosed()
{
}

/**
 * \brief returns the licenses for all the relative libraries plus application version information
 */
const QVariantMap CGUIHelper::licenses()
{
    QVariantMap ret;
    QByteArray InfoContent;

    InfoContent.append(getInfoPage("Versions"));

    InfoContent.append("THANKS\n");
    InfoContent.append("------\n");
    InfoContent.append(getInfoPage("Thanks"));

    ret.insert("FileContent", InfoContent);

    return ret;
}

const QByteArray CGUIHelper::getFileContent(QString filepath)
{
    QFile *File;
    QByteArray InfoContent;
    File = new QFile(filepath);
    File->open(QFile::ReadOnly);
    InfoContent.append(File->readAll());
    InfoContent.append("\n");
    delete File;
    return InfoContent;
}

const QByteArray CGUIHelper::getInfoPage(QString pageName)
{
    QByteArray InfoContent;

    if (pageName == "Versions") {
        InfoContent.append(tr("App version") + ": " + QString(CURRENT_VERSION) + "\n");
        QString ts = QString(__TIMESTAMP__).replace("  "," ");
        QDateTime tsDT = QLocale(QLocale::C).toDateTime(ts, "ddd MMM d hh:mm:ss yyyy");
        InfoContent.append(tr("Built on") + ": " + tsDT.toString(Qt::ISODate) + "\n");
        InfoContent.append(tr("QT version") + ": " + qVersion() + "\n");
        InfoContent.append("\n");
    } else {
        return getFileContent(":/THANKS");
    }

    return InfoContent;
}

void CGUIHelper::motUpdate(mot_file_t mot_file)
{
    std::clog  << "SLS ContentName: " << mot_file.content_name << std::endl;
    std::clog  << "catSLS Category: " << std::to_string(mot_file.category) << " SlideID: " << std::to_string(mot_file.slide_id) << std::endl;
    std::clog  << "catSLS CategoryTitle: " << mot_file.category_title << std::endl;
    std::clog  << "ClickThroughURL: " << mot_file.click_through_url << std::endl;

    QString pictureName =
            "/" + QString::number(mot_file.category) +
            "/" + QString::fromStdString(mot_file.category_title) +
            "/" + QString::number(mot_file.slide_id) +
            "/" + QString::fromStdString(mot_file.content_name);

    QByteArray qdata(reinterpret_cast<const char*>(mot_file.data.data()), static_cast<int>(mot_file.data.size()));
    QImage motImage;
    motImage.loadFromData(qdata, mot_file.content_sub_type == 0 ? "GIF" : mot_file.content_sub_type == 1 ? "JPEG" : mot_file.content_sub_type == 2 ? "BMP" : "PNG");
    motImageProvider->setPixmap(QPixmap::fromImage(motImage), pictureName);

    emit motChanged(pictureName, QString::fromStdString(mot_file.category_title), mot_file.category, mot_file.slide_id);
}

void CGUIHelper::motReset()
{
    motImageProvider->clear();
    emit motReseted();
}

void CGUIHelper::showErrorMessage(QString Text)
{
#ifndef QT_NO_SYSTEMTRAYICON
    trayIcon->showMessage(QCoreApplication::applicationName(), Text, QIcon(":/icon.png"), 5000);
#else
    (void)Text;
#endif
}

void CGUIHelper::showInfoMessage(QString Text)
{
#ifndef QT_NO_SYSTEMTRAYICON
    trayIcon->showMessage(QCoreApplication::applicationName(), Text, QIcon(":/icon.png"), 5000);
#else
    (void)Text;
#endif
}

void CGUIHelper::showWindow(QSystemTrayIcon::ActivationReason r)
{
#ifndef QT_NO_SYSTEMTRAYICON
    if (r == QSystemTrayIcon::Trigger)
        emit restoreWindow();
#endif
}

void CGUIHelper::tryHideWindow()
{
#ifndef QT_NO_SYSTEMTRAYICON
    QSettings settings;
    int count = settings.value("hideWindowTrayMessageDisplayCount",0).toInt();

    // Hide only if system tray is available otherwise ignore it. Standard Gnome doesn't have a system tray so user would lose the control.
    if(trayIcon->isSystemTrayAvailable() && count < 4) {
        trayIcon->showMessage(QCoreApplication::applicationName(),
                              tr("The program will keep running in the "
                              "system tray. To terminate the program, "
                              "choose \"%1\" in the context menu "
                              "of the system tray entry.").arg(
                                //: "Quit" translation should be the same as the one of system tray
                                tr("Quit")
                              ),
                              QIcon(":/icon.png"), 5000);
        settings.setValue("hideWindowTrayMessageDisplayCount", count+1);
        emit minimizeWindow();
    }
#endif
}

void CGUIHelper::saveMotImages()
{
    motImageProvider->saveAll();
}

void CGUIHelper::openAutoDevice()
{
    CDeviceID deviceId;
    deviceId = radioController->openDevice();
    emit newDeviceId(static_cast<int>(deviceId));
}

void CGUIHelper::openNull()
{
    radioController->openDevice(CDeviceID::NULLDEVICE);
}

void CGUIHelper::openAirspy()
{
    radioController->openDevice(CDeviceID::AIRSPY);
}

void CGUIHelper::setBiasTeeAirspy(bool isOn)
{
    radioController->setDeviceParam("biastee", isOn ? 1 : 0);
}

void CGUIHelper::openRtlSdr()
{
#ifdef __ANDROID__
    radioController->openDevice(CDeviceID::ANDROID_RTL_SDR);
#else
    radioController->openDevice(CDeviceID::RTL_SDR);
#endif
}

void CGUIHelper::setBiasTeeRtlSdr(bool isOn)
{
    radioController->setDeviceParam("biastee", isOn ? 1 : 0);
}

void CGUIHelper::openSoapySdr()
{
    radioController->openDevice(CDeviceID::SOAPYSDR);
}

void CGUIHelper::setAntennaSoapySdr(QString text)
{
    radioController->setDeviceParam("SoapySDRAntenna", text);
}

void CGUIHelper::setDriverArgsSoapySdr(QString text)
{
    radioController->setDeviceParam("SoapySDRDriverArgs", text);
}

void CGUIHelper::setClockSourceSoapySdr(QString text)
{
    radioController->setDeviceParam("SoapySDRClockSource", text);
}

void CGUIHelper::openRtlTcp(QString serverAddress, int IpPort, bool force)
{
    radioController->openDevice(CDeviceID::RTL_TCP, force, serverAddress, IpPort);
}

void CGUIHelper::openRawFile(QString fileFormat)
{
#ifdef __ANDROID__
    // Open file selection dialog
    const auto ACTION_OPEN_DOCUMENT = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "ACTION_OPEN_DOCUMENT");
    QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", ACTION_OPEN_DOCUMENT.object());
    const auto CATEGORY_OPENABLE = QAndroidJniObject::getStaticObjectField<jstring>("android/content/Intent", "CATEGORY_OPENABLE");
    intent.callObjectMethod("addCategory", "(Ljava/lang/String;)Landroid/content/Intent;", CATEGORY_OPENABLE.object());
    intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;", QAndroidJniObject::fromString(QStringLiteral("*/*")).object());

    // Open file dialog
    activityResultReceiver = new FileActivityResultReceiver(this, fileFormat);
    QtAndroid::startActivity( intent.object<jobject>(), 12, activityResultReceiver);
#else
    (void) fileFormat;
#endif
}

void CGUIHelper::openRawFile(QString filename, QString fileFormat)
{
    radioController->openDevice(CDeviceID::RAWFILE, true, filename, fileFormat);
}

void CGUIHelper::setNewDebugOutput(QString text)
{
    text = text.remove('\n');
    emit newDebugOutput(text);
}

void CGUIHelper::setTranslator(QTranslator *translator)
{
    this->translator = translator;
}

QString CGUIHelper::mapToLanguage(QString Language)
{
    // use only Polish Language
    if(Language == "pl_PL")
    {
        qDebug() << "main:" <<  "Użyto języka Polskiego";
        Language = "pl_PL";
    }
    return Language;
}

bool CGUIHelper::loadTranslationFile(QTranslator *translator, QString Language)
{
    bool isLoaded = translator->load(QString(":/i18n/") + Language);

    if(!isLoaded)
    {
        qDebug() << "main:" <<  "Error while loading language" << Language << "use untranslated text (ie. English)";
    }

    return isLoaded;
}

void CGUIHelper::updateTranslator(QString Language, QObject *obj)
{
    QString lang = mapToLanguage(Language);

    // Set locale e.g. time formarts
    QLocale curLocale(QLocale((const QString&)lang));
    QLocale::setDefault(curLocale);

    loadTranslationFile(translator, lang);
    translateGUI(obj);
}

void CGUIHelper::translateGUI(QObject *obj)
{
    // Save previous width & height
    // (because they are reset by the call to retranslate())
    QVariant width = QQmlProperty::read(obj, "width");
    QVariant height = QQmlProperty::read(obj, "height");

    // Start translation of GUI
    QQmlContext *currentContext = QQmlEngine::contextForObject(obj);
    QQmlEngine *engine = currentContext->engine();
    engine->retranslate();

    // Restore previous width & height
    // (because they are reset by the call to retranslate())
    QQmlProperty::write(obj, "width", width);
    QQmlProperty::write(obj, "height", height);

    // Start translation of non-QML GUI
#ifndef QT_NO_SYSTEMTRAYICON
    minimizeAction->setText(tr("Mi&nimize"));
    maximizeAction->setText(tr("Ma&ximize"));
    restoreAction->setText(tr("&Restore"));
    quitAction->setText(tr("&Quit"));
#endif

    emit translationFinished();
}

#ifdef __ANDROID__
void FileActivityResultReceiver::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &intent) {
    if (!intent.isValid()) {
        return;
    }

    const auto uri = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
    if (!uri.isValid()) {
        return;
    }

    const auto scheme = uri.callObjectMethod("getScheme", "()Ljava/lang/String;");
    if (scheme.toString() == QLatin1String("content")) {
        const auto tmpFile = uri.callObjectMethod("toString", "()Ljava/lang/String;");
        guiHelper->openRawFile(QString(tmpFile.toString()), fileFormat);
    }
}
#endif

QString CGUIHelper::getQQStyleToLoad(QString styleNameArg)  // Static
{
    QSettings settings;
    QString settingStyle = settings.value("qQStyle","").toString();

    // In case this is a first launch where the setting in the config file is not set
    if (settingStyle.isEmpty()) {
        if (styleNameArg.isEmpty()) {
            settings.setValue("qQStyle", "Default");
            return "Default";
        }
        else {
            settings.setValue("qQStyle", styleNameArg);
            return styleNameArg;
        }
    }

    QStringList availableStyle = QQuickStyle::availableStyles();

    for ( const QString& curStyle : availableStyle ) {
         if (settingStyle == curStyle)
             return settingStyle;
    }
    if (settingStyle == "System_Auto")
        return QString();
    else
        return "Default";
}

const QStringList CGUIHelper::qQStyleComboList()
{
    if ( !m_comboList.isEmpty() )
        return m_comboList;

    m_comboList = QQuickStyle::availableStyles();
    m_comboList.sort();
    int position = m_comboList.indexOf("Default");
    m_comboList.move(position, 0);
    m_comboList.insert(1, "System_Auto");

    QString settingStyle = settings.value("qQStyle","").toString();
    settingsStyleInAvailableStyles = false;

    for ( const auto& style : m_comboList ) {
         if (settingStyle == style)
             settingsStyleInAvailableStyles = true;
    }

    if ( settingsStyleInAvailableStyles == false ) {
        m_comboList.append(settingStyle);
        qDebug() << "Style from the settings " << settingStyle << " not available on system. Adding it to the list of styles and loading 'Default' instead.";
    }

    return m_comboList;
}

bool CGUIHelper::isThemableStyle(QString style)
{
    return (style == "Universal" || style == "Material");
}

int CGUIHelper::getIndexOfQQStyle(QString style)
{
    //qDebug() << "getIndexOfQQStyle: " << style;
    return m_comboList.indexOf(style);
}

QString CGUIHelper::getQQStyle()
{
    return settings.value("qQStyle","").toString();
}

void CGUIHelper::saveQQStyle(int index)
{
    //qDebug() << "saveQQStyle : " << index;
    settings.setValue("qQStyle",m_comboList.value(index));
    emit styleChanged();
}

StyleModel::StyleModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

StyleModel* CGUIHelper::qQStyleComboModel()
{
    if (m_styleModel != nullptr)
        m_styleModel = nullptr;

    QString settingStyle = settings.value("qQStyle","").toString();

    QStringList styleList = qQStyleComboList();

    m_styleModel = new StyleModel();
    for ( const auto& style : styleList  ) {
        if ( !settingsStyleInAvailableStyles && (settingStyle == style)) {
            m_styleModel->addStyle(Style(Style(style + tr(" (unavailable, fallback to Default)"), style)));
        }
        else {
            if (style == "System_Auto")
                m_styleModel->addStyle(Style(tr("Style of system"), style));
            else if (style == "Default")
                m_styleModel->addStyle(Style("Default" + tr(" (Recommended)"), style));
            else
                m_styleModel->addStyle(Style(style, style));
        }
    }
    return m_styleModel;
}

QHash<int, QByteArray> StyleModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LabelRole] = "label";
    roles[StyleRole] = "style";
    return roles;
}

Style::Style(const QString &label, const QString &style)
    : m_label(label), m_style(style)
{
}

QString Style::label() const
{
    return m_label;
}

QString Style::style() const
{
    return m_style;
}

void StyleModel::addStyle(const Style &style)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_styles << style;
    endInsertRows();
}

int StyleModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_styles.count();
}

QVariantMap StyleModel::get(int row) const
{
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;
    QModelIndex idx = index(row, 0);
    while (i.hasNext()) {
        i.next();
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}

QVariant StyleModel::data(const QModelIndex & index, int role) const
{
    if (index.row() < 0 || index.row() >= m_styles.count())
        return QVariant();

    const Style &style = m_styles[index.row()];
    if (role == LabelRole)
        return style.label();
    else if (role == StyleRole)
        return style.style();
    return QVariant();
}

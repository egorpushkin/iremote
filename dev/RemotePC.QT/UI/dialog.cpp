/**
 * Copyright (c) 2013 Egor Pushkin. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Common/Common.h"

#include "dialog.h"
#include "ui_dialog.h"

// UI tools
#include "UIConfig.h"
#include "Controls.h"

#include "Server/ServerContext.h"
#include "Server/Config.h"
#include "Server/NetworkHelper.h"
#include "Messages/VolumeMessage.h"
#include "Messages/AuthMessage.h"
#include "Server/IUIEvents.h"
#include "UI/Annunciators/Events.h"
#include "Versions/Versions.h"

// Users manager support
#include "Server/UsersManager/User.h"
#include "Server/UsersManager/DUsersManager.h"
#include "Server/UsersManager/UsersManager.h"

// Audio subsystem
#include "Hardware/Hardware.h"
// Screenshot capturing services.
#include "Hardware/Shared/ScreenshotSpreader.h"
#if defined(WIN32)
#include "Hardware/Win32/IVolumeControlInitializer.h"
#endif // WIN32

// Frame Buffer service
#include "../Server/Services/IServiceHandler.h"
#include "../Server/Services/IServicesManager.h"

// Remote PC forms
#include "authdialog.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "usersmngdialog.h"

#include <QTimer>
#include <QMutexLocker>
#include <QDesktopWidget>
#include <QBuffer>
#include <QCloseEvent>
#include <QMenu>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>

Dialog::Dialog(QWidget *parent)
	: QDialog(parent, RemotePC::UIConfig::dialogFlags_)
	, ui(new Ui::DialogClass)
	, trayIcon_(NULL)
	, trayIconMenu_(NULL)
	, events_()
	, serverThread_()
	, serverControl_()
	, currentAuth_(0)
	, srcTimer_( new QTimer(this) )
	, volumeTimer_( new QTimer(this) )
{
	// Configure UI
	//////////////////////////////////////////////////////////////////////////

	ui->setupUi(this);

    // Register size_t.
    qRegisterMetaType<size_t>("size_t");

	// Connect self to self signals. This trick is used to perform actions from
	// server context's thread on main UI thread.
	QObject::connect(
		this,
		SIGNAL(ServiceStartedSig()),
		this,
		SLOT(ServiceStartedSlot()));

	QObject::connect(
		this,
		SIGNAL(FailedToStartServiceSig()),
		this,
		SLOT(FailedToStartServiceSlot()));

	QObject::connect(
		this,
        SIGNAL(ProveAuthenticationSig(size_t)),
		this,
        SLOT(ProveAuthenticationSlot(size_t)));

	QObject::connect(
		this,
        SIGNAL(ShowDeviceSig(size_t)),
		this,
        SLOT(ShowDeviceSlot(size_t)));

	QObject::connect(
		this,
        SIGNAL(HideDeviceSig(size_t)),
		this,
        SLOT(HideDeviceSlot(size_t)));

	QObject::connect(
		this,
        SIGNAL(ShowOldiRemoteWarningSig(size_t)),
		this,
        SLOT(ShowOldiRemoteWarningSlot(size_t)));

	// Configure system tray icon
	trayIcon_ = new QSystemTrayIcon(this);
	connect(trayIcon_ , SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

	quitAction_ = new QAction(tr("&Quit"), this);
	connect(quitAction_, SIGNAL(triggered()), qApp, SLOT(quit()));

	trayIconMenu_ = new QMenu(this);
	trayIconMenu_->addAction(quitAction_);

	QIcon icon(":/Resources/Icon.png");
	trayIcon_->setIcon(icon);
	trayIcon_->setContextMenu(trayIconMenu_);
	trayIcon_->setToolTip(tr("Remote PC Suite"));
	trayIcon_->show();
	setWindowIcon(icon);

    // Format version.
    ui->version_->setText(
        QString::number(RemotePC::REMOTEPC_VER_MAJOR) + QString(".")
        + QString::number(RemotePC::REMOTEPC_VER_MINOR) + QString(" ")
        + QString::fromUtf8(RemotePC::RemotePCBuildStatus.c_str()) );

	// Configure hardware
	//////////////////////////////////////////////////////////////////////////

	// Initialize volume control
	RemotePC::IVolumeControlPtr volumeControl = RemotePC::HardwareProvider::Instance().GetVolumeControl();
	// Call stub to avoid warning. This step may be omitted and volumeControl
	// may be moved inside '#if defined(WIN32)' scope, but it is preferable
	// to perform initialization at startup rather than at first request.
	// In this case invocation of GetVolumeControl() constructs instance of 
	// volume control for all platforms.
	volumeControl->GetMinimumVolume(); 
#if defined(WIN32)
	RemotePC::IVolumeControlInitializerPtr volumeControlInitializer(volumeControl);
	if ( volumeControlInitializer )
		volumeControlInitializer->Initialize(winId());
#endif // WIN32

	// Start application service
	//////////////////////////////////////////////////////////////////////////

	// Initialize tool responsible for events delivery.
	events_ = mc::Class< RemotePC::Events >::Create( this );

	// Start service.
	RestartService();

	// Display list of host endpoints.
	std::vector< std::string > endpoints = RemotePC::NetworkHelper::EnumHostEndpoints();
	for ( std::vector< std::string >::iterator iter = endpoints.begin() ; iter != endpoints.end() ; ++iter )
	{
		ui->endpoints_->addItem( (*iter).c_str() );
	}

	// Start screen capturing if necessary.
	connect(srcTimer_, SIGNAL(timeout()), this, SLOT(updateScreenshot()));
	ResetTimer();

	// Start volume update service.
	connect(volumeTimer_, SIGNAL(timeout()), this, SLOT(updateVolume()));
	volumeTimer_->start( RemotePC::Config::Instance().GetVolumeUpdateInterval() );
}

Dialog::~Dialog()
{
	StopService();
	delete ui;
}

void Dialog::ResetTimer()
{
	if ( RemotePC::Config::Instance().ShareScreenshots() )
	{
		srcTimer_->start( GetTimerInterval() );
	}
	else
	{
		srcTimer_->stop();
	}
}

int Dialog::GetTimerInterval()
{
	return (int)( 1000.0f / RemotePC::Config::Instance().GetFPS() );
}

void Dialog::StopService()
{
	MC_LOG_ROUTINE;
	// Stop current service.
	if ( serverControl_ && serverThread_ )
	{
		serverControl_->Stop();
		serverThread_->Join();
		serverControl_ = NULL;
		serverThread_ = NULL;
	}
}

void Dialog::RestartService()
{
	MC_LOG_ROUTINE;
	// Stop current service.
	StopService();

	// Start new service.
	serverThread_ = mc::Library::Thread();
    serverControl_ = mc::Class< RemotePC::ServerContext >::Create( events_ );
	serverThread_->SetContext(serverControl_);
	serverThread_->Start();
}

void Dialog::ServiceStartedSlot()
{
	// Enable UI here.
}

void Dialog::FailedToStartServiceSlot()
{
	// Perform cleanup.
	if ( serverControl_ && serverThread_ )
	{
		serverControl_->Stop();
		serverThread_->Join();
		serverControl_ = NULL;
		serverThread_ = NULL;
	}

	// Notify UI. 
	RemotePC::Controls::Warning(
		this,
		"Critical error",
		"Server has failed to start. \nChange port value in application settings.");
}

void Dialog::ProveAuthenticationSlot(size_t cookie)
{
	MC_LOG_ROUTINE;
	mc::SemaphoreScopeRelease locker( events_->GetAuthSem() );

	try 
	{
		// Acquire client by id.
		RemotePC::Client client = serverControl_->GetClient(cookie);

		// Due to the fact that cookie maybe reused (as application in 'Release'
		// mode may allocate objects of the same time at the same regions), 
		// it may occur so that client with such cookie has been already
		// authenticated. If so, we should just skip the procedure. It is fine.
		if ( client.IsAuthenticated() )
			return;

		// Check whether limit of free connected devices is reached.
		if ( !serverControl_->TestClient(client) )
			// This routine takes care about client disconnection itself.
			return;

		// Display auth dialog.
		currentAuth_ = cookie;
		AuthDialog auth(
			QString(client.GetIp().c_str()), 
			QString(client.GetId().c_str()), 
			QString(client.GetName().c_str()), 
			client.GetDevice(), 
			client.SupportPasswordProtection(),
			this);
		QObject::connect(this, SIGNAL(CloseAuthDialogSig()), &auth, SLOT(Close()));
		int result = auth.exec();
		currentAuth_ = 0;

		// Check whether user decided to remember current device.
		if ( AuthDialog::Remembered == result )
		{
			// Add new user to users manager.
			RemotePC::User user(client.GetId(), client.GetName(), client.GetIp(), true);
			user.SetPassword(auth.GetPassword().toUtf8().constData());
			RemotePC::UsersManager::Instance().AddUser(user);
			
			// Request password from the client.
			serverControl_->RequestPassword(cookie, user.IsAuthRequested());

			// Nothing else to do here. We are waiting for password without any 
			// UI on host side.
			return;
		}

		if ( !serverControl_->HasClient(cookie) )
			return;

		// Check whether limit of free connected devices is reached.
		if ( !serverControl_->TestClient(client) )
			// This routine takes care about client disconnection itself.
			return;

		// Server should now handle client connection.
		serverControl_->HandleClient( cookie, ( Accepted == result ) );
	}
	catch ( ... )
	{
		// Exception may be thrown if client record is not found or 
		// it is configured incorrectly (does not have auth message attached).
		// Should not do anything special here. Just quit.
	}
}

QTreeWidgetItem *Dialog::FindDevicesBranch(int device) 
{
	if ( RemotePC::AuthMessage::DEVICE_IPHONE == device )			
		return ui->connections_->topLevelItem( 0 );
	else if ( RemotePC::AuthMessage::DEVICE_ANDROID == device )			
		return ui->connections_->topLevelItem( 1 );
	throw std::exception();
}

void Dialog::ShowDeviceSlot(size_t cookie)
{
	MC_LOG_ROUTINE;
	try 
	{
		RemotePC::Client client = serverControl_->GetClient(cookie);
	
		QTreeWidgetItem * devicesBranch = FindDevicesBranch(client.GetDevice());
		QTreeWidgetItem * deviceItem = 
			new QTreeWidgetItem( devicesBranch, QStringList( QString(client.GetName().c_str()) ) );
        deviceItem->setData( 0, Qt::UserRole, (quint64)cookie );

		ui->connections_->expandAll();
	}
	catch ( ... )
	{
		// Exception may be thrown if client record is not found or 
		// it is configured incorrectly (does not have auth message attached).
		// Should not do anything special here. Just quit.
	}
}

void Dialog::RemoveDevice(size_t cookie, int branch)
{
	QTreeWidgetItem * iPhones = ui->connections_->topLevelItem( branch );
	for ( int i = 0 ; i < iPhones->childCount() ; ++i )
	{
		QTreeWidgetItem * childDevice = iPhones->child( i );
        if ( childDevice->data(0, Qt::UserRole) == (quint64)cookie )
		{
			iPhones->removeChild( childDevice );
			return;
		}
	}
}

void Dialog::HideDeviceSlot(size_t cookie)
{
	MC_LOG_ROUTINE;
    if ( cookie == currentAuth_ )
	{
		emit CloseAuthDialogSig();
		return;
	}

	RemoveDevice(cookie, 0);
	RemoveDevice(cookie, 1);
}

void Dialog::ShowOldiRemoteWarningSlot(size_t cookie)
{
	MC_LOG_ROUTINE;
	// Enable other modal notifications after this is closed.
	mc::SemaphoreScopeRelease locker( events_->GetAuthSem() );
	try
	{
		// Acquire client by id.
		RemotePC::Client client = serverControl_->GetClient(cookie);
		QString name(client.GetName().c_str());

		currentAuth_ = cookie;
		// Notify UI on iRemote incompatibility.
		QString messageTitle("iRemote compatibility error");
		QString messageText =
			QString("iRemote Suite software on your device ('") + name + QString("') is too old.") +
			QString("\nProceed to AppStore to check for the latest version of the software.");
		QMessageBox warningBox(QMessageBox::Warning, messageTitle, messageText);
#if defined(__MACH__)
		warningBox.setText(QString("<b>")+messageTitle+QString("</b>"));
		warningBox.setInformativeText(messageText);
#endif // Platform
		QObject::connect(this, SIGNAL(CloseAuthDialogSig()), &warningBox, SLOT(accept()));
		warningBox.exec();
		currentAuth_ = 0;

		// Disconnect client.
		serverControl_->HandleClient( cookie, false );
	}
	catch ( ... )
	{
		// Exception may be thrown if client record is not found or
		// it is configured incorrectly (does not have auth message attached).
		// Should not do anything special here. Just quit.
	}
}

void Dialog::ServiceStarted()
{
	MC_LOG_ROUTINE;
	emit ServiceStartedSig();
}

void Dialog::FailedToStartService()
{
	MC_LOG_ROUTINE;
	emit FailedToStartServiceSig();
}

void Dialog::ProveAuthentication(size_t cookie)
{
	MC_LOG_ROUTINE;
    emit ProveAuthenticationSig(cookie);
}

void Dialog::ShowDevice(size_t cookie)
{
	MC_LOG_ROUTINE;
	emit ShowDeviceSig(cookie);
}

void Dialog::HideDevice(size_t cookie)
{
	MC_LOG_ROUTINE;
	emit HideDeviceSig(cookie);
}

void Dialog::ShowOldiRemoteWarning(size_t cookie)
{
	MC_LOG_ROUTINE;
	emit ShowOldiRemoteWarningSig(cookie);
}

void Dialog::CheckAuth()
{
	MC_LOG_ROUTINE;

	try 
	{
		// Acquire client record. 
		RemotePC::Client client = serverControl_->GetClient(currentAuth_);
		if ( !serverControl_->TestClient(client) )
			// Authentication should be terminated if client does not satisfy 
			// at least one criterion. 
			emit CloseAuthDialogSig();
	}
	catch ( ... )
	{
		// Exception may be thrown if client record is not found or 
		// it is configured incorrectly (does not have auth message attached).
		
		// There is no sense to continue authentication in this case.
		emit CloseAuthDialogSig();
	}
}

void Dialog::disconnect() 
{
	QList<QTreeWidgetItem *> selectedItems = ui->connections_->selectedItems();
	if ( 1 != selectedItems.size() )
		return;
	QTreeWidgetItem * selectedItem = selectedItems.at(0);
	size_t cookie = selectedItem->data(0, Qt::UserRole).toInt();
	if ( 0 == cookie )
		return;
	serverControl_->DisconnectClient(cookie);
}

void Dialog::settings()
{
	// Remember current settings.
	std::string service = RemotePC::Config::Instance().GetService();
	bool screenshotsMode = RemotePC::Config::Instance().ShareScreenshots();
	float fps = RemotePC::Config::Instance().GetFPS();

	// Display settings dialog.
	SettingsDialog settings(this);
	int result = settings.exec();
	if ( Accepted != result )
		return;

	// Check whether:
	// - service is changed 
	// - or service is not started
	// and apply changes.
	if ( ( RemotePC::Config::Instance().GetService() != service ) ||
		( !serverControl_ ) )
	{
		RestartService();
	}
	// Check whether screenshots sharing mode is changed.
    if ( RemotePC::Config::Instance().ShareScreenshots() != screenshotsMode )
	{
		ResetTimer();
	}
	// Check whether fps is changed.
	if ( RemotePC::Config::Instance().GetFPS() != fps )
	{
		srcTimer_->setInterval( GetTimerInterval() );
	}

    // Complete apply procedure.
    RemotePC::Config::Instance().FlushFlags();
}

void Dialog::showUsersMng()
{
	UsersMngDialog usersMng(this);
	usersMng.exec();
}

void Dialog::updateScreenshot()
{
	MC_LOG_ROUTINE;
	if ( !serverControl_ )
		return;
    serverControl_->HandleAuthorized( mc::Class< RemotePC::ScreenshotSpreader >::Create() );
}

void Dialog::updateVolume()
{
	MC_LOG_ROUTINE;
	if ( !serverControl_ )
		return;

	// Acquire current volume value.
	unsigned long value = 0;
	RemotePC::HardwareProvider::Instance().GetVolumeControl()->GetNormalizedVolume(value);
	// Construct message do spread new value.
	mc::IMessagePtr message(
		mc::Class< RemotePC::VolumeMessage >::Create(
		RemotePC::VolumeMessage::DEVICE_MASTER, value ) );
	// Send message to all authenticated clients.
	serverControl_->SpreadMessage( message );
}

void Dialog::help()
{
	QDir appDir(QApplication::applicationDirPath());	
#if defined(WIN32)
	QString helpFile = 
		QString("file:///") + 
		appDir.absolutePath() + 
		QString("/help/index.htm");
#elif defined(__MACH__)
	appDir.cdUp();
	QString helpFile = 
		QString("file://") +
		appDir.absolutePath() + 
		QString("/Resources/help/index.htm");
#endif
	QDesktopServices::openUrl(helpFile);
}

void Dialog::about()
{	
	AboutDialog about(this);
	about.exec();	
}

void Dialog::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
		if ( isVisible() )
			hide();
		else
			show();
		break;
	default:
		break;
	}
}

void Dialog::closeEvent(QCloseEvent *event)
{
	if (trayIcon_->isVisible()) 
	{
		hide();
		event->ignore();
	}
}

void Dialog::reject()
{
	if (trayIcon_->isVisible()) 
	{
		hide();
	}
}

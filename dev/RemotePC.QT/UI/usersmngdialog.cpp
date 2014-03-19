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

#include "UsersMngDialog.h"
#include "ui_usersmngdialog.h"

#include "edituserdialog.h"

// UI tools.
#include "UIConfig.h"
#include "Controls.h"

// Users manager support
#include "Server/UsersManager/User.h"
#include "Server/UsersManager/DUsersManager.h"
#include "Server/UsersManager/UsersManager.h"

// Events dispatcher
#include "Annunciators/UsersEvents.h"

UsersMngDialog::UsersMngDialog(QWidget *parent) 
	: QDialog(parent, RemotePC::UIConfig::dialogFlags_)
	, m_ui(new Ui::UsersMngDialog)
	, events_()
{
    m_ui->setupUi(this);
	ShowAllUsers();
	// Configure events dispatcher.
	events_ = mc::Class< RemotePC::UsersEvents >::Create( this );
	RemotePC::UsersManager::Instance().Advise(events_);
}

UsersMngDialog::~UsersMngDialog()
{
	// Unadvise local dispatcher from users manager events before destructing 
	// component UI.
	RemotePC::UsersManager::Instance().Unadvise(events_);
	delete m_ui;
}

void UsersMngDialog::AddUser(size_t userCookie)
{
	RemotePC::User& user = RemotePC::UsersManager::Instance().GetUserByCookie(userCookie);
	ShowUser(QString(user.GetDeviceName().c_str()), user.GetCookie());
}

void UsersMngDialog::ItemDoubleClicked( QTreeWidgetItem * /* item */, int /* column */ )
{
	EditUser();
}

void UsersMngDialog::EditUser()
{
	// Find selected item
	QList<QTreeWidgetItem *> selectedItems = m_ui->users_->selectedItems();
	if ( 1 != selectedItems.size() )
		return;
	QTreeWidgetItem * selectedItem = selectedItems.at(0);
    size_t userCookie = selectedItem->data(0, Qt::UserRole).toULongLong();
	if ( 0 == userCookie )
		return;

	// User settings to be acquired.
	QString deviceName;
	QString deviceId;
	bool requireIp;
	QString ipAddress;

	// Acquire user settings.
	try
	{
		// Lock users manager for reading.
		mc::IReadWriteLock::Reader_ locker(RemotePC::UsersManager::Instance().GetLock());	
		RemotePC::User& user = RemotePC::UsersManager::Instance().GetUserByCookie(userCookie);
		deviceName = QString( user.GetDeviceName().c_str() );
		deviceId = QString( user.GetDeviceId().c_str() );
		requireIp = user.IsIpRequired();
		ipAddress = QString( user.GetIp().c_str() );
	}
	catch ( ... )
	{
		return;
	}

	// Display user edit dialog.
	EditUserDialog userdit(deviceName, deviceId, requireIp, ipAddress, this);
	int result = userdit.exec();
	if ( Accepted != result )
		return;

	// Apply new settings for the user.
	try
	{
		// Lock users manager for reading.
		mc::IReadWriteLock::Reader_ locker(RemotePC::UsersManager::Instance().GetLock());	

		RemotePC::User& user = RemotePC::UsersManager::Instance().GetUserByCookie(userCookie);
		std::string password(userdit.GetPassword().toUtf8().constData());
		std::string ip(userdit.GetIp().toUtf8().constData());
		bool ipRequired = userdit.IsIpRequired();
		user.SetIp(ip);		
		user.SetIpRequired(ipRequired);		
		// Apply new password only if it is not empty.
		if ( 0 != password.size() )
			user.SetPassword(password);		
	}
	catch ( ... )
	{
	}	
}

void UsersMngDialog::RemoveUser()
{
	// Find selected item
	QList<QTreeWidgetItem *> selectedItems = m_ui->users_->selectedItems();
	if ( 1 != selectedItems.size() )
		return;
	QTreeWidgetItem * selectedItem = selectedItems.at(0);
    size_t userCookie = selectedItem->data(0, Qt::UserRole).toULongLong();
	if ( 0 == userCookie )
		return;

	// User settings to be acquired.
	QString deviceName;

	// Acquire user settings.
	try
	{
		// Lock users manager for reading.
		mc::IReadWriteLock::Reader_ locker(RemotePC::UsersManager::Instance().GetLock());	
		RemotePC::User& user = RemotePC::UsersManager::Instance().GetUserByCookie(userCookie);
		deviceName = QString( user.GetDeviceName().c_str() );
	}
	catch ( ... )
	{
		return;
	}

	// Prepare warning message.
	QString warningTitle("Confirm device removal");
	QString warningMessage 
		= QString("Authentication rule for '")
		+ deviceName
		+ QString("' device will be permanently removed from the database. Are you sure that you want to proceed?");

	// Ask for confirmation.
	int button = RemotePC::Controls::Question(this, warningTitle, warningMessage);
	if ( QMessageBox::Yes != button ) 
		// Removal was not confirmed.
		return;

	// Remove item from tree.
	int itemIndex = m_ui->users_->indexOfTopLevelItem(selectedItem);
	m_ui->users_->takeTopLevelItem(itemIndex);
	// Remove user from object model.
	RemotePC::UsersManager::Instance().RemoveUser(userCookie);
}

void UsersMngDialog::ShowAllUsers()
{
	// Lock users manager for reading.
	mc::IReadWriteLock::Reader_ locker(RemotePC::UsersManager::Instance().GetLock());	

	size_t usersCount = RemotePC::UsersManager::Instance().GetUsersCount();
	for ( size_t i = 0 ; i < usersCount ; ++i )
	{
		RemotePC::User& user = RemotePC::UsersManager::Instance().GetUser(i);
		ShowUser(QString(user.GetDeviceName().c_str()), user.GetCookie());
	}		
}

void UsersMngDialog::ShowUser(const QString& deviceName, size_t userCookie)
{
	QTreeWidgetItem * userItem = new QTreeWidgetItem( 
		m_ui->users_, 
		QStringList( deviceName ) );
    userItem->setData( 0, Qt::UserRole, (quint64)userCookie );
}

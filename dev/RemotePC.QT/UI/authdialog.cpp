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

#include "authdialog.h"
#include "ui_authdialog.h"

// Child forms
#include "rememberuserdialog.h"

// UI tools
#include "UIConfig.h"
#include "Controls.h"

AuthDialog::AuthDialog(const QString& ip, const QString& id, const QString& name, int /* device */, bool pswSupported, QWidget *parent) 
	: QDialog(parent, RemotePC::UIConfig::alwaysOnTopdialogFlags_)
	, m_ui(new Ui::AuthDialog)
	, pswSupported_(pswSupported)
	, password_()
{
    m_ui->setupUi(this);

	m_ui->ipAddress_->setText(ip);
	m_ui->deviceName_->setText(name);
	m_ui->deviceId_->setText(id);
}

AuthDialog::~AuthDialog()
{
    delete m_ui;
}

void AuthDialog::Close()
{
	emit Closed();
	done( Rejected );
}

void AuthDialog::allowed()
{
	done( Accepted );
}

void AuthDialog::remembered()
{
	// Check whether device software supports password protection.
	if ( !pswSupported_ )
	{
		RemotePC::Controls::Warning(
			this,
			"Compatibility error",
			"Please, update your iRemote Suite software to get benefit of this feature.");
		return;
	}

	// Initiate user remember procedure.
	RememberUserDialog rememberUser(this);
	QObject::connect(this, SIGNAL(Closed()), &rememberUser, SLOT(Close()));
	int result = rememberUser.exec();
	if ( Accepted != result )
		return;
	password_ = rememberUser.GetPassword();
	done( Remembered );
}

void AuthDialog::denied()
{
	done( Rejected );
}

void AuthDialog::reject()
{
	// This dialog cannot be rejected.
}

QString AuthDialog::GetPassword()
{
	return password_;
}

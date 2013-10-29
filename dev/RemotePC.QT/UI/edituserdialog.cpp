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

#include "edituserdialog.h"

// UI tools
#include "Controls.h"

EditUserDialog::EditUserDialog(const QString& deviceName, const QString& deviceId, bool requireIp, const QString& ip, QWidget *parent)
	: QDialog(parent)
	, ui_(new Ui::EditUserDialog())
{
	ui_->setupUi(this);
	ui_->deviceName_->setText(deviceName);
	ui_->deviceId_->setText(deviceId);
	ui_->password_->setFocus();
	ui_->associateWithIp_->setCheckState( requireIp ? Qt::Checked : Qt::Unchecked );
	ui_->ipAddress_->setText(ip);
}

EditUserDialog::~EditUserDialog()
{
	delete ui_;
}

QString EditUserDialog::GetPassword()
{
	return ui_->password_->text();
}

bool EditUserDialog::IsIpRequired()
{
	return ( ui_->associateWithIp_->checkState() == Qt::Checked );
}

QString EditUserDialog::GetIp()
{
	return ui_->ipAddress_->text();
}

void EditUserDialog::accept()
{
	// Warning code is disabled because empty password leads to keeping old 
	// value.
	/* if ( 0 == ui_->password_->text().size() )
	{
		QMessageBox::warning(
			this,
			"Error in password field",
			"Password cannot be empty.",
			QMessageBox::Ok, QMessageBox::Ok );
		ui_->password_->setFocus();
		return;
	} */
	if ( 0 == ui_->ipAddress_->text().size() )
	{
		RemotePC::Controls::Warning(
			this,
			"Error in IP address field",
			"Fill IP address field correctly (\"a.b.c.d\").");
		ui_->ipAddress_->setFocus();
		return;
	}	
	QDialog::accept();
}

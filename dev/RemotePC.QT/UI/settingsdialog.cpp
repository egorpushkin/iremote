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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

// UI tools
#include "UIConfig.h"
#include "Controls.h"

// Application persistent settings.
#include "../Server/Config.h"

#include <sstream>

SettingsDialog::SettingsDialog(QWidget *parent) 
	: QDialog(parent, RemotePC::UIConfig::dialogFlags_)
	, m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

	m_ui->service_->setText( RemotePC::Config::Instance().GetService().c_str() );
	m_ui->screenshotsMode_->setCheckState( RemotePC::Config::Instance().ShareScreenshots() ? Qt::Checked : Qt::Unchecked );
	m_ui->screenshotsFPS_->setText( QString::number( RemotePC::Config::Instance().GetFPS() ) );
    m_ui->sfbCompr_->setValue( RemotePC::Config::Instance().GetSFBCompression() );
	compressionChanged( RemotePC::Config::Instance().GetSFBCompression() );
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::apply()
{
	// Check whether user made any changes and attempt to apply it.

	// 1. Service.
    std::string service( m_ui->service_->text().toUtf8() );
	if ( RemotePC::Config::Instance().GetService() != service )
	{
		// Check whether new service is appropriate
		int serviceCode = 0;
		std::stringstream str;
		str << service;
		str >> serviceCode;

		if ( serviceCode < 1024 || serviceCode >= 65536 )
		{
			RemotePC::Controls::Warning(
				this,
				"Error in service name",
				"Wrong service name specified. \nYou may use values from range: [1024; 65535].");
            m_ui->tabs_->setCurrentIndex(0);
			m_ui->service_->setFocus();
			return;
		}

		// Save new value.
		RemotePC::Config::Instance().SetService( service );
	}

	// 2. Screenshots
    bool screenshotsMode = ( m_ui->screenshotsMode_->checkState() == Qt::Checked );
	if ( RemotePC::Config::Instance().ShareScreenshots() != screenshotsMode )
	{
		// Save new value.
		RemotePC::Config::Instance().SetScreenshotsSharing( screenshotsMode );
	}

	// 3. FPS.
    float fps = m_ui->screenshotsFPS_->text().toFloat();
	if ( RemotePC::Config::Instance().GetFPS() != fps )
	{
		// Check whether new value is appropriate
		if ( fps > 20.0f )
		{
			RemotePC::Controls::Warning(
				this,
				"Error in fps value",
				"You've specified too high fps value. \nYou may use frame rates up to 20.");
            m_ui->tabs_->setCurrentIndex(0);
			m_ui->screenshotsFPS_->setFocus();
			return;
		}
		if ( fps <= 0.0f )
		{
			RemotePC::Controls::Warning(
				this,
				"Error in fps value",
				"FPS should be positive floating point value.");
            m_ui->tabs_->setCurrentIndex(0);
			m_ui->screenshotsFPS_->setFocus();
			return;
		}

		// Save new value.
		RemotePC::Config::Instance().SetFPS( fps );
	}

    // 4. SFB. Compression level.
    int sfbCompr = m_ui->sfbCompr_->value();
    if ( RemotePC::Config::Instance().GetSFBCompression() != sfbCompr )
    {
        // Save new value.
        RemotePC::Config::Instance().SetSFBCompression(sfbCompr);
    }

    // Save config file.
    RemotePC::Config::Instance().Save();

	// Close the form.
	done( Accepted );
}

void SettingsDialog::compressionChanged(int value)
{
    m_ui->sfbComprLabel_->setText( QString::number( value ) + QString("%") );
}

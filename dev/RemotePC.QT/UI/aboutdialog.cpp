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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "Versions/Versions.h"
#include "UIConfig.h"

#include <QUrl>
#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent) 
	: QDialog(parent, RemotePC::UIConfig::dialogFlags_)
	, m_ui(new Ui::AboutDialog)
{
    m_ui->setupUi(this);
    // Format version.
    m_ui->version_->setText(
        QString::number(RemotePC::REMOTEPC_VER_MAJOR) + QString(".")
        + QString::number(RemotePC::REMOTEPC_VER_MINOR) + QString(".")
        + QString::number(RemotePC::REMOTEPC_VER_REV) + QString(" ")
        + QString::fromUtf8(RemotePC::RemotePCBuildStatus.c_str()) );
}

void AboutDialog::VisitHomepage()
{
	QString url("http://www.scientific-soft.com/mobile/apps/iremote/");
	QDesktopServices::openUrl(url);	
}

void AboutDialog::VisitAppStore()
{
	QString url("http://itunes.apple.com/WebObjects/MZStore.woa/wa/viewSoftware?id=312222737&mt=8");
	QDesktopServices::openUrl(url);	
}

void AboutDialog::VisitAndroidMarket()
{
    QString url("https://market.android.com/details?id=com.scientificsoft.iremote.android.iremote");
    QDesktopServices::openUrl(url);
}

AboutDialog::~AboutDialog()
{
    delete m_ui;
}

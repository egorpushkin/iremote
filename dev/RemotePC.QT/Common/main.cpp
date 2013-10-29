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

#include <QtGui/QApplication>
#include <QDir>
#include "UI/dialog.h"
#include "UI/UIConfig.h"

// Persistent application configuration. 
#include "Server/Config.h"

// Shell support.
#include "Server/Shell/ShellSupport.h"

// Instance locking mechanism.
#if defined(__MACH__)
#include <boost/interprocess/sync/file_lock.hpp>
#endif // Platform
#include "InstanceLock.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	// Check whether another instance of the application is already running.
	RemotePC::InstanceLockPtr instanceLock = RemotePC::InstanceLock::SharedLock();
	if ( !instanceLock )
	{
		// TODO: Notify another instance on this attempt ...
		// .. and quit.
		return 0;
	}

	// Configure default style sheet.
	app.setStyleSheet(RemotePC::UIConfig::globalStylesheet_);

	// Prevents from terminating application when:
	//  main window is hidden to tray and user closes any other window.
	app.setQuitOnLastWindowClosed(false);

	// Configure qt environment.
#if !defined(IREMOTE_NO_QT_PLUGINS)
	// Specify subdirectory where plugins are located.
#	if defined(WIN32)
	// QApplication::addLibraryPath("./plugins");
	QDir pluginsDir(QApplication::applicationDirPath());
	pluginsDir.cd("plugins");
	QApplication::setLibraryPaths(QStringList(pluginsDir.absolutePath()));
#	elif defined(__MACH__)
	QDir pluginsDir(QApplication::applicationDirPath());
	pluginsDir.cdUp();
	pluginsDir.cd("plugins");
	QApplication::setLibraryPaths(QStringList(pluginsDir.absolutePath()));
#endif // Platform
#endif //Plugins

	// Load application settings.
	QDir configDir(QApplication::applicationDirPath());
	RemotePC::Config::Instance().Load(configDir.absolutePath().toUtf8().constData());

	// Launch shell service.
	RemotePC::ShellHost::Instance().Launch();

	// Display application UI.
    Dialog w;
    w.show();
    int retCode = app.exec();

	// Stop shell service.
	RemotePC::ShellHost::Instance().Exit();

	// Save config when application is subject to close.
	RemotePC::Config::Instance().Save();

	return retCode;
}

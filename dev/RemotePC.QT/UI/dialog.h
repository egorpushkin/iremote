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

#ifndef DIALOG_H
#define DIALOG_H

#include "Common/Common.h"

#include <QDialog>
#include <QTreeWidgetItem>
#include <QSystemTrayIcon>
#include <QMutex>

#include "Server/Client.h"
#include "Server/IClientHandler.h"
#include "Server/Services/Services.h"
#include "Server/IServerControl.h"
#include "Server/IUIEvents.h"

namespace Ui
{
	class DialogClass;
}

class Dialog : public QDialog
{
	Q_OBJECT

public:
	Dialog(QWidget *parent = 0);
	~Dialog();

	void ServiceStarted();
	void FailedToStartService();
	void ProveAuthentication(size_t cookie);
	void ShowDevice(size_t cookie);
	void HideDevice(size_t cookie);
	void CheckAuth();
	void ShowOldiRemoteWarning(size_t cookie);

private:

	QTreeWidgetItem *FindDevicesBranch(int device);
	void RemoveDevice(int cookie, int branch);

private slots:

	void ServiceStartedSlot();
	void FailedToStartServiceSlot();
	void ProveAuthenticationSlot(int cookie);
	void ShowDeviceSlot(int cookie);
	void HideDeviceSlot(int cookie);
	void ShowOldiRemoteWarningSlot(int cookie);
	void settings();
	void help();
	void about();
	void disconnect();
	void showUsersMng();
	void updateScreenshot();
	void updateVolume();
	void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
	
signals:

	void ServiceStartedSig();
	void FailedToStartServiceSig();
	void ProveAuthenticationSig(int cookie);
	void ShowDeviceSig(int cookie);
	void HideDeviceSig(int cookie);
	void CloseAuthDialogSig();
	void ShowOldiRemoteWarningSig(int cookie);

protected:

	void ResetTimer();
	int GetTimerInterval();
	void StopService();
	void RestartService();

protected:

	void reject();
	void closeEvent(QCloseEvent *event);

private:
	Ui::DialogClass *ui;

	/** System tray resources. */
	QSystemTrayIcon *trayIcon_;
	QMenu *trayIconMenu_;
	QAction *quitAction_;

	RemotePC::IUIEventsPtr events_;
	mc::IThreadPtr serverThread_;
	RemotePC::IServerControlPtr serverControl_;
    RemotePC::IServiceHandlerPtr fbService_;

    /**
      * Contains cookie corresponding to currently opened authentication
      * session.
      */
	size_t currentAuth_;

	/** Fires any time when screenshot should be spread. */
	QTimer *srcTimer_;

	/**
	 * Fires periodically to notify devices on volume changes. This changes are
	 * no longer sent immediately (in response to MM_MIXM_CONTROL_CHANGE message)
	 * because this leads to incorrect behavior of volume slider on device side
	 * (on particular device where user manipulates slider).
	 */
	QTimer *volumeTimer_;
};

#endif // DIALOG_H

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

#include "Controls.h"

namespace RemotePC
{

	void Controls::Warning(QWidget *parent, const QString& title, const QString& text)
	{
		Controls::Question(parent, title, text, QMessageBox::Ok, QMessageBox::Ok, QMessageBox::Information);
	}

	int Controls::Question(QWidget *parent, const QString& title, const QString& text,
		QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton, QMessageBox::Icon icon)
	{
#if defined(WIN32)
		QMessageBox msgBox(icon, title, text, buttons, parent);
		msgBox.setDefaultButton(defaultButton);
		return msgBox.exec();
#elif defined(__MACH__)
		QString formattedTitle
			= QString("<b>")
			+ title
			+ QString("</b>");
		QMessageBox msgBox(parent);
		msgBox.setText(formattedTitle);
		msgBox.setInformativeText(text);
		msgBox.setStandardButtons(buttons);
		msgBox.setDefaultButton(defaultButton);
		msgBox.setIcon(icon);
		return msgBox.exec();
#endif // Platform
	}

}

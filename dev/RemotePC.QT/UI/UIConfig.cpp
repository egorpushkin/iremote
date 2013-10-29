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

#include <Qt>
#include <QString>

#include "UIConfig.h"

namespace RemotePC
{

	Qt::WindowFlags UIConfig::dialogFlags_ 
		= Qt::Dialog 
		| Qt::WindowTitleHint 
		| Qt::WindowSystemMenuHint 
		| Qt::WindowCloseButtonHint;

	Qt::WindowFlags UIConfig::alwaysOnTopdialogFlags_
		= Qt::Dialog 
		| Qt::WindowTitleHint 
		| Qt::WindowSystemMenuHint 
		| Qt::WindowCloseButtonHint
// TODO: Replace __LINUX__ symbol with the one identifying X11 manager.
#if defined(__LINUX__)
		// Note that on some window managers on X11 you also have to pass 
		// Qt::X11BypassWindowManagerHint for this flag to work correctly.
		| Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint;
#else
		| Qt::WindowStaysOnTopHint;
#endif

QString UIConfig::globalStylesheet_ =
    QString("															\
        QDialog {														\
            background-color: rgb(58,58,58);							\
        }																\
        QWidget {														\
            color: rgb(255,255,255);									\
        }																\
        QTreeView {														\
            border: 1px solid gray;										\
            border-radius: 4px;											\
            padding-top: 4px;											\
        }																\
        QTreeView QHeaderView::section {								\
            background: rgb(58,58,58);									\
            border-radius: 4px;											\
            border-bottom: 1px dashed gray;								\
            padding: 0px 4px 4px 8px;									\
        }																\
        QScrollBar {													\
        }																\
        QScrollBar:vertical {											\
            width: 8px;													\
        }																\
        QScrollBar::handle:vertical {									\
            background: rgb(126,126,126);								\
            min-height: 50px;											\
            margin: 26px 0px 4px 0px;									\
            border-width: 1px;											\
            border-style: solid;										\
            border-radius: 4px;											\
        }																\
        QScrollBar::add-line:vertical {									\
            border: none;												\
            height: 0px;												\
        }																\
        QScrollBar::sub-line:vertical {									\
            border: none;												\
            height: 0px;												\
        }																\
        QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {\
            border: none;												\
            width: 0px;													\
            height: 0px;												\
        }																\
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {	\
            background: none;											\
            height: 0px;												\
        }																\
        QMessageBox {													\
            messagebox-text-interaction-flags: 0;						\
        }																\
        QLineEdit {														\
            border: 1px solid gray;									\
            border-radius: 4px;											\
            selection-background-color: darkgray;						\
            padding: 1px 1px 2px 1px;									\
        }																\
        QCheckBox::indicator {											\
            border: 1px solid gray;										\
            border-radius: 4px;											\
            width: 13px;												\
            height: 13px;												\
        }																\
        QCheckBox::indicator:unchecked:pressed {						\
            background: rgb(100,100,100);								\
        }																\
        QCheckBox::indicator:checked {									\
            image: url(:/Resources/CheckBoxIndicator.png);				\
        }																\
        QCheckBox::indicator:checked:pressed {							\
            background: rgb(100,100,100);								\
        }																\
    ")
#if defined(WIN32)
    + QString("															\
        QPushButton {													\
            background-image: url(:/Resources/ButtonNormal.png);		\
            border: none;												\
            width: 75px;												\
            height: 25px;												\
            color: rgb(255, 255, 255);									\
            font: bold 8pt \"MS Shell Dlg 2\";							\
        }																\
        QPushButton:pressed {											\
            padding-top: 2px;											\
            padding-left: 2px;											\
        }																\
        QSlider::groove:horizontal {									\
            background:#303030;											\
            border: 1px inset #202020;									\
            height: 2px;												\
            padding:0 1px;												\
            border-radius:2px;											\
        }																\
        QSlider::handle:horizontal {									\
            background:#c0c0c0;											\
            border: 1px solid #606060;									\
            width: 13px;												\
            margin: -3px 0;												\
            border-radius: 4px;											\
        }																\
        QComboBox {														\
            border: 1px solid gray;										\
            border-radius: 4px;											\
            padding: 1px 18px 1px 10px;									\
            min-width: 6em;												\
        }																\
        QComboBox:editable {											\
            background: gray;											\
        }																\
        QComboBox:!editable, QComboBox::drop-down:editable {			\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,	\
                stop: 0 #515151, stop: 0.5 #333333, stop: 1.0 #333333);\
        }																\
        QComboBox:!editable:on, QComboBox::drop-down:editable:on {		\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,	\
                stop: 0 #333333, stop: 0.5 #404040, stop: 1.0 #404040);\
        }																\
        QComboBox::drop-down {											\
            subcontrol-origin: padding;									\
            subcontrol-position: top right;								\
            width: 15px;												\
            border-left-width: 1px;										\
            border-left-color: gray;									\
            border-left-style: dashed;									\
            border-top-right-radius: 3px;								\
            border-bottom-right-radius: 3px;							\
        }																\
        QComboBox::down-arrow {											\
            image: url(:/Resources/ComboBoxDown.png);					\
        }																\
        QComboBox::down-arrow:on {										\
            top: 1px;													\
            left: 1px;													\
        }																\
        QComboBox QAbstractItemView {									\
            border: 1px solid gray;										\
            padding: 5px 18px 5px 10px;									\
            border-radius: 4px;											\
        }																\
    ");
#elif defined(__MACH__)
    + QString("															\
        QPushButton {													\
            background-image: url(:/Resources/ButtonNormal.png);		\
            border: none;												\
            width: 75px;												\
            height: 25px;												\
            color: rgb(255, 255, 255);									\
            font: bold 10pt \"Lucida Grande\";							\
        }																\
        QPushButton:pressed {											\
            padding-top: 2px;											\
            padding-left: 2px;											\
        }																\
    ");
#endif // Platform
}

::
:: Script to prepare Remote PC package contents.
::

:: Dependencies
set QT_PATH=c:\Dev\Qt\5.2.1\msvc2012_64_opengl
set VS_PATH=c:\Program Files (x86)\Microsoft Visual Studio 11.0

:: Copy VC++ runtime dependencies.
copy /Y "%VS_PATH%\VC\redist\x64\Microsoft.VC110.CRT\msvcp110.dll" .\release
copy /Y "%VS_PATH%\VC\redist\x64\Microsoft.VC110.CRT\msvcr110.dll" .\release

:: Copy QT libraries.
copy /Y %QT_PATH%\bin\icudt51.dll .\release
copy /Y %QT_PATH%\bin\icuin51.dll .\release
copy /Y %QT_PATH%\bin\icuuc51.dll .\release
copy /Y %QT_PATH%\bin\Qt5Core.dll .\release
copy /Y %QT_PATH%\bin\Qt5Gui.dll .\release
copy /Y %QT_PATH%\bin\Qt5Widgets.dll .\release
mkdir .\release\plugins\platforms
mkdir .\release\plugins\imageformats
copy /Y %QT_PATH%\plugins\platforms\qwindows.dll .\release\plugins\platforms
copy /Y %QT_PATH%\plugins\imageformats\qjpeg.dll .\release\plugins\imageformats

:: Copy help contents.
mkdir .\release\help
XCOPY ..\..\..\doc\RemotePCHelp\*.* .\release\help /E /I /Y /EXCLUDE:ignore.txt

:: Copy license stuff.
COPY /Y ..\..\..\doc\License\EULA.txt .\release
COPY /Y ..\..\..\doc\License\LIBS.txt .\release

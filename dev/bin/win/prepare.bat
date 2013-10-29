::
:: Script to prepare Remote PC package contents.
::

:: Copy VC++ runtime dependencies.
copy /Y "C:\Program Files\Microsoft Visual Studio 10.0\VC\redist\x86\Microsoft.VC100.CRT\msvcp100.dll" .\Release
copy /Y "C:\Program Files\Microsoft Visual Studio 10.0\VC\redist\x86\Microsoft.VC100.CRT\msvcr100.dll" .\Release

:: Copy QT libraries.
copy /Y C:\Qt\2010.05\qt\bin\QtCore4.dll .\Release
copy /Y C:\Qt\2010.05\qt\bin\QtGui4.dll .\Release
mkdir .\Release\plugins\imageformats
copy /Y C:\Qt\2010.05\qt\plugins\imageformats\qjpeg4.dll .\Release\plugins\imageformats

:: Copy help contents.
mkdir .\Release\help
XCOPY ..\..\..\doc\RemotePCHelp\*.* .\Release\help /E /I /Y /EXCLUDE:ignore.txt

:: Copy license stuff.
COPY /Y ..\..\..\doc\License\EULA.txt .\Release
COPY /Y ..\..\..\doc\License\LIBS.txt .\Release

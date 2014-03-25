#!/bin/bash
#
# This script prepares Remote PC Suite installation package content.
#  

# Settings
QT_VER="5.2.1"

# Clear environment.
rm -r build

# Create directory for the package content.
mkdir build
cd build
mkdir Applications
cd ..

# Copy latest release build to the build folder.
cp -R ./release/RemotePC.app ./build/Applications

# Clear config.xml.
rm ./build/Applications/RemotePC.app/Contents/MacOS/config.xml
echo > ./build/Applications/RemotePC.app/Contents/MacOS/config.xml

# Copy additional resources.
cp -R ./components/RemotePC.app ./build/Applications

# Copy license stuff.
cp ../../../doc/License/EULA.txt ./build/Applications/RemotePC.app/Contents/Resources
cp ../../../doc/License/LIBS.txt ./build/Applications/RemotePC.app/Contents/Resources

# Copy help.
cp -R ../../../doc/RemotePCHelp/ ./build/Applications/RemotePC.app/Contents/Resources/help

# Remove '.svn' folders.
cd build
find . -name .svn -print0 | xargs -0 rm -rf

# Remove '.DS_Store' files.
find . -name '*.DS_Store' -type f -delete
cd ..

# Copy QT libraries.
function copyQtLib {
    # $1 source
    # $2 destination
	mkdir -p $(dirname $2)
	cp $1 $2    
} 
copyQtLib "/Applications/Qt/$QT_VER/clang_64/lib/QtCore.framework/Versions/Current/QtCore" "./build/Applications/RemotePC.app/Contents/Frameworks/QtCore.framework/Versions/Current/QtCore"
copyQtLib "/Applications/Qt/$QT_VER/clang_64/lib/QtGui.framework/Versions/Current/QtGui" "./build/Applications/RemotePC.app/Contents/Frameworks/QtGui.framework/Versions/Current/QtGui"
copyQtLib "/Applications/Qt/$QT_VER/clang_64/lib/QtWidgets.framework/Versions/Current/QtWidgets" "./build/Applications/RemotePC.app/Contents/Frameworks/QtWidgets.framework/Versions/Current/QtWidgets"
copyQtLib "/Applications/Qt/$QT_VER/clang_64/lib/QtPrintSupport.framework/Versions/Current/QtPrintSupport" "./build/Applications/RemotePC.app/Contents/Frameworks/QtPrintSupport.framework/Versions/Current/QtPrintSupport"
copyQtLib "/Applications/Qt/$QT_VER/clang_64/plugins/platforms/libqcocoa.dylib" "./build/Applications/RemotePC.app/Contents/plugins/platforms/libqcocoa.dylib"
copyQtLib "/Applications/Qt/$QT_VER/clang_64/plugins/imageformats/libqjpeg.dylib" "./build/Applications/RemotePC.app/Contents/plugins/imageformats/libqjpeg.dylib"

# Fix dependencies in QT libraries.
function updateRef {
    # $1 library to be patched
    # $2 referred framework name
    install_name_tool -change /Applications/Qt/$QT_VER/clang_64/lib/$2.framework/Versions/5/$2 @executable_path/../Frameworks/$2.framework/Versions/Current/$2 $1
} 
updateRef "./build/Applications/RemotePC.app/Contents/MacOS/RemotePC" "QtWidgets"
updateRef "./build/Applications/RemotePC.app/Contents/MacOS/RemotePC" "QtGui"
updateRef "./build/Applications/RemotePC.app/Contents/MacOS/RemotePC" "QtCore"
updateRef "./build/Applications/RemotePC.app/Contents/Frameworks/QtWidgets.framework/Versions/Current/QtWidgets" "QtGui"
updateRef "./build/Applications/RemotePC.app/Contents/Frameworks/QtWidgets.framework/Versions/Current/QtWidgets" "QtCore"
updateRef "./build/Applications/RemotePC.app/Contents/Frameworks/QtGui.framework/Versions/Current/QtGui" "QtCore"
updateRef "./build/Applications/RemotePC.app/Contents/Frameworks/QtPrintSupport.framework/Versions/Current/QtPrintSupport" "QtCore"
updateRef "./build/Applications/RemotePC.app/Contents/Frameworks/QtPrintSupport.framework/Versions/Current/QtPrintSupport" "QtGui"
updateRef "./build/Applications/RemotePC.app/Contents/Frameworks/QtPrintSupport.framework/Versions/Current/QtPrintSupport" "QtWidgets"
updateRef "./build/Applications/RemotePC.app/Contents/plugins/platforms/libqcocoa.dylib" "QtCore"
updateRef "./build/Applications/RemotePC.app/Contents/plugins/platforms/libqcocoa.dylib" "QtGui"
updateRef "./build/Applications/RemotePC.app/Contents/plugins/platforms/libqcocoa.dylib" "QtWidgets"
updateRef "./build/Applications/RemotePC.app/Contents/plugins/platforms/libqcocoa.dylib" "QtPrintSupport"
updateRef "./build/Applications/RemotePC.app/Contents/plugins/imageformats/libqjpeg.dylib" "QtGui"
updateRef "./build/Applications/RemotePC.app/Contents/plugins/imageformats/libqjpeg.dylib" "QtCore"

# Change package content permissions.
sudo chown -R root:admin ./build
sudo chmod -R g+w ./build

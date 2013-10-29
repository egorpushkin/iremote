#!/bin/bash
#
# This script prepares Remote PC Suite installation package content.
#  

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
	mkdir -p $(dirname $2)
	cp $1 $2    
} 
copyQtLib "/Library/Frameworks/QtCore.framework/Versions/4/QtCore" "./build/Applications/RemotePC.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore"
copyQtLib "/Library/Frameworks/QtGui.framework/Versions/4/QtGui" "./build/Applications/RemotePC.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui"
copyQtLib "/Developer/Applications/Qt/plugins/imageformats/libqjpeg.dylib" "./build/Applications/RemotePC.app/Contents/plugins/imageformats/libqjpeg.dylib"

# Fix dependencies from QT libraries for the following assemblies:
#     RemotePC.app/Contents/MacOS/RemotePC
#         QtCore
#         QtGui
#     RemotePC.app/Frameworks/QtGui.framework/Versions/4/QtGui
#         QtCore
#     RemotePC.app/plugins/imageformats/libqjpeg.dylib
#         QtCore
#         QtGui
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore ./build/Applications/RemotePC.app/Contents/MacOS/RemotePC
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui ./build/Applications/RemotePC.app/Contents/MacOS/RemotePC
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore ./build/Applications/RemotePC.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore ./build/Applications/RemotePC.app/Contents/plugins/imageformats/libqjpeg.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui ./build/Applications/RemotePC.app/Contents/plugins/imageformats/libqjpeg.dylib

# Change package content permissions.
sudo chown -R root:admin ./build
sudo chmod -R g+w ./build

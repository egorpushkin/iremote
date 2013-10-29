#!/bin/bash
#
# This script creates Remote PC Suite distribution package (.dmg).
#  

# Clear environment.
rm -f ./pack.temp.dmg
rm -f ./RemotePCSetup.dmg

# Prepare background image.
cp -R ./components/.background ./build/Applications

# Create a R/W DMG.
hdiutil create -srcfolder "./build/Applications/" -volname "Remote PC Setup" -fs HFS+ \
	-fsargs "-c c=64,a=16,e=16" -format UDRW -size 102400k -anyowners pack.temp.dmg

# Mount the disk image.
device=$(hdiutil attach -readwrite -noverify -noautoopen "pack.temp.dmg" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')

# Set visual styles.
echo '
   tell application "Finder"
     tell disk "Remote PC Setup"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {300, 100, 898, 453}
           set theViewOptions to the icon view options of container window
           set arrangement of theViewOptions to not arranged
           set icon size of theViewOptions to 128
           set background picture of theViewOptions to file ".background:InstallerBackground.png"
           make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
           set position of item "RemotePC.app" of container window to {150, 190}
           set position of item "Applications" of container window to {435, 190}
           close
           open
           update without registering applications
           delay 5
     end tell
   end tell
' | osascript

# Finalize DMG.
sudo chown -R root:admin /Volumes/"Remote PC Setup"
sudo chmod -Rf g+w /Volumes/"Remote PC Setup"
sync
hdiutil detach ${device} 
hdiutil convert "./pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o "RemotePCSetup.dmg"
rm -f ./pack.temp.dmg 

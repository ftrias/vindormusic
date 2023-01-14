#!/bin/sh
# npm install --global create-dmg
rm vindorplayer-1.0.0.dmg
create-dmg ../Builds/MacOSX/build/Release/VindorPlayer.app
# for compatability before 10.11
hdiutil convert VindorPlayer-1.0.0.dmg -format UDZO -o V.dmg
mv V.dmg VindorPlayer-1.0.0.dmg
codesign -s ftrias@vindormusic.com --keychain ~/Library/Keychains/login.keychain VindorPlayer-1.0.0.dmg

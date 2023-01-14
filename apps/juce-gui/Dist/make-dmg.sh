#!/bin/sh
# npm install --global create-dmg
OUT=Vindor-1.0.5.dmg
IN=./Vindor.app
ls -lR $IN
rm $OUT
create-dmg $IN
hdiutil convert $OUT -format UDZO -o tmp.dmg
mv tmp.dmg $OUT
codesign -s "Mac Developer: ftrias@vindormusic.com" --keychain ~/Library/Keychains/login.keychain $OUT

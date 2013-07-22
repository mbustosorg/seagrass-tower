#! /usr/bin/env bash

svn update
svn list -v

echo '----Committing Changes----'
svn commit project_sync.sh -m 'Auto-sync commit' --password mauricio
svn commit src/furSwarmMember.ino -m 'Auto-sync commit' --password mauricio
svn commit build/lib/arduino/makefile -m 'Auto-sync commit' --password mauricio
svn commit build/lib/teensy/makefile -m 'Auto-sync commit' --password mauricio
svn commit build/tower/makefile -m 'Auto-sync commit' --password mauricio
svn commit build/hat/makefile -m 'Auto-sync commit' --password mauricio
svn commit build/vest/makefile -m 'Auto-sync commit' --password mauricio
svn commit build/towerVest/makefile -m 'Auto-sync commit' --password mauricio
FILES=./src/*.cpp
for f in $FILES
do
	svn commit $f -m 'Auto-sync commit' --password mauricio
done
FILES=./src/*.h
for f in $FILES
do
	svn commit $f -m 'Auto-sync commit' --password mauricio
done

echo '----Archive Offsite----'
echo '----Clear Previous Deployment----'
rm -rf /Users/mauricio/Dropbox/apps/furSwarmMember/
echo '----Publish----'
cp -R . /Users/mauricio/Dropbox/apps/furSwarmMember/

echo '----Update Documentation----'
/Applications/Development/Doxygen.app/Contents/Resources/doxygen ~/Documents/development/furSwarmMember/doxygen.config
echo '----Clear Previous Deployment----'
rm -rf /Users/mauricio/Dropbox/apps/furSwarmDocumentation
echo '----Publish----'
cd ../furSwarmDocumentation
cp -R . /Users/mauricio/Dropbox/apps/furSwarmDocumentation

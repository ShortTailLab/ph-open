#!/bin/sh
# Auto Increment Version Script
# set CFBundleVersion to 1.0.1 first!!!
# the perl regex splits out the last part of a build number (ie: 1.1.1) and increments it by one
# if you have a build number that is more than 3 components, add a '\.\d+' into the first part of the regex.
plist=${INFOPLIST_FILE}
build_plist=$BUILT_PRODUCTS_DIR/$INFOPLIST_PATH
dsym_plist=$DWARF_DSYM_FOLDER_PATH/$DWARF_DSYM_FILE_NAME/Contents/Info.plist

function writePlist( ) {
    /usr/libexec/PListBuddy -c "Set :CFBundleVersion $2" "$1"
}

newVersion=`/usr/libexec/PlistBuddy -c "Print CFBundleVersion" "$plist" | /usr/bin/perl -pe 's/(\d+\.\d+\.)(\d+)/$1.($2+1)/eg'`

writePlist $plist $newVersion

if [test -s "$build_plist"] ; then
    writePlist $build_plist $newVersion
fi

if [test -s "$dsym_plist"] ; then
    writePlist $dsym_plist $newVersion
fi

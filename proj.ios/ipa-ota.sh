#!/usr/bin/env bash

if [[ ! ( # any of the following are not true
        # 1st arg is an existing regular file
        -f "$1" &&
        # ...and it has a .ipa extension
        "${1##*.}" == "ipa" &&
        # 2nd arg is a non-empty string
        -n "$2"
        ) ]];
    then
        echo "    Usage: $(basename "$0") Application.ipa http://dl.dropbox.com/u/310759/iphone_hosting_base_URL/"
    exit;
fi

## Exit on use of an uninitialized variable
set -o nounset
## Exit if any statement returns a non-true return value (non-zero)
set -o errexit
## Announce commands
#set -o xtrace

realpath(){
    echo "$(cd "$(dirname "$1")"; echo -n "$(pwd)/$(basename "$1")")";
}

IPA="$(realpath $1)"
URL="$2"
BASE="$(basename $IPA .ipa)"
TMP="$(mktemp -d /tmp/$(basename "$0" .sh).$(basename $IPA .ipa).XXXXX)"
cd "$TMP"
unzip -q "$IPA" "Payload/*.app/Info.plist"
plutil -convert xml1 Payload/*.app/Info.plist -o Info.plist

id=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleIdentifier' Info.plist)
ver=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleShortVersionString' Info.plist)
build=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleVersion' Info.plist)
name=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleDisplayName' Info.plist)

cd - > /dev/null
rm -rf "${TMP}"

rm -f "${BASE}.plist"
cat << plist > "${BASE}.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>items</key>
  <array>
    <dict>
      <key>assets</key>
      <array>
        <dict>
          <key>kind</key>
          <string>software-package</string>
          <key>url</key>
          <!-- CHANGEME on next line -->
          <string>http://example.com/CHANGEME_TO_URL_TO_HOSTED_IPA/app.ipa</string>
        </dict>
      </array>
      <key>metadata</key>
      <dict>
        <key>bundle-identifier</key>
        <!-- CHANGEME on next line -->
        <string>com.example.CHANGEME</string>
        <key>bundle-version</key>
        <string>2.0</string>
        <key>kind</key>
        <string>software</string>
        <key>title</key>
        <!-- CHANGEME on next line -->
        <string>CHANGEME TO YOUR DESIRED TITLE (Only used for the 'example.com would like to install "App Title"' popup.)</string>
      </dict>
    </dict>
  </array>
</dict>
</plist>
plist

/usr/libexec/PlistBuddy -c "Set :items:0:assets:0:url ${URL}${BASE}.ipa" "${BASE}.plist"
/usr/libexec/PlistBuddy -c "Set :items:0:metadata:bundle-version $ver.$build" "${BASE}.plist"
/usr/libexec/PlistBuddy -c "Set :items:0:metadata:bundle-identifier $id" "${BASE}.plist"
/usr/libexec/PlistBuddy -c "Set :items:0:metadata:title $(sed "s/'/\\\\'/"<<<$name)($3 版本号：$build)" "${BASE}.plist"
if [[ ! -t 1 ]]; then
cat << HTML
<html>
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <head>
    <title>Enterprise App Installer</title>
    <link rel="stylesheet" href="http://raw.github.com/RichardBronosky/ota-tools/master/mobile.css">
  </head>
  <body>

    <ul>
      <h2 id="Apps">Apps</h2>
HTML
fi
cat << HTML
      <li><a href="itms-services://?action=download-manifest&url=${URL}${BASE}.plist">Install $name</a></li>
HTML
if [[ ! -t 1 ]]; then
cat << HTML
    </ul>
  </body>
</html>
HTML
fi

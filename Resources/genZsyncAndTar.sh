#!/bin/sh

TP="/usr/local/bin/TexturePacker"
VERSION="3.6"

if [ "${ACTION}" = "clean" ]
then
    echo "cleaning..."
    
    rm iphonehd/*.plist
    rm iphone/*.plist
    
    # ....
    # add all files to be removed in clean phase
    # ....
else
    echo "building..."

    rm -f ../server/static/sync/iphonehd.${VERSION}.tar ../server/static/sync/iphone.${VERSION}.tar

    tar -c --exclude=".DS_Store" -f "../server/static/sync/iphonehd.${VERSION}.tar" iphonehd
    tar -c --exclude=".DS_Store" -f "../server/static/sync/iphone.${VERSION}.tar" iphone
	cp -f "../server/static/sync/iphonehd.${VERSION}.tar" "../server/static/sync/iphonehd.Head.tar"
	cp -f "../server/static/sync/iphone.${VERSION}.tar" "../server/static/sync/iphone.Head.tar"

    /opt/local/bin/zsyncmake -b 4096 -u "http://pt.shorttaillab.com/static/sync/iphonehd.${VERSION}.tar" -o "../server/static/sync/iphonehd.${VERSION}.tar.zsync"  "../server/static/sync/iphonehd.${VERSION}.tar"
    /opt/local/bin/zsyncmake -b 4096 -u "http://pt.shorttaillab.com/static/sync/iphone.${VERSION}.tar" -o "../server/static/sync/iphone.${VERSION}.tar.zsync"  "../server/static/sync/iphone.${VERSION}.tar"
fi
exit 0

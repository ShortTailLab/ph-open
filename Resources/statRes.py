#!/usr/bin/python

import os
import re

def getFilesInPlists():
    plists = []
    for root, dirs, files in os.walk("./iphonehd/"): # Walk directory tree
        for f in files:
            if ".plist" in f:
                plists.append(f)
    return plists

def getFilesInDir(mroot, ext):
    stuff = []
    for root, dirs, files in os.walk(mroot):
        for f in files:
            if ext in f:
                path = os.path.join(root, f)
                #relpath = os.path.relpath(path, mroot)
                stuff.append(path)
    return stuff

def findOverlaps(p1, p2):
    overlaps = []

    stuff1 = getFilesInDir(p1, ".png")
    rels1 = map(lambda x: os.path.relpath(x, p1), stuff1)

    stuff2 = getFilesInDir(p2, ".png")
    rels2 = map(lambda x: os.path.relpath(x, p2), stuff2)

    for f1 in rels1:
        if f1 in rels2:
            overlaps.append(s1)
    return overlaps

def getPngsFromPlist(plist):
    keys = []
    with open(plist) as f:
        stuff = map(lambda s: s.strip(), f.readlines())
        for line in stuff:
            m = re.match('<key>(.+\.png)</key>', line)
            if m:
                key = m.group(1)
                keys.append(key)
    return keys

# Stage 1
# check files both in iphonehd/gameui and in iphonehdFull/gameui
# those files should only be present in iphonehd/gameui
print "Stage 1, check dups in iphonehd/gameui and iphonehdFull/gameui"
overlaps = findOverlaps("./iphonehd/gameui/", "./iphonehdFull/gameui/")
for f in overlaps:
    path = os.path.join("./iphonehdFull/gameui/", f)
#    os.remove(path)
    print "removing ", path
print "Stage 1 ... Done\n"

# Stage 2
# find all files in plists that are also in iphonehd/
print "Stage 2, check dups in plists"
pngs = getFilesInDir("./iphonehd/", ".png")
plists = getFilesInDir("./iphonehd/", ".plist")
keys = []
for plist in plists:
    ks = getPngsFromPlist(plist)
    if ks:
        keys.extend(ks)

keyset = set()
for key in keys:
    if key in keyset:
        print "file ", key, " is a dup"
    else:
        keyset.add(key)

print "Stage 2 ... Done\n"

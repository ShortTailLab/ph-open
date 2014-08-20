#! /usr/bin/env python
# -*- coding: utf-8 -*-f

import os
import re
import json
import hashlib
import argparse

LOW_ROOT = "./iphone/"
HIGH_ROOT = "./iphonehd/"
TPSRoot = "./iphonehdFull/packs"
TP="/usr/local/bin/TexturePacker"

PARALLEL="/opt/local/bin/parallel"
CONVERT_GM="/opt/local/bin/gm convert"
CONVERT_IM="/opt/local/bin/convert"
MOGRIFY_GM="/opt/local/bin/gm mogrify"
MOGRIFY_IM="/opt/local/bin/mogrify"


has_parallel = False #os.path.exists(PARALLEL)
has_gm = os.path.exists("/opt/local/bin/gm")

CONVERT = CONVERT_GM if has_gm else CONVERT_IM
MOGRIFY = MOGRIFY_GM if has_gm else MOGRIFY_IM


# build high and low resolution resources.

def isPlistPNG(path):
    (name, ext) = os.path.splitext(path)
    plist = name + ".plist"
    return os.path.exists(plist)

def isImage(path):
    (name, ext) = os.path.splitext(path)
    return ext.lower() == ".png" or ext.lower() == ".jpg"

def isTPS(path):
    (name, ext) = os.path.splitext(path)
    return ext.lower() == ".tps"

def resizeHalf(srcPath):
    convert_command = CONVERT + ' {0} -resize 50% -unsharp 2x0.6+0.8+0.03  {1}'
    dstPath = srcPath.replace(HIGH_ROOT, LOW_ROOT)
    if not os.path.exists(os.path.dirname(dstPath)):
        os.system("mkdir -p '%s'" % os.path.dirname(dstPath))

    cmd = convert_command.format(srcPath, dstPath)
    print srcPath, '->', dstPath, " ... done"
    os.system(cmd)

def buildLowRes(r):
    if has_parallel:
        cmd = "cd '{0}' && find . -mindepth 2 ! -path './effects/*' -name '*.png' -o -name '*.jpg' | {2} -X {3} -output-directory ../{1} -create-directories -resize 50% -unsharp 2x0.6+0.8+0.03 {{}}".format(HIGH_ROOT, LOW_ROOT, PARALLEL, MOGRIFY)
        print cmd
        os.system(cmd)
        print "build low res done"
    else:
        cmd = "cd '{0}' && find . -mindepth 2 ! -path './effect/*' -name '*.png' -o -name '*.jpg' | xargs {2} -output-directory ../{1} -create-directories -resize 50% -unsharp 2x0.6+0.8+0.03 {{}}".format(HIGH_ROOT, LOW_ROOT, MOGRIFY)
        print cmd
        os.system(cmd)
        print "build low res done"

def buildTPS(r):
    for root, dirs, files in os.walk(r):
        if has_parallel:
            cmd = "{0} {1} ::: {2}/*.tps".format(PARALLEL, TP, root)
        else:
            cmd = "{0} {1}/*.tps".format(TP, root)
        os.system(cmd)
        print "texture pack ... done"

def removeUnusedLowRes():
    for root, dirs, files in os.walk(LOW_ROOT):
        for f in files:
            lowPath = os.path.join(root, f)
            highPath = lowPath.replace(LOW_ROOT, HIGH_ROOT)
            if not os.path.exists(highPath):
                print lowPath, " ... not present in high res, removed"
                os.system("rm " +  lowPath)
   
parser = argparse.ArgumentParser(description='Process some integers.')
#parser.add_argument('--clean', action="store_true", help='remove unused resources in low res folder', default=False)
args = parser.parse_args()

# build low resolution images
buildLowRes(HIGH_ROOT)

# build packed textures
buildTPS(TPSRoot)

#removeUnusedLowRes()

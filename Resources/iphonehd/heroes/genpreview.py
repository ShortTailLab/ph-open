#!/opt/local/bin/python2.7
import os
import math
from os.path import join
from PIL import Image
SRC_ROOT = '.'

def walk(path):
    entries = os.listdir(path)
    for entry in entries:
        srcPath = join(path, entry)
        dstPath = "./_preview/" + srcPath.replace('./', '').replace('/', '_')

        if os.path.isdir(srcPath):
            walk(srcPath)
        else: 
            name, ext = os.path.splitext(srcPath)

            if ext in ['.png', '.jpg'] and "body" in name:
                print srcPath, '->', dstPath, " ... done"
                cmd = "cp " + srcPath + " " + dstPath
                os.system(cmd)

walk(SRC_ROOT)

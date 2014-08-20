#import <Foundation/Foundation.h>
#import <stdio.h>
#import "CCNative.h"

extern "C"{

int mymkstemp(char* filename)
{
    NSString* folder = NSTemporaryDirectory();
    filename = tmpnam(filename);
    NSString* tmpFilePath = [folder stringByAppendingPathComponent:[[NSString stringWithUTF8String:filename] lastPathComponent]];
    strcpy(filename, tmpFilePath.UTF8String);
//    return open(tmpFilePath.UTF8String, O_RDWR | O_CREAT | O_SHLOCK);
    // workaround, this will leak a FILE*, but not the underlying file descriptor
    cocos2d::extension::CCNative::addSkipBackupAttributeToItem(tmpFilePath.UTF8String);
    return fileno(fopen(tmpFilePath.UTF8String, "wb+"));
}

}

/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-common
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#include "CCImage_richlabel.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreText/CoreText.h>
#include <math.h>

// tag char
#define TAG_START '['
#define TAG_END ']'

// span type
typedef enum {
    UNKNOWN,
    COLOR,
    FONT,
    SIZE,
    BOLD,
    ITALIC,
    UNDERLINE
} SpanType;

/// span
typedef struct Span {
    // span type
    SpanType type;
    
    // close tag?
    bool close;
    
	// pos in plain text
    // for close tag, it is exclusive, i.e., it is next char
    int pos;
	
	// color
    union {
        int color;
        int fontSize;
        char* fontName;
    } data;
} Span;
typedef vector<Span> SpanList;

typedef struct {
    unsigned int height;
    unsigned int width;
    int          bitsPerComponent;
    bool         hasAlpha;
    bool         isPremultipliedAlpha;
    bool         hasShadow;
    CGSize       shadowOffset;
    float        shadowBlur;
    int          shadowColor;
    bool         hasStroke;
    float        strokeColorR;
    float        strokeColorG;
    float        strokeColorB;
    float        strokeSize;
    float        tintColorR;
    float        tintColorG;
    float        tintColorB;
    unsigned char*  data;
} tImageInfo;

// alignment
#define ALIGN_TOP    1
#define ALIGN_CENTER 3
#define ALIGN_BOTTOM 2

// tag parsing state
typedef enum {
    READY,
    START_TAG,
    CLOSE_TAG,
    EQUAL,
    SUCCESS,
    FAIL
} TagParseState;

static int parseColor(const unichar* p, int len) {
	int color = 0;
	for(int i = 0; i < len; i++) {
		color <<= 4;
		char c = p[i];
		if(c >= '0' && c <= '9') {
			color |= c - '0';
		} else if(c >= 'a' && c <= 'f') {
			color |= c - 'a' + 10;
		} else if(c >= 'A' && c <= 'F') {
			color |= c - 'A' + 10;
		}
	}
	
	return color;
}

static SpanType checkTagName(unichar* p, int start, int end) {
    int len = end - start;
    switch(len) {
        case 1:
            if(p[start] == 'b') {
                return BOLD;
            } else if(p[start] == 'i') {
                return ITALIC;
            } else if(p[start] == 'u') {
                return UNDERLINE;
            }
            break;
        case 4:
            if(p[start] == 'f' &&
               p[start + 1] == 'o' &&
               p[start + 2] == 'n' &&
               p[start + 3] == 't') {
                return FONT;
            } else if(p[start] == 's' &&
                      p[start + 1] == 'i' &&
                      p[start + 2] == 'z' &&
                      p[start + 3] == 'e') {
                return SIZE;
            }
        case 5:
            if(p[start] == 'c' &&
               p[start + 1] == 'o' &&
               p[start + 2] == 'l' &&
               p[start + 3] == 'o' &&
               p[start + 4] == 'r') {
                return COLOR;
            }
            break;
            
    }
    
    return UNKNOWN;
}

// if parse failed, endTagPos will be len, otherwise it is end tag position
static SpanType checkTag(unichar* p, int start, int len, int* endTagPos, int* dataStart, int* dataEnd, bool* close) {
    SpanType type = UNKNOWN;
    TagParseState state = READY;
    int tagNameStart, tagNameEnd;
    *close = false;
    *endTagPos = len;
    *dataStart = -1;
    int i = start;
    while(i < len) {
        switch (state) {
            case READY:
                if(p[i] == TAG_START) {
                    state = START_TAG;
                    tagNameStart = ++i;
                } else {
                    state = FAIL;
                }
                break;
            case START_TAG:
                if((i == start + 1) && p[i] == '/') {
                    state = CLOSE_TAG;
                    *close = true;
                    tagNameStart = ++i;
                } else if(p[i] == '=') {
                    state = EQUAL;
                    tagNameEnd = i++;
                    type = checkTagName(p, tagNameStart, tagNameEnd);
                    *dataStart = i;
                } else if(p[i] == TAG_END) {
                    state = SUCCESS;
                    *endTagPos = i;
                    *dataEnd = i;
                    tagNameEnd = i;
                    if(type == UNKNOWN) {
                        type = checkTagName(p, tagNameStart, tagNameEnd);
                    }
                } else {
                    i++;
                }
                break;
            case CLOSE_TAG:
                if(p[i] == TAG_END) {
                    state = SUCCESS;
                    *endTagPos = i;
                    tagNameEnd = i;
                    type = checkTagName(p, tagNameStart, tagNameEnd);
                } else {
                    i++;
                }
                break;
            case EQUAL:
                if(p[i] == TAG_END) {
                    state = SUCCESS;
                    *endTagPos = i;
                    *dataEnd = i;
                } else {
                    i++;
                }
                break;
            default:
                break;
        }
        
        if(state == FAIL || state == SUCCESS)
            break;
    }
    
    if(state != SUCCESS)
        type = UNKNOWN;
    
    return type;
}

static unichar* buildSpan(const char* pText, SpanList& spans, int* outLen) {
    // get unichar of string
    NSString* ns = [NSString stringWithUTF8String:pText];
    int uniLen = [ns length];
    unichar* uniText = (unichar*)malloc(uniLen * sizeof(unichar));
    [ns getCharacters:uniText range:NSMakeRange(0, uniLen)];
    
	int plainLen = 0;
	unichar* plain = (unichar*)malloc(sizeof(unichar) * uniLen);
	for(int i = 0; i < uniLen; i++) {
		unichar c = uniText[i];
		switch(c) {
			case '\\':
				if(i < uniLen - 1) {
					unichar cc = uniText[i + 1];
					if(cc == TAG_START || cc == TAG_END) {
						plain[plainLen++] = cc;
						i++;
					}
				} else {
					plain[plainLen++] = c;
				}
				break;
			case TAG_START:
			{
                // parse the tag
                Span span;
                int endTagPos, dataStart, dataEnd;
                SpanType type = checkTag(uniText, i, uniLen, &endTagPos, &dataStart, &dataEnd, &span.close);
                
                // fill span info
                do {
                    // if type is unknown, discard
                    CC_BREAK_IF(type == UNKNOWN);
                    
                    // parse span data
                    span.type = type;
                    span.pos = plainLen;
                    if(!span.close) {
                        switch(span.type) {
                            case COLOR:
                                span.data.color = parseColor(uniText + dataStart, dataEnd - dataStart);
                                break;
                            case FONT:
                            {
                                NSString* font = [NSString stringWithCharacters:uniText + dataStart
                                                                         length:dataEnd - dataStart];
                                const char* cFont = [font cStringUsingEncoding:NSUTF8StringEncoding];
                                int len = strlen(cFont);
                                span.data.fontName = (char*)calloc(sizeof(char), len + 1);
                                strcpy(span.data.fontName, cFont);
                                break;
                            }
                            case SIZE:
                            {
                                NSString* size = [NSString stringWithCharacters:uniText + dataStart
                                                                         length:dataEnd - dataStart];
                                span.data.fontSize = [size intValue];
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    
                    // add span
                    spans.push_back(span);
                    
                    // set i pos
                    i = endTagPos;
                } while(0);
				
				break;
			}
			case TAG_END:
				// just discard it
				break;
			default:
				plain[plainLen++] = c;
				break;
		}
	}
	
	// return length
    if(outLen)
        *outLen = plainLen;
	
#ifdef COCOS2D_DEBUG
//	// debug output span info
//	int i = 0;
//	for(SpanList::iterator iter = spans.begin(); iter != spans.end(); iter++) {
//		Span& span = *iter;
//		CCLOG("span %d: %d, %d, %d", i++, span.type, span.close, span.pos);
//	}
#endif
    
    // release
    free(uniText);
	
	// return plain str
	return plain;
}

static bool _initWithString(const char * pText, CCImage::ETextAlign eAlign, const char * pFontName, int nSize, tImageInfo* pInfo) {
    bool bRet = false;
    do {
        CC_BREAK_IF(!pText || !pInfo);
        
        // On iOS custom fonts must be listed beforehand in the App info.plist (in order to be usable) and referenced only the by the font family name itself when
        // calling [UIFont fontWithName]. Therefore even if the developer adds 'SomeFont.ttf' or 'fonts/SomeFont.ttf' to the App .plist, the font must
        // be referenced as 'SomeFont' when calling [UIFont fontWithName]. Hence we strip out the folder path components and the extension here in order to get just
        // the font family name itself. This stripping step is required especially for references to user fonts stored in CCB files; CCB files appear to store
        // the '.ttf' extensions when referring to custom fonts.
        NSString* fntName = [NSString stringWithUTF8String:pFontName];
        fntName = [[fntName lastPathComponent] stringByDeletingPathExtension];
        
        // create the font
        UIFont* uiDefaultFont = [UIFont fontWithName:fntName size:nSize];
        CC_BREAK_IF(!uiDefaultFont);
        CTFontRef defaultFont = CTFontCreateWithName((CFStringRef)uiDefaultFont.fontName, nSize, NULL);

        // get plain text and extract span list
		SpanList spans;
        int uniLen;
        unichar* plain = buildSpan(pText, spans, &uniLen);
        
        // create attributed string
        CFStringRef plainCFStr = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                              (const UniChar*)plain,
                                                              uniLen);
        CFMutableAttributedStringRef plainCFAStr = CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
        CFAttributedStringReplaceString(plainCFAStr, CFRangeMake(0, 0), plainCFStr);
        int plainLen = CFAttributedStringGetLength(plainCFAStr);
        
        // font and color stack
        int defaultColor = 0xff000000
            | ((int)(pInfo->tintColorR * 255) << 16)
            | ((int)(pInfo->tintColorG * 255) << 8)
            | (int)(pInfo->tintColorB * 255);
        vector<CTFontRef> fontStack;
        vector<int> colorStack;
        fontStack.push_back(defaultFont);
        colorStack.push_back(defaultColor);
        
        // iterate all spans, install attributes
        int colorStart = 0;
        int fontStart = 0;
        int underlineStart = -1;
        CGColorSpaceRef colorSpace  = CGColorSpaceCreateDeviceRGB();
        for(SpanList::iterator iter = spans.begin(); iter != spans.end(); iter++) {
            Span& span = *iter;
            if(span.close) {
                switch(span.type) {
                    case COLOR:
                    {
                        if(span.pos > colorStart) {
                            int curColor = *colorStack.rbegin();
                            CGFloat comp[] = {
                                ((curColor >> 16) & 0xff) / 255.0f,
                                ((curColor >> 8) & 0xff) / 255.0f,
                                (curColor & 0xff) / 255.0f,
                                ((curColor >> 24) & 0xff) / 255.0f
                            };
                            CGColorRef fc = CGColorCreate(colorSpace, comp);
                            CFAttributedStringSetAttribute(plainCFAStr,
                                                           CFRangeMake(colorStart, span.pos - colorStart),
                                                           kCTForegroundColorAttributeName,
                                                           fc);
                            CGColorRelease(fc);
                            
                            // start need to be reset
                            colorStart = span.pos;
                        }
                        
                        // pop color
                        colorStack.pop_back();
                        
                        break;
                    }
                    case FONT:
                    case SIZE:
                    case BOLD:
                    case ITALIC:
                    {
                        // set previous span
                        CTFontRef font = *fontStack.rbegin();
                        if(span.pos > fontStart && font) {
                            CFAttributedStringSetAttribute(plainCFAStr,
                                                           CFRangeMake(fontStart, plainLen - fontStart),
                                                           kCTFontAttributeName,
                                                           font);
                            fontStart = span.pos;
                        }
                        
                        // pop font
                        fontStack.pop_back();
                        if(font)
                            CFRelease(font);
                        
                        break;
                    }
                    case UNDERLINE:
                    {
                        if(underlineStart > -1) {
                            CFIndex style = kCTUnderlineStyleSingle;
                            CFNumberRef styleNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberCFIndexType, &style);
                            CFAttributedStringSetAttribute(plainCFAStr,
                                                           CFRangeMake(underlineStart, span.pos - underlineStart),
                                                           kCTUnderlineStyleAttributeName,
                                                           styleNum);
                            CFRelease(styleNum);
                            underlineStart = -1;
                        }
                        break;
                    }
                    default:
                        break;
                }
            } else {
                switch(span.type) {
                    case COLOR:
                    {
                        // process previous run
                        if(span.pos > colorStart) {
                            int curColor = *colorStack.rbegin();
                            CGFloat comp[] = {
                                ((curColor >> 16) & 0xff) / 255.0f,
                                ((curColor >> 8) & 0xff) / 255.0f,
                                (curColor & 0xff) / 255.0f,
                                ((curColor >> 24) & 0xff) / 255.0f
                            };
                            CGColorRef fc = CGColorCreate(colorSpace, comp);
                            CFAttributedStringSetAttribute(plainCFAStr,
                                                           CFRangeMake(colorStart, span.pos - colorStart),
                                                           kCTForegroundColorAttributeName,
                                                           fc);
                            CGColorRelease(fc);
                            
                            // start need to be reset
                            colorStart = span.pos;
                        }
                        
                        // push color
                        colorStack.push_back(span.data.color);
                        
                        break;
                    }
                    case FONT:
                    {
                        // set previous span
                        CTFontRef curFont = *fontStack.rbegin();
                        if(span.pos > fontStart) {
                            CFAttributedStringSetAttribute(plainCFAStr,
                                                           CFRangeMake(fontStart, plainLen - fontStart),
                                                           kCTFontAttributeName,
                                                           curFont);
                            fontStart = span.pos;
                        }
                        
                        // create the font
                        NSString* fontName = [NSString stringWithCString:span.data.fontName
                                                                encoding:NSUTF8StringEncoding];
                        fontName = [[fontName lastPathComponent] stringByDeletingPathExtension];
                        UIFont* uiFont = [UIFont fontWithName:fontName
                                                         size:CTFontGetSize(curFont)];
                        CC_BREAK_IF(!uiFont);
                        CTFontRef font = CTFontCreateWithName((CFStringRef)uiFont.fontName,
                                                              CTFontGetSize(curFont),
                                                              NULL);
                        fontStack.push_back(font);
                        
                        break;
                    }
                    case SIZE:
                    {
                        // set previous span
                        CTFontRef curFont = *fontStack.rbegin();
                        if(span.pos > fontStart) {
                            CFAttributedStringSetAttribute(plainCFAStr,
                                                           CFRangeMake(fontStart, plainLen - fontStart),
                                                           kCTFontAttributeName,
                                                           curFont);
                            fontStart = span.pos;
                        }
                        
                        // push new font
                        CTFontDescriptorRef fd = CTFontCopyFontDescriptor(curFont);
                        CTFontRef font = CTFontCreateCopyWithAttributes(curFont,
                                                                        span.data.fontSize * CC_CONTENT_SCALE_FACTOR(),
                                                                        NULL,
                                                                        fd);
                        fontStack.push_back(font);
                        CFRelease(fd);
                        
                        break;
                    }
                    case BOLD:
                    {
                        // set previous span
                        CTFontRef curFont = *fontStack.rbegin();
                        if(span.pos > fontStart) {
                            CFAttributedStringSetAttribute(plainCFAStr,
                                                           CFRangeMake(fontStart, plainLen - fontStart),
                                                           kCTFontAttributeName,
                                                           curFont);
                            fontStart = span.pos;
                        }
                        
                        // create new font
                        CTFontRef font = CTFontCreateCopyWithSymbolicTraits(curFont,
                                                                            CTFontGetSize(curFont),
                                                                            NULL,
                                                                            kCTFontBoldTrait,
                                                                            kCTFontBoldTrait);
                        fontStack.push_back(font);
                        
                        break;
                    }
                    case ITALIC:
                    {
                        // set previous span
                        CTFontRef curFont = *fontStack.rbegin();
                        if(span.pos > fontStart) {
                            CFAttributedStringSetAttribute(plainCFAStr,
                                                           CFRangeMake(fontStart, plainLen - fontStart),
                                                           kCTFontAttributeName,
                                                           curFont);
                            fontStart = span.pos;
                        }
                   
                        // create new font
                        CTFontRef font = CTFontCreateCopyWithSymbolicTraits(curFont,
                                                                            CTFontGetSize(curFont),
                                                                            NULL,
                                                                            kCTFontItalicTrait,
                                                                            kCTFontItalicTrait);
                        fontStack.push_back(font);
                        
                        break;
                    }
                    case UNDERLINE:
                    {
                        underlineStart = span.pos;
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        
        // last segment
        if(plainLen > colorStart) {
            int curColor = *colorStack.rbegin();
            CGFloat comp[] = {
                ((curColor >> 16) & 0xff) / 255.0f,
                ((curColor >> 8) & 0xff) / 255.0f,
                (curColor & 0xff) / 255.0f,
                ((curColor >> 24) & 0xff) / 255.0f
            };
            CGColorRef fc = CGColorCreate(colorSpace, comp);
            CFAttributedStringSetAttribute(plainCFAStr,
                                           CFRangeMake(colorStart, plainLen - colorStart),
                                           kCTForegroundColorAttributeName,
                                           fc);
            CGColorRelease(fc);
        }
        if(plainLen > fontStart) {
            CTFontRef font = *fontStack.rbegin();
            CFAttributedStringSetAttribute(plainCFAStr,
                                           CFRangeMake(fontStart, plainLen - fontStart),
                                           kCTFontAttributeName,
                                           font);
        }
	       
        // set paragraph style, including line spacing and alignment
        CTTextAlignment alignment = kCTLeftTextAlignment;
        switch(eAlign & 0x0f) {
            case 2:
                alignment = kCTRightTextAlignment;
                break;
            case 3:
                alignment = kCTCenterTextAlignment;
                break;
            default:
                break;
        }
        float space = 0;
        CTParagraphStyleSetting paraSettings[] = {
            { kCTParagraphStyleSpecifierAlignment, sizeof(alignment), &alignment},
            { kCTParagraphStyleSpecifierLineSpacingAdjustment, sizeof(CGFloat), &space },
            { kCTParagraphStyleSpecifierMaximumLineSpacing, sizeof(CGFloat), &space },
            { kCTParagraphStyleSpecifierMinimumLineSpacing, sizeof(CGFloat), &space }
        };
        CTParagraphStyleRef paraStyle = CTParagraphStyleCreate(paraSettings,
                                                               sizeof(paraSettings) / sizeof(paraSettings[0]));
        CFAttributedStringSetAttribute(plainCFAStr,
                                       CFRangeMake(0, CFAttributedStringGetLength(plainCFAStr)),
                                       kCTParagraphStyleAttributeName,
                                       paraStyle);
        
        // create frame setter
        CTFramesetterRef fs = CTFramesetterCreateWithAttributedString(plainCFAStr);
        
        // constrain size
        CFRange range;
        CGSize constrainSize = CGSizeMake(pInfo->width, pInfo->height);
        CGSize ctConstrainSize = constrainSize;
        if(ctConstrainSize.width <= 0) {
            ctConstrainSize.width = CGFLOAT_MAX;
        }
        if(ctConstrainSize.height <= 0) {
            ctConstrainSize.height = CGFLOAT_MAX;
        }
        CGSize dim = CTFramesetterSuggestFrameSizeWithConstraints(fs,
                                                                  CFRangeMake(0, 0),
                                                                  NULL,
                                                                  ctConstrainSize,
                                                                  &range);
        dim.width = ceilf(dim.width);
        dim.height = ceilf(dim.height);
        
        // text origin offset
        int startY = 0;
        int startX = 0;
        if (constrainSize.height > dim.height) {
            // vertical alignment
            unsigned int vAlignment = (eAlign >> 4) & 0x0F;
            if (vAlignment == ALIGN_TOP) {
                startY = 0;
            } else if (vAlignment == ALIGN_CENTER) {
                startY = (constrainSize.height - dim.height) / 2;
            } else {
                startY = constrainSize.height - dim.height;
            }
        }
        
        // adjust text rect
        if (constrainSize.width > 0 && constrainSize.width > dim.width) {
            dim.width = constrainSize.width;
        }
        if (constrainSize.height > 0 && constrainSize.height > dim.height) {
            dim.height = constrainSize.height;
        }
        
        // create frame
        CGMutablePathRef path = CGPathCreateMutable();
        CGPathAddRect(path, NULL, CGRectMake(0, 0, dim.width, dim.height));
        CTFrameRef frame = CTFramesetterCreateFrame(fs,
                                                    CFRangeMake(0, 0),
                                                    path,
                                                    NULL);
        
        // compute the padding needed by shadow and stroke
        float shadowStrokePaddingX = 0.0f;
        float shadowStrokePaddingY = 0.0f;
        if (pInfo->hasStroke) {
            shadowStrokePaddingX = ceilf(pInfo->strokeSize);
            shadowStrokePaddingY = ceilf(pInfo->strokeSize);
        }
        if (pInfo->hasShadow) {
            shadowStrokePaddingX = MAX(shadowStrokePaddingX, (float)fabs(pInfo->shadowOffset.width));
            shadowStrokePaddingY = MAX(shadowStrokePaddingY, (float)fabs(pInfo->shadowOffset.height));
            if(pInfo->shadowOffset.height < 0) {
                startY += pInfo->shadowOffset.height;
            }
            if(pInfo->shadowOffset.width < 0) {
                startX += pInfo->shadowOffset.width;
            }
        }
        
        // add the padding (this could be 0 if no shadow and no stroke)
        dim.width  += shadowStrokePaddingX;
        dim.height += shadowStrokePaddingY;
        
		// allocate data for bitmap
        unsigned char* data = new unsigned char[(int)(dim.width * dim.height * 4)];
        memset(data, 0, (int)(dim.width * dim.height * 4));
        
        // create context
        CGContextRef context = CGBitmapContextCreate(data,
                                                     dim.width,
                                                     dim.height,
                                                     8,
                                                     dim.width * 4,
                                                     colorSpace,
                                                     kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
        
        // if fail to create context, break
        // if ok, draw text by core text
		do {
			CC_BREAK_IF(!context);
			
			// store the current context
            UIGraphicsPushContext(context);
            
            // alow anti-aliasing
            CGContextSetAllowsAntialiasing(context, YES);
            
            // take care of stroke if needed
            if (pInfo->hasStroke) {
                CGContextSetTextDrawingMode(context, kCGTextFillStroke);
                CGContextSetRGBStrokeColor(context, pInfo->strokeColorR, pInfo->strokeColorG, pInfo->strokeColorB, 1);
                CGContextSetLineWidth(context, pInfo->strokeSize);
            }
            
            // take care of shadow if needed
            if (pInfo->hasShadow) {
                CGSize offset;
                offset.height = pInfo->shadowOffset.height;
                offset.width  = pInfo->shadowOffset.width;
                CGFloat comp[] = {
                    ((pInfo->shadowColor >> 16) & 0xff) / 255.0f,
                    ((pInfo->shadowColor >> 8) & 0xff) / 255.0f,
                    (pInfo->shadowColor & 0xff) / 255.0f,
                    ((pInfo->shadowColor >> 24) & 0xff) / 255.0f
                };
                CGColorRef shadowColor = CGColorCreate(colorSpace, comp);
                CGContextSetShadowWithColor(context, offset, pInfo->shadowBlur, shadowColor);
                CGColorRelease(shadowColor);
            }
            
            // vertical alignment
            CGContextTranslateCTM(context, -startX, -startY);
            
            // draw frame
            CTFrameDraw(frame, context);
			
            // pop the context
            UIGraphicsPopContext();
		} while(0);
        
        // release
		if(context)
			CGContextRelease(context);
        CGColorSpaceRelease(colorSpace);
        CFRelease(plainCFStr);
        CFRelease(plainCFAStr);
        CFRelease(fs);
        CFRelease(defaultFont);
        CFRelease(frame);
        CFRelease(path);
        CFRelease(paraStyle);
		free(plain);
        for(SpanList::iterator iter = spans.begin(); iter != spans.end(); iter++) {
            Span& span = *iter;
            if(span.type == FONT && !span.close) {
                free(span.data.fontName);
            }
        }
		
        // output params
        pInfo->data                 = data;
        pInfo->hasAlpha             = true;
        pInfo->isPremultipliedAlpha = true;
        pInfo->bitsPerComponent     = 8;
        pInfo->width                = dim.width;
        pInfo->height               = dim.height;
        bRet                        = true;
    } while(0);
	
    return bRet;
}

NS_CC_BEGIN

CCImage_richlabel::CCImage_richlabel() {
}

CCImage_richlabel::~CCImage_richlabel() {
}

bool CCImage_richlabel::initWithRichStringShadowStroke(const char * pText,
														int         nWidth,
														int         nHeight,
														ETextAlign eAlignMask,
														const char * pFontName,
														int         nSize,
														float       textTintR,
														float       textTintG,
														float       textTintB,
														bool shadow,
														float shadowOffsetX,
														float shadowOffsetY,
                                                        int   shadowColor,
														float shadowBlur,
														bool  stroke,
														float strokeR,
														float strokeG,
														float strokeB,
														float strokeSize) {
    tImageInfo info = {0};
    info.width                  = nWidth;
    info.height                 = nHeight;
    info.hasShadow              = shadow;
    info.shadowOffset.width     = shadowOffsetX;
    info.shadowOffset.height    = shadowOffsetY;
    info.shadowBlur             = shadowBlur;
    info.shadowColor            = shadowColor;
    info.hasStroke              =  stroke;
    info.strokeColorR           =  strokeR;
    info.strokeColorG           = strokeG;
    info.strokeColorB           = strokeB;
    info.strokeSize             = strokeSize;
    info.tintColorR             = textTintR;
    info.tintColorG             = textTintG;
    info.tintColorB             = textTintB;
    
    
    if (! _initWithString(pText, eAlignMask, pFontName, nSize, &info))
    {
        return false;
    }
    m_nHeight = (short)info.height;
    m_nWidth = (short)info.width;
    m_nBitsPerComponent = info.bitsPerComponent;
    m_bHasAlpha = info.hasAlpha;
    m_bPreMulti = info.isPremultipliedAlpha;
    m_pData = info.data;
    
    return true;
}

NS_CC_END

#endif // #if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
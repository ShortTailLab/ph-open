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
package org.cocos2dx.lib;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.text.Layout.Alignment;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.text.style.AbsoluteSizeSpan;
import android.text.style.ForegroundColorSpan;
import android.text.style.TypefaceSpan;
import android.text.style.UnderlineSpan;
import android.util.Log;

public class RichLabelBitmap {
	/* The values are the same as cocos2dx/platform/CCImage.h. */
	private static final int HORIZONTALALIGN_LEFT = 1;
	private static final int HORIZONTALALIGN_RIGHT = 2;
	private static final int HORIZONTALALIGN_CENTER = 3;
	private static final int VERTICALALIGN_TOP = 1;
	private static final int VERTICALALIGN_BOTTOM = 2;
	private static final int VERTICALALIGN_CENTER = 3;
	
	private static final char TAG_START = '[';
	private static final char TAG_END = ']';
	
	private enum SpanType {
		UNKNOWN,
	    COLOR,
	    FONT,
	    SIZE,
	    BOLD,
	    ITALIC,
	    UNDERLINE
	}
	
	// span info
	final static class Span {
		public SpanType type;
		public boolean close;
		public int pos;
		
		// only for color
		public int color;
		
		// only for size
		public float fontSize;
		
		// only for font
		public String fontName;
	}
	
	// tag parse result
	final static class TagParseResult {
		SpanType type;
		boolean close;
		int endTagPos;
		int dataStart;
		int dataEnd;
	}
	
	// tag parse state
	private enum TagParseState {
	    READY,
	    START_TAG,
	    CLOSE_TAG,
	    EQUAL,
	    SUCCESS,
	    FAIL
	}
	
	static class CustomTypefaceSpan extends TypefaceSpan {
        private final Typeface newType;

        public CustomTypefaceSpan(String family, Typeface type) {
            super(family);
            newType = type;
        }

        @Override
        public void updateDrawState(TextPaint ds) {
            applyCustomTypeFace(ds, newType);
        }

        @Override
        public void updateMeasureState(TextPaint paint) {
            applyCustomTypeFace(paint, newType);
        }

        private void applyCustomTypeFace(Paint paint, Typeface tf) {
            int oldStyle;
            Typeface old = paint.getTypeface();
            if (old == null) {
                oldStyle = 0;
            } else {
                oldStyle = old.getStyle();
            }

            int fake = oldStyle & ~tf.getStyle();
            if ((fake & Typeface.BOLD) != 0) {
                paint.setFakeBoldText(true);
            }

            if ((fake & Typeface.ITALIC) != 0) {
                paint.setTextSkewX(-0.25f);
            }

            paint.setTypeface(tf);
        }
    }
	
	public static void createRichLabelBitmap(String pString, final String pFontName, final int pFontSize,
	        final float fontTintR, final float fontTintG, final float fontTintB, final int pAlignment,
	        final int pWidth, final int pHeight, final boolean shadow, final float shadowDX, final float shadowDY,
	        final int shadowColor, final float shadowBlur, final boolean stroke, final float strokeR, final float strokeG,
	        final float strokeB, final float strokeSize, float contentScaleFactor) {
		// extract span info and return text without span style
		List<Span> spans = new ArrayList<Span>();
		String plain = buildSpan(pString, spans);
		
		// alignment
		int horizontalAlignment = pAlignment & 0x0F;
		int verticalAlignment = (pAlignment >> 4) & 0x0F;
		Alignment align = Alignment.ALIGN_NORMAL;
		switch(horizontalAlignment) {
			case HORIZONTALALIGN_CENTER:
				align = Alignment.ALIGN_CENTER;
				break;
			case HORIZONTALALIGN_RIGHT:
				align = Alignment.ALIGN_OPPOSITE;
				break;
		}
		
		// create paint
		TextPaint paint = new TextPaint();
        Typeface defaultFont = Typeface.DEFAULT;
		paint.setColor(Color.WHITE);
		paint.setTextSize(pFontSize); 
		paint.setAntiAlias(true);
		if (pFontName.endsWith(".ttf")) {
			try {
                defaultFont = Cocos2dxTypefaces.get(Cocos2dxActivity.getContext(), pFontName);
			} catch (final Exception e) {
				Log.e("ColorLabelBitmap", "error to create ttf type face: " + pFontName);

				/* The file may not find, use system font. */
                defaultFont = Typeface.create(pFontName, Typeface.NORMAL);
			}
		} else {
			defaultFont = Typeface.create(pFontName, Typeface.NORMAL);
		}
        paint.setTypeface(defaultFont);
		
		// shadow
		// shadowBlur can be zero in android Paint, so set a min value to 1
		if (shadow) {
			paint.setShadowLayer(Math.max(shadowBlur, 1), shadowDX, shadowDY, shadowColor);
		}
		
        // compute the padding needed by shadow and stroke
        float shadowStrokePaddingX = 0.0f;
        float shadowStrokePaddingY = 0.0f;
        if (stroke) {
            shadowStrokePaddingX = (float)Math.ceil(strokeSize);
            shadowStrokePaddingY = (float)Math.ceil(strokeSize);
        }
        if (shadow) {
            shadowStrokePaddingX = Math.max(shadowStrokePaddingX, (float)Math.abs(shadowDX));
            shadowStrokePaddingY = Math.max(shadowStrokePaddingY, (float)Math.abs(shadowDY));
        }
        
        // stack of color and font
        int defaultColor = 0xff000000 
        		| ((int)(fontTintR * 255) << 16) 
        		| ((int)(fontTintG * 255) << 8) 
        		| (int)(fontTintB * 255);
        Stack<Integer> colorStack = new Stack<Integer>();
        Stack<Typeface> fontStack = new Stack<Typeface>();
        Stack<Float> fontSizeStack = new Stack<Float>();
        colorStack.push(defaultColor);
        fontStack.push(defaultFont);
        fontSizeStack.push(pFontSize / contentScaleFactor);
        
        // build spannable string
        int colorStart = 0;
        int fontStart = 0;
        int sizeStart = 0;
        int underlineStart = -1;
        SpannableString rich = new SpannableString(plain);
        for(Span span : spans) {
        	if(span.close) {
                switch(span.type) {
                    case COLOR:
                    {
                    	int curColor = colorStack.pop();
                        if(span.pos > colorStart) {
                            rich.setSpan(new ForegroundColorSpan(curColor), 
                            		colorStart, 
                            		span.pos, 
                            		Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                            
                            // start need to be reset
                            colorStart = span.pos;
                        }
                        
                        break;
                    }
                    case FONT:
                    case BOLD:
                    case ITALIC:
                    {
                        // set previous span
                        Typeface font = fontStack.pop();
                        if(span.pos > fontStart && font != null) {
                        	rich.setSpan(new CustomTypefaceSpan("", font),
                        			fontStart,
                        			span.pos,
                        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                            fontStart = span.pos;
                        }
                        
                        break;
                    }
                    case SIZE:
                    {
                    	float size = fontSizeStack.pop();
                        if(span.pos > sizeStart) {
                        	rich.setSpan(new AbsoluteSizeSpan((int)(size * contentScaleFactor)),
                        			sizeStart,
                        			span.pos,
                        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                        	sizeStart = span.pos;
                        }
                    	break;
                    }
                    case UNDERLINE:
                    {
                        if(underlineStart > -1) {
                        	rich.setSpan(new UnderlineSpan(), 
                        			underlineStart, 
                        			span.pos, 
                        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                            underlineStart = -1;
                        }
                        break;
                    }
                }
        	} else {
                switch(span.type) {
                    case COLOR:
                    {
                        // process previous run
                        if(span.pos > colorStart) {
                            int curColor = colorStack.peek();
                            rich.setSpan(new ForegroundColorSpan(curColor), 
                            		colorStart, 
                            		span.pos, 
                            		Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                            
                            // start need to be reset
                            colorStart = span.pos;
                        }
                        
                        // push color
                        colorStack.push(span.color);
                        
                        break;
                    }
                    case FONT:
                    {
                        // set previous span
                    	Typeface curFont = fontStack.peek();
                        if(span.pos > fontStart) {
                        	rich.setSpan(new CustomTypefaceSpan("", curFont), 
                        			fontStart, 
                        			span.pos, 
                        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                            fontStart = span.pos;
                        }
                        
                        // create the font
                        Typeface font = null;
                		if (span.fontName.endsWith(".ttf")) {
                			try {
                				font = Cocos2dxTypefaces.get(Cocos2dxActivity.getContext(), span.fontName);
                			} catch (final Exception e) {
                			}
                		} else {
                			font = Typeface.create(span.fontName, Typeface.NORMAL);
                			if(font == null) {
                				font = Typeface.DEFAULT;
                			}
                		}
                		fontStack.push(font);
                        
                        break;
                    }
                    case SIZE:
                    {
                        // set previous span
                        if(span.pos > sizeStart) {
                        	float size = fontSizeStack.peek();
                        	rich.setSpan(new AbsoluteSizeSpan((int)(size * contentScaleFactor)), 
                        			fontStart, 
                        			span.pos, 
                        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                        	sizeStart = span.pos;
                        }
                        
                        // push new size
                        fontSizeStack.push(span.fontSize);
                        
                        break;
                    }
                    case BOLD:
                    {
                        // set previous span
                    	Typeface curFont = fontStack.peek();
                        if(span.pos > fontStart) {
                        	rich.setSpan(new CustomTypefaceSpan("", curFont), 
                        			fontStart, 
                        			span.pos, 
                        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                            fontStart = span.pos;
                        }
                        
                        // create new font
                        Typeface font = Typeface.create(curFont, Typeface.BOLD | curFont.getStyle());
                        fontStack.push(font);
                        
                        break;
                    }
                    case ITALIC:
                    {
                        // set previous span
                    	Typeface curFont = fontStack.peek();
                        if(span.pos > fontStart) {
                        	rich.setSpan(new CustomTypefaceSpan("", curFont), 
                        			fontStart, 
                        			span.pos, 
                        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
                            fontStart = span.pos;
                        }
                        
                        // create new font
                        Typeface font = Typeface.create(curFont, Typeface.ITALIC | curFont.getStyle());
                        fontStack.push(font);
                        
                        break;
                    }
                    case UNDERLINE:
                    {
                        underlineStart = span.pos;
                        break;
                    }
                }
        	}
        }
        
        // last segment
        if(plain.length() > colorStart) {
            int curColor = colorStack.peek();
            rich.setSpan(new ForegroundColorSpan(curColor), 
            		colorStart, 
            		plain.length(), 
            		Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        if(plain.length() > sizeStart) {
        	float size = fontSizeStack.peek();
        	rich.setSpan(new AbsoluteSizeSpan((int)(size * contentScaleFactor)), 
        			fontStart, 
        			plain.length(), 
        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        }
        if(plain.length() > fontStart) {
        	Typeface curFont = fontStack.peek();
        	rich.setSpan(new CustomTypefaceSpan("", curFont), 
        			fontStart, 
        			plain.length(), 
        			Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        }
		
		// layout this text
		StaticLayout layout = new StaticLayout(rich,
				paint, 
				pWidth <= 0 ? (int)StaticLayout.getDesiredWidth(rich, paint) : pWidth, 
				align, 
				1, 
				0, 
				false);
		
		// size of layout
		int width = layout.getWidth();
		int height = layout.getHeight();
		
		// add padding of stroke
		width += shadowStrokePaddingX;
		height += shadowStrokePaddingY;
		
        // text origin
        int startY = 0;
        int startX = 0;
        if (pHeight > height) {
            // vertical alignment
            if (verticalAlignment == VERTICALALIGN_TOP) {
                startY = 0;
            } else if (verticalAlignment == VERTICALALIGN_CENTER) {
                startY = (pHeight - height) / 2;
            } else {
                startY = pHeight - height;
            }
        }
        if(shadow) {
        	if(shadowDY < 0) {
        		startY -= shadowDY; 
        	}
        	if(shadowDX < 0) {
        		startX -= shadowDX;
        	}
        }
		
		// adjust layout
		if(pWidth > 0 && pWidth > width)
			width = pWidth;
		if(pHeight > 0 && pHeight > height)
			height = pHeight;

        if (width == 0 || height == 0)
        {
            Log.d("puzzlehero", "weight or height is zero");
            return;
        }
		
		// create bitmap and canvas
		Bitmap bitmap = Bitmap.createBitmap(width, height, Config.ARGB_8888);
		Canvas c = new Canvas(bitmap);
		
		// translate for vertical alignment
		c.translate(startX, startY);
		
		// draw text
		layout.draw(c);
		
		// draw again if stroke is enabled
		if(stroke) {
			// reset paint to stroke mode
			paint.setStyle(Paint.Style.STROKE);
			paint.setStrokeWidth(strokeSize);
			paint.setARGB(255, (int)strokeR * 255, (int)strokeG * 255, (int)strokeB * 255);
			paint.clearShadowLayer();
			
			// clear color and underline span
			for(ForegroundColorSpan span : rich.getSpans(0, rich.length(), ForegroundColorSpan.class)) {
				rich.removeSpan(span);
			}
			for(UnderlineSpan span : rich.getSpans(0, rich.length(), UnderlineSpan.class)) {
				rich.removeSpan(span);
			}
			
			layout.draw(c);
		}
		
		// transfer bitmap data to native layer, and release bitmap when done
		initNativeObject(bitmap);
		bitmap.recycle();
	}
	
	// if parse failed, endTagPos will be len, otherwise it is end tag position
	private static TagParseResult checkTag(String p, int start) {
		TagParseResult r = new TagParseResult();
		r.type = SpanType.UNKNOWN;
	    TagParseState state = TagParseState.READY;
	    int tagNameStart = 0, tagNameEnd = 0;
	    r.close = false;
	    int len = p.length();
	    r.endTagPos = len;
	    r.dataStart = -1;
	    int i = start;
	    while(i < len) {
	        switch (state) {
	            case READY:
	                if(p.charAt(i) == TAG_START) {
	                    state = TagParseState.START_TAG;
	                    tagNameStart = ++i;
	                } else {
	                    state = TagParseState.FAIL;
	                }
	                break;
	            case START_TAG:
	                if((i == start + 1) && p.charAt(i) == '/') {
	                    state = TagParseState.CLOSE_TAG;
	                    r.close = true;
	                    tagNameStart = ++i;
	                } else if(p.charAt(i) == '=') {
	                    state = TagParseState.EQUAL;
	                    tagNameEnd = i++;
	                    r.type = checkTagName(p, tagNameStart, tagNameEnd);
	                    r.dataStart = i;
	                } else if(p.charAt(i) == TAG_END) {
	                    state = TagParseState.SUCCESS;
	                    r.endTagPos = i;
	                    r.dataEnd = i;
	                    tagNameEnd = i;
	                    if(r.type == SpanType.UNKNOWN) {
	                        r.type = checkTagName(p, tagNameStart, tagNameEnd);
	                    }
	                } else {
	                    i++;
	                }
	                break;
	            case CLOSE_TAG:
	                if(p.charAt(i) == TAG_END) {
	                    state = TagParseState.SUCCESS;
	                    r.endTagPos = i;
	                    tagNameEnd = i;
	                    r.type = checkTagName(p, tagNameStart, tagNameEnd);
	                } else {
	                    i++;
	                }
	                break;
	            case EQUAL:
	                if(p.charAt(i) == TAG_END) {
	                    state = TagParseState.SUCCESS;
	                    r.endTagPos = i;
	                    r.dataEnd = i;
	                } else {
	                    i++;
	                }
	                break;
	            default:
	                break;
	        }
	        
	        if(state == TagParseState.FAIL || state == TagParseState.SUCCESS)
	            break;
	    }
	    
	    if(state != TagParseState.SUCCESS)
	        r.type = SpanType.UNKNOWN;
	    
	    return r;
	}
	
	private static SpanType checkTagName(String p, int start, int end) {
	    int len = end - start;
	    switch(len) {
	        case 1:
	            if(p.charAt(start) == 'b') {
	                return SpanType.BOLD;
	            } else if(p.charAt(start) == 'i') {
	                return SpanType.ITALIC;
	            } else if(p.charAt(start) == 'u') {
	                return SpanType.UNDERLINE;
	            }
	            break;
	        case 4:
	            if(p.charAt(start) == 'f' &&
	               p.charAt(start + 1) == 'o' &&
	               p.charAt(start + 2) == 'n' &&
	               p.charAt(start + 3) == 't') {
	                return SpanType.FONT;
	            } else if(p.charAt(start) == 's' &&
	                      p.charAt(start + 1) == 'i' &&
	                      p.charAt(start + 2) == 'z' &&
	                      p.charAt(start + 3) == 'e') {
	                return SpanType.SIZE;
	            }
	        case 5:
	            if(p.charAt(start) == 'c' &&
	               p.charAt(start + 1) == 'o' &&
	               p.charAt(start + 2) == 'l' &&
	               p.charAt(start + 3) == 'o' &&
	               p.charAt(start + 4) == 'r') {
	                return SpanType.COLOR;
	            }
	            break;
	            
	    }
	    
	    return SpanType.UNKNOWN;
    }

	private static String buildSpan(String text, List<Span> spans) {
		int uniLen = text.length();
		StringBuilder plain = new StringBuilder();
		for(int i = 0; i < uniLen; i++) {
			char c = text.charAt(i);
			switch(c) {
				case '\\':
					if(i < uniLen - 1) {
						char cc = text.charAt(i + 1);
						if(cc == '[' || cc == ']') {
							plain.append(cc);
							i++;
						}
					} else {
						plain.append(c);
					}
					break;
				case TAG_START:
				{
	                // parse the tag
	                Span span = new Span();
	                TagParseResult r = checkTag(text, i);
	                
	                // fill span info
	                do {
	                    // if type is unknown, discard
	                    if(r.type == SpanType.UNKNOWN)
	                    	break;
	                    
	                    // parse span data
	                    span.type = r.type;
	                    span.close = r.close;
	                    span.pos = plain.length();
	                    if(!r.close) {
	                        switch(span.type) {
	                            case COLOR:
	                                span.color = parseColor(text, r.dataStart, r.dataEnd - r.dataStart);
	                                break;
	                            case FONT:
	                            	span.fontName = text.substring(r.dataStart, r.dataEnd);
	                                break;
	                            case SIZE:
	                            	try {
	                                    span.fontSize = Integer.parseInt(text.substring(r.dataStart, r.dataEnd));
                                    } catch (NumberFormatException e) {
                                    	span.fontSize = 16;
                                    }
	                                break;
	                            default:
	                                break;
	                        }
	                    }
	                    
	                    // add span
	                    spans.add(span);
	                    
	                    // set i pos
	                    i = r.endTagPos;
	                } while(false);
					
					break;
				}
				case TAG_END:
					// just discard it
					break;
				default:
					plain.append(c);
					break;
			}
		}
		
		// return plain str
		return plain.toString();
	}
	
	private static int parseColor(String text, int start, int len) {
		int color = 0;
		int end = start + len;
		for(int i = start; i < end; i++) {
			color <<= 4;
			char c = text.charAt(i);
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

	private static byte[] getPixels(final Bitmap pBitmap) {
		if (pBitmap != null) {
			final byte[] pixels = new byte[pBitmap.getWidth()
					* pBitmap.getHeight() * 4];
			final ByteBuffer buf = ByteBuffer.wrap(pixels);
			buf.order(ByteOrder.nativeOrder());
			pBitmap.copyPixelsToBuffer(buf);
			return pixels;
		}

		return null;
	}

	private static void initNativeObject(final Bitmap pBitmap) {
		final byte[] pixels = getPixels(pBitmap);
		if (pixels == null) {
			return;
		}

		nativeInitBitmapDC(pBitmap.getWidth(), pBitmap.getHeight(), pixels);
	}
	
	private static native void nativeInitBitmapDC(int pWidth, int pHeight, byte[] pPixels);
}

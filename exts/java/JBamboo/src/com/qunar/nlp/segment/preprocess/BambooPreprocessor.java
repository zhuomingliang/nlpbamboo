package com.qunar.nlp.segment.preprocess;

import java.util.ArrayList;
import java.util.List;

import com.qunar.nlp.segment.BambooToken;
import com.qunar.nlp.segment.BambooToken.Attr;
import com.qunar.nlp.segment.Token;

public class BambooPreprocessor implements Preprocessor {
	public static final int MAX_TOKEN_LENGTH = 512;
	private static enum State {
		UNKNOWN,
		ALPHA,
		NUMBER,
		PUNCT,
		WHITESPACE,
		IDENT,
		BEGIN,
		END,
	}
	
	private static boolean isConcat(char c) {
		return c == '-' || c == '_';
	}
	
	private static boolean isAlpha(char ch) {
		return ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z';
	}
	private static boolean isDigit(char ch) {
		return ch >= '0' && ch <= '9';
	}
	private static boolean isSpace(char ch) {
		switch (ch) {
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 32:
			return true;
		default:
			return false;
		}
	}
	private static boolean isPunct(char ch) {
		return ch > 0 && ch < 128 && !isSpace(ch) && !isAlpha(ch) && !isDigit(ch);
	}
	
	public static char dbc2sbc(char c) {
		if (c < 128) {
			return c;
		}
		if (c == '\u3000') {
			return ' ';
		}
		if (c < '\uFF5F' && c > '\uFF00') {
			return (char) (c - 65248);
		} else {
			return 0;
		}
	}
	
	public List<Token> process(String text) {
		StringBuilder dbc = new StringBuilder();
		StringBuilder sbc = new StringBuilder();
		State state, lastState = State.BEGIN;
		boolean firstToken = true;
		int beginIndex = -1;
		List<Token> tokens = new ArrayList<Token>();
		char cch = 0, uch = 0;
		for (int index = 0; index <= text.length(); ++index) {
			if (index == text.length()) {
				state = State.END;
			} else {
				uch = text.charAt(index);
				cch = dbc2sbc(uch);
				
				if (isAlpha(cch)) {
					state = State.ALPHA;
				} else if (cch == '.' && lastState == State.NUMBER) {
					state = State.NUMBER;
				} else if (isDigit(cch)) {
					state = State.NUMBER;
				} else if (isConcat(cch)) {
					state = State.IDENT;
				} else if (isPunct(cch) || uch == '。' || uch == '、' || uch == '《' || uch == '》' || uch == '•') {
					state = State.PUNCT;
				} else if (isSpace(cch)) {
					state = State.WHITESPACE;
				} else {
					state = State.UNKNOWN;
				}
			}
			
			if (state != lastState
					&& (lastState == State.ALPHA || lastState == State.NUMBER)
					&& (state == State.ALPHA || state == State.NUMBER || state == State.IDENT)
			) {
				lastState = state = State.IDENT;
			} else if (lastState == State.IDENT && (state == State.ALPHA || state == State.NUMBER)) {
				state = State.IDENT;
			}
			
			if (state != lastState || state == State.UNKNOWN || state == State.PUNCT || state == State.WHITESPACE) {
				if (dbc.length() > 0) {
					Attr attr;
					switch (lastState) {
					case IDENT:
					case ALPHA:
						attr = Attr.ALPHA;
						break;
					case NUMBER:
						attr = Attr.NUMBER;
						break;
					case WHITESPACE:
						attr = Attr.WHITESPACE;
						break;
					case PUNCT:
						attr = Attr.PUNCT;
						break;
					default:
						attr = Attr.UNKNOWN;
						break;
					}
					if (sbc.length() > 0) {
						tokens.add(new BambooToken(sbc.toString(), text, beginIndex, beginIndex + dbc.length(), attr));
					} else {
						tokens.add(new BambooToken(dbc.toString(), text, beginIndex, beginIndex + dbc.length(), attr));
					}
					sbc.setLength(0);
					dbc.setLength(0);
					if (firstToken) {
						firstToken = false;
					}
				}
				if (state == State.END) {
					break;
				}
			}
			if (dbc.length() == 0) {
				beginIndex = index;
			}
			dbc.append(uch);
			if (dbc.length() > MAX_TOKEN_LENGTH) {
				state = State.UNKNOWN;
			}
			if (cch != 0) {
				sbc.append(cch);
			}
			lastState = state;
		}
		return tokens;
	}
}

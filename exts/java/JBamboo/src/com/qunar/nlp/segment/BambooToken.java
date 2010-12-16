package com.qunar.nlp.segment;

public class BambooToken extends Token {
	public static enum Attr {
		UNKNOWN,
		ALPHA,
		NUMBER,
		PUNCT,
		WHITESPACE,
		CWORD,
	}
	private String text;
	private Attr attr;
	
	public BambooToken(String text, String context, int beginIndex, int endIndex, Attr attr) {
		super(context, beginIndex, endIndex);
		this.text = text;
		this.attr = attr;
	}

	public BambooToken(String context, int beginIndex, int endIndex) {
		super(context, beginIndex, endIndex);
	}
	public String getText() {
		return text;
	}

	public void setText(String text) {
		this.text = text;
	}

	public Attr getAttr() {
		return attr;
	}

	public void setAttr(Attr attr) {
		this.attr = attr;
	}
	
	public int getLength() {
		return text.length();
	}
}

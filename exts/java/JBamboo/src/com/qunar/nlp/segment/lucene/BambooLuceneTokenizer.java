package com.qunar.nlp.segment.lucene;

import java.io.IOException;
import java.io.Reader;
import java.util.List;

import org.apache.lucene.analysis.Tokenizer;
import org.apache.lucene.analysis.tokenattributes.OffsetAttribute;
import org.apache.lucene.analysis.tokenattributes.TermAttribute;
import org.apache.lucene.util.AttributeSource;

import com.qunar.nlp.segment.BambooChineseSegmentor;
import com.qunar.nlp.segment.Token;

public class BambooLuceneTokenizer extends Tokenizer {
	private static final int BUFFER_SIZE = 1024 * 4;
	
	private List<? extends Token> tokens;
	private int currentIndex = -1;

	private TermAttribute termAtt;
	private OffsetAttribute offsetAtt;
	private BambooChineseSegmentor segmentor;

	private int maxConsumeLength = 0;
	
	public static String getText(Reader reader, int bufferSize, int maxConsumeLength) throws IOException {
		char[] buffer = new char[bufferSize];
		int count = 0; 
		StringBuilder sb = new StringBuilder();
		do {
			int readLength = maxConsumeLength > 0 ? Math.min(maxConsumeLength - count, bufferSize) : bufferSize;
			int n = reader.read(buffer, 0, readLength);
			if (-1 == n) {
				break;
			}
			count += n;
			sb.append(buffer, 0, n);
		} while (count < maxConsumeLength);
		return sb.toString();
	}
	private void consumeReader(Reader reader) throws IOException {
		String text = getText(reader, BUFFER_SIZE, maxConsumeLength);
		tokens = segmentor.segment(text);
	}

	public BambooLuceneTokenizer(Reader reader, BambooChineseSegmentor segmentor)
			throws IOException {
		super(reader);
		this.segmentor = segmentor;
		this.termAtt = (TermAttribute) addAttribute(TermAttribute.class);
		this.offsetAtt = (OffsetAttribute) addAttribute(OffsetAttribute.class);
		consumeReader(reader);
	}

	public BambooLuceneTokenizer(Reader reader,
			BambooChineseSegmentor segmentor, int maxConsumeLength) throws IOException {
		super(reader);
		this.segmentor = segmentor;
		this.maxConsumeLength = maxConsumeLength;
		this.termAtt = (TermAttribute) addAttribute(TermAttribute.class);
		this.offsetAtt = (OffsetAttribute) addAttribute(OffsetAttribute.class);
		consumeReader(reader);
	}

	public BambooLuceneTokenizer(AttributeSource source, Reader reader,
			BambooChineseSegmentor segmentor) throws IOException {
		super(source, reader);
		this.segmentor = segmentor;
		this.termAtt = (TermAttribute) addAttribute(TermAttribute.class);
		this.offsetAtt = (OffsetAttribute) addAttribute(OffsetAttribute.class);
		consumeReader(reader);
	}

	public BambooLuceneTokenizer(AttributeFactory factory, Reader reader,
			BambooChineseSegmentor segmentor) throws IOException {
		super(factory, reader);
		this.segmentor = segmentor;
		this.termAtt = (TermAttribute) addAttribute(TermAttribute.class);
		this.offsetAtt = (OffsetAttribute) addAttribute(OffsetAttribute.class);
		consumeReader(reader);
	}

	@Override
	public final boolean incrementToken() throws IOException {
		clearAttributes();
		if (currentIndex + 1 >= tokens.size()) {
			return false;
		}
		Token currentToken = tokens.get(++currentIndex);
		String context = currentToken.getContext();
		int beginIndex = currentToken.getBeginIndex();
		int endIndex = currentToken.getEndIndex();
		int length = endIndex - beginIndex;
		termAtt.setTermBuffer(context, beginIndex, length);
		termAtt.setTermLength(length);
		offsetAtt.setOffset(beginIndex, endIndex);
		return true;
	}

	@Override
	public void reset(Reader input) throws IOException {
		super.reset(input);
		consumeReader(input);
		currentIndex = -1;
	}
}

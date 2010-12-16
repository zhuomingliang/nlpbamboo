package com.qunar.nlp.segment;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.Reader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Properties;

import com.qunar.nlp.crf.CRFTagger;
import com.qunar.nlp.crf.Tagger;
import com.qunar.nlp.segment.BambooToken.Attr;
import com.qunar.nlp.segment.postprocess.BambooPostProcessor;
import com.qunar.nlp.segment.postprocess.Postprocessor;
import com.qunar.nlp.segment.preprocess.BambooPreprocessor;
import com.qunar.nlp.segment.preprocess.Preprocessor;

/**
 * @author xuqy
 *
 */
public class BambooChineseSegmentor {
	private static final String SINGLE_TAG = "S";
	private static final String END_TAG = "E";
	public static String getTypeStr(Attr attr) {
		switch (attr) {
		case ALPHA:
		case NUMBER:
			return "ASCII";
		case PUNCT:
			return "PUNC";
		case CWORD:
			return "CN";
		default:
			return "CN";
		}
	}
	
	private Preprocessor preprocessor;
	private Tagger tagger;
	private Postprocessor postprocessor;
	
	public BambooChineseSegmentor(String propertiesFilename, String charset) throws IOException {
		Properties properties = new Properties();
		Reader reader = new BufferedReader(new InputStreamReader(new FileInputStream(propertiesFilename), charset));
		properties.load(reader);
		String textModelFilename = properties.getProperty("text_model_filename");
		String textModelCharset = properties.getProperty("text_model_file_charset", "UTF-8");
		boolean textModelIsGZipped = Boolean.parseBoolean(properties.getProperty("text_model_is_gzipped", "true"));
		File textModelFile = new File(textModelFilename);
		tagger = new CRFTagger(textModelFile, textModelIsGZipped, textModelCharset);
		preprocessor = new BambooPreprocessor();
		boolean useCombine = Boolean.parseBoolean(properties.getProperty("use_combine", "true"));
		if (useCombine) {
			postprocessor = new BambooPostProcessor(properties);
		} else {
			postprocessor = null;
		}
	}
	
	public BambooChineseSegmentor(String propertiesFilename) throws IOException {
		this(propertiesFilename, "UTF-8");
	}
	
	private static List<BambooToken> crfTag(String context, List<Token> preTokens, int offset, String[] tags) {
		List<BambooToken> postTokens = new ArrayList<BambooToken>();
		StringBuilder textSB = new StringBuilder();
		int beginIndex = -1;
		for (int index = 0; index < tags.length; index++) {
			String tag = tags[index];
			BambooToken preToken = (BambooToken) preTokens.get(offset + index);
			if (textSB.length() == 0) {
				beginIndex = preToken.getBeginIndex();
			}
			textSB.append(preToken.getText());
			Attr attr = preToken.getAttr();
			if (attr == Attr.UNKNOWN) {
				attr = Attr.CWORD;
			} else if (attr == Attr.ALPHA || attr == Attr.NUMBER || attr == Attr.PUNCT) {
				tag = SINGLE_TAG;
			}
			if (tag.equals(END_TAG) || tag.equals(SINGLE_TAG)) {
				BambooToken postToken = new BambooToken(textSB.toString(), context, beginIndex, beginIndex + textSB.length(), attr);
				postTokens.add(postToken);
				textSB.setLength(0);
			}
		}
		return postTokens;
	}
	
	public List<Token> segment(String text) {
		if (text == null || text.length() == 0) {
			return Collections.emptyList();
		}
		List<Token> preTokens = preprocessor.process(text);
		List<String[]> features = new ArrayList<String[]>();
		List<Token> tokens = new ArrayList<Token>();
		int offset = 0;
		int i = 0;
		for (; i < preTokens.size(); ++i) {
			BambooToken token = (BambooToken) preTokens.get(i);
			boolean append = token.getAttr() != BambooToken.Attr.WHITESPACE;
			if (append) {
				features.add(new String[] {token.getText(), getTypeStr(token.getAttr())});
			}
			char firstChar = token.getText().charAt(0);
			if (token.getAttr() == BambooToken.Attr.WHITESPACE
				|| firstChar == '!' || firstChar == '?' || firstChar == ';' || firstChar == '。') {
				String[] tags = tagger.tag(features);
				offset = i - features.size() + (append ? 1 : 0);
				tokens.addAll(crfTag(text, preTokens, offset, tags));
				features.clear();
			}
		}
		if (!features.isEmpty()) {
			String[] tags = tagger.tag(features);
			offset = i - features.size();
			tokens.addAll(crfTag(text, preTokens, offset, tags));
		}
		if (postprocessor != null) {
			List<Token> out = new ArrayList<Token>(tokens.size());
			postprocessor.process(tokens, out);
			return out;
		} else {
			return tokens;
		}
	}
	
	public static void main(String[] args) throws IOException {
		if (args.length != 3) {
			System.err.println("Usage: BambooChineseSegmentor <config_file> <test_file> <result_file>");
			System.exit(1);
		}
		String configFilename = args[0], testFilename = args[1], resultFilename = args[2];
		BambooChineseSegmentor segmentor = new BambooChineseSegmentor(configFilename);
		String line;
		BufferedReader br = new BufferedReader(new FileReader(new File(testFilename)));
		PrintWriter pw = new PrintWriter(resultFilename);
		while ((line = br.readLine()) != null) {
			List<Token> tokens = segmentor.segment(line);
			for (Token token : tokens) {
				pw.print(token.toString() + " ");
			}
			pw.println();
		}
		pw.close();
		br.close();
	}
}
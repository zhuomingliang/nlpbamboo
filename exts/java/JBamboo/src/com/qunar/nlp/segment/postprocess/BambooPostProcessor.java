package com.qunar.nlp.segment.postprocess;

import gnu.trove.TObjectIntHashMap;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.List;
import java.util.Properties;

import com.qunar.nlp.segment.BambooToken;
import com.qunar.nlp.segment.BambooToken.Attr;
import com.qunar.nlp.segment.Token;

/**
 * @author liangliang.fu
 * 
 */
public class BambooPostProcessor implements Postprocessor {
	private static final int LEFT 	= 1 << 2;
	private static final int MIDDLE = 1 << 1;
	private static final int RIGHT 	= 1;
	
	private TObjectIntHashMap<String> combineLexicon;
	private TObjectIntHashMap<String> numberTrailingLexicon;
	private boolean forwardCombine;
	private boolean backwardCombine;
	private boolean neighborComine;
	private boolean kokoCombine;

	public BambooPostProcessor(Properties properties) throws IOException {
		String combineLexiconFilename = properties
				.getProperty("combine_lexicon_filename");
		String numberTrailingLexiconFilename = properties
				.getProperty("number_trailing_lexicon_filename");

		forwardCombine = Boolean.valueOf(properties
				.getProperty("forward_combine"));
		backwardCombine = Boolean.valueOf(properties
				.getProperty("backward_combine"));
		neighborComine = Boolean.valueOf(properties
				.getProperty("neighbor_comine"));
		kokoCombine = Boolean.valueOf(properties.getProperty("koko_combine"));
		if (combineLexiconFilename != null && combineLexiconFilename.trim().length() > 0) {
			combineLexicon = loadLexicon(combineLexiconFilename, "UTF-8");
		} else {
			combineLexicon = new TObjectIntHashMap<String>();
		}
		if (numberTrailingLexiconFilename != null && numberTrailingLexiconFilename.trim().length() > 0) {
			numberTrailingLexicon = loadLexicon(numberTrailingLexiconFilename, "UTF-8");
		} else {
			numberTrailingLexicon = new TObjectIntHashMap<String>();
		}
	}

	private TObjectIntHashMap<String> loadLexicon(String lexiconFilename,
			String charset) throws IOException {
		BufferedReader br = null;
		TObjectIntHashMap<String> lexicon = new TObjectIntHashMap<String>();
		try {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(
					lexiconFilename)));
			String line;
			while ((line = br.readLine()) != null) {
				line = line.trim();
				if (line.length() > 0) {
					lexicon.put(line, 1);
				}
			}
		} finally {
			if (br != null) {
				br.close();
			}
		}
		return lexicon;
	}

	public void process(List<Token> in, List<Token> out) {
		for (int i = 0; i < in.size(); i++) {
			BambooToken token = (BambooToken) in.get(i);
			if (token == null) {
				continue;
			}
			int match = 0;
			BambooToken combinedToken = null;

			String context = token.getContext();
			if (i + 1 < in.size()
					&& in.get(i + 1) != null
					&& token.getAttr() == Attr.NUMBER
					&& numberTrailingLexicon.contains(in.get(i + 1)
							.toString())) {
				match = MIDDLE | RIGHT;
				combinedToken = new BambooToken(context,
						token.getBeginIndex(), in.get(i + 1).getEndIndex());
			}
			
			if (token.getLength() <= 2) {
				if (neighborComine && i > 0 && i + 1 < in.size()
						&& in.get(i - 1) != null && in.get(i + 1) != null) {
					String combinedText = context.substring(in.get(i - 1)
							.getBeginIndex(), in.get(i + 1).getEndIndex());
					if (combineLexicon.containsKey(combinedText)) {
						match = LEFT | MIDDLE | RIGHT;
						combinedToken = new BambooToken(context, in.get(i - 1)
								.getBeginIndex(), in.get(i + 1).getEndIndex());
					}
				} else if (forwardCombine && i > 0 && in.get(i - 1) != null) {
					String combinedText = context.substring(in.get(i - 1)
							.getBeginIndex(), token.getEndIndex());
					if (combineLexicon.containsKey(combinedText)) {
						match = LEFT | MIDDLE;
						combinedToken = new BambooToken(context, in.get(i - 1)
								.getBeginIndex(), token.getEndIndex());
					}
				} else if (backwardCombine && i + 1 < in.size()
						&& in.get(i + 1) != null) {
					String combinedText = context.substring(
							token.getBeginIndex(), in.get(i + 1).getEndIndex());
					if (combineLexicon.containsKey(combinedText)) {
						match = MIDDLE | RIGHT;
						combinedToken = new BambooToken(context,
								token.getBeginIndex(), in.get(i + 1)
										.getEndIndex());
					}
				} else if (kokoCombine && i > 0 && in.get(i - 1) != null
						&& in.get(i - 1).getLength() == 1
						&& token.toString().equals(in.get(i - 1).toString())) {
					match = LEFT | MIDDLE;
					combinedToken = new BambooToken(context, in.get(i - 1)
							.getBeginIndex(), token.getEndIndex());
				}
			}

			if (match != 0) {
				if ((match & LEFT) != 0) {
					out.remove(out.size() - 1);
					in.set(i - 1, null);
				}
				if ((match & MIDDLE) != 0) {
					in.set(i, null);
				}
				if ((match & RIGHT) != 0) {
					in.set(i + 1, null);
				}
				out.add(combinedToken);
			} else {
				out.add(token);
			}
		}
	}
}

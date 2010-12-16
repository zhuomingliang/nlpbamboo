package com.qunar.nlp.segment;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.List;
import java.util.StringTokenizer;

import com.qunar.nlp.segment.preprocess.BambooPreprocessor;
import com.qunar.nlp.segment.preprocess.Preprocessor;

public class Pdc2CrfppTrain {
	public static void main(String[] args) throws IOException {
		if (args.length < 2 || args.length > 4) {
			System.err.println("Usage: Pdc2CrfppTrain <input_filename> <output_filename> [input_charset] [output_charset]");
			System.exit(1);
		}
		String inputFilename = args[0], outputFilename = args[1];
		String inputCharset = "UTF-8", outputCharset = "UTF-8";
		if (args.length > 2) {
			inputCharset = args[2];
		}
		if (args.length > 3) {
			outputCharset = args[3];
		}
		
		Preprocessor preprocessor = new BambooPreprocessor();
		
		BufferedReader br = null;
		PrintWriter pw = null;
		try {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(inputFilename), inputCharset));
			pw = new PrintWriter(new OutputStreamWriter(new FileOutputStream(outputFilename), outputCharset));
			String line;
			int wordCount = 0, lineCount = 0;
			while ((line = br.readLine()) != null) {
				StringTokenizer tokenizer = new StringTokenizer(line);
				if (!tokenizer.hasMoreTokens()) {
					continue;
				}
				tokenizer.nextToken(); // 去掉人民日报语料的第一栏，即日期栏
				boolean hasToken = false;
				while (tokenizer.hasMoreTokens()) {
					String field = tokenizer.nextToken();
					field = field.replaceAll("/\\w+", ""); // 去掉词性标注
					field = field.replaceAll("\\[|\\]\\w+", "");  // 去掉合并词组标注
					if (!hasToken) {
						hasToken = true;
					}
					List<Token> tokens = preprocessor.process(field);
					String tag, lastTag = null;
					for (int i = 0; i < tokens.size(); ++i) {
						BambooToken token = (BambooToken) tokens.get(i);
						if (tokens.size() == 1) {
							tag = "S";
						} else if (i == 0) {
							tag = "B";
						} else if (i + 1 == tokens.size()) {
							tag = "E";
						} else if ("B".equals(lastTag)) {
							tag = "B1";
						} else if ("B1".equals(lastTag)) {
							tag = "B2";
						} else {
							tag = "M";
						}
						pw.println(token.getText() + " " + BambooChineseSegmentor.getTypeStr(token.getAttr()) + " " + tag);
						lastTag = tag;
					}
					wordCount++;
				}
				if (hasToken) {
					pw.println();
					lineCount++;
					if (lineCount % 1000 == 0) {
						System.err.println("Record: " + lineCount);
					}
				}
			}
			System.err.println("done!");
			System.err.println("#line " + lineCount + "; #word " + wordCount);
		} finally {
			if (br != null) {
				br.close();
			}
			if (pw != null) {
				pw.close();
			}
		}
	}
}

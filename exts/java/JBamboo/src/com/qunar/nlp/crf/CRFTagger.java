package com.qunar.nlp.crf;

import gnu.trove.TObjectIntHashMap;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.zip.GZIPInputStream;

public final class CRFTagger implements Tagger {
	public static final String[] BOS = {
		"_B-1", "_B-2", "_B-3", "_B-4"
	};
	public static final String[] EOS = {
		"_B+1", "_B+2", "_B+3", "_B+4"
	};
	
	public String[] templates;
	public int xsize;
	public int ysize;
	public String[] y;
	public int maxId;
	public double[] alpha;
	public int stateStartIndex;
	
	private TObjectIntHashMap<String> dict;
	
	public CRFTagger(File textModelFile, boolean isGziped, String charset) throws IOException {
		BufferedReader br = null;
		if (isGziped) {
			br = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(textModelFile)), charset));
		} else {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(textModelFile), charset));
		}
		
		String line;
		try {
			// header
			line = br.readLine();
			String versionHeader = "version: ";
			if (!line.startsWith(versionHeader)) {
				throw new IllegalStateException("wrong format: " + line);
			}
			
			line = br.readLine();
			String costFactorHeader = "cost-factor: ";
			if (line.startsWith(costFactorHeader)) {
				Double.parseDouble(line.substring(costFactorHeader.length()));
			} else {
				throw new IllegalStateException("wrong format: " + line);
			}
			
			line = br.readLine();
			String maxIdHeader = "maxid: ";
			if (line.startsWith(maxIdHeader)) {
				maxId = Integer.parseInt(line.substring(maxIdHeader.length()));
			} else {
				throw new IllegalStateException("wrong format: " + line);
			}
			
			line = br.readLine();
			String xsizeHeader = "xsize: ";
			if (line.startsWith(xsizeHeader)) {
				xsize = Integer.parseInt(line.substring(xsizeHeader.length()));
			} else {
				throw new IllegalStateException("wrong format: " + line);
			}
			br.readLine();
			
			// y
			List<String> yl = new ArrayList<String>();
			while ((line = br.readLine()) != null) {
				if (line.trim().length() == 0) {
					break;
				} else {
					yl.add(line);
				}
			}
			y = yl.toArray(new String[yl.size()]);
			ysize = y.length;
			
			// template
			List<String> tmpls = new ArrayList<String>();
			while ((line = br.readLine()) != null) {
				if (line.trim().length() == 0) {
					break;
				} else {
					if (line.startsWith("U")) {
						tmpls.add(line);
					}
				}
			}
			templates = tmpls.toArray(new String[tmpls.size()]);
			
			// dict
			dict = new TObjectIntHashMap<String>();
			
			while ((line = br.readLine()) != null) {
				if (line.trim().length() == 0) {
					break;
				}
				StringTokenizer tokenizer = new StringTokenizer(line);
				int i = Integer.parseInt(tokenizer.nextToken());
				String token = tokenizer.nextToken();
				if (token.equals("B")) {
					stateStartIndex = i;
				}
				dict.put(token, i + 1); // // Trove中的map在key不存在时返回0而不是-1
			}
			
			// alpha
			alpha = new double[maxId];
			int i = 0;
			while ((line = br.readLine()) != null) {
				if (line.trim().length() == 0) {
					break;
				}
				alpha[i++] = Double.parseDouble(line);
			}
		} finally {
			br.close();
		}
	}
	
	private String applyRule(String template, int pos, List<String[]> sentence) {
		StringBuilder sb = new StringBuilder();
		for (int i = 0; i < template.length(); i++) {
			char c = template.charAt(i);
			switch (c) {
			case '%':
				i++;
				if (i >= template.length()) {
					throw new IllegalStateException("wrong template format: " + template);
				}
				char nextChar = template.charAt(i);
				switch (nextChar) {
				case 'x':
					i++;
					i = getIndex(sb, template, i, pos, sentence);
					break;
				default:
					throw new IllegalStateException("wrong template format: " + template);
				}
				break;
			default:
				sb.append(c);
			}
		}
		return sb.toString();
	}
	
	private int getIndex(StringBuilder sb, String template, int index, int pos, List<String[]> sentences) {
		char firstChar = template.charAt(index++);
		if (firstChar != '[') {
			throw new IllegalStateException("wrong template format: " + template);
		}
		int col = 0, row = 0;
		int neg = 1;
		if (template.charAt(index++) == '-') {
			neg = -1;
		} else {
			index--;
		}
		
		NEXT_ROW:
		for (; index < template.length(); index++) {
			char c = template.charAt(index);
			switch (c) {
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
				row = row * 10 + (c - '0');
				break;
			case ',':
				index++;
				break NEXT_ROW;
			default: 
				throw new IllegalStateException("wrong template format: " + template);
			}
		}
		
		NEXT_COLUMN:
		for (; index < template.length(); index++) {
			char c = template.charAt(index);
			switch (c) {
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
				col = col * 10 + (c - '0');
				break;
			case ']':
				break NEXT_COLUMN;
			default:
				throw new IllegalStateException("wrong template format: " + template);
			}
		}
		
		row *= neg;
		if (row < -4 || row > 4 || col < 0 || col > xsize) {
			throw new IllegalStateException("wrong template format: " + template);
		}
		
		int idx = pos + row;
		if (idx < 0) {
			sb.append(BOS[-idx - 1]);
		} else if (idx >= sentences.size()) {
			sb.append(EOS[idx - sentences.size()]);
		} else {
			sb.append(sentences.get(idx)[col]);
		}
		return index;
	}
	
	public String[] tag(List<String[]> sentence) {
		if (sentence == null || sentence.size() == 0) {
			return new String[0];
		}
		int size = sentence.size();
	    int[][] features = new int[size][];
		for (int i = 0; i < size; i++) {
			int[] feature = new int[templates.length];
			int index = 0;
			for (String template : templates) {
				String attribute = applyRule(template, i, sentence);
				feature[index++] = dict.get(attribute) - 1;
			}
			features[i] = feature;
		}
		
		double[][] bestCosts = new double[size][];
		int[][] previousIndexes = new int[size][];
		for (int i = 0; i < size; i++) {
			bestCosts[i] = new double[ysize];
			int[] feature = features[i];
			if (i > 0) {
			    previousIndexes[i] = new int[ysize];
			}
			for (int toIndex = 0; toIndex < ysize; toIndex++) {
				double cost = 0;
				for (int featureId : feature) {
				    if (featureId >= 0) {
					    cost += alpha[featureId + toIndex];
					}
				}
				if (i > 0) {
				    double bestAdditionalCost = -Double.MAX_VALUE;
				    int previousIndex = -1;
				    for (int fromIndex = 0; fromIndex < ysize; fromIndex++) {
				        double additionalCost = bestCosts[i - 1][fromIndex] + alpha[stateStartIndex + fromIndex * ysize + toIndex];
				        if (additionalCost > bestAdditionalCost) {
				    	    bestAdditionalCost = additionalCost;
				    	    previousIndex = fromIndex;
				        }
				    }
				    bestCosts[i][toIndex] = cost + bestAdditionalCost;
				    previousIndexes[i][toIndex] = previousIndex;
				} else {
					bestCosts[i][toIndex] = cost;
				}
			}
		}
		
		double bestCost = -Double.MAX_VALUE;
		int bestLastIndex = -1;
		for (int j = 0; j < ysize; j++) {
			double cost = bestCosts[size - 1][j];
			if (cost > bestCost) {
				bestCost = cost;
				bestLastIndex = j;
			}
		}
		
		String[] result = new String[size];
		result[size - 1] = y[bestLastIndex];
		int index = size - 1;
		int previousIndex = bestLastIndex;
		while (index > 0) {
			previousIndex = previousIndexes[index][previousIndex];
			index--;
			result[index] = y[previousIndex];
		}
		return result;
	}
}

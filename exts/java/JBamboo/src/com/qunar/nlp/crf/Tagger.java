package com.qunar.nlp.crf;

import java.util.List;

public interface Tagger {
	String[] tag(List<String[]> features);
}

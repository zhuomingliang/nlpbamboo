package com.qunar.nlp.segment.preprocess;

import java.util.List;

import com.qunar.nlp.segment.Token;

public interface Preprocessor {
	List<Token> process(String text);
}

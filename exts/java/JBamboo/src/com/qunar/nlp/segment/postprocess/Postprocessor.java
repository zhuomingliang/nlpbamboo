package com.qunar.nlp.segment.postprocess;

import java.util.List;

import com.qunar.nlp.segment.Token;

/**
 * @author liangliang.fu
 *
 */
public interface Postprocessor {
	void process(List<Token> in, List<Token> out);
}

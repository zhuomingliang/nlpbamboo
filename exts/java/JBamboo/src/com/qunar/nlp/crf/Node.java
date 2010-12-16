package com.qunar.nlp.crf;

import java.util.ArrayList;
import java.util.List;


public final class Node {
	private static final double MINUS_LOG_EPSILON = 50;
	
	public int x;
	public int y;
	public double alpha;
	public double beta;
	public double cost;
	public double bestCost;
	public Node previousNode;
	public int[] feature;
	public List<Path> leftPaths;
	public List<Path> rightPaths;
	
	public Node(int ysize, boolean isFirst, boolean isLast) {
		if (!isFirst) {
			leftPaths = new ArrayList<Path>(ysize);
		}
		if (!isLast) {
			rightPaths = new ArrayList<Path>(ysize);
		}
	}
	
	
	public void calculateAlpha() {
		alpha = 0.0;
		boolean first = true;
		for (Path leftPath : leftPaths) {
			if (first) {
				alpha = leftPath.cost + leftPath.leftNode.alpha;
				first = false;
			} else {
				alpha =  logSumExp(alpha, leftPath.cost + leftPath.leftNode.alpha);
			}
		}
		alpha += cost;
	}
	public void calculateBeta() {
		beta = 0.0;
		boolean first = true;
		for (Path rightPath : rightPaths) {
			if (first) {
				beta = rightPath.cost + rightPath.rightNode.beta;
				first = false;
			} else {
				beta = logSumExp(beta, rightPath.cost + rightPath.rightNode.beta);
			}
		}
		beta += cost;
	}
	private static double logSumExp(double x, double y) {
//		if (flag) {	// init mode
//			return y;
//		}
		double min = Math.min(x, y);
		double max = Math.max(x, y);
		if (max > min + MINUS_LOG_EPSILON) {
			return max;
		} else {
			return max + Math.log(Math.exp(min - max) + 1.0);
		}
	}
}

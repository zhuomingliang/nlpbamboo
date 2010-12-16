package com.qunar.nlp.crf;

public final class Path {
	public Node leftNode;
	public Node rightNode;
	public int[] feature;
	public double cost;
	public Path() {}
	public void add(Node leftNode, Node rightNode) {
		this.leftNode = leftNode;
		this.rightNode = rightNode;
		leftNode.rightPaths.add(this);
		rightNode.leftPaths.add(this);
	}
}

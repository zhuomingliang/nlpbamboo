#include "udgraph.hxx"
#include <cstring>
#include <cstdio>

namespace bamboo { namespace kea {

UDGraph::UDGraph(int max_num) {
	this->_MAX = max_num;
	int size = (max_num+1)*max_num/2-max_num+1;
	_matrix = new double[size];
	_norm_factor = new double[max_num];

	for(int i=0; i<size; ++i) _matrix[i]=0;
	for(int i=0; i<max_num; ++i) _norm_factor[i]=0;
}

UDGraph::~UDGraph() {
	delete [] _matrix;
	delete [] _norm_factor;
}

void UDGraph::add_edge(int v1, int v2, double w) {
	if(v1 != v2)
	{
		int index = _get_index(v1+1, v2+1);
		_matrix[index] = w;
		_norm_factor[v2] += w;
		_norm_factor[v1] += w;
	}
}

void UDGraph::delete_edge(int v1, int v2) {
	if(v1 != v2)
	{
		int index = _get_index(v1+1, v2+1);
		_norm_factor[v1] -= _matrix[index];
		_norm_factor[v2] -= _matrix[index];
		_matrix[index] = 0;
	}
}

double UDGraph::get_edge(int v1, int v2) {
	if(v1 == v2)
		return 0;

	int index = _get_index(v1+1, v2+1);
	return _matrix[index];
}

double UDGraph::get_norm_factor(int i) {
	if(i<_MAX)
		return _norm_factor[i];
	return 0;
}

int UDGraph::_get_index(int i, int j) {
	if(i>j)
	{
		int k = j;
		j = i;
		i = k;
	}
	if(j==_MAX)
	{
		return int(int(i*_MAX)-int(i*(i+1)/2))-1;
	}

	return (int)((j-i) + this->_get_index(i-1, _MAX));
}

}} //end of namespace bamboo::kea


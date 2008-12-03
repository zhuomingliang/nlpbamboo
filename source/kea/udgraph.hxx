#ifndef KEA_UDGRAPH_HXX
#define KEA_UDGRAPH_HXX

namespace bamboo { namespace kea {

class UDGraph {
private:
	int _MAX;
	double *_matrix;
	double *_norm_factor;

public:
	UDGraph(int max_num);
	~UDGraph();

	void add_edge(int v1, int v2, double w);
	void delete_edge(int v1, int v2);

	double get_edge(int v1, int v2);
	double get_norm_factor(int i);

private:
	int _get_index(int i, int j);
};

}} //end of namespace bamboo::kea

#endif //end of KEA_UDGRAPH_HXX

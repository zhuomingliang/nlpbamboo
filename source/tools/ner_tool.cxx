#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <unistd.h>
#include "utf8.hxx"

#define SEPT " \r\n\t"
#define USAGE "ner_tool -s corpus -t entity_type\nGenerate Training files for ns/nt entity recognition\n\nentity type including: ns nt\n"

const char * suffix[] = {"省","市","县","州","郡","区","乡","镇","村","庄","城","屯","苑","街","路","里","口","道","巷","弄","楼","馆","寺","宫","府","园","门","桥","山","岗","峰","河","江","湖","海",NULL};

const char * direct[] = {
"东", "南", "西", "北", "中", "内", "外", NULL
};

const char * cn_digit[] = {
"一", "二", "三", "四", "五", "六", "七", "八", "九", "零", "○", NULL
};

std::set<std::string> suffix_set;
std::set<std::string> direct_set;
std::set<std::string> cn_digit_set;

void ns_init() {
	const char ** p = suffix;
	while(*p!=NULL) {
		suffix_set.insert(*p);
		p++;
	}
	p = direct;
	while(*p!=NULL) {
		direct_set.insert(*p);
		p++;
	}
	p = cn_digit;
	while(*p!=NULL) {
		cn_digit_set.insert(*p);
		p++;
	}
}

int get_ns_prop(const char * str) {
	if(suffix_set.find(str) != suffix_set.end()) {
		return 1;
	}
	if(direct_set.find(str) != direct_set.end()) {
		return 2;
	}
	if(cn_digit_set.find(str) != cn_digit_set.end()) {
		return 3;
	}

	const char * p = str;
	while(*p) {
		if(*p<'0' || *p>'9')
			return 5;
		p++;
	}
	return 4;
}

const char * ns_prop_str[] = {
	"0", "1", "2", "3", "4", "5"
};


std::string aim_ner;

enum state_t {
	st_error,
	st_bner,
	st_ener,
	st_normal
};

const char * get_6tag(int p, int total) {
	if(total == 1)
		return "S";
	else if(p == total)
		return "E";

	switch(p) {
	case 1:
		return "B";
	case 2:
		return "B1";
	case 3:
		return "B2";
	default:
		return "M";
	}
}

const char * get_4tag(int p, int total) {
	if(total == 1)
		return "S";
	else if(p == total)
		return "E";

	switch(p) {
	case 1:
		return "B";
	default:
		return "M";
	}
}

size_t str_to_vec(std::string str, const char *sept, std::vector<std::string> &res) {
	std::string tmpStr;
	int pos;
	res.clear();
	if(res.capacity() < str.size()) res.reserve(str.size()); 
	do {
		pos = str.find_first_of(sept);
		if(pos<0) {
			if(str.length()>0) {
				res.push_back(str);
			}
			break;
		}
		tmpStr = str.substr(0,pos);
		if(tmpStr.length()>0) {
			res.push_back(tmpStr);
		}
		str = str.substr(pos+1,str.length());
	} while(pos>=0);
	return res.size();
}

size_t utf8_to_vec(const char *s, std::vector<std::string> &vec) {
	char uch[8], cch;
	size_t step;
	std::string buf("");

	vec.clear();
	for(cch=0;;s+=step,cch=0) {
		step = bamboo::utf8::first(s, uch);
		cch = bamboo::utf8::dbc2sbc(uch, step);
		if(cch!=0 && !isspace(cch)) {
			buf += cch;
		} else if(cch!=0 && isspace(cch)) {
			if(buf.size()>0)
				vec.push_back(buf);
			buf.clear();
		} else if(*uch != 0) {
			if(buf.size()>0)
				vec.push_back(buf);
			buf.clear();
			vec.push_back(uch);
		} else {
			if(buf.size()>0)
				vec.push_back(buf);
			buf.clear();
			break;
		}
	}
	return vec.size();
}

void parse_token(std::string &token, state_t & st, std::string & pos, std::string & ner_pos) {
	pos = ner_pos = "";
	std::string::size_type p;

	if(token.size() == 0) {
		st = st_error;
		return;
	} else if(token[0] == '[') {
		st = st_bner;
		token = token.substr(1);
	} else {
		p = token.find_last_of(']');
		if(p != std::string::npos) {
			st = st_ener;
			ner_pos = token.substr(p+1);
			token = token.substr(0, p);
		} else {
			st = st_normal;
		}
	}

	p = token.find_last_of('/');
	if(p != std::string::npos) {
		pos = token.substr(p+1);
		token = token.substr(0, p);
	} else {
		st = st_error;
		return;
	}
}

void parse_line(std::string line,
		std::vector<std::pair<std::string,
		std::pair<std::string, std::string> > > & line_res) {
	std::vector<std::string> utf8_vec;
	std::vector<std::string> vec;

	line_res.clear();
	size_t i, len = str_to_vec(line, SEPT, vec);
	size_t ner_stack = 0;
	size_t elem_cnt = 0;
	std::vector<std::pair<std::string, std::string> > ner_arr;

	for(i=0; i<len; ++i) {
		if(i==0) continue;
		elem_cnt++;
		state_t st;
		std::string pos, ner_pos, token = vec[i];
		parse_token(token, st, pos, ner_pos);

		if(st == st_error) continue;
		if(st == st_bner) {
			++ner_stack;
		}

		if(st == st_normal && ner_stack == 0) {
			size_t t1, t2 = utf8_to_vec(token.c_str(), utf8_vec);
			bool flag = false;
			if(!aim_ner.compare(pos)) {
				flag = true;
			}
			for(t1 = 0; t1 < t2; ++t1) {
				const char * tag = "O";
				if(flag) {
					tag = get_6tag(t1+1, t2);
				}
				line_res.push_back(std::make_pair(utf8_vec[t1],
					std::make_pair(get_4tag(t1+1, t2), tag)));
			}
			continue;
		}

		ner_arr.push_back(std::make_pair(token, pos));

		if(st == st_ener) {
			--ner_stack;
			if(ner_stack == 0) {
				size_t s1, s2;
				if(!aim_ner.compare(ner_pos)) {
					s2 = ner_arr.size();
					std::vector<std::pair<std::string, const char *> > tmp_vec;
					for(s1 = 0; s1 < s2; ++s1) {
						utf8_to_vec(ner_arr[s1].first.c_str(), utf8_vec);
						size_t k1, k2 = utf8_vec.size();
						for(k1 = 0; k1 < k2; ++k1) {
							tmp_vec.push_back(std::make_pair(utf8_vec[k1],
								get_4tag(k1+1, k2)));
						}
					}
					s2 = tmp_vec.size();
					for(s1 = 0; s1 < s2; ++s1) {
						line_res.push_back(std::make_pair(tmp_vec[s1].first,
							std::make_pair(tmp_vec[s1].second,
								get_6tag(s1+1, s2))));
					}
				} else {
					s2 = ner_arr.size();
					for(s1 = 0; s1 < s2; ++s1) {
						std::string tmp_token = ner_arr[s1].first;
						std::string tmp_pos = ner_arr[s1].second;
						size_t t1, t2 = utf8_to_vec(tmp_token.c_str(), utf8_vec);
						bool flag = false;
						if(!aim_ner.compare(tmp_pos)) {
							flag = true;
						}
						for(t1 = 0; t1 < t2; ++t1) {
							const char * tag = "O";
							if(flag) {
								tag = get_6tag(t1+1, t2);
							}
							line_res.push_back(std::make_pair(utf8_vec[t1],
								std::make_pair(get_4tag(t1+1, t2), tag)));
						}
					}
				}

				ner_arr.clear();
			}
		}
	}
}

void generate_ns(std::fstream & fin) {
	ns_init();

	std::string line;
	std::vector<std::pair<std::string,
		std::pair<std::string, std::string> > > line_res;
	while(std::getline(fin, line)) {
		if(line.size()==0) continue;
		parse_line(line, line_res);
		size_t i, size = line_res.size();
		for(i=0; i<size; ++i) {
			int t = get_ns_prop(line_res[i].first.c_str());
			const char * ns_prop = ns_prop_str[t];
			std::cout<<line_res[i].first<<"\t"
				<<line_res[i].second.first<<"\t"
				<<ns_prop<<"\t"
				<<line_res[i].second.second
				<<std::endl;
		}
		if(size>0)
			std::cout<<std::endl;
		line.clear();
	}
}

void generate_nt(std::fstream & fin) {
	std::string line;
	std::vector<std::pair<std::string,
		std::pair<std::string, std::string> > > line_res;
	while(std::getline(fin, line)) {
		if(line.size()==0) continue;
		parse_line(line, line_res);
		size_t i, size = line_res.size();
		for(i=0; i<size; ++i) {
			std::cout<<line_res[i].first<<"\t"
				<<line_res[i].second.first<<"\t"
				<<line_res[i].second.second
				<<std::endl;
		}
		if(size>0)
			std::cout<<std::endl;
		line.clear();
	}
}

int main(int argc, char *argv[]) {

	int opt;
	const char * input_file = NULL, * ner_type = NULL;

	while( (opt=getopt(argc, argv, "s:t:")) != -1) {
		switch(opt) {
		case 's':
			input_file = optarg;
			break;
		case 't':
			ner_type = optarg;
			break;
		case '?':
			std::cerr << USAGE << std::endl;
			exit(1);
		}
	}

	if(!input_file || !ner_type) {
		std::cerr << USAGE << std::endl;
		exit(1);
	}

	aim_ner = ner_type;

	std::fstream fin(input_file, std::ios::in);

	if(aim_ner == "nt") {
		generate_nt(fin);
	} else if(aim_ner == "ns") {
		generate_ns(fin);
	} else {
		std::cerr << USAGE << std::endl;
		exit(1);
	}

	fin.close();
	return 0;
}

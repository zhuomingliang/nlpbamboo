#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "utf8.hxx"

using namespace bamboo;

size_t str2vec(std::string str, const char *separator, std::vector<std::string> &res) {
	std::string tmpStr;
	int pos;
	res.clear();
	if(res.capacity() < str.size()) res.reserve(str.size()); 
	do {
		pos = str.find_first_of(separator);
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
		step = utf8::first(s, uch);
		cch = utf8::dbc2sbc(uch, step);
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

int main(int argc, char *argv[]) {
	std::string line;
	std::vector<std::string> vec;
	std::fstream fin(argv[1], std::ios::in);
	while(getline(fin, line)) {
		str2vec(line, " \r\n\t", vec);
		size_t i, size=vec.size();
		if(size==0) continue;
		for(i=0; i<size; ++i) {
			bool flag;
			std::string cur_word = vec[i];
			std::string::size_type ptr = cur_word.find("/nr");
			if(ptr==std::string::npos) flag = false;
			else {
				flag = true;
				cur_word = cur_word.substr(0, ptr);
			}

			std::vector<std::string> vec2;
			utf8_to_vec(cur_word.c_str(), vec2);

			size_t j, len=vec2.size();
			if(len==0) continue;
			const char * tag;
			for(j=0; j<len-1; ++j) {
				switch(j) {
					case 0: tag = "B";break;
					case 1: tag = "B1";break;
					case 2: tag = "B2";break;
					default: tag = "M";break;
				}
				if(!flag) tag = "O";
				std::cout << vec2[j] << "\t"
					<< tag <<std::endl;
			}
			if(len==1)
				tag = "S";
			else
				tag = "E";
			if(!flag)
				tag = "O";
			std::cout << vec2[j] << "\t"
				<< tag << std::endl;
		}
		std::cout<<std::endl;
	}
	fin.close();
	return 0;
}

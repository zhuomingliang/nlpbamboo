/*
 * Copyright (c) 2008, weibingzheng@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY weibingzheng@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL weibingzheng@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <getopt.h>
#include <limits.h>
#include "utf8.hxx"


using namespace std;

static const std::string delimiter = " \t\r\n";
#define USAGE "crf2_tool [-i input_file] -o output_file"

void str_to_vec(string source_str, string delimiter, std::vector<std::string> &vec) {
	std::string str;
	int pos;

	do {
		pos = source_str.find_first_of(delimiter);
		if(pos<0) {
			if(source_str.length()>0)
				vec.push_back(source_str);
			break;
		}
		str = source_str.substr(0,pos);
		if(str.length()>0)
			vec.push_back(str);
		source_str = source_str.substr(pos+1,source_str.length());
	} while(pos>=0);
}

const char *get_attr_str(int attr) {
	switch(attr) {
	case 0:
		return "ASCII";
	case 1:
		return "CN";
	case 2:
		return "PUNC";
	default:
		return "CN";
	};
}

std::string generate(vector<std::string> &vec_str) {
	size_t i, len=vec_str.size();
	std::string bucket_cn, bucket_en;
	std::vector<std::string> buckets_cn, buckets_en;
	std::vector<int> attr;
	stringstream str_stream;
	for(i=0; i<len; ++i) {
		enum {
			ascii=0,
			cword,
			punct,
			begin,
			end,
		} state, last;
		char uch[8], cch;
		const char * s = vec_str[i].c_str();
		size_t step;

		attr.clear();
		bucket_en.clear(); bucket_cn.clear();
		buckets_en.clear(); buckets_cn.clear();
		for(state=begin;;s+=step) {
			last = state;
			step = utf8::first(s, uch); 
			cch = utf8::to_dbc(uch, step);
			if(ispunct(cch)&&step==vec_str[i].size()) state = punct;
			else if(!strcmp(uch, "。")&&step==vec_str[i].size()) state = punct;
			else if(*uch==0) state = end;
			else if(isascii(cch) && cch!=0) state = ascii;
			else state = cword;

			if(last != state) {
				if(bucket_en.size()>0) {
					buckets_en.push_back(bucket_en);
					buckets_cn.push_back(bucket_cn);
					attr.push_back(ascii);
					bucket_en.clear(); bucket_cn.clear();
				}
				if(state == end) break;
			}
			if(state == punct) {
				std::string str;
				if(cch) str = cch; else str = uch;
				buckets_en.push_back(str);
				buckets_cn.push_back(uch);
				attr.push_back(punct);
				bucket_en.clear(); bucket_cn.clear();
			}
			if(state == cword) {
				buckets_en.push_back(uch);
				buckets_cn.push_back(uch);
				attr.push_back(cword);
				bucket_en.clear(); bucket_cn.clear();
			}
			if(state == ascii) {
				bucket_cn += uch;
				bucket_en += cch;
			}
		}
		size_t size = buckets_cn.size();
		if(size==1) str_stream<<buckets_cn[0]<<" "<<get_attr_str(attr[0])<<" S"<<endl;
		else {
			size_t j;
			for(j=0; j<size-1; ++j) {
				switch(j) {
				case 0:
					str_stream<<buckets_cn[j]<<" "<<get_attr_str(attr[j])<<" B"<<endl; break;
				case 1:
					str_stream<<buckets_cn[j]<<" "<<get_attr_str(attr[j])<<" B1"<<endl; break;
				case 2:
					str_stream<<buckets_cn[j]<<" "<<get_attr_str(attr[j])<<" B2"<<endl; break;
				default:
					str_stream<<buckets_cn[j]<<" "<<get_attr_str(attr[j])<<" M"<<endl; break;
				}
			}
			str_stream<<buckets_cn[j]<<" "<<get_attr_str(attr[j])<<" E"<<endl;
		}
	}
	if(len >= 1)
		str_stream<<endl;
	return str_stream.str();
}

int main(int argc, char *argv[]) {
	int opt;

	char *input_file = NULL;
	char *output_file = NULL;
	while( (opt=getopt(argc, argv, "i:o:h")) != -1) {
		switch(opt) {
		case 'i':
			input_file = optarg;
			break;
		case 'o':
			output_file = optarg;
			break;
		case 'h':
			printf("%s\n", USAGE);
			exit(0);
		case '?':
			printf("%s\n", USAGE);
			exit(1);
		}
	}

	FILE *fin, *fout;
	if(!output_file) {
		printf("%s\n", USAGE);
		exit(1);
	}

	if(!input_file) fin = stdin;
	else fin = fopen(input_file, "r");

	if(!fin) {
		std::cerr<<"Cannot open file "<<input_file<<std::endl;
		exit(1);
	}

	fout = fopen(output_file, "w");

	if(!fout) {
		std::cerr<<"Cannot open file "<<output_file<<std::endl;
		exit(1);
	}

	char buf[LINE_MAX];
	std::vector<std::string> vec_str;

	while(fgets(buf, LINE_MAX, fin)!=NULL) {
		vec_str.clear();
		str_to_vec(buf, delimiter, vec_str);
		string tmp = generate(vec_str);
		fprintf(fout, "%s", tmp.c_str());
	}

	fclose(fout);
	fclose(fin);
	return 0;
}

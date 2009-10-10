/*
 * Copyright (c) 2008, detrox@gmail.com
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
 * THIS SOFTWARE IS PROVIDED BY detrox@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL detrox@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef CONFIG_FINDER_HXX
#define CONFIG_FINDER_HXX

#include <sys/types.h>
#include <sys/stat.h>
#include <config_factory.hxx>

namespace bamboo {

class ConfigFinder
{
private:
	static ConfigFinder		*_instance;

	ConfigFinder() {};
public:
	static ConfigFinder *get_instance()
	{
		if (_instance == NULL)
			_instance = new ConfigFinder();
		
		return _instance;
	}

	IConfig *find(const char *file, const char *top = NULL, bool verbose = false) 
	{
		std::vector<std::string>::size_type i;
		struct stat buf;
		std::vector<std::string> v;

		if (top)
			v.push_back(top);
		if (file[0])
			v.push_back(file);
		v.push_back(std::string("./etc/bamboo").append(file));
		v.push_back(std::string("/etc/bamboo/").append(file));
#ifdef BAMBOO_CFGDIR
		v.push_back(std::string(BAMBOO_CFGDIR).append(file));
#endif
		for (i = 0; i < v.size(); i++) {
			if (verbose)
				std::cerr << "trying " << v[i] << std::endl;
			if (stat(v[i].c_str(), &buf) == 0) {
				if (verbose)
					std::cerr	<< "use configuration: " << v[i]
								<< "." << std::endl;
				return ConfigFactory::create(v[i].c_str());
			}
		}

		throw std::runtime_error(std::string("can not find configuration: ") + file);
	}
};

} /* namespace bamboo */
#endif

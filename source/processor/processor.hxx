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

#ifndef PROCESSOR_HXX
#define PROCESSOR_HXX


#include <sys/time.h>
#include <vector>
#include "config_factory.hxx"
#include "token_impl.hxx"

namespace bamboo {


#define PROCESSOR_MAGIC /* Nothing */
#define PROCESSOR_MODULE(NAME) /* Nothing */

class Processor {
protected:
	virtual bool _can_process(TokenImpl *) = 0;
	virtual void _process(TokenImpl *token, std::vector<TokenImpl *> &out) = 0;
public:
	Processor() {};
	Processor(IConfig *_config) {};
	virtual ~Processor() {};

	virtual void init(const char *parameter) {};
	virtual void process(std::vector<TokenImpl *> &in, std::vector<TokenImpl *> &out)
	{
		size_t i, length;
		if (in.empty()) return;
		length = in.size();
		for (i = 0; i < length; i++) {
			if (_can_process(in[i])) {
				_process(in[i], out);
				delete in[i];
			} else {
				out.push_back(in[i]);
			}
		}
	}
};

} //namespace bamboo

#endif // PROCESSOR_HXX

/**
 *  Copyright (c) 2017-2018, Mike Tegtmeyer
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the author nor the names of its contributors may
 *        be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_options.h"

namespace cmd_options {
namespace alt {

namespace co = cmd_options;

/*
  Split a format string of the form:
    [long_name][delim][map_key]
  where everything after a missing delim is optional. The following is ok:

    long
    long,
    long,key

  Assertion failure for trailing delims or missing long. That is,

    ,key[,.*]
    long,key,*.*
*/
template<typename CharT>
inline std::pair<std::basic_string<CharT>,std::basic_string<CharT>>
split_fopt(const std::basic_string<CharT> &str, CharT delim)
{
  typedef std::basic_string<CharT> string_type;
  typedef std::pair<string_type,string_type> result_type;

  result_type result;

  auto lloc = std::find(str.begin(),str.end(),delim);
  assert(lloc != str.begin());

  result.first = string_type(str.begin(),lloc);

  if(lloc != str.end()) {
    assert(std::find(lloc+1,str.end(),delim) != str.end());

    result.second = string_type(lloc+1,str.end());
  }
  else
    result.second = result.first;

  return result;
}

template<typename CharT>
co::basic_option_pack<CharT>
unpack_f(const std::basic_string<CharT> &s)
{
  typedef std::basic_string<CharT> string_type;

  if (s.find(string_type{'-','f'}) == 0) {
    if (s.substr(2, 3) == string_type{'n','o','-'})
      return co::basic_option_pack<CharT>{true,true,s.substr(5),{},{'0'}};
    else
      return co::basic_option_pack<CharT>{true,true,s.substr(2),{},{'1'}};
  }

  return co::basic_option_pack<CharT>{false,{},{},{},{}};
}

template<typename T, typename CharT>
co::basic_option_description<CharT>
make_f_flag(const std::basic_string<CharT> &opt_spec,
  const std::basic_string<CharT> &ext_desc, bool negative_form = false,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  typedef std::basic_string<CharT> string_type;
  typedef co::basic_variable_map<CharT> variable_map_type;

  string_type long_opt;
  string_type mapped_key;
  std::tie(long_opt,mapped_key) = split_fopt(opt_spec,',');

  assert(std::all_of(long_opt.begin(),long_opt.end(),co::is_portable<CharT>));

  co::basic_option_description<CharT> desc;

  desc.unpack_option = unpack_f<CharT>;

  desc.mapped_key = [=](const string_type &_opt, std::size_t, std::size_t,
    const variable_map_type &)
    {
      if(_opt == long_opt)
        return std::make_pair(parse_flag::accept,mapped_key);

      return std::make_pair(parse_flag::reject,string_type());
    };

  if(negative_form) {
    desc.key_description = [=](void) {
      return string_type{'-','f','n','o','-'} + long_opt;
    };
  }
  else {
    desc.key_description = [=](void) {
      return string_type{'-','f'} + long_opt;
    };
  }

  desc.extended_description = [=](void) { return ext_desc; };
  co::set_default_option_value(co::basic_value<T,CharT>(),desc);
  co::set_default_constraints(cnts,desc,mapped_key);

  return desc;
}

template<typename T, typename CharT>
co::basic_option_description<CharT>
make_f_flag(const std::basic_string<CharT> &opt_spec,
  const std::basic_string<CharT> &ext_desc,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  return make_f_flag(opt_spec,ext_desc,false,cnts);
}

template<typename T, typename CharT>
inline co::basic_option_description<CharT>
make_f_flag(const CharT *opt_spec, const CharT *ext_desc,
  bool negative_form = false,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  return make_f_flag<T,CharT>(string_type(opt_spec),string_type(ext_desc),
    negative_form,cnts);
}

template<typename T, typename CharT>
inline co::basic_option_description<CharT>
make_f_flag(const CharT *opt_spec, const CharT *ext_desc,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  return make_f_flag<T,CharT>(string_type(opt_spec),string_type(ext_desc),
    false,cnts);
}

template<typename T, typename CharT>
inline co::basic_option_description<CharT>
make_f_flag(const CharT *opt_spec, const std::basic_string<CharT> &ext_desc,
  bool negative_form = false,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  return make_f_flag<T,CharT>(string_type(opt_spec),ext_desc,
    negative_form,cnts);
}

template<typename T, typename CharT>
inline co::basic_option_description<CharT>
make_f_flag(const CharT *opt_spec, const std::basic_string<CharT> &ext_desc,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  return make_f_flag<T,CharT>(string_type(opt_spec),ext_desc,
    false,cnts);
}

template<typename T, typename CharT>
inline co::basic_option_description<CharT>
make_f_flag(const std::basic_string<CharT> &opt_spec, const CharT *ext_desc,
  bool negative_form = false,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  return make_f_flag<T,CharT>(opt_spec,string_type(ext_desc),
    negative_form,cnts);
}

template<typename T, typename CharT>
inline co::basic_option_description<CharT>
make_f_flag(const std::basic_string<CharT> &opt_spec, const CharT *ext_desc,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  return make_f_flag<T,CharT>(opt_spec,string_type(ext_desc),
    false,cnts);
}






template<typename T, typename CharT>
co::basic_option_description<CharT>
make_hidden_f_flag(const std::basic_string<CharT> &opt_spec,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  auto &&desc = make_f_flag(opt_spec,std::basic_string<CharT>(),false,cnts);
  desc.key_description = nullptr;

  return desc;
}

template<typename T, typename CharT>
inline co::basic_option_description<CharT>
make_hidden_f_flag(const CharT *opt_spec,
  const co::basic_constraint<CharT> &cnts = co::basic_constraint<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  auto &&desc =  make_f_flag<T,CharT>(string_type(opt_spec),string_type(),
    false,cnts);
  desc.key_description = nullptr;

  return std::move(desc);
}


}
}

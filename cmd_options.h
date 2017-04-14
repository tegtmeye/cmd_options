/**
 *  Copyright (c) 2017, Mike Tegtmeyer
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

#ifndef LEMMA_CMD_OPTIONS_H
#define LEMMA_CMD_OPTIONS_H

#include <boost/any.hpp>

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <functional>

#include <iostream>

namespace lemma {

namespace cmd_options {


class command_option_error : public std::runtime_error {
  public:
    command_option_error(const std::string &what, std::size_t argno)
      :std::runtime_error(what), _where(argno) {}

    std::size_t where(void) const {
      return _where;
    }

  private:
    std::size_t _where;
};


typedef std::multimap<std::string,boost::any> variable_map;

/*
  function that takes key and returns ??
*/
template<typename CharT>
struct basic_option_description {
  typedef CharT                     char_type;
  typedef std::basic_string<CharT>  string_type;

  /*
    For the given long option (without the '--' prefix), return the key
    that is placed in the the corresponding variable_map if the argument
    value represents this descriptions long key or the empty string
    otherwise. If \c map_long_key is not set, then this description has
    no long key. For example, suppose this description represents the
    --foo argument. Now suppose the --bar argument was seen. Each
    option_description will be queried against the \c map_long_key
    function. If it is not set, this description is passed over. The
    function will be passed the "bar" string. Since this
    option_description represents the "foo" argument, it returns the
    empty string. Suppose later the --foo argument is provided. Since
    this option_description represents the "foo" argument, it returns
    "foo" which will be inserted into the variable_map as a key. Now
    suppose that the argument --foo and --bar are equivalent but we
    always want the value's key to be "foo" in the variable_map, then
    this function should return "foo" if the provided string equals
    either "foo" or "bar".
  */
  std::function<string_type(const string_type &)> map_long_key;

  //std::string_type long_option_description

  /*
    For the given short option (without the '-' prefix), return the key
    that is placed in the the corresponding variable_map if the argument
    value represents this descriptions short key or the empty string
    otherwise. If \c map_short_key is not set, then this description has
    no short key. For example, suppose this description represents the
    -f argument. Now suppose the -b argument was seen. Each
    option_description will be queried against the \c map_short_key
    function. If it is not set, this description is passed over. The
    function will be passed the "b" string. Since this
    option_description represents the "f" argument, it returns the empty
    string. Suppose later the -f argument is provided. Since this
    option_description represents the "f" argument, it returns "f" which
    will be inserted into the variable_map as a key. Now suppose that
    the argument -f and -b are equivalent but we always want the value's
    key to be "f" in the variable_map, then this function should return
    "f" if the provided string equals either "f" or "b".

    By convention, short arguments are alias for their longer
    counterparts. Therefore, the returned string should be the
    map_long_key. That is, if -f is short for --foo, then if the
    function receives "f", it should return "foo". This makes querying
    the final variable_map for values much easier. This is not required
    though as it is entirely possible that one may want to have a short
    argument without the corresponding long argument for legacy reasons
    or because they have different meanings.
  */
  std::function<string_type(const string_type &)> map_short_key;

  /*
    If no long- or short_keys are provided, then the option is a
    \c positional option. In this case, the option mean different things
    based on where it exists in the option list. The argument to the
    given function is the number of the positional option not including
    the options given a short or long key. For example, in the option
    list: --foo one --bar two -f -g three the positional options are:
    one, two, and three with position 1, 2, and 3 respectively. If
    set, the function can return a unique key that will appear in
    the variable_map for each positional option or the same if users
    choose to. If the function is not set, then all positional options
    will be available using the default-constructed \c string_type as
    the variable_map key. Like \c map_long_key and \c map_short_key, if
    this description does not define the given positional option, then
    return the empty string.
  */
  std::function<string_type(std::size_t)> implicit_key;


  /*
    Non-positional option cases:
      -- strictly do not provide a value (no make_value)
        -- option has an implicit, possibly empty, value (uses implicit_value)
        -- no implicit value, use default constructed

      -- provided value is optional (make_value exists)
        -- if given, option uses provided value (calls make_value)
        -- if missing, use an implicit, possibly empty, value
            (uses implicit_value)
        -- no implicit value, use default constructed

      -- providing a value is mandatory (make_value exists, no implicit_value)
        -- use the one provided (calls make_value)

    If set, return the implicit value of the option as a boost::any
    object. This value is used whenever the option either explicitly
    forbids a value or it is optional and one is not provided. If not
    set, the value defaults to \c string_type().

    The function's first parameter is the string given as the option's
    key exactly as provided to the option without the long- or
    short_option_flag prefix. This is useful to deal with nonstandard
    syntaxes. For example, given -frtti vs -fno-rtti, the variable_map
    key could be 'rtti' with a boolean value. The option forbids values
    so the mapped value must be determined by the key (specifically the
    presence or absence of the 'no' prefix.
  */
  std::function<
    boost::any(const string_type &, const variable_map &)> implicit_value;

  /*
    Positional option case:
      -- interpret the option value (make_value is set)
      -- use default constructed value (make_value is not set)

    If set, return the value of the as a boost::any as determined by the
    contents of the second parameter. If not set, the option explicitly
    forbids option values. I.e. --foo=bar or --foo bar will return an
    error unless the 'bar' is a valid positional option in the later
    case.

    The function's first parameter is the string given as the option's
    key exactly as provided to the option without the long- or
    short_option_flag prefix. This can be useful to deal with
    nonstandard syntaxes. If the option was a positional one, then the
    first parameter is the result of \c implicit_key if set or a
    default-constructed string_type if not set.

    The function's second parameter is the string given as the options
    value. It is possible for this value to be empty in the case where
    the given option is explicitly set to be empty. For example, the
    value of --foo "" is explicitly given (not missing) to be the empty
    string. The \c make_value function must be prepared to handle this
    case. This does not happen in the case of compound arguments. For
    example, the values in  -f"" and --foo="" are considered missing.

    The provided variable map may be used to selectively set the value
    based on the current state of the argument parsing or examine the
    current contents of the map to ensure validity before returning the
    value. For example, if only a certain number of options with the
    same key are valid or if certain combinations of options are
    mutually-exclusive.
  */

  std::function<
    boost::any(const string_type &,const string_type &, const variable_map &)>
      make_value;
};


template<typename CharT>
struct parse_options {
  typedef std::basic_string<CharT> string_type;

  string_type ignore_remaining_str = "--";
  string_type long_option_flag = "--";
  string_type short_option_flag = "-";
  typename string_type::size_type short_option_length = 1;
};


template<typename CharT>
std::pair<std::basic_string<CharT>,
  typename std::vector<basic_option_description<CharT> >::const_iterator>
find_long_key(const std::basic_string<CharT> &key,
  const std::vector<basic_option_description<CharT> > &grp)
{
  typedef std::basic_string<CharT> string_type;

  auto result = std::make_pair(string_type(),grp.begin());

  while(result.second != grp.end()) {
    if(result.second->map_long_key) {
      result.first = result.second->map_long_key(key);

      if(!result.first.empty())
        break;
    }

    ++result.second;
  }

  return result;
}

template<typename CharT>
std::pair<std::basic_string<CharT>,
  typename std::vector<basic_option_description<CharT> >::const_iterator>
find_short_key(const std::basic_string<CharT> &key,
  const std::vector<basic_option_description<CharT> > &grp)
{
  typedef std::basic_string<CharT> string_type;

  auto result = std::make_pair(string_type(),grp.begin());

  while(result.second != grp.end()) {
    if(result.second->map_short_key) {
      result.first = result.second->map_short_key(key);

      if(!result.first.empty())
        break;
    }

    ++result.second;
  }

  return result;
}

template<typename CharT>
std::pair<std::basic_string<CharT>,
  typename std::vector<basic_option_description<CharT> >::const_iterator>
find_positional_key(std::size_t pos,
  const std::vector<basic_option_description<CharT> > &grp)
{
  typedef std::basic_string<CharT> string_type;

  auto result = std::make_pair(string_type(),grp.begin());

  while(result.second != grp.end()) {
    if(result.second->implicit_key) {
      result.first = result.second->implicit_key(pos);

      if(!result.first.empty())
        break;
    }

    ++result.second;
  }

  return result;
}


template<typename CharT>
void parse_long(const CharT * const argv[], std::size_t &pos,
  std::size_t argc, const std::vector<basic_option_description<CharT> > &grp,
  variable_map &vm, const parse_options<CharT> &opt)
{
  typedef std::basic_string<CharT> string_type;
  typedef typename string_type::size_type size_type;
  typedef std::vector<basic_option_description<CharT> > options_group_type;

  // eat long_option_flag
  string_type arg(argv[pos]);

  string_type arg1 = arg.substr(opt.long_option_flag.size());

  size_type eqloc = arg1.find('=');
  string_type key = arg1.substr(0,eqloc);

  auto &&mapped_key_desc = find_long_key(key,grp);
  const string_type &mapped_key = mapped_key_desc.first;
  typename options_group_type::const_iterator desc = mapped_key_desc.second;

  if(desc == grp.end()) {
    std::stringstream err;
    err << "unrecognized option '" << argv[pos] << "'";
    throw command_option_error(err.str(),pos);
  }

  boost::any mapped_value;
  if(!desc->make_value) {
    // key only
    if(eqloc != string_type::npos) {
    // has an '=' but shouldn't
      std::stringstream err;
      err << "option '" << argv[pos] << "' does not take any arguments";
      throw command_option_error(err.str(),pos);
    }

    if(desc->implicit_value)
      mapped_value = desc->implicit_value(key,vm);
  }
  else {
    // may or may not have a value
    bool empty_arg = false;
    string_type value;
    if(eqloc != string_type::npos) {
      // form is key=value otherwise key=[value missing]
      value = arg1.substr(++eqloc);
    }
    else {
      // value must be in next argument unless it is another option
      if(pos+1 < argc) {
        string_type arg2(argv[pos+1]);

        if(arg2.empty()) {
          empty_arg = true;
          ++pos;
        }
        else if((arg2.find(opt.short_option_flag) != 0) &&
          (arg2.find(opt.long_option_flag) != 0) &&
          (arg2.find(opt.ignore_remaining_str) != 0))
        {
          value = arg2;
          /*
            increment pos to show that the next argument was a value and
            not an option
          */
          ++pos;
        }
      }
    }

    if(value.empty() && !empty_arg) {
      // use implicit values
      if(desc->implicit_value)
        mapped_value = desc->implicit_value(key,vm);
      else {
        std::stringstream err;
        err << "the required argument for option '" << argv[pos]
          << "' is missing";
        throw command_option_error(err.str(),pos);
      }
    }
    else
      mapped_value = desc->make_value(key,value,vm);
  }

  // place key/value in vm
  vm.insert(std::make_pair(mapped_key,mapped_value));
}

/*
  Short options are of the form:

  -{option_char}{value} ie -v3
  -{option_char} {value} ie -o outfile

  does not handle multiswitch ie -abcd for equivalent -a -b -c -d
*/
template<typename CharT>
void parse_short(const CharT * const argv[], std::size_t &pos,
  std::size_t argc, const std::vector<basic_option_description<CharT> > &grp,
  variable_map &vm, const parse_options<CharT> &opt)
{
  typedef std::basic_string<CharT> string_type;
  typedef typename string_type::size_type size_type;
  typedef std::vector<basic_option_description<CharT> > options_group_type;

  string_type arg1(argv[pos]);

  // eat short_option_flag
  string_type key = arg1.substr(opt.short_option_flag.size());

  auto &&mapped_key_desc = find_short_key(key,grp);
  const string_type &mapped_key = mapped_key_desc.first;
  typename options_group_type::const_iterator desc = mapped_key_desc.second;

  if(desc == grp.end()) {
    std::stringstream err;
    err << "unrecognized option '" << argv[pos] << "'";
    throw command_option_error(err.str(),pos);
  }

  string_type value =
    arg1.substr(opt.short_option_flag.size()+opt.short_option_length);

  boost::any mapped_value;
  if(!desc->make_value) {
    // key only
    if(!value.empty()) {
      // has a value but shouldn't
      std::stringstream err;
      err << "option '" << argv[pos] << "' does not take any arguments";
      throw command_option_error(err.str(),pos);
    }

    if(desc->implicit_value)
      mapped_value = desc->implicit_value(key,vm);
  }
  else {
    bool empty_arg = false;
    if(value.empty()) {
      // value must be in next argument unless it is another option
      if(pos+1 < argc) {
        string_type arg2(argv[pos+1]);

        if(arg2.empty()) {
          empty_arg = true;
          ++pos;
        }
        else if((arg2.find(opt.short_option_flag) != 0) &&
          (arg2.find(opt.long_option_flag) != 0) &&
          (arg2.find(opt.ignore_remaining_str) != 0))
        {
          value = arg2;
          /*
            increment pos to show that the next argument was a value and
            not an options
          */
          ++pos;
        }
      }

      if(value.empty() && !empty_arg) {
        // use implicit values
        if(desc->implicit_value)
          mapped_value = desc->implicit_value(key,vm);
        else {
          std::stringstream err;
          err << "the required argument for option '" << argv[pos]
            << "' is missing";
          throw command_option_error(err.str(),pos);
        }
      }
      else
        mapped_value = desc->make_value(key,value,vm);
    }
  }

  // place key/value in vm
  vm.insert(std::make_pair(mapped_key,mapped_value));
}


/*
  Parse the options contained in the array \c argv of length \c argc
  according to the options group \c grp and place the results in the
  returned variable_map copied from \c vm.
*/
template<typename CharT>
variable_map parse_arguments(const CharT * const argv[], std::size_t argc,
  std::size_t &endc, const std::vector<basic_option_description<CharT> > &grp,
  const variable_map &_vm, const parse_options<CharT> &opt)
{
  typedef std::basic_string<CharT> string_type;
  typedef typename string_type::size_type size_type;
  typedef std::vector<basic_option_description<CharT> > options_group_type;

  variable_map vm = _vm;

  std::size_t pos_option = 0;
  for(endc=0; endc<argc; ++endc) {
    string_type arg(argv[endc]);

    bool is_positional = false;
    if(arg == opt.ignore_remaining_str) {
      // kill further processing
      return vm;
    }
    else if(opt.short_option_flag.find(opt.long_option_flag) == 0) {
      /*
        long option is a subset of the short option. Unusual.
      */
      if(arg.find(opt.short_option_flag) == 0) {
        // i is incremented if value is in next argument
        parse_short(argv,endc,argc,grp,vm,opt);
      }
      else if(arg.find(opt.long_option_flag) == 0) {
        // i is incremented if value is in next argument
        parse_long(argv,endc,argc,grp,vm,opt);
      }
      else {
        // positional
        is_positional = true;
      }
    }
    else {
      /*
        short option is a subset of the long option or the two do not
        overlap. Case where long_option_flag is '--' and
        short_option_flag is '-'
      */
      if(arg.find(opt.long_option_flag) == 0) {
        // i is incremented if value is in next argument
        parse_long(argv,endc,argc,grp,vm,opt);
      }
      else if(arg.find(opt.short_option_flag) == 0) {
        // i is incremented if value is in next argument
        parse_short(argv,endc,argc,grp,vm,opt);
      }
      else {
        // positional
        is_positional = true;
      }
    }

    if(is_positional) {
      // positional options are just values
      auto &&mapped_key_desc = find_positional_key(pos_option,grp);
      const string_type &mapped_key = mapped_key_desc.first;
      typename options_group_type::const_iterator desc =
        mapped_key_desc.second;

      if(desc == grp.end()) {
        std::stringstream err;
        err << "unrecognized positional option '" << argv[endc] << "'";
        throw command_option_error(err.str(),endc);
      }

      boost::any mapped_value;
      if(desc->make_value)
        mapped_value = desc->make_value(mapped_key,arg,vm);

      // place key/value in vm
      vm.insert(std::make_pair(mapped_key,mapped_value));
    }
  }

  return vm;
}

template<typename CharT>
inline variable_map parse_arguments(const CharT * const argv[],
  std::size_t argc, const std::vector<basic_option_description<CharT> > &grp,
  const parse_options<CharT> &opt)
{
  std::size_t endc;
  return parse_arguments(argv,argc,endc,grp,variable_map(),opt);
}

template<typename CharT>
inline variable_map parse_arguments(const CharT * const argv[],
  std::size_t argc, const std::vector<basic_option_description<CharT> > &grp,
  const variable_map &_vm)
{
  std::size_t endc;
  return parse_arguments(argv,argc,endc,grp,_vm,parse_options<CharT>());
}

template<typename CharT>
inline variable_map parse_arguments(const CharT * const argv[],
  std::size_t argc, const std::vector<basic_option_description<CharT> > &grp)
{
  std::size_t endc;
  return parse_arguments(argv,argc,endc,grp,variable_map(),
    parse_options<CharT>());
}


template<typename CharT>
inline variable_map parse_arguments(const CharT * const argv[],
  std::size_t argc, std::size_t &endc,
  const std::vector<basic_option_description<CharT> > &grp,
  const parse_options<CharT> &opt)
{
  return parse_arguments(argv,argc,endc,grp,variable_map(),opt);
}

template<typename CharT>
inline variable_map parse_arguments(const CharT * const argv[],
  std::size_t argc, std::size_t &endc,
  const std::vector<basic_option_description<CharT> > &grp,
  const variable_map &_vm)
{
  return parse_arguments(argv,argc,endc,grp,_vm,parse_options<CharT>());
}

template<typename CharT>
inline variable_map parse_arguments(const CharT * const argv[],
  std::size_t argc, std::size_t &endc,
  const std::vector<basic_option_description<CharT> > &grp)
{
  return parse_arguments(argv,argc,endc,grp,variable_map(),
    parse_options<CharT>());
}


typedef basic_option_description<char> option_description;
typedef std::vector<basic_option_description<char> > options_group;









}

}




#endif

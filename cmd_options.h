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
#include <regex>
#include <tuple>
#include <utility>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <codecvt>
#include <functional>

#include <iostream>

namespace lemma {

namespace cmd_options {

/*
  Exception class that is inherited from std::runtime error used to
  indicate command option errors.

  Errors thrown from the cmd_options library are caught and certain
  sequences are replaced with a substitution string depending on where
  it was thrown. These sequences are:

   - '%@' : This string is replaced with the command argument as provided
   - '%L' : This string is replaced by parse_options.long_option_flag
   - '%S' : This string is replaced by parse_options.short_option_flag

  Not all sequences are available everywhere. For example, if an error
  is observed in an option_description finalize function, the '%@'
  sequence is not available because this function is called at the end
  of command option processing and therefore is not associated with any
  particular option. See the documentation for each individual function
  for availability of these sequences.

  These substitutions are available because some error processing only
  has knowledge of a variable_map key or because argument processing is
  agnostic of the given long_ or short_option_flag.

  Examples:

    - If the 'foo' key associated with the '--foo' or '-f' option is
    required and not seen in the finalize function. The finalize
    function doesn't care about the configured long_ and
    short_option_flag but needs it to provide informative feedback. Thus
    the following exception could be thrown from finalize():

      throw command_option_error("missing %Lfoo or %Sf");

    Which is rethrown as:

      command_option_error("missing --foo or -f");


    - Suppose a case-insensitive command option that maps to the
    variable_map key 'foo'. Further suppose that the make_value function
    expects a string convertible to an integer but the conversion fails.
    The following exception could be thrown from make_value():

      throw command_option_error("option %@ is expecting and integer argument");

    Given the argument '--FoO', the exception is rethrown as:

      command_option_error("option --FoO is expecting and integer argument");

*/
class command_option_error : public std::runtime_error {
  public:
    command_option_error(const std::string &what)
        :std::runtime_error(what) {}
};


/*
  Currently uses boost::any but will eventually transition to std::any in C++17
*/
using any = boost::any;

/*
  Checking if any has a value is different between boost and C++17.

  Provide an abstraction
*/
inline bool is_empty(const any &_val)
{
  return _val.empty();
}

/*
  Convenience syntax for any_cast between boost::any and std::any

  Would rather use a "using any_cast = boost::any_cast" syntax but aliases
  are not available for functions
*/
template<typename T, typename AnyT>
inline T any_cast(AnyT && _val)
{
  return boost::any_cast<T>(std::forward<AnyT>(_val));
}

/*
  Convenience for bad_any_cast between boost::any and std::any
*/
using bad_any_cast = boost::bad_any_cast;


template<typename CharT>
using basic_variable_map = std::multimap<std::basic_string<CharT>,any>;

/*
  Structure describing an unpacked argument.

  The elements represent:
    - \c prefix A string specifying the start of the option.

    - \c raw_key A string containing the raw key.

    - \c packed_key A string containing any remaining packed keys if
    present. To prevent undefined behavior, the \c prefix string will be
    prepended to the \c packed_key prior to any further processing.

    - \c value A string containing the value if present.

    - \c value_provided A boolean value indicating that the argument
      contained a packed value. This is necessary because it is possible
      to provide a empty value

  This structure is intended to represent an unpacked argument, not it's
  semantic value and is a byproduct of the may different ways that an
  option argument can be packed together. For example: here are some
  known ways that option arguments can be represented:

      1) --foo bar    = long option, key=foo, value=bar
                      : prefix = "--"
                      : raw_key = "foo"
                      : packed_key = value = ""
                      : value_provided = false.
                      : "bar" is not part of the option pack

      2) --foo=bar    = long option, key=foo, value=bar
                      : prefix = "--"
                      : raw_key = "foo"
                      : packed_key = ""
                      : value = "bar"
                      : value_provided = true

      3) -foo bar     = long option, key=foo, value=bar
                      : prefix = "-"
                      : raw_key = "foo"
                      : packed_key = value = ""
                      : value_provided = false
                      : "bar" is not part of the option pack

      2) -foo=bar     = long option, key=foo, value=bar
                      : prefix = "-"
                      : raw_key = "foo"
                      : packed_key = ""
                      : value = "bar"
                      : value_provided = true

      5) -f bar       = short option, key=f, value=bar
                      : prefix = "-"
                      : raw_key = "f"
                      : packed_key = value = ""
                      : value_provided = false
                      : "bar" is not part of the option pack

      6) -fBar        = short option, key=f, value=Bar
                      : prefix = "-"
                      : raw_key = "f"
                      : packed_key = value = ""
                      : value = "Bar"
                      : value_provided = true

      7) -fbar        = 4 short options with keys: 'f', 'b', 'a', and 'r'
                      : prefix = "-"
                      : raw_key = "f"
                      : packed_key = "bar"
                      : value = ""
                      : value_provided = false
*/
template<typename CharT>
struct basic_option_pack {
  typedef std::basic_string<CharT> string_type;

  string_type prefix;
  string_type raw_key;
  string_type packed_key;
  string_type value;
  bool value_provided = false;
};


/*
  Unpack long arguments in the form: "--key" or "--key=value"
*/
template<typename CharT>
basic_option_pack<CharT> unpack_std_long(const std::basic_string<CharT> &str)
{
  typedef basic_option_pack<CharT> option_pack;
  typedef std::basic_string<CharT> string_type;
  typedef typename string_type::size_type size_type;

  static const string_type prefix("--");
  static const string_type assignment("=");

  size_type loc = str.find(prefix);
  if(loc == string_type::npos)
    return option_pack();

  loc = loc+prefix.size();
  size_type asmt_loc = str.find(assignment,loc);
  if(asmt_loc == string_type::npos)
    return option_pack{prefix,str.substr(loc,asmt_loc)};

  return option_pack{
    prefix,str.substr(loc,asmt_loc),{},str.substr(asmt_loc+1),true};
}

/*
  Unpack short arguments in the form: "-k" where 'k' is the key
*/
template<typename CharT>
basic_option_pack<CharT> unpack_std_short(const std::basic_string<CharT> &str)
{
  typedef basic_option_pack<CharT> option_pack;
  typedef std::basic_string<CharT> string_type;
  typedef typename string_type::size_type size_type;

  static const string_type prefix("-");

  size_type loc = str.find(prefix);
  if(loc == string_type::npos || loc+prefix.size()+1 == str.size())
    return option_pack();

  return option_pack{prefix,str.substr(loc+prefix.size())};
}

/*
  A description for a single option
*/
template<typename CharT>
struct basic_option_description {
  typedef CharT                     char_type;
  typedef std::basic_string<CharT>  string_type;
  typedef basic_variable_map<CharT> variable_map;
  typedef basic_option_pack<CharT>  option_pack;

  /*
    Unpack the raw option argument pack. If \c unpack_argument is not
    provided, then the this description is assumed to describe a
    positional argument.

    If the argument cannot be unpacked by this description, then return
    the empty string in the \c raw_key position (or an empty option_pack).

    This function is intended to essentially parse the option argument
    into its constitute properties but it can also be used to inject
    hidden options that can be useful. For example, if --foo is given
    implies --bar, even if --bar was not provided, then the unpacked
    argument for "foo" can return "bar" in the \c packed_key field.
  */
  std::function<option_pack(const string_type &arg)> unpack_argument;

  /*
    For the given long option (without the '--' prefix), return the mapped_key
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

    The \c vm argument is provided to allow a callee to examine the
    current state of the variable map if needed. This is typically used
    to provide error checking. For example, if the option was provided
    more times than allowed or if it is mutually exclusive with another
    option.

    command_option_error substitution is available for '%@', '%L', '%S'
  */
  std::function<string_type(const string_type &option,
    const variable_map &vm)> map_long_key;

  /*
    Return the human-readable description for this option's long key
    without the long_key_flag. This does not necessarily need to be the
    result provided in \c map_long_key (although it often does). The
    value is not used for options parsing but typically used in the help
    message. For example, supposed this option is given by --foo, return
    "foo". This value can be any meaningful string based on the need.
    For example, suppose for some nonstandard syntax the key --foo1bar
    --foo2bar is possible, a reasonable response from this function
    could be: "foo[integer]bar".
  */
  std::function<string_type(void)> long_key_description;

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

    By convention (although not strictly required), short arguments are
    alias for their longer counterparts. Therefore, the returned string
    should be the same as map_long_key. That is, if -f is short for
    --foo, then if the function receives "f", it should return "foo".
    This makes querying the final variable_map for values much easier.
    This is not required though as it is entirely possible that one may
    want to have a short argument without the corresponding long
    argument for legacy reasons or because they have different meanings.

    The \c vm argument is provided to allow a callee to examine the
    current state of the variable map if needed. This is typically used
    to provide error checking. For example, if the option was provided
    more times than allowed or if it is mutually exclusive with another
    option.

    command_option_error substitution is available for '%@', '%L', '%S'
  */
  std::function<string_type(const string_type &option,
    const variable_map &vm)> map_short_key;

  /*
    Return the human-readable description for this option's short key
    without the short_key_flag. This does not necessarily need to be the
    result provided in \c map_short_key (although it often does). The
    value is not used for options parsing but typically used in the help
    message. For example, supposed this option is given by -f, return
    "f". This value can be any meaningful string based on the need. For
    example, suppose for some nonstandard syntax the key -1 -2 is
    possible, a reasonable response from this function could be:
    "[integer]"
  */
  std::function<string_type(void)> short_key_description;

  /*
    Return a human-readable concise description of this option's
    functionality.
  */
  std::function<string_type(void)> extended_description;

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
  std::function<string_type(std::size_t i)> implicit_key;


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

    If set, return the implicit value of the option as a any
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
    any(const string_type &key, const variable_map &vm)> implicit_value;

  /*
    Return the human-readable description for this option's implicit
    value. This description is not used for parsing but rather automatic
    help messages. For example, suppose the option --foo has an implicit
    value of '5' if another value was not provided. A reasonable return
    value from this function would be "5".
  */
  std::function<string_type(void)> implicit_value_description;

  /*
    Positional option case:
      -- interpret the option value (make_value is set)
      -- use default constructed value (make_value is not set)

    If set, return the value of the argument as a any as
    determined by the contents of the second parameter. If not set, the
    option explicitly forbids option values. I.e. --foo=bar or --foo bar
    will return an error unless the 'bar' is a valid positional option
    in the later case.

    The function's first parameter is the string given as the option's
    mapped_key exactly as provided by the \c map_long_key and/or \c
    map_short_key function. This can be useful for checking the state of
    the vm and throwing an error if too many instances of a certain
    option has been given or if certain combinations of options are
    mutually-exclusive or inclusive.

    If the option was a positional one, then the
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

    command_option_error substitution is available for '%@', '%L', '%S'
  */

  std::function<
    any(const string_type &mapped_key,const string_type &value,
      const variable_map &vm)> make_value;

  /*
    If set, this function is called for each option_description by the
    parse_* functions after all options are parsed. A typical use of
    this function is to inspect the state of the variable_map argument
    for option-specific inconsistencies or illegal combinations. For
    example, if a minimum number of options are expected, the
    variable_map can be queried to see if it has been satisfied and
    throw an error if not.

    command_option_error substitution is available for '%L' and '%S'
  */
  std::function<void(const variable_map &vm)> finalize;

};

template<typename CharT>
using basic_options_group = std::vector<basic_option_description<CharT> >;



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
  typename basic_options_group<CharT>::const_iterator>
find_long_key(const std::basic_string<CharT> &key,
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm)
{
  typedef std::basic_string<CharT> string_type;

  auto result = std::make_pair(string_type(),grp.begin());

  while(result.second != grp.end()) {
    if(result.second->map_long_key) {
      result.first = result.second->map_long_key(key,vm);

      if(!result.first.empty())
        break;
    }

    ++result.second;
  }

  return result;
}

template<typename CharT>
std::pair<std::basic_string<CharT>,
  typename basic_options_group<CharT>::const_iterator>
find_short_key(const std::basic_string<CharT> &key,
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm)
{
  typedef std::basic_string<CharT> string_type;

  auto result = std::make_pair(string_type(),grp.begin());

  while(result.second != grp.end()) {
    if(result.second->map_short_key) {
      result.first = result.second->map_short_key(key,vm);

      if(!result.first.empty())
        break;
    }

    ++result.second;
  }

  return result;
}

template<typename CharT>
std::pair<std::basic_string<CharT>,
  typename basic_options_group<CharT>::const_iterator>
find_positional_key(std::size_t pos,
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm)
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
  std::size_t argc, const basic_options_group<CharT> &grp,
  basic_variable_map<CharT> &vm, const parse_options<CharT> &opt)
{
  typedef std::basic_string<CharT> string_type;
  typedef typename string_type::size_type size_type;
  typedef basic_options_group<CharT> options_group_type;

  // eat long_option_flag
  string_type arg(argv[pos]);

  string_type arg1 = arg.substr(opt.long_option_flag.size());

  size_type eqloc = arg1.find('=');
  string_type key = arg1.substr(0,eqloc);

  auto &&mapped_key_desc = find_long_key(key,grp,vm);
  const string_type &mapped_key = mapped_key_desc.first;
  typename options_group_type::const_iterator desc = mapped_key_desc.second;

  if(desc == grp.end())
    throw command_option_error("unrecognized option '%@'");

  any mapped_value;
  if(!desc->make_value) {
    // key only, check to see if it has an '=' but shouldn't
    if(eqloc != string_type::npos)
      throw command_option_error("option '%@' does not take any arguments");

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
        throw command_option_error(
          "the required argument for option '%@' is missing");
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
  std::size_t argc, const basic_options_group<CharT> &grp,
  basic_variable_map<CharT> &vm, const parse_options<CharT> &opt)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_options_group<CharT> options_group_type;

  string_type arg1(argv[pos]);

  // eat short_option_flag
  string_type key = arg1.substr(opt.short_option_flag.size());
std::cerr << "KEY IS: " << key << "\n";
  auto &&mapped_key_desc = find_short_key(key,grp,vm);
  const string_type &mapped_key = mapped_key_desc.first;
  typename options_group_type::const_iterator desc = mapped_key_desc.second;

  if(desc == grp.end())
    throw command_option_error("unrecognized option '%@'");

  string_type value =
    arg1.substr(opt.short_option_flag.size()+opt.short_option_length);

  any mapped_value;
  if(!desc->make_value) {
    // key only, check if it has arguments but shouldn't
    if(!value.empty())
      throw command_option_error("option '%@' does not take any arguments");

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
          throw command_option_error(
            "the required argument for option '%@' is missing");
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
basic_variable_map<CharT>
parse_arguments(const CharT * const argv[], std::size_t argc,
  std::size_t &endc, const basic_options_group<CharT> &grp,
  const basic_variable_map<CharT> &vm, const parse_options<CharT> &opt,
  bool partial)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_options_group<CharT> options_group_type;
  typedef basic_variable_map<CharT> variable_map;

  variable_map _vm = vm;

  try {
    std::size_t pos_option = 0;
    for(endc=0; endc<argc; ++endc) {
      string_type arg(argv[endc]);

      try {
        bool is_positional = false;
        if(arg == opt.ignore_remaining_str) {
          // kill further processing
          return _vm;
        }
        else if(opt.short_option_flag.find(opt.long_option_flag) == 0) {
          /*
            long option is a subset of the short option. Unusual.
          */
          if(arg.find(opt.short_option_flag) == 0) {
            // i is incremented if value is in next argument
            parse_short(argv,endc,argc,grp,_vm,opt);
          }
          else if(arg.find(opt.long_option_flag) == 0) {
            // i is incremented if value is in next argument
            parse_long(argv,endc,argc,grp,_vm,opt);
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
            parse_long(argv,endc,argc,grp,_vm,opt);
          }
          else if(arg.find(opt.short_option_flag) == 0) {
            // i is incremented if value is in next argument
            parse_short(argv,endc,argc,grp,_vm,opt);
          }
          else {
            // positional
            is_positional = true;
          }
        }

        if(is_positional) {
          // positional options are just values
          auto &&mapped_key_desc = find_positional_key(pos_option,grp,_vm);
          const string_type &mapped_key = mapped_key_desc.first;
          typename options_group_type::const_iterator desc =
            mapped_key_desc.second;

          if(desc == grp.end())
            throw command_option_error("unrecognized positional option '%@'");

          any mapped_value;
          if(desc->make_value)
            mapped_value = desc->make_value(mapped_key,arg,_vm);

          // place key/value in _vm
          _vm.insert(std::make_pair(mapped_key,mapped_value));
        }
      }
      catch(const command_option_error &ex) {
        std::regex argex("%@");
        std::string err = std::regex_replace(ex.what(),argex,arg);
        throw command_option_error(err);
      }
    }

    if(!partial) {
      for(auto &desc : grp) {
        if(desc.finalize)
          desc.finalize(_vm);
      }
    }
  }
  catch(const command_option_error &ex) {
    std::regex longex("%L");
    std::regex shortex("%S");
    std::string err =
      std::regex_replace(ex.what(),longex,opt.long_option_flag);
    err = std::regex_replace(err,shortex,opt.short_option_flag);
    throw command_option_error(err);
  }


  return _vm;
}

/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp with parse_options \c opt. If
  \c partial is true, then finalize parsing. Options are returned in a
  new variable map.
*/
template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(const CharT * const argv[],
  std::size_t argc, const basic_options_group<CharT> &grp,
  const parse_options<CharT> &opt, bool partial = false)
{
  std::size_t endc;
  return parse_arguments(argv,argc,endc,grp,basic_variable_map<CharT>(),
    opt,partial);
}

/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp with parse_options \c opt. If
  \c partial is true, then finalize parsing. Options are added to a copy
  of the variable map \c _vm and returned.
*/
template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(const CharT * const argv[], std::size_t argc,
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &_vm,
  bool partial = false)
{
  std::size_t endc;
  return parse_arguments(argv,argc,endc,grp,_vm,parse_options<CharT>(),partial);
}

/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp with default parse_options. If
  \c partial is true, then finalize parsing. Options are returned in a
  new variable map.
*/
template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(const CharT * const argv[], std::size_t argc,
  const basic_options_group<CharT> &grp, bool partial = false)
{
  std::size_t endc;
  return parse_arguments(argv,argc,endc,grp,basic_variable_map<CharT>(),
    parse_options<CharT>(),partial);
}

/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp with parse_options \c opt. If
  \c partial is true, then finalize parsing. Options are returned in a
  new variable map. If parsing is halted due to a "end of parse"
  indicator (normally '--') then \c endc is updated to the index of the
  first non-parsed argument.
*/
template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(const CharT * const argv[], std::size_t argc,
  std::size_t &endc, const basic_options_group<CharT> &grp,
  const parse_options<CharT> &opt, bool partial = false)
{
  return parse_arguments(argv,argc,endc,grp,basic_variable_map<CharT>(),
    opt,partial);
}

/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp with parse_options \c opt. If
  \c partial is true, then finalize parsing. Options are added to a copy
  of the variable map \c _vm and returned. If parsing is halted due to a
  "end of parse" indicator (normally '--') then \c endc is updated to
  the index of the first non-parsed argument.
*/
template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(const CharT * const argv[], std::size_t argc,
  std::size_t &endc, const basic_options_group<CharT> &grp,
  const basic_variable_map<CharT> &_vm, bool partial = false)
{
  return parse_arguments(argv,argc,endc,grp,_vm,parse_options<CharT>(),partial);
}

/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp with default parse_options. If
  \c partial is true, then finalize parsing. Options are returned in a
  new variable map. If parsing is halted due to a
  "end of parse" indicator (normally '--') then \c endc is updated to
  the index of the first non-parsed argument.
*/
template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(const CharT * const argv[], std::size_t argc,
  std::size_t &endc, const basic_options_group<CharT> &grp,
  bool partial = false)
{
  return parse_arguments(argv,argc,endc,grp,
    basic_variable_map<CharT>(),parse_options<CharT>(),partial);
}


typedef basic_option_description<char> option_description;
typedef std::vector<basic_option_description<char> > options_group;
typedef std::multimap<std::basic_string<char>,any> variable_map;

typedef basic_option_description<wchar_t> woption_description;
typedef std::vector<basic_option_description<wchar_t> > woptions_group;
typedef std::multimap<std::basic_string<wchar_t>,any> wvariable_map;








/*
  EZ interface

  Be overly descriptive. ie use
    value<int>().occurrences(1,2).mutually_exclusive({"foo"}).
      mutually_inclusive({"bar"});
  rather than
    value<int>{1,2,{"foo"},{"foo"}};

  ie is "foo" inclusive or exclusive? what if don't want to use range?
  therefore lots of constructors that someone needs to be read.
*/


template<typename CharT>
struct constrain {
  typedef std::basic_string<CharT> string_type;

  constrain<CharT> & occurrences(std::size_t n)
  {
    _min = _max = n;
    return *this;
  }

  constrain<CharT> & occurrences(std::size_t n, std::size_t m)
  {
    _min = n;
    _max = m;
    assert(_min <= _max);
    return *this;
  }

  constrain<CharT> &
  mutual_exclusion(const std::vector<string_type> &mapped_key_vec)
  {
    _mutual_exclusion = mapped_key_vec;
    return *this;
  }

  constrain<CharT> &
  mutual_inclusion(const std::vector<string_type> &mapped_key_vec)
  {
    _mutual_inclusion = mapped_key_vec;
    return *this;
  }

  int _position = -1;
  std::size_t _min = 0;
  std::size_t _max = std::numeric_limits<std::size_t>::max();
  std::vector<string_type> _mutual_exclusion;
  std::vector<string_type> _mutual_inclusion;
};


template<typename T>
struct value {
  typedef T value_type;

  value(void) = default;
  value(const T &val) :_implicit(std::make_shared<T>(val)) {}

  std::shared_ptr<value_type> _implicit;
};



template <typename CharT>
inline const std::basic_string<CharT> &
to_xstring(std::basic_string<CharT> &str)
{
    return str;
}

/*
  to_xstring taken from:

  http://stackoverflow.com/questions/34961274/implement-to-xstring-to-unite-to-string-and-to-wstring
*/
template <typename CharT, typename T>
inline
typename std::enable_if<std::is_same<CharT, char>::value, std::string>::type
    to_xstring(const T &t)
{
    return std::to_string(t);
}


template <typename CharT, typename T>
inline
typename std::enable_if<!std::is_same<CharT, char>::value, std::wstring>::type
    to_xstring(const T &t)
{
    return std::to_wstring(t);
}

inline std::string as_utf8(const std::string &s)
{
  return s;
}

template<typename CharT>
inline std::string as_utf8(const std::basic_string<CharT> &s)
{
  typedef std::wstring_convert<
    std::codecvt_utf8<CharT>,CharT> converter_type;

  converter_type converter;

  return converter.to_bytes(s);
}








namespace detail {

template<bool hidden, typename CharT>
inline void add_long_opt(const std::basic_string<CharT> &opt,
  const std::basic_string<CharT> &key, basic_option_description<CharT> &desc)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map;

  desc.map_long_key = [=](const string_type &_opt, const variable_map &)
    {
      return (_opt == opt ? key : string_type());
    };

  if(!hidden) {
    desc.long_key_description = [=](void) {
      return opt;
    };
  }
}

template<bool hidden, typename CharT>
inline void add_short_opt(const std::basic_string<CharT> &opt,
  const std::basic_string<CharT> &key, basic_option_description<CharT> &desc)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map;

  desc.map_short_key = [=](const string_type &_opt, const variable_map &)
    {
      return (_opt == opt ? key : string_type());
    };

  if(!hidden) {
    desc.short_key_description = [=](void)
      {
        return opt;
      };
  }
}

template<typename CharT>
inline void add_extended_desc(const std::basic_string<CharT> &str,
  basic_option_description<CharT> &desc)
{
  desc.extended_description = [=](void) {
    return str;
  };
}

template<typename T, typename CharT>
inline void add_value(const value<T> &val,
  basic_option_description<CharT> &desc)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map;

  if(val._implicit) {
    desc.implicit_value = [=](const string_type &, const variable_map &)
    {
      return any(*(val._implicit));
    };

    desc.implicit_value_description = [=](void) {
      return to_xstring<CharT>(*(val._implicit));
    };
  }

  desc.make_value = [](const string_type &,const string_type &val,
    const variable_map &)
  {
    T _val;
    std::basic_stringstream<CharT> in(val);
    in >> _val;
    return any(_val);
  };
}

template<typename CharT>
inline void add_key(const std::basic_string<CharT> &key, int pos,
  basic_option_description<CharT> &desc)
{
  if(pos >= 0) {
    desc.implicit_key = [=](std::size_t i) {
      if(i == pos)
        return key;
      return std::basic_string<CharT>();
    };
  }
  else {
    desc.implicit_key = [=](std::size_t) {
      return key;
    };
  }
}



/*
  Used for constraints on options. Cannot be empty string mapped keys.

  Although \c mapped_key and long_opt are the same for the EZ
  interface, it is separated here for completeness
*/
template<typename CharT>
void add_option_constrains(const constrain<CharT> &cnts,
  basic_option_description<CharT> &desc,
  const std::basic_string<CharT> &mapped_key,
  const std::basic_string<CharT> &long_opt,
  const std::basic_string<CharT> &short_opt)
{
  desc.finalize = [=](const basic_variable_map<CharT> &vm) {

    if(vm.count(mapped_key) > cnts._max) {
      std::stringstream err;
      err << "option ";
      if(!long_opt.empty())
        err << "'%L" << as_utf8(long_opt) << "' ";
      if(!long_opt.empty() && !short_opt.empty())
        err << "or ";
      if(!short_opt.empty())
        err << "'%S" << as_utf8(short_opt) << "' ";
      err << "cannot be specified more than "
        << cnts._max << (cnts._max==1?" time":" times");
      throw command_option_error(err.str());
    }

    if(vm.count(mapped_key) < cnts._min) {
      std::stringstream err;
      err << "option ";
      if(!long_opt.empty())
        err << "'%L" << as_utf8(long_opt) << "' ";
      if(!long_opt.empty() && !short_opt.empty())
        err << "or ";
      if(!short_opt.empty())
        err << "'%S" << as_utf8(short_opt) << "' ";
      err << "must be specified at least "
        << cnts._min << (cnts._max==1?" time":" times");
      throw command_option_error(err.str());
    }

    for(auto &exclusive_key : cnts._mutual_exclusion) {
      if(vm.count(exclusive_key) != 0) {
        std::stringstream err;
        err << "option ";
        if(!long_opt.empty())
          err << "'%L" << as_utf8(long_opt) << "' ";
        if(!long_opt.empty() && !short_opt.empty())
          err << "and ";
        if(!short_opt.empty())
          err << "'%S" << as_utf8(short_opt) << "' ";
        err << "cannot be specified along with '"
          << as_utf8(exclusive_key) << "'";
        throw command_option_error(err.str());
      }
    }

    for(auto &inclusive_key : cnts._mutual_inclusion) {
      if(vm.count(inclusive_key) != 0) {
        std::stringstream err;
        err << "option ";
        if(!long_opt.empty())
          err << "'%L" << as_utf8(long_opt) << "' ";
        if(!long_opt.empty() && !short_opt.empty())
          err << "or ";
        if(!short_opt.empty())
          err << "'%S" << as_utf8(short_opt) << "' ";
        err << "must be specified along with '"
          << as_utf8(inclusive_key) << "'";
        throw command_option_error(err.str());
      }
    }
  };
}

/*
  Used for constraints on positional. CAN be empty string mapped keys.
*/
template<typename CharT>
void add_positional_constrains(const constrain<CharT> &cnts,
  const std::basic_string<CharT> &mapped_key,
  basic_option_description<CharT> &desc)
{
  desc.finalize = [=](const basic_variable_map<CharT> &vm) {
    if(vm.count(mapped_key) > cnts._max) {
      std::stringstream err;
      err << "positional values cannot be specified more than "
        << cnts._max << (cnts._max==1?" time":" times");
      throw command_option_error(err.str());
    }

    if(vm.count(mapped_key) < cnts._min) {
      std::stringstream err;
      if(cnts._max==1)
        err << "a positional value ";
      else
        err << "positional values ";
      err << "must be specified at least "
        << cnts._min << (cnts._max==1?" time":" times");
      throw command_option_error(err.str());
    }

    for(auto &exclusive_key : cnts._mutual_exclusion) {
      if(vm.count(exclusive_key) != 0) {
        std::stringstream err;
        err << "option '" << as_utf8(exclusive_key)
            << "' is incompatible with the given positional options";
        throw command_option_error(err.str());
      }
    }

    for(auto &inclusive_key : cnts._mutual_inclusion) {
      if(vm.count(inclusive_key) != 0) {
        std::stringstream err;
        err << "option '" << as_utf8(inclusive_key)
            << "' must be provided with the given positional options";
        throw command_option_error(err.str());
      }
    }
  };
}

}




template<typename CharT>
inline std::pair<std::basic_string<CharT>,std::basic_string<CharT> >
split(const std::basic_string<CharT> &str, CharT delim)
{
  typedef std::basic_string<CharT> string_type;

  typename string_type::size_type loc = str.find(delim);
  if(loc == string_type::npos)
    return std::make_pair(str,string_type());

  return std::make_pair(str.substr(0,loc),str.substr(loc+1));
}

/*
  The following functions are for convenience only. They provide automatic
  type deduction and therefore implicit conversion from string literals
  to the basic_string equivalents as well as defaults for unused arguments.
*/

/*
  Cases line 2, 4, 6
*/
inline basic_option_description<char>
make_option(const std::basic_string<char> &opt_spec,
  const std::basic_string<char> &extended_desc,
  const constrain<char> &cnts = constrain<char>(), char delim = ',')
{
  std::basic_string<char> long_opt;
  std::basic_string<char> short_opt;
  std::tie(long_opt,short_opt) = split(opt_spec,delim);

  basic_option_description<char> desc;

  if(!long_opt.empty())
    detail::add_long_opt<false>(long_opt,long_opt,desc);
  if(!short_opt.empty())
    detail::add_short_opt<false>(short_opt,long_opt,desc); // long_opt is key

  detail::add_extended_desc(extended_desc,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Cases line 3, 5, 7
*/
inline basic_option_description<char>
make_hidden_option(const std::basic_string<char> &opt_spec,
  const constrain<char> &cnts = constrain<char>(), char delim = ',')
{
  std::basic_string<char> long_opt;
  std::basic_string<char> short_opt;
  std::tie(long_opt,short_opt) = split(opt_spec,delim);

  basic_option_description<char> desc;

  if(!long_opt.empty())
    detail::add_long_opt<true>(long_opt,long_opt,desc);
  if(!short_opt.empty())
    detail::add_short_opt<true>(short_opt,long_opt,desc); // long_opt is key

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 8 10 12 14 16 18
*/
template<typename T>
inline basic_option_description<char>
make_option(const std::basic_string<char> &opt_spec,
  const value<T> &val, const std::basic_string<char> &extended_desc,
  const constrain<char> &cnts = constrain<char>(), char delim = ',')
{
  std::basic_string<char> long_opt;
  std::basic_string<char> short_opt;
  std::tie(long_opt,short_opt) = split(opt_spec,delim);

  basic_option_description<char> desc;

  if(!long_opt.empty())
    detail::add_long_opt<false>(long_opt,long_opt,desc);
  if(!short_opt.empty())
    detail::add_short_opt<false>(short_opt,long_opt,desc); // long_opt is key

  detail::add_value(val,desc);

  detail::add_extended_desc(extended_desc,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 9 11 13 15 17 19
*/
template<typename T>
inline basic_option_description<char>
make_hidden_option(const std::basic_string<char> &opt_spec, const value<T> &val,
  const constrain<char> &cnts = constrain<char>(), char delim = ',')
{
  std::basic_string<char> long_opt;
  std::basic_string<char> short_opt;
  std::tie(long_opt,short_opt) = split(opt_spec,delim);

  basic_option_description<char> desc;

  if(!long_opt.empty())
    detail::add_long_opt<true>(long_opt,long_opt,desc);
  if(!short_opt.empty())
    detail::add_short_opt<true>(short_opt,long_opt,desc); // long_opt is key

  detail::add_value(val,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 20, 22
*/
inline basic_option_description<char>
make_positional(const std::basic_string<char> &extended_desc,
  const std::basic_string<char> &implicit_key = std::basic_string<char>(),
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  detail::add_extended_desc(extended_desc,desc);

  if(!implicit_key.empty() || cnts._position >= 0)
    detail::add_key(implicit_key,cnts._position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}

/*
  Case line 21, 23
*/
template<typename T>
inline basic_option_description<char>
make_positional(const std::basic_string<char> &extended_desc,
  const value<T> &val,
  const std::basic_string<char> &implicit_key = std::basic_string<char>(),
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  detail::add_extended_desc(extended_desc,desc);

  detail::add_value(val,desc);

  if(!implicit_key.empty() || cnts._position >= 0)
    detail::add_key(implicit_key,cnts._position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}

/*
  Case line 24 is default constructor

  Case line 24, 26
*/
inline basic_option_description<char>
make_hidden_positional(
  const std::basic_string<char> &implicit_key = std::basic_string<char>(),
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  if(!implicit_key.empty() || cnts._position >= 0)
    detail::add_key(implicit_key,cnts._position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}

/*
  Case line 25, 27
*/
template<typename T>
inline basic_option_description<char>
make_hidden_positional(const value<T> &val,
  const std::basic_string<char> &implicit_key = std::basic_string<char>(),
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  detail::add_value(val,desc);

  if(!implicit_key.empty() || cnts._position >= 0)
    detail::add_key(implicit_key,cnts._position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}





/*
  Specializations for wchar_t
*/
/*
  Cases line 2, 4, 6
*/
inline basic_option_description<wchar_t>
make_woption(const std::basic_string<wchar_t> &opt_spec,
  const std::basic_string<wchar_t> &extended_desc,
  const constrain<wchar_t> &cnts = constrain<wchar_t>(), wchar_t delim = L',')
{
  std::basic_string<wchar_t> long_opt;
  std::basic_string<wchar_t> short_opt;
  std::tie(long_opt,short_opt) = split(opt_spec,delim);

  basic_option_description<wchar_t> desc;

  if(!long_opt.empty())
    detail::add_long_opt<false>(long_opt,long_opt,desc);
  if(!short_opt.empty())
    detail::add_short_opt<false>(short_opt,long_opt,desc); // long_opt is key

  detail::add_extended_desc(extended_desc,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Cases line 3, 5, 7
*/
inline basic_option_description<wchar_t>
make_hidden_woption(const std::basic_string<wchar_t> &opt_spec,
  const constrain<wchar_t> &cnts = constrain<wchar_t>(), wchar_t delim = L',')
{
  std::basic_string<wchar_t> long_opt;
  std::basic_string<wchar_t> short_opt;
  std::tie(long_opt,short_opt) = split(opt_spec,delim);

  basic_option_description<wchar_t> desc;

  if(!long_opt.empty())
    detail::add_long_opt<true>(long_opt,long_opt,desc);
  if(!short_opt.empty())
    detail::add_short_opt<true>(short_opt,long_opt,desc); // long_opt is key

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 8 10 12 14 16 18
*/
template<typename T>
inline basic_option_description<wchar_t>
make_woption(const std::basic_string<wchar_t> &opt_spec,
  const value<T> &val, const std::basic_string<wchar_t> &extended_desc,
  const constrain<wchar_t> &cnts = constrain<wchar_t>(), wchar_t delim = L',')
{
  std::basic_string<wchar_t> long_opt;
  std::basic_string<wchar_t> short_opt;
  std::tie(long_opt,short_opt) = split(opt_spec,delim);

  basic_option_description<wchar_t> desc;

  if(!long_opt.empty())
    detail::add_long_opt<false>(long_opt,long_opt,desc);
  if(!short_opt.empty())
    detail::add_short_opt<false>(short_opt,long_opt,desc); // long_opt is key

  detail::add_value(val,desc);

  detail::add_extended_desc(extended_desc,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 9 11 13 15 17 19
*/
template<typename T>
inline basic_option_description<wchar_t>
make_hidden_woption(const std::basic_string<wchar_t> &opt_spec,
  const value<T> &val, const constrain<wchar_t> &cnts = constrain<wchar_t>(),
  wchar_t delim = L',')
{
  std::basic_string<wchar_t> long_opt;
  std::basic_string<wchar_t> short_opt;
  std::tie(long_opt,short_opt) = split(opt_spec,delim);

  basic_option_description<wchar_t> desc;

  if(!long_opt.empty())
    detail::add_long_opt<true>(long_opt,long_opt,desc);
  if(!short_opt.empty())
    detail::add_short_opt<true>(short_opt,long_opt,desc); // long_opt is key

  detail::add_value(val,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 20, 22
*/
inline basic_option_description<wchar_t>
make_wpositional(const std::basic_string<wchar_t> &extended_desc,
  const std::basic_string<wchar_t> &implicit_key = std::basic_string<wchar_t>(),
  const constrain<wchar_t> &cnts = constrain<wchar_t>())
{
  basic_option_description<wchar_t> desc;

  detail::add_extended_desc(extended_desc,desc);

  if(!implicit_key.empty() || cnts._position >= 0)
    detail::add_key(implicit_key,cnts._position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}

/*
  Case line 21, 23
*/
template<typename T>
inline basic_option_description<wchar_t>
make_wpositional(const std::basic_string<wchar_t> &extended_desc,
  const value<T> &val,
  const std::basic_string<wchar_t> &implicit_key = std::basic_string<wchar_t>(),
  const constrain<wchar_t> &cnts = constrain<wchar_t>())
{
  basic_option_description<wchar_t> desc;

  detail::add_extended_desc(extended_desc,desc);

  detail::add_value(val,desc);

  if(!implicit_key.empty() || cnts._position >= 0)
    detail::add_key(implicit_key,cnts._position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}

/*
  Case line 24 is default constructor

  Case line 24, 26
*/
inline basic_option_description<wchar_t>
make_hidden_wpositional(
  const std::basic_string<wchar_t> &implicit_key = std::basic_string<wchar_t>(),
  const constrain<wchar_t> &cnts = constrain<wchar_t>())
{
  basic_option_description<wchar_t> desc;

  if(!implicit_key.empty() || cnts._position >= 0)
    detail::add_key(implicit_key,cnts._position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}

/*
  Case line 25, 27
*/
template<typename T>
inline basic_option_description<wchar_t>
make_hidden_wpositional(const value<T> &val,
  const std::basic_string<wchar_t> &implicit_key = std::basic_string<wchar_t>(),
  const constrain<wchar_t> &cnts = constrain<wchar_t>())
{
  basic_option_description<wchar_t> desc;

  detail::add_value(val,desc);

  if(!implicit_key.empty() || cnts._position >= 0)
    detail::add_key(implicit_key,cnts._position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}



namespace detail {

template<typename CharT>
basic_options_group<CharT> make_universal_group(void)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_options_group<CharT> options_group;
  typedef basic_variable_map<CharT> variable_map;

  basic_option_description<CharT> desc;

  desc.map_long_key = desc.map_short_key =
    [](const string_type &option, const variable_map &) { return option; };

  desc.long_key_description = desc.short_key_description =
    [](void) { return string_type("any"); };

  desc.extended_description = [](void) {
      return string_type(
        "Accepts any form of long option (prefaced with '--') or short option "
        "(prefaced with '-')"
      );
    };

  desc.implicit_value =
    [](const string_type &, const variable_map &) {
      return any(string_type());
    };

  desc.make_value =
    [](const string_type &, const string_type &value, const variable_map &) {
        return any(value);
      };

  return options_group{desc};
}

}


/*
  Convenience default parser

  Parse the arguments contained in \c argv with size \c argc. Options
  are returned in a new variable map. Long options are prefaced with a
  '--' and short options (single character) are prefaced with a '-'. In
  each case, the option is stored as a key in the variable_map. If
  parsing is halted due to a "end of parse" indicator '--' then \c endc
  is updated to the index of the first non-parsed argument.
*/
template<typename CharT>
basic_variable_map<CharT>
parse_arguments(const CharT * const argv[], std::size_t argc,
  std::size_t &endc)
{
  static const basic_options_group<CharT> grp =
    detail::make_universal_group<CharT>();

  return parse_arguments(argv,argc,endc,grp,
    basic_variable_map<CharT>(),parse_options<CharT>(),false);
}





























template<typename CharT>
bool option_comp(const basic_option_description<CharT> &lhs,
  const basic_option_description<CharT> &rhs)
{
  bool lhs_short = lhs.map_short_key && lhs.short_key_description;
  bool lhs_long = lhs.map_long_key && lhs.long_key_description;
  bool rhs_short = rhs.map_short_key && rhs.short_key_description;
  bool rhs_long = rhs.map_long_key && rhs.long_key_description;

  if(lhs_short && rhs_short)
    return (lhs.short_key_description() < rhs.short_key_description());

  if(!lhs_short && !rhs_short) {
    if(lhs_long && rhs_long)
      return (lhs.long_key_description() < rhs.long_key_description());

    if(!lhs_long && !rhs_long)
      return (&lhs < &rhs);

    return !lhs_long;
  }

  return !lhs_short;
}

template<typename CharT>
std::basic_string<CharT>
option_to_string(const basic_option_description<CharT> &desc,
  const parse_options<CharT> &opt)
{
  typedef std::basic_string<CharT> string_type;

  std::basic_ostringstream<CharT> out;
  out << "  ";

  if(desc.map_long_key && desc.map_short_key) {
    if(!(desc.long_key_description && desc.short_key_description))
      return string_type(); // hidden

    out << opt.short_option_flag << desc.short_key_description() << " [ "
      << opt.long_option_flag << desc.long_key_description() << " ]";
  }
  else if(desc.map_long_key) {
    if(!desc.long_key_description)
      return string_type();

    out << opt.long_option_flag << desc.long_key_description();
  }
  else {
    if(!desc.short_key_description)
      return string_type();

    out << opt.short_option_flag << desc.short_key_description();
  }

  if(desc.make_value) {
    if(desc.implicit_value) {
      // value is optional
      out << "  [arg]";
      if(desc.implicit_value_description)
        out << "=(" << desc.implicit_value_description() << ')';
    }
    else
      out << "  <arg>";
  }

  return out.str();
}




/*
  Adapted from rosettacode.org

  https://www.rosettacode.org/wiki/Word_wrap#C.2B.2B
*/
template<typename CharT>
std::basic_string<CharT>
wrap(const std::basic_string<CharT> &text, std::size_t width)
{
  std::basic_istringstream<CharT> words(text);
  std::basic_ostringstream<CharT> wrapped;
  std::basic_string<CharT> word;

  if (words >> word) {
    wrapped << word;
    size_t space_left = width - word.length();
    while (words >> word) {
      if (space_left < word.length() + 1) {
        wrapped << '\n' << word;
        space_left = width - word.length();
      }
      else {
        wrapped << ' ' << word;
        space_left -= word.length() + 1;
      }
    }
  }

  return wrapped.str();
}

template<typename CharT>
std::basic_string<CharT> make_column(const std::basic_string<CharT> &text,
  const std::basic_string<CharT> &pad, std::size_t width)
{
  typedef std::basic_string<CharT> string_type;

  static const std::basic_regex<CharT> regex("\n",
    std::regex_constants::optimize);

  static const string_type nl("\n");

  string_type wrapped_text = wrap(text,width-pad.size());

  return std::regex_replace(wrapped_text,regex,nl+pad);
}

/*
  Indent is the amount of space from the left edge that the text will
  start to be drawn. That is, in two column format, the column width is
  (width-indent) whereas in a single column (or paragraph-mode), the
  first line starts at indent and is (width-indent) wide.
*/
template<typename CharT>
std::basic_string<CharT>
extended_to_string(const basic_option_description<CharT> &desc,
  const parse_options<CharT> &opt, std::size_t indent, std::size_t width)
{
  typedef std::basic_string<CharT> string_type;

  static const string_type nl("\n");
  static const std::basic_regex<CharT> regex("\n",
    std::regex_constants::optimize);

  if(desc.extended_description) {
    string_type wrapped_text = wrap(desc.extended_description(),width-indent);

    string_type pad(indent,' ');
    return std::regex_replace(wrapped_text,regex,nl+pad);
  }

  return "no description";
}


/*
  Control the output format when converting to a string

  \c width is the total allowed with of the formatted extents

  \c max_option_width is the maximum width of the option column. If a
  formatted option width exceeds the returned value plust the returned
  value of \c spacing_width, then the remaining columns start on a new
  line. The return value can be dynamically obtained as a percentage of
  the total width.

  \c spacing_width is the blank space between the option column and the
  description column.

  \c fmt_option Return a string containing the fully formatted option
  (just the option part, not the extended description) or the empty
  string if the option should be hidden. Default is \c option_to_string

  \c cmp [OPTIONAL] If set, is used to sort the descriptors according to
  the comparison function.
*/
template<typename CharT>
struct basic_cmd_option_fmt {
  typedef std::basic_string<CharT> string_type;
  typedef basic_option_description<CharT> description_type;

  std::function<std::size_t(void)> width = [](void){ return 80; };
  std::function<std::size_t(void)> max_option_width = [](void){ return 40; };
  std::function<std::size_t(void)> spacing_width = [](void){ return 2; };

  std::function<string_type(const description_type &,
    const parse_options<CharT> &)> fmt_option = option_to_string<CharT>;

  std::function<string_type(const description_type &,
    const parse_options<CharT> &, std::size_t, std::size_t)> fmt_extended =
      extended_to_string<CharT>;

  std::function<bool(const description_type &,
    const description_type &)> sort_cmp = option_comp<CharT>;
};

typedef basic_cmd_option_fmt<char> cmd_option_fmt;
typedef basic_cmd_option_fmt<wchar_t> wcmd_option_fmt;

template<typename CharT>
std::basic_string<CharT>
to_string(const std::vector<basic_option_description<CharT> > &grp,
  const parse_options<CharT> &opt = parse_options<CharT>(),
  const basic_cmd_option_fmt<CharT> &fmt = basic_cmd_option_fmt<CharT>())
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_option_description<CharT> description_type;

  std::basic_ostringstream<CharT> out;

  typedef std::pair<const description_type *,string_type> output_pair_type;
  std::vector<output_pair_type> output_grp;
  output_grp.reserve(grp.size());

  std::size_t extent = 0;
  for(auto &desc : grp) {
    // hidden means not long or short key descriptions
    if((desc.map_long_key && desc.long_key_description) ||
      (desc.map_short_key && desc.short_key_description))
    {
      output_grp.emplace_back(std::make_pair(&desc,fmt.fmt_option(desc,opt)));
      extent = std::max(extent,output_grp.back().second.size());
    }
  }

  // Sort the results if requested
  if(fmt.sort_cmp) {
    std::sort(output_grp.begin(),output_grp.end(),
      [&](const output_pair_type &lhs, const output_pair_type &rhs) {
        return fmt.sort_cmp(*lhs.first,*rhs.first);
      });
  }

  std::size_t total_width = fmt.width();
  std::size_t max_option_width = fmt.max_option_width();
  std::size_t spacing_width = fmt.spacing_width();
  std::size_t rindent = std::min(extent+spacing_width,max_option_width);
  string_type rindent_pad(rindent,' ');
  for(std::size_t i=0; i<output_grp.size(); ++i) {
    out << output_grp[i].second;

    if(output_grp[i].second.size()+spacing_width > rindent)
      out << '\n' << rindent_pad;
    else
      out << string_type(rindent-output_grp[i].second.size(),' ');

    out << fmt.fmt_extended(*output_grp[i].first,opt,rindent,total_width)
      << '\n';
  }

  return out.str();
}

template<typename CharT>
inline std::basic_string<CharT>
to_string(const std::vector<basic_option_description<CharT> > &grp,
  const parse_options<CharT> &opt)
{
  return to_string(grp,opt,basic_cmd_option_fmt<CharT>());
}

template<typename CharT>
inline std::basic_string<CharT>
to_string_debug(const basic_option_description<CharT> &option)
{
  std::basic_ostringstream<CharT> out;

  out
    << "map_long_key: " << bool(option.map_long_key) << "\n"
    << "long_key_description: " << bool(option.long_key_description) << "\n"
    << "map_short_key: " << bool(option.map_short_key) << "\n"
    << "short_key_description: " << bool(option.short_key_description) << "\n"
    << "extended_description: " << bool(option.extended_description) << "\n"
    << "implicit_key: " << bool(option.implicit_key) << "\n"
    << "implicit_value: " << bool(option.implicit_value) << "\n"
    << "implicit_value_description: "
      << bool(option.implicit_value_description) << "\n"
    << "make_value: " << bool(option.make_value) << "\n"
    << "finalize: " << bool(option.finalize) << "\n";

  return out.str();
};




}

}




#endif



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
#include <stack>
#include <string>
#include <cassert>
#include <sstream>
#include <locale>
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

    - \c raw_key A string containing the raw key. Return the empty string to
    indicate that this option does not handle this option.

    - \c packed_key A string containing any remaining packed keys if
    present. To prevent undefined behavior, the \c prefix string will be
    prepended to the \c packed_key prior to any further processing.

    - \c value A string containing the value if present.

    - \c value_provided A boolean value indicating that the argument
      contained a packed value. This is necessary because it is possible
      to explicitly provide an empty value

    - \c cease_processing A boolean value indicating that this argument
      should be ignored and further processing argument should cease.

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
                      : packed_key = ""
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

  bool cease_processing;
  bool value_provided;
  string_type prefix;
  string_type raw_key;
  string_type packed_key;
  string_type value;
};

template<typename CharT>
std::ostream & operator<<(std::ostream &out,
  const basic_option_pack<CharT> &option_pack)
{
  out << "process opt:\n"
    << "\tcease_processing: '" << option_pack.cease_processing << "'\n"
    << "\tvalue_provided: '" << option_pack.value_provided << "'\n"
    << "\tprefix: '" << option_pack.prefix << "'\n"
    << "\traw_key: '" << option_pack.raw_key << "'\n"
    << "\tpacked_key: '" << option_pack.packed_key << "'\n"
    << "\tvalue: '" << option_pack.value << "'\n";

  return out;
}

template<typename CharT>
inline typename std::basic_string<CharT>::const_iterator
trimmed_leading_ws(const std::basic_string<CharT> &str,
  const std::locale &loc=std::locale())
{
  typename std::basic_string<CharT>::const_iterator cur = str.begin();
  while(cur != str.end() && std::isspace(*cur,loc))
    ++cur;

  return cur;
}

template<typename CharT>
inline typename std::basic_string<CharT>::const_iterator
trimmed_trailing_ws(const std::basic_string<CharT> &str,
  const std::locale &loc=std::locale())
{
  typename std::basic_string<CharT>::const_reverse_iterator cur = str.rbegin();
  while(cur != str.rend() && std::isspace(*cur,loc))
    ++cur;
  return cur.base();
}

/*
  POSIX argument syntax. Unpack arguments in the form:

  POSIX:
    "-k", "-kvalue"
*/
template<typename CharT>
basic_option_pack<CharT> unpack_posix(const std::basic_string<CharT> &str)
{
  typedef basic_option_pack<CharT> option_pack;
  typedef std::basic_string<CharT> string_type;

  static const string_type cease("--");
  static const string_type sprefix("-");


  typename string_type::const_iterator first = trimmed_leading_ws(str);

  if(first == str.end())
    return option_pack();

  typename string_type::const_iterator last = trimmed_trailing_ws(str);

  if(std::search(first,last,cease.begin(),cease.end()) == first)
    return option_pack{true};

  if(std::search(first,last,sprefix.begin(),sprefix.end()) == first) {
    first += sprefix.size();

    if(first == last) // orphan '-'
      return option_pack();

    if(first+1 == last)
      return option_pack{false,false,sprefix,{first,first+1},{},{}};

    return option_pack{false,true,sprefix,{first,first+1},{},{first+1,last}};
  }

  return option_pack();
}



/*
  GNU extension to POSIX argument syntax. Unpack arguments in the form:

  POSIX:
    "-k", "-kvalue"

  GNU:
    "--key", "--key=value"
*/
template<typename CharT>
basic_option_pack<CharT> unpack_gnu(const std::basic_string<CharT> &str)
{
  typedef basic_option_pack<CharT> option_pack;
  typedef std::basic_string<CharT> string_type;

  static const string_type lprefix("--");
  static const string_type sprefix("-");
  static const string_type assignment("=");

  typename string_type::const_iterator first = trimmed_leading_ws(str);

  if(first == str.end())
    return option_pack();

  typename string_type::const_iterator last = trimmed_trailing_ws(str);

  // must check for long option first
  if(std::search(first,last,lprefix.begin(),lprefix.end()) == first) {
    first += lprefix.size();

    if(first == last) // orphan '--' or cease
      return option_pack{true};

    typename string_type::const_iterator assign_loc =
      std::search(first,last,assignment.begin(),assignment.end());

    if(assign_loc == last)
      return option_pack{false,false,lprefix,{first,last},{},{}};

    return option_pack{false,true,lprefix,{first,assign_loc},{},
      {assign_loc+1,last}};
  }

  if(std::search(first,last,sprefix.begin(),sprefix.end()) == first) {
    first += sprefix.size();

    if(first == last) // orphan '-'
      return option_pack();

    if(first+1 == last)
      return option_pack{false,false,sprefix,{first,first+1},{},{}};

    return option_pack{false,true,sprefix,{first,first+1},{},{first+1,last}};
  }

  return option_pack();
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
    For the given raw key (without any prefix), return the mapped_key
    that is placed in the the corresponding variable_map if the raw_key
    is associated with this option_description or the empty string
    otherwise. If \c mapped_key is not set, then this description will
    use the raw key as the mapped_key. The byproduct of omitting \c
    mapped_key is that this option_description will match any option
    that the provided \c unpack_argument accepts. For example, to accept
    all unknown options, then provide an option_description with the
    appropriate \c unpacked_argument and no \c mapped_key set as the
    last description in an \c options_group.
  */
  std::function<string_type(const string_type &raw_key,
    const variable_map &vm)> mapped_key;

  /*
    Return the human-readable description for this option. This does not
    necessarily need to be the result provided in \c mapped_key
    (although it often does). The value is not used for options parsing
    but typically used in the help message. For example, supposed this
    option is given by --foo, return "--foo". This value can be any
    meaningful string based on the need. For example, suppose for some
    nonstandard syntax the key --foo1bar --foo2bar is possible, a
    reasonable response from this function could be: "foo[integer]bar".
    Another example is in the case of long and short options. Typically
    the short option is hidden and the long option is fully specified
    with a description containing the associated short option. That is:
    --foo, -f.
  */
  std::function<string_type(void)> key_description;

  /*
    Return a human-readable concise description of this option's
    functionality.
  */
  std::function<string_type(void)> extended_description;

  /*
    If \c unpack_argument is not provided, then the option is a \c
    positional option. In this case, the option may mean different
    things based on where it exists in the option list. The argument to
    the given function is the number of the positional option not
    including the non-positional options. For example, in the option
    list where '--foo', '--bar', '-f', and '-g' do not take options:
      "--foo one --bar two -f -g three"
    the positional options are: one, two, and three with position 1, 2,
    and 3 respectively. If set, the function can return a unique key
    that will appear in the variable_map for each positional option or
    the same if users choose to. If the function is not set, then all
    positional options will be available using the default-constructed
    \c string_type as the variable_map key. Like \c mapped_key, if this
    description does not define the given positional option, then return
    the empty string. For example, if this description handles only the
    XXth positional option.
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


#if 0
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
  basic_variable_map<CharT> &vm)
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
  basic_variable_map<CharT> &vm)
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
#endif






/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp. If \c partial is false, then
  finalize parsing. Options are added to a copy of the variable map \c
  _vm and returned. If parsing is halted due to a "end of parse"
  indicator (normally '--') then \c endc is updated to the index of the
  first non-parsed argument.
*/
template<typename CharT>
basic_variable_map<CharT>
parse_arguments(const CharT * const argv[], std::size_t argc,
  std::size_t &endc, const basic_options_group<CharT> &grp,
  const basic_variable_map<CharT> &vm, bool partial)
{
  typedef std::basic_string<CharT> string_type;
  typedef std::basic_stringstream<CharT> sstream_type;
  typedef basic_option_pack<CharT> option_pack_type;
  typedef basic_options_group<CharT> options_group_type;
  typedef basic_variable_map<CharT> variable_map;

  variable_map _vm = vm;

  std::size_t pos_count = 0;
  for(endc=0; endc<argc; ++endc) {
    std::stack<string_type> arg_stack;
    arg_stack.emplace(argv[endc]);

    while(!arg_stack.empty()) {
      /*
        An option is _always_ handled. This is, one of the following happens

        - A description matches the option including a "catch-all" positional
        - An error is generated
      */
      string_type &&arg = std::move(arg_stack.top());
      arg_stack.pop();
std::cerr << "arg: " << arg << "\n";
      try {
        // try and unpack the argument
        typename options_group_type::const_iterator desc;
        for(desc = grp.begin(); desc != grp.end(); ++desc) {
          // see if the argument is an option or a positional
          if(desc->unpack_argument) {
            option_pack_type &&option_pack = desc->unpack_argument(arg);

            if(option_pack.raw_key.empty())
              continue;

            // unpacking was successful
            std::cerr << option_pack;

            if(option_pack.cease_processing)
              return _vm;

            // Attempt to process the unpacked options. This description may
            // still not be the one to handle this option though.
            string_type mapped_key;
            if(desc->mapped_key) {
              mapped_key = desc->mapped_key(option_pack.raw_key,_vm);
              if(mapped_key.empty())
                continue;
            }
            else
              mapped_key = option_pack.raw_key;

            if(!desc->make_value && option_pack.value_provided) {
              sstream_type err;
              err << "option '" << option_pack.prefix << option_pack.raw_key
                << "' strictly does not accept values";
              throw command_option_error(err.str());

              _vm.emplace(mapped_key,any());
            }
            else if(!option_pack.value_provided) {
              if(!desc->implicit_value) {
                sstream_type err;
                err << "option '" << option_pack.prefix << option_pack.raw_key
                  << "' strictly requires a value";
                throw command_option_error(err.str());
              }

              // use implicit value
              _vm.emplace(mapped_key,desc->implicit_value(mapped_key,_vm));
            }
            else {
              // use make_value
              _vm.emplace(mapped_key,
                desc->make_value(mapped_key,option_pack.value,_vm));
            }

            if(!option_pack.packed_key.empty())
              arg_stack.emplace(option_pack.prefix+option_pack.packed_key);

            break;
          }
          else {
            //positional option
            string_type pos_key;

            if(desc->implicit_key) {
              pos_key = desc->implicit_key(pos_count);
              if(pos_key.empty())
                continue;
            }

            // handle this positional option
            if(desc->make_value)
              _vm.emplace(pos_key,desc->make_value(pos_key,arg,_vm));
            else
              _vm.emplace(pos_key,any());

            ++pos_count;
            break;
          }

        }
        if(desc == grp.end())
          throw command_option_error("Unknown option '%@'");
      }
      catch(const command_option_error &ex) {
        std::regex exp("%@");
        throw command_option_error(std::regex_replace(ex.what(),exp,arg));
      }
    }
  }

  return _vm;
}






#if 0

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

#endif









/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp. If
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
    partial);
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
  return parse_arguments(argv,argc,endc,grp,_vm,partial);
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
  bool partial = false)
{
  return parse_arguments(argv,argc,endc,grp,basic_variable_map<CharT>(),
    partial);
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

template<typename CharT>
const std::basic_string<CharT> & default_long_prefix(void); // undefined

template<>
const std::basic_string<char> & default_long_prefix<char>(void)
{
  static const std::basic_string<char> val("--");

  return val;
}

template<>
const std::basic_string<wchar_t> & default_long_prefix<wchar_t>(void)
{
  static const std::basic_string<wchar_t> val(L"--");

  return val;
}

template<typename CharT>
const std::basic_string<CharT> & default_short_prefix(void); // undefined

template<>
const std::basic_string<char> & default_short_prefix<char>(void)
{
  static const std::basic_string<char> val("-");

  return val;
}

template<>
const std::basic_string<wchar_t> & default_short_prefix<wchar_t>(void)
{
  static const std::basic_string<wchar_t> val(L"-");

  return val;
}






namespace detail {

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

template<typename CharT>
std::pair<std::basic_string<CharT>,std::basic_string<CharT> >
add_option_spec(const std::basic_string<CharT> &opt_spec,
  CharT delim, basic_option_description<CharT> &desc)
{
  typedef std::basic_string<char> string_type;
  typedef basic_variable_map<CharT> variable_map;

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) = detail::split(opt_spec,delim);

  /*
    If both are provided, the long option is the vm key

    If none are provided, then it is unmapped_key
  */
  string_type key_desc;
  if(opt_spec.empty() || (long_opt.empty() && short_opt.empty())) {
    key_desc = default_long_prefix<CharT>() + "[all] ,  "
        + default_short_prefix<CharT>() + "[all]";
  }
  else if(!long_opt.empty() && !short_opt.empty()) {
    desc.mapped_key = [=](const string_type &_opt, const variable_map &)
      {
        if(_opt == long_opt || _opt == short_opt)
          return long_opt;

        return string_type();
      };

    key_desc = default_long_prefix<CharT>() + long_opt + " ,  "
        + default_short_prefix<CharT>() + short_opt;
  }
  else if(!long_opt.empty()) {
    desc.mapped_key = [=](const string_type &_opt, const variable_map &)
      {
        if(_opt == long_opt)
          return _opt;

        return string_type();
      };

    key_desc = default_long_prefix<CharT>() + long_opt;
  }
  else {
    desc.mapped_key = [=](const string_type &_opt, const variable_map &)
      {
        if(_opt == short_opt)
          return _opt;

        return string_type();
      };

    key_desc = default_short_prefix<CharT>() + short_opt;
  }

  desc.key_description = [=](void) { return key_desc; };

  return std::make_pair(long_opt,short_opt);
}









template<typename CharT>
std::pair<std::basic_string<CharT>,std::basic_string<CharT> >
add_hidden_option_spec(const std::basic_string<CharT> &opt_spec,
  CharT delim, basic_option_description<CharT> &desc)
{
  typedef std::basic_string<char> string_type;
  typedef basic_variable_map<CharT> variable_map;

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) = detail::split(opt_spec,delim);

  /*
    If both are provided, the long option is the vm key

    If none are provided, then it is unmapped_key
  */
  if(!long_opt.empty() && !short_opt.empty()) {
    desc.mapped_key = [=](const string_type &_opt, const variable_map &)
      {
        if(_opt == long_opt || _opt == short_opt)
          return long_opt;

        return string_type();
      };
  }
  else if(!long_opt.empty()) {
    desc.mapped_key = [=](const string_type &_opt, const variable_map &)
      {
        if(_opt == long_opt)
          return _opt;

        return string_type();
      };
  }
  else if(!short_opt.empty()){
    desc.mapped_key = [=](const string_type &_opt, const variable_map &)
      {
        if(_opt == short_opt)
          return _opt;

        return string_type();
      };
  }

  return std::make_pair(long_opt,short_opt);
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
inline void add_key(const std::basic_string<CharT> &key, int position,
  basic_option_description<CharT> &desc)
{
  assert(!key.empty());

  if(position >= 0) {
    desc.implicit_key = [=](std::size_t i) {
      if(i == position)
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







/*
  The following functions are for convenience only. They provide automatic
  type deduction and therefore implicit conversion from string literals
  to the basic_string equivalents as well as defaults for unused arguments.
*/

/*
  Cases line 2, 4
*/
inline basic_option_description<char>
make_option(const std::basic_string<char> &opt_spec,
  const std::basic_string<char> &extended_desc,
  const constrain<char> &cnts = constrain<char>(), char delim = ',')
{
  typedef std::basic_string<char> string_type;

  basic_option_description<char> desc{unpack_gnu<char>};

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) = detail::add_option_spec(opt_spec,delim,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Cases line 3, 5
*/
inline basic_option_description<char>
make_hidden_option(const std::basic_string<char> &opt_spec =
  std::basic_string<char>(),
  const constrain<char> &cnts = constrain<char>(), char delim = ',')
{
  typedef std::basic_string<char> string_type;

  basic_option_description<char> desc{unpack_gnu<char>};

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) =
    detail::add_hidden_option_spec(opt_spec,delim,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 6, 8, 10, 12
*/
template<typename T>
inline basic_option_description<char>
make_option(const std::basic_string<char> &opt_spec,
  const value<T> &val, const std::basic_string<char> &extended_desc,
  const constrain<char> &cnts = constrain<char>(), char delim = ',')
{
  typedef std::basic_string<char> string_type;

  basic_option_description<char> desc{unpack_gnu<char>};

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) = detail::add_option_spec(opt_spec,delim,desc);

  detail::add_value(val,desc);

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 7, 9, 11, 13
*/
template<typename T>
inline basic_option_description<char>
make_hidden_option(const std::basic_string<char> &opt_spec, const value<T> &val,
  const constrain<char> &cnts = constrain<char>(), char delim = ',')
{
  typedef std::basic_string<char> string_type;

  basic_option_description<char> desc{unpack_gnu<char>};

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) =
    detail::add_hidden_option_spec(opt_spec,delim,desc);

  detail::add_value(val,desc);

  detail::add_option_constrains(cnts,desc,long_opt,long_opt,short_opt);

  return desc;
}

/*
  Case line 14, 18
*/
template<typename T>
inline basic_option_description<char>
make_positional(const value<T> &val,
  const std::basic_string<char> &extended_desc,
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_value(val,desc);

  detail::add_positional_constrains(cnts,std::basic_string<char>(),desc);

  return desc;
}

/*
  Case line 15, 19
*/
template<typename T>
inline basic_option_description<char>
make_hidden_positional(const value<T> &val,
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  detail::add_value(val,desc);

  detail::add_positional_constrains(cnts,std::basic_string<char>(),desc);

  return desc;
}

/*
  Case line 16, 20
*/
template<typename T>
inline basic_option_description<char>
make_positional(const std::basic_string<char> &implicit_key,
  const value<T> &val, const std::basic_string<char> &extended_desc,
  int position = -1, const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_value(val,desc);

  detail::add_key(implicit_key,position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}

/*
  Case line 17, 21
*/
template<typename T>
inline basic_option_description<char>
make_hidden_positional(const std::basic_string<char> &implicit_key,
  const value<T> &val, int position = -1,
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  detail::add_value(val,desc);

  detail::add_key(implicit_key,position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}


/*
  Case line 22
*/
inline basic_option_description<char>
make_positional(const std::basic_string<char> &extended_desc,
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_positional_constrains(cnts,std::basic_string<char>(),desc);

  return desc;
}

/*
  Case line 23
*/
inline basic_option_description<char>
make_hidden_positional(const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  detail::add_positional_constrains(cnts,std::basic_string<char>(),desc);

  return desc;
}

/*
  Case line 24
*/
inline basic_option_description<char>
make_positional(const std::basic_string<char> &implicit_key,
  const std::basic_string<char> &extended_desc, int position = -1,
  const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_key(implicit_key,position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}

/*
  Case line 25
*/
inline basic_option_description<char>
make_hidden_positional(const std::basic_string<char> &implicit_key,
  int position = -1, const constrain<char> &cnts = constrain<char>())
{
  basic_option_description<char> desc;

  detail::add_key(implicit_key,position,desc);

  detail::add_positional_constrains(cnts,implicit_key,desc);

  return desc;
}




/*
  Specializations for wchar_t

  Add me
*/




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
  basic_option_description<CharT> option_desc =
    make_option("",value<std::string>(""),"any");

  option_desc.unpack_argument = unpack_gnu<CharT>;

  basic_options_group<CharT> grp{
    option_desc
  };

  return parse_arguments(argv,argc,endc,grp,basic_variable_map<CharT>(),false);
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
option_to_string(const basic_option_description<CharT> &desc)
{
//  typedef std::basic_string<CharT> string_type;

  std::basic_ostringstream<CharT> out;
#if 0
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
#endif
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
  std::size_t indent, std::size_t width)
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

  std::function<string_type(const description_type &)> fmt_option =
    option_to_string<CharT>;

  std::function<string_type(const description_type &,
    std::size_t, std::size_t)> fmt_extended = extended_to_string<CharT>;

  std::function<bool(const description_type &,
    const description_type &)> sort_cmp = option_comp<CharT>;
};

typedef basic_cmd_option_fmt<char> cmd_option_fmt;
typedef basic_cmd_option_fmt<wchar_t> wcmd_option_fmt;

template<typename CharT>
std::basic_string<CharT>
to_string(const std::vector<basic_option_description<CharT> > &grp,
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
      output_grp.emplace_back(std::make_pair(&desc,fmt.fmt_option(desc)));
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

    out << fmt.fmt_extended(*output_grp[i].first,rindent,total_width)
      << '\n';
  }

  return out.str();
}

template<typename CharT>
inline std::basic_string<CharT>
to_string(const std::vector<basic_option_description<CharT> > &grp)
{
  return to_string(grp,basic_cmd_option_fmt<CharT>());
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



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

#if __cplusplus < 201703L
#include <boost/any.hpp>
#endif

#include <map>
#include <regex>
#include <tuple>
#include <utility>
#include <vector>
#include <stack>
#include <string>
#include <cassert>
#include <sstream>
#include <codecvt>
#include <functional>

#include <iostream>

#define CMD_OPTION_QUOTE(x) #x
#define CMD_OPTION_STR(x) CMD_OPTION_QUOTE(x)

#ifndef CMD_OPTIONS_DEFAULT_OPERAND_KEY
#define CMD_OPTIONS_DEFAULT_OPERAND_KEY
#endif

#define CMD_OPTIONS_DEFAULT_OPERAND_KEY_STR  \
  CMD_OPTION_STR(CMD_OPTIONS_DEFAULT_OPERAND_KEY)

namespace lemma {

namespace cmd_options {

/*
  Currently uses boost::any but will eventually transition to std::any in C++17
*/
#if __cplusplus < 201703L
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
#else
  using any = std::any;
#endif

#ifndef CMD_OPTIONS_DEFAULT_OPERAND_KEY
#define CMD_OPTIONS_DEFAULT_OPERAND_KEY
#endif

static const std::string
  default_operand_key(CMD_OPTIONS_DEFAULT_OPERAND_KEY_STR);


namespace detail {

// In the future (post C++14) use std::mismatch
template<class InputIt1, class InputIt2>
inline std::pair<InputIt1, InputIt2>
mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
  while (first1 != last1 && first2 != last2 && *first1 == *first2) {
      ++first1, ++first2;
  }
  return std::make_pair(first1, first2);
}

}

/*
  Exception class hierarchy inherited from std::runtime error used to
  indicate command option errors.
*/
class parse_error : public std::runtime_error {
  public:
    parse_error(std::size_t posn, std::size_t argn)
      :std::runtime_error("parse_error"), _position(posn),
        _argument(argn) {}

    std::size_t position(void) const {
      return _position;
    }

    std::size_t argument(void) const {
      return _argument;
    }

  protected:
    parse_error(const std::string &what, std::size_t pos, std::size_t arg)
      :std::runtime_error(what), _position(pos), _argument(arg) {}

  private:
    std::size_t _position;
    std::size_t _argument;
};

class unknown_option_error : public parse_error {
  public:
    unknown_option_error(const std::string &option, std::size_t posn,
      std::size_t argn) :parse_error("unknown_option_error",posn,argn),
        _option(option) {}

    const char * option(void) const noexcept {
      return _option.what();
    }

  private:
    std::runtime_error _option;
};

class missing_argument_error : public parse_error {
  public:
    missing_argument_error(const std::string &option, std::size_t posn,
      std::size_t argn) :parse_error("missing_argument_error",posn,argn),
        _option(option) {}

    const char * option(void) const noexcept {
      return _option.what();
    }

  private:
    std::runtime_error _option;
};

class unexpected_argument_error : public parse_error {
  public:
    unexpected_argument_error(const std::string &option,
      const std::string &argument, std::size_t posn, std::size_t argn)
        :parse_error("unexpected_argument_error",posn,argn), _option(option),
          _argument(argument) {}

    const char * option(void) const noexcept {
      return _option.what();
    }

    const char * argument(void) const noexcept {
      return _argument.what();
    }

  private:
    std::runtime_error _option;
    std::runtime_error _argument;
};

class unexpected_operand_error : public parse_error {
  public:
    unexpected_operand_error(const std::string &operand, std::size_t posn,
      std::size_t argn)
        :parse_error("unexpected_operand_error",posn,argn), _operand(operand) {}

    const char * operand(void) const noexcept {
      return _operand.what();
    }

  private:
    std::runtime_error _operand;
};

class constraint_error : public std::runtime_error {
  public:
    constraint_error(const std::string &mapped_key)
      :std::runtime_error("constraint_error"), _mapped_key(mapped_key) {}

    const char * mapped_key(void) const noexcept {
      return _mapped_key.what();
    }

  protected:
    constraint_error(const std::string &what, const std::string &mapped_key)
      :std::runtime_error(what), _mapped_key(mapped_key) {}

  private:
    std::runtime_error _mapped_key;
};

class occurrence_error : public constraint_error {
  public:
    occurrence_error(const std::string &mapped_key, std::size_t min,
      std::size_t max, std::size_t occurrences)
        :constraint_error("occurrence_error",mapped_key), _min(min), _max(max),
          _occurrences(occurrences) {}

    const std::size_t min(void) const noexcept {
      return _min;
    }

    const std::size_t max(void) const noexcept {
      return _max;
    }

    const std::size_t occurrences(void) const noexcept {
      return _occurrences;
    }

  private:
    std::size_t _min;
    std::size_t _max;
    std::size_t _occurrences;
};

class mutually_exclusive_error : public constraint_error {
  public:
    mutually_exclusive_error(const std::string &mapped_key,
      const std::string &exclusive_mapped_key)
        :constraint_error("mutually_exclusive_error",mapped_key),
          _exclusive_mapped_key(exclusive_mapped_key) {}

    const char * exclusive_mapped_key(void) const noexcept {
      return _exclusive_mapped_key.what();
    }

  private:
    std::runtime_error _exclusive_mapped_key;
};

class mutually_inclusive_error : public constraint_error {
  public:
    mutually_inclusive_error(const std::string &mapped_key,
      const std::string &inclusive_mapped_key)
        :constraint_error("mutually_inclusive_error",mapped_key),
          _inclusive_mapped_key(inclusive_mapped_key) {}

    const char * inclusive_mapped_key(void) const noexcept {
      return _inclusive_mapped_key.what();
    }

  private:
    std::runtime_error _inclusive_mapped_key;
};




template<typename CharT>
using basic_variable_map = std::multimap<std::basic_string<CharT>,any>;

/*
  Structure describing an unpacked argument.

  bool did_unpack;
  bool value_provided;
  string_type prefix;
  string_type raw_key;
  std::vector<string_type> packed_arguments;
  string_type value;

  The elements represent:
    - \c did_unpack A boolean value indicating that the structure
    contains the result of unpacking the given argument into its
    constitute properties.

    - \c value_provided A boolean value indicating that the argument
      contained a packed value. This is necessary because it is possible
      to explicitly provide an empty value

    - \c prefix A string specifying the option prefix, ie '-' or '--'

    - \c raw_key A string containing the raw key.

    - \c packed_arguments A vector of strings containing any remaining
    packed arguments if present. If the arguments require a prefix, they
    must be prepended to each argument otherwise it will be considered an
    operand argument. For example, if you wish to allow "-abcd" to be
    equivalent to "-a -b --cat=dog -d", then the raw key is "a" and the
    packed options are: {"-b","--cat=dog","d"}. An appropriate unpack
    method could add packed arguments for other reasons. For example, if
    "-b" always implies a hidden (deprecated) "--bar_old", then raw_key
    is "b" and include "--bar_old" to the packed arguments;

    - \c value A string containing the value if present.



  This structure is intended to represent an unpacked argument, not it's
  semantic value and is a byproduct of the many different ways that an
  option argument can be packed together. For example: here are some
  known ways that option arguments can be represented:

      1) --foo bar    = long option, key=foo, value=bar
                      : prefix = "--"
                      : raw_key = "foo"
                      : packed_arguments = ""
                      : value = ""
                      : value_provided = false.

      2) --foo=bar    = long option, key=foo, value=bar
                      : prefix = "--"
                      : raw_key = "foo"
                      : packed_arguments = ""
                      : value = "bar"
                      : value_provided = true

      3) -foo bar     = long option, key=foo, value=bar
                      : prefix = "-"
                      : raw_key = "foo"
                      : packed_arguments = ""
                      : value = ""
                      : value_provided = false

      2) -foo=bar     = long option, key=foo, value=bar
                      : prefix = "-"
                      : raw_key = "foo"
                      : packed_arguments = ""
                      : value = "bar"
                      : value_provided = true

      5) -f bar       = short option, key=f, value=bar
                      : prefix = "-"
                      : raw_key = "f"
                      : packed_arguments = ""
                      : value = ""
                      : value_provided = false

      6) -fBar        = short option, key=f, value=Bar
                      : prefix = "-"
                      : raw_key = "f"
                      : packed_arguments = ""
                      : value = "Bar"
                      : value_provided = true

      7) -fbar        = 4 short options with keys: 'f', 'b', 'a', and 'r'
                      : prefix = "-"
                      : raw_key = "f"
                      : packed_arguments = {"-b","-a","-r"}
                      : value = ""
                      : value_provided = false

      8) -nobar       = long flag, key=bar, value=false
                      : prefix = "-"
                      : raw_key = "bar"
                      : packed_arguments = ""
                      : value = "false"
                      : value_provided = true

      9) -Wl,<args>   = long flag, key=Wl, value=<args>
                      : prefix = "-"
                      : raw_key = "Wl"
                      : packed_arguments = ""
                      : value = "<args>"
                      : value_provided = true
*/
template<typename CharT>
struct basic_option_pack {
  typedef std::basic_string<CharT> string_type;
  typedef std::vector<string_type> packed_arg_seq;

  bool did_unpack;
  bool value_provided;
  string_type prefix;
  string_type raw_key;
  packed_arg_seq packed_arguments;
  string_type value;
};

template<typename CharT>
std::ostream & operator<<(std::ostream &out,
  const basic_option_pack<CharT> &option_pack)
{
  out << "process opt:\n"
    << "\tdid_unpack: '" << option_pack.did_unpack << "'\n"
    << "\tvalue_provided: '" << option_pack.value_provided << "'\n"
    << "\tprefix: '" << option_pack.prefix << "'\n"
    << "\traw_key: '" << option_pack.raw_key << "'\n"
    << "\tpacked_arguments: {";
  for(auto &&arg : option_pack.packed_arguments)
    out << "'" << arg << "' ";
  out
    << "}\n"
    << "\tvalue: '" << option_pack.value << "'\n";

  return out;
}

/*
  A description for a single option
*/
template<typename CharT>
struct basic_option_description {
  typedef CharT                     char_type;
  typedef std::basic_string<CharT>  string_type;
  typedef basic_variable_map<CharT> variable_map_type;
  typedef basic_option_pack<CharT>  option_pack;

  /*
    Unpack the raw option pack. If \c unpack_option is not provided,
    then this description is assumed to describe an operand argument.

    If the option cannot be unpacked by this description, then return
    false in the \c did_unpack field (or an empty option_pack).

    This function is intended to essentially parse the option
    into its constitute properties but it can also be used to inject
    hidden options that can be useful. For example, if --foo is given
    implies --bar, even if --bar was not provided, then the unpacked
    option for "foo" can return "bar" in the \c packed_arguments field.

    A note on operand values... If the options group _only_ contains
    operands--or descriptions that do not have the \c unpack_option
    set--then any argument is considered a operand---even if looks
    like an option. For example, if there are no option_descriptions
    with \c unpack_option set, then '--foo', which looks like a option
    flag, is considered a operand. This may not be what was intended.
    Reasonable machinery in this case is to include a 'dummy' option
    that has \c unpack_option set and where \c mapped_key throws a \c
    unknown_option_error indicating the unknown option.
  */
  std::function<option_pack(const string_type &option)> unpack_option;

  /*
    If this description can handle the given argument, return true
    and the key that will be used in the variable map to represent this
    option or operand depending on the presence or absence of
    \c unpack_option.

    If unpack_option is set and can successfully unpack the argument,
    then the argument is an option and \c mapped_key should return the
    mapped key for the given raw key (without any prefix) as provided by
    the \c option_pack.

    If unpack_option is not set, then the argument is an operand and
    mapped_key should return the appropriate mapped key for the default
    raw key \c default_operand_key (or simply return \c default_operand_key).

    If \c mapped_key is not set then this description will be marked to
    handle the argument and \c raw_key will be used for the key in the
    variable map. This means that this description will match all
    options is \c unpack_option is set (ie accept all unknown options
    able to be unpacked by \c unpack_option) and will match all operands
    if \c unpack_option is not set (ie accept an indefinite list of
    operands).

    Regardless of whether or not the argument is an option or an
    operand, the implementation of \c mapped_key can selectively choose
    to handle or not to handle the argument based on the option's or
    operand's position in the argument list. \c posn is the ith option
    or operand in the argument list and \c argn is the current argument
    number. A common use case is for some utility:

      foo infile outfile

    Where \c infile is always first and \outfile is always second.
  */
  std::function<std::pair<bool,string_type>(
    const string_type &raw_key, std::size_t posn, std::size_t argn,
    const variable_map_type &vm)> mapped_key;

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
    Non-operand option cases:
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
    syntaxes. For example, given -frtti vs -fno-rtti, the
    variable_map_type key could be 'rtti' with a boolean value. The
    option forbids values so the mapped value must be determined by the
    key (specifically the presence or absence of the 'no' prefix.
  */
  std::function<
    any(const string_type &key, const variable_map_type &vm)> implicit_value;

  /*
    Return the human-readable description for this option's implicit
    value. This description is not used for parsing but rather automatic
    help messages. For example, suppose the option --foo has an implicit
    value of '5' if another value was not provided. A reasonable return
    value from this function would be "5".
  */
  std::function<string_type(void)> implicit_value_description;

  /*
    Operand option case:
      -- interpret the option value (make_value is set)
      -- use default constructed value (make_value is not set)

    If set, return the value of the argument as a any as
    determined by the contents of the second parameter. If not set, the
    option explicitly forbids option values. I.e. --foo=bar or --foo bar
    will return an error unless the 'bar' is a valid operand in the
    later case.

    The function's first parameter is the string given as the option's
    mapped_key exactly as provided by the \c map_long_key and/or \c
    map_short_key function. This can be useful for checking the state of
    the vm and throwing an error if too many instances of a certain
    option has been given or if certain combinations of options are
    mutually-exclusive or inclusive.

    If the argument was an operand, then the first parameter is the
    result of \c implicit_key if set or a default-constructed
    string_type if not set.

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
    any(const string_type &mapped_key, const string_type &value,
      const variable_map_type &vm)> make_value;

  /*
    If set, this function is called for each option_description by the
    parse_* functions after all options are parsed. A typical use of
    this function is to inspect the state of the variable_map_type
    argument for option-specific inconsistencies or illegal
    combinations. For example, if a minimum number of options are
    expected, the variable_map_type can be queried to see if it has been
    satisfied and throw an error if not.
  */
  std::function<void(const variable_map_type &vm)> finalize;

};

template<typename CharT>
using basic_options_group = std::vector<basic_option_description<CharT> >;


template<typename CharT>
std::ostream & operator<<(std::ostream &out,
  const basic_option_description<CharT> &desc)
{
  out << "option_description:\n"
    << "\tunpack_option: " << bool(desc.unpack_option) << "\n"
    << "\tmapped_key: " << bool(desc.mapped_key) << "\n"
    << "\tkey_description: " << bool(desc.key_description) << "\n"
    << "\textended_description: " << bool(desc.extended_description) << "\n"
    << "\timplicit_value: " << bool(desc.implicit_value) << "\n"
    << "\timplicit_value_description: " << bool(desc.implicit_value_description)
      << "\n"
    << "\tmake_value: " << bool(desc.make_value) << "\n"
    << "\tfinalize: " << bool(desc.finalize) << "\n";

  return out;
}




/*
  POSIX flag syntax. Unpack alphanumeric arguments in the form:

    "-f", "-fabcde"

  if uses_packed_flags is true then:

    "-fabcde" is equivalent to "-f -a -b -c -d -e"

  otherwise

    "f" is the key and "abcde" is the value

  The Single Unix Specification Utility Syntax Guideline 3 states that a
  POSIX option name should be a single alphanumeric character from the
  portable character set. A question then arises what does it mean when
  a non-alphanumeric character is encountered? If the unpack function
  chooses to report that it cannot unpack the option, then the option
  may get picked up as an operand which would be incorrect unless
  there is some "fall-through" handler that intercepts the incorrect
  option and indicates the error.  If, on the other hand, this
  unpack-function reports it as an error, then it precludes the
  possibility of some later, non-SUS conforming unpack function handling
  the option. The solution here is that this function will unpack the
  flag and let the option description determine if it will accept the
  non-conforming option or not.

  N.B. any leading and trailing whitespace is assumed to be handled
    appropriately prior to this call. That is, " -f" is not the same as "-f".
*/
template<bool uses_packed_flags, typename CharT>
basic_option_pack<CharT> unpack_posix(const std::basic_string<CharT> &str)
{
  typedef basic_option_pack<CharT> option_pack;
  typedef typename option_pack::string_type string_type;
  typedef typename option_pack::packed_arg_seq packed_arg_seq;

  static const string_type sprefix("-");

  auto &&res =
    detail::mismatch(sprefix.begin(),sprefix.end(),str.begin(),str.end());
  if(res.first != sprefix.end() || res.second == str.end())
    return option_pack{false};

  if(res.second+1 == str.end())
    return option_pack{true,false,sprefix,{res.second,res.second+1},{},{}};

  if(uses_packed_flags) {
    string_type raw_key{res.second,++res.second};
    packed_arg_seq packed_arguments;
    while(res.second != str.end())
      packed_arguments.push_back(sprefix+(*(res.second++)));

    return option_pack{true,false,sprefix,raw_key,packed_arguments};
  }

  return option_pack{true,true,sprefix,{res.second,res.second+1},{},
    {res.second+1,str.end()}};
}

/*
  GNU extension (long option) to POSIX argument syntax. Unpack arguments in the
  form:

  POSIX:
    if uses_packed_flags is true then:

      "-fabcde" is equivalent to "-f -a -b -c -d -e"

    otherwise

      "f" is the key and "abcde" is the value

  GNU:
    "--key"
    "--key=value"

    where:

      first form: "key" is the key and "" is the value
      second form: "key" is the key and "value" is the value

  Herein lies a design decision derived from ambiguity in the GNU long
  option specification (if one really exists). I believe that this is
  unpacking method of least surprise and one that is used on every tool
  that I've checked. Specifically what is the proper handling for an
  long option the does not accept value but one is given. For example
  given "--key=value", what should the correct behavior be?

    - accept the option where the long option is the entire string? ie
      the option is "key=value"
    - accept the option decomposing it into
      the key-value even though we've specified that this option doesn't
      accept arguments (values).

  We are choosing the second option as it appears to lead to the most
  expected behavior, the downside of this is that if someone chooses \c
  unpack_gnu_flag by accident when they would like to accept arguments,
  the parser will correctly parse long options with values but fail to
  accept short options with values.

  N.B. any leading and trailing whitespace is assumed to be handled
    appropriately prior to this call. That is, " -f" is not the same as "-f".
*/
template<bool uses_packed_flags, typename CharT>
basic_option_pack<CharT> unpack_gnu(const std::basic_string<CharT> &str)
{
  typedef basic_option_pack<CharT> option_pack;
  typedef std::basic_string<CharT> string_type;

  static const string_type lprefix("--");
  static const string_type assignment("=");

  auto &&res =
    detail::mismatch(lprefix.begin(),lprefix.end(),str.begin(),str.end());
  if(res.first != lprefix.end() || res.second == str.end())
    return unpack_posix<uses_packed_flags>(str);

  typename string_type::const_iterator assign_loc =
    std::search(res.second,str.end(),assignment.begin(),assignment.end());

  if(assign_loc == str.end())
    return option_pack{true,false,lprefix,{res.second,str.end()},{},{}};

  return option_pack{true,true,lprefix,{res.second,assign_loc},{},
    {assign_loc+1,str.end()}};
}



/*
  Parse the arguments contained in \c argv with size \c argc according
  to the option description group \c grp. If \c partial is false, then
  finalize parsing. Options are added to a copy of the variable map
  \c _vm and returned.

  Weird cases:

  option takes an argument but the given argument is another (possibly invalid)
  option. For example:

  utility_name -f option_argument

  Given

  utility_name -f -b

  Should:
    1) option_argument be "-b"?
    2) Complain that -b is an unknown option?
    3) Complain that -f requires an option but one was not given?

  We are going with (3) for efficiency reasons. Ie if _any_ option_description
  can successfully unpack "-b" then "-b" is considered an option and not an
  option_argument even if it is an invalid option
*/
template<typename CharT>
basic_variable_map<CharT>
parse_incremental_arguments(std::size_t _argc, const CharT *_argv[],
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm,
  const std::basic_string<CharT> &end_of_options)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_option_pack<CharT> option_pack_type;
  typedef basic_options_group<CharT> options_group_type;
  typedef basic_variable_map<CharT> variable_map_type;

  typedef std::vector<string_type> args_list;
  typedef std::vector<args_list> cmd_stack_type;

  variable_map_type _vm = vm;

  cmd_stack_type cmd_stack{
    args_list{
      std::reverse_iterator<const CharT **>(_argv+_argc),
      std::reverse_iterator<const CharT **>(_argv)
    }
  };

  std::size_t arg_count = 0;
  std::size_t operand_count = 0;
  std::size_t option_count = 0;

  string_type arg;
  string_type mapped_key;
  option_pack_type option_pack;

  typename options_group_type::const_iterator desc;

  int state = 0;
  while(!cmd_stack.empty()) {
    args_list &current_cmdlist = cmd_stack.back();
    if(current_cmdlist.empty()) {
      cmd_stack.pop_back();
      continue;
    }

    switch(state) {
      case 0: {
        // pull arg off cmdlist and determine if it is an option or an
        // operand
        if(cmd_stack.size() == 1 && current_cmdlist.back() == end_of_options) {
          current_cmdlist.pop_back();
          state = 4;
          break;
        }

        state = 3; // assume an operand

        for(desc = grp.begin(); desc != grp.end(); ++desc) {
          if(!desc->unpack_option)
            continue;

          option_pack = std::move(desc->unpack_option(current_cmdlist.back()));

          if(!option_pack.did_unpack)
            continue;

          if(desc->mapped_key) {
            bool handles_arg = false;
            std::tie(handles_arg,mapped_key) =
              desc->mapped_key(option_pack.raw_key,option_count,arg_count,_vm);
            if(handles_arg) {
              state = 1;
              break;
            }
          }
          else {
            mapped_key = option_pack.raw_key;
            state = 1;
            break;
          }

          state = 2; // is option but no handler
        }
      } break;

      case 1: {
        // option is found pull off and handle it
        arg = std::move(current_cmdlist.back());
        current_cmdlist.pop_back();

        if(!desc->make_value) {
          // strictly no values
          if(option_pack.value_provided)
            throw unexpected_argument_error(option_pack.raw_key,
              option_pack.value,option_count,arg_count);

          // handle the flag only
          _vm.emplace(mapped_key,any());
        }
        else {
          // required or optional value
          // is it embedded in the option pack?
          if(option_pack.value_provided) {
            // handle the provided value
            _vm.emplace(mapped_key,
              desc->make_value(mapped_key,option_pack.value,_vm));
          }
          else if(current_cmdlist.empty()) {
            // no more items in the current pack, use optional if available
            if(desc->implicit_value)
              _vm.emplace(mapped_key,desc->implicit_value(mapped_key,_vm));
            else
              throw missing_argument_error(arg,option_count,arg_count);
          }
          else {
            // try to use the next argument on the command list. If any
            // desc can unpack it, then it is a (possibly ill-formed)
            // option and not an argument
            typename options_group_type::const_iterator next;
            for(next = grp.begin(); next != grp.end(); ++next) {
              if(!next->unpack_option)
                continue;

              option_pack =
                std::move(next->unpack_option(current_cmdlist.back()));

              if(option_pack.did_unpack)
                break;
            }

            if(next != grp.end()) {
              if(desc->implicit_value)
                _vm.emplace(mapped_key,desc->implicit_value(mapped_key,_vm));
              else
                throw missing_argument_error(arg,option_count,arg_count);
            }
            else {
              _vm.emplace(mapped_key,
                desc->make_value(mapped_key,current_cmdlist.back(),_vm));
                current_cmdlist.pop_back();
            }
          }
        }

        // see if there were any packed options
        if(!option_pack.packed_arguments.empty()) {
          std::reverse(option_pack.packed_arguments.begin(),
            option_pack.packed_arguments.end());
          cmd_stack.emplace_back(std::move(option_pack.packed_arguments));
        }
        state = 0;
        ++option_count;
        ++arg_count;
      } break;

      case 2: {
        // was unpacked as an option but couldn't find desc to handle it
        throw unknown_option_error(arg,option_count,arg_count);
      } break;


      case 3:
        state = 0;
        // note the fall through so only execute case 4 once

      case 4: {
        // operand, if end_of_option is set, then we continually repeat here
        arg = std::move(current_cmdlist.back());
        current_cmdlist.pop_back();

        // always process as operand, try to find desc to handle it
        for(desc = grp.begin(); desc != grp.end(); ++desc) {
          if(desc->unpack_option)
            continue;

          std::pair<bool,string_type> operand_key{false,default_operand_key};

          if(desc->mapped_key) {
            operand_key =
              desc->mapped_key(default_operand_key,operand_count,arg_count,_vm);
            if(!operand_key.first)
              continue;
          }

          // handle this operand
          if(desc->make_value) {
            _vm.emplace(operand_key.second,
              desc->make_value(operand_key.second,arg,_vm));
          }
          else
            _vm.emplace(operand_key.second,any());

          ++operand_count;

          break;
        }

        if(desc == grp.end())
          throw unexpected_operand_error(arg,operand_count,arg_count);

        ++arg_count;
      } break;

      default:
        abort();
    }
  }

  return _vm;
}

template<typename CharT>
inline basic_variable_map<CharT>
parse_incremental_arguments(std::size_t _argc, const CharT *_argv[],
  const basic_options_group<CharT> &grp,
  const std::basic_string<CharT> &end_of_options)
{
  return parse_incremental_arguments(_argc,_argv,grp,
    basic_variable_map<CharT>(),end_of_options);
}

template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(std::size_t _argc, const CharT *_argv[],
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm,
  const std::basic_string<CharT> &end_of_options)
{
  basic_variable_map<CharT> _vm =
    std::move(parse_incremental_arguments(_argc,_argv,grp,vm,end_of_options));

  for(auto &desc : grp) {
    if(desc.finalize)
      desc.finalize(_vm);
  }

  return _vm;
}

template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(std::size_t _argc, const CharT *_argv[],
  const basic_options_group<CharT> &grp,
  const std::basic_string<CharT> &end_of_options)
{
  return parse_arguments(_argc,_argv,grp,basic_variable_map<CharT>(),
    end_of_options);
}



typedef basic_option_description<char> option_description;
typedef std::vector<basic_option_description<char> > options_group;
typedef std::multimap<std::basic_string<char>,any> variable_map;

typedef basic_option_description<wchar_t> woption_description;
typedef std::vector<basic_option_description<wchar_t> > woptions_group;
typedef std::multimap<std::basic_string<wchar_t>,any> wvariable_map;






/*
  EZ interface start
*/

/*
  Indicate that the operand or option has constraints with respect to
  the number, position, or exclusivity of other operands or options.

  Be overly descriptive. ie use
    basic_constraint<CharT>().occurrences(1,2).mutually_exclusive({"foo"}).
      mutually_inclusive({"bar"});
  rather than
    basic_constraint<CharT>{1,2,{"foo"},{"foo"}};

  ie is "foo" inclusive or exclusive? what if don't want to use range?
  therefore lots of constructors that someone needs to be read.

  Default indicates allow any number of occurrences, at any argument
  position both mutually exclusive and mutually inclusive of nothing.

*/
template<typename CharT>
struct basic_constraint {
  typedef std::basic_string<CharT> string_type;

  /*
    Constrain the number of occurrences to exactly \c n
  */
  basic_constraint<CharT> & occurrences(std::size_t n)
  {
    _min = _max = n;
    return *this;
  }

  /*
    Constrain the number of occurrences to between \c n and \c m

    assert \c must be smaller than \m
  */
  basic_constraint<CharT> & occurrences(std::size_t n, std::size_t m)
  {
    _min = n;
    _max = m;
    assert(_min <= _max);
    return *this;
  }

  basic_constraint<CharT> &
  mutually_exclusive(const std::vector<string_type> &mapped_key_vec)
  {
    _mutually_exclusive = mapped_key_vec;
    return *this;
  }

  basic_constraint<CharT> &
  mutually_inclusive(const std::vector<string_type> &mapped_key_vec)
  {
    _mutually_inclusive = mapped_key_vec;
    return *this;
  }

  /*
    Negative indicates no constraint
  */
  basic_constraint<CharT> &
  at_position(int n)
  {
    _position = n;
    return *this;
  }

  /*
    Negative indicates no constraint
  */
  basic_constraint<CharT> &
  at_argument(int n)
  {
    _argument = n;
    return *this;
  }

  int _position = -1;
  int _argument = -1;

  std::size_t _min = 0;
  std::size_t _max = std::numeric_limits<std::size_t>::max();
  std::vector<string_type> _mutually_exclusive;
  std::vector<string_type> _mutually_inclusive;
};

typedef basic_constraint<char> constrain;
typedef basic_constraint<wchar_t> wconstrain;



template<typename T>
struct value {
  typedef T value_type;


  value<T> & implicit(const T &val) {
    _implicit = std::make_shared<T>(val);

    return *this;
  }

  std::shared_ptr<value_type> _implicit;
};

template<typename CharT>
const std::basic_string<CharT> & default_long_prefix(void); // undefined

template<>
inline const std::basic_string<char> & default_long_prefix<char>(void)
{
  static const std::basic_string<char> val("--");

  return val;
}

template<>
inline const std::basic_string<wchar_t> & default_long_prefix<wchar_t>(void)
{
  static const std::basic_string<wchar_t> val(L"--");

  return val;
}

template<typename CharT>
const std::basic_string<CharT> & default_short_prefix(void); // undefined

template<>
inline const std::basic_string<char> & default_short_prefix<char>(void)
{
  static const std::basic_string<char> val("-");

  return val;
}

template<>
inline const std::basic_string<wchar_t> & default_short_prefix<wchar_t>(void)
{
  static const std::basic_string<wchar_t> val(L"-");

  return val;
}

template<typename CharT>
const std::basic_string<CharT> & default_accept_all(void); // undefined

template<>
inline const std::basic_string<char> & default_accept_all<char>(void)
{
  static const std::basic_string<char> val("*");

  return val;
}

template<>
inline const std::basic_string<wchar_t> & default_accept_all<wchar_t>(void)
{
  static const std::basic_string<wchar_t> val(L"*");

  return val;
}

template<typename CharT>
CharT default_option_delim(void); // undefined

template<>
inline char default_option_delim<char>(void)
{
  return ',';
}

template<>
inline wchar_t default_option_delim<wchar_t>(void)
{
  return L',';
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
typename std::enable_if<std::is_same<CharT, wchar_t>::value, std::wstring>::type
    to_xstring(const T &t)
{
    return std::to_wstring(t);
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
  CharT delim, basic_option_description<CharT> &desc, bool hidden)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map_type;

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) = detail::split(opt_spec,delim);

  /*
    If both are provided, the long option is the vm key

    If none are provided, then it is unmapped_key
  */
  string_type key_desc;
  if(opt_spec.empty() || (long_opt.empty() && short_opt.empty())) {
    if(!hidden) {
      desc.key_description = [=](void) {
        return default_long_prefix<CharT>() + default_accept_all<CharT>()
          + delim + default_short_prefix<CharT>() + default_accept_all<CharT>();
      };
    }
  }
  else if(!long_opt.empty() && !short_opt.empty()) {
    desc.mapped_key = [=](const string_type &_opt, std::size_t, std::size_t,
      const variable_map_type &)
      {
        if(_opt == long_opt || _opt == short_opt)
          return std::make_pair(true,long_opt);

        return std::make_pair(false,string_type());
      };

    if(!hidden) {
      desc.key_description = [=](void) {
        return default_long_prefix<CharT>() + long_opt + delim
          + default_short_prefix<CharT>() + short_opt;
      };
    }
  }
  else if(!long_opt.empty()) {
    desc.mapped_key = [=](const string_type &_opt, std::size_t, std::size_t,
      const variable_map_type &)
      {
        if(_opt == long_opt)
          return std::make_pair(true,_opt);

        return std::make_pair(false,string_type());
      };

    if(!hidden) {
      desc.key_description = [=](void) {
        return default_long_prefix<CharT>() + long_opt;
      };
    }
  }
  else {
    desc.mapped_key = [=](const string_type &_opt, std::size_t, std::size_t,
      const variable_map_type &)
      {
        if(_opt == short_opt)
          return std::make_pair(true,_opt);

        return std::make_pair(false,string_type());
      };

    if(!hidden) {
      desc.key_description = [=](void) {
        return default_short_prefix<CharT>() + short_opt;
      };
    }
  }

  return std::make_pair(long_opt,short_opt);
}


template <typename T, typename CharT>
inline typename std::enable_if<
  std::is_same<T,std::basic_string<CharT> >::value,any>::type
to_value(const std::basic_string<CharT> &,
  const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
{
    return any(val);
}

template <typename T, typename CharT>
inline typename std::enable_if<
  !std::is_same<T,std::basic_string<CharT> >::value,any>::type
to_value(const std::basic_string<CharT> &,
  const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
{
  T _val;
  std::basic_stringstream<CharT> in(val);
  in >> _val;
  return any(_val);
}

template<typename T, typename CharT>
inline void add_option_value(const value<T> &val,
  basic_option_description<CharT> &desc)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map_type;

  if(val._implicit) {
    desc.implicit_value = [=](const string_type &, const variable_map_type &)
    {
      return any(*(val._implicit));
    };

    desc.implicit_value_description = [=](void) {
      return to_xstring<CharT>(*(val._implicit));
    };
  }

  desc.make_value = [](const string_type &mapped_key, const string_type &value,
      const variable_map_type &vm)
  {
    return to_value<T>(mapped_key,value,vm);
  };
}

template<typename T, typename CharT>
inline void add_operand_value(const value<T> &val,
  basic_option_description<CharT> &desc)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map_type;

  if(val._implicit) {
    desc.make_value = [=](const string_type &, const string_type &,
      const variable_map_type &)
    {
      return any(*(val._implicit));
    };
  }
  else {
    desc.make_value = [](const string_type &mapped_key,
      const string_type &value, const variable_map_type &vm)
    {
      return to_value<T>(mapped_key,value,vm);
    };
  }
}

template<typename CharT>
inline void add_operand_key(const std::basic_string<CharT> &key, int posn,
  int argn, basic_option_description<CharT> &desc)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map_type;

  if(posn >= 0 || argn >= 0) {
    desc.mapped_key = [=](const string_type &, std::size_t _posn,
      std::size_t _argn, const variable_map_type &)
    {
      if((posn<0 || posn == _posn) && (argn<0 || argn == _argn))
        return std::make_pair(true,key);
      return std::make_pair(false,std::basic_string<CharT>());
    };
  }
  else {
    desc.mapped_key = [=](const string_type &, std::size_t, std::size_t,
      const variable_map_type &)
    {
      return std::make_pair(true,key);
    };
  }
}

/*
  Used for constraints on options.
*/
template<typename CharT>
void add_option_constraints(const basic_constraint<CharT> &cnts,
  basic_option_description<CharT> &desc,
  const std::basic_string<CharT> &mapped_key)
{
  desc.finalize = [=](const basic_variable_map<CharT> &vm) {

    std::size_t occurrances = vm.count(mapped_key);
    if(occurrances > cnts._max || occurrances < cnts._min) {
      throw occurrence_error(mapped_key,cnts._min,cnts._max,
        occurrances);
    }

    for(auto &exclusive_key : cnts._mutually_exclusive) {
      if(vm.count(exclusive_key) != 0)
        throw mutually_exclusive_error(mapped_key,exclusive_key);
    }

    for(auto &inclusive_key : cnts._mutually_inclusive) {
      if(vm.count(inclusive_key) == 0)
        throw mutually_inclusive_error(mapped_key,inclusive_key);
    }
  };
}

/*
  Used for constraints on operands.
*/
template<typename CharT>
void add_operand_constraints(const basic_constraint<CharT> &cnts,
  const std::basic_string<CharT> &mapped_key,
  basic_option_description<CharT> &desc)
{
  desc.finalize = [=](const basic_variable_map<CharT> &vm) {
    std::size_t occurrances = vm.count(mapped_key);
    if(occurrances < cnts._min || occurrances > cnts._max) {
        throw occurrence_error(mapped_key,cnts._min,cnts._max,
          occurrances);
    }

    for(auto &exclusive_key : cnts._mutually_exclusive) {
      if(vm.count(exclusive_key) != 0)
        throw mutually_exclusive_error(mapped_key,exclusive_key);
    }

    for(auto &inclusive_key : cnts._mutually_inclusive) {
      if(vm.count(inclusive_key) == 0)
        throw mutually_inclusive_error(mapped_key,inclusive_key);
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
template<typename CharT>
inline basic_option_description<CharT>
make_option(const std::basic_string<CharT> &opt_spec,
  const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  basic_option_description<CharT> desc{unpack_gnu<true,CharT>};

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) =
    detail::add_option_spec(opt_spec,delim,desc,false);

  detail::add_option_constraints(cnts,desc,long_opt);

  return desc;
}

template<typename CharT>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec, const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  return make_option(std::basic_string<CharT>(opt_spec),
    std::basic_string<CharT>(extended_desc),cnts,delim);
}

template<typename CharT>
inline basic_option_description<CharT>
make_option(const std::basic_string<CharT> &opt_spec,
  const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  return make_option(opt_spec,std::basic_string<CharT>(extended_desc),
    cnts,delim);
}

template<typename CharT>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec,
  const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  return make_option(std::basic_string<CharT>(opt_spec),extended_desc,
    cnts,delim);
}


/*
  Cases line 3, 5
*/
template<typename CharT>
inline basic_option_description<CharT>
make_hidden_option(const std::basic_string<CharT> &opt_spec,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  basic_option_description<CharT> desc{unpack_gnu<true,CharT>};

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) =
    detail::add_option_spec(opt_spec,delim,desc,true);

  detail::add_option_constraints(cnts,desc,long_opt);

  return desc;
}

template<typename CharT>
inline basic_option_description<CharT>
make_hidden_option(const CharT *opt_spec,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  return make_hidden_option(std::basic_string<CharT>(opt_spec),cnts,delim);
}


/*
  Case line 6, 8, 10, 12
*/
template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const std::basic_string<CharT> &opt_spec,
  const value<T> &val, const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  basic_option_description<CharT> desc{unpack_gnu<false,CharT>};

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) =
    detail::add_option_spec(opt_spec,delim,desc,false);

  detail::add_option_value(val,desc);

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_option_constraints(cnts,desc,long_opt);

  return desc;
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec,
  const value<T> &val, const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  return make_option(std::basic_string<CharT>(opt_spec),val,
    std::basic_string<CharT>(extended_desc),cnts,delim);
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const std::basic_string<CharT> &opt_spec,
  const value<T> &val, const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  return make_option(opt_spec,val,std::basic_string<CharT>(extended_desc),
    cnts,delim);
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec,
  const value<T> &val, const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  return make_option(std::basic_string<CharT>(opt_spec),val,extended_desc,
    cnts,delim);
}

/*
  Case line 7, 9, 11, 13
*/
template<typename CharT, typename T>
inline basic_option_description<CharT>
make_hidden_option(const std::basic_string<CharT> &opt_spec,
  const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  typedef std::basic_string<CharT> string_type;

  basic_option_description<CharT> desc{unpack_gnu<false,CharT>};

  string_type long_opt;
  string_type short_opt;
  std::tie(long_opt,short_opt) =
    detail::add_option_spec(opt_spec,delim,desc,true);

  detail::add_option_value(val,desc);

  detail::add_option_constraints(cnts,desc,long_opt);

  return desc;
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_hidden_option(const CharT *opt_spec, const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = default_option_delim<CharT>())
{
  return make_hidden_option(std::basic_string<CharT>(opt_spec),val,cnts,delim);
}

/*
  The \c make_operand and \c make_hidden_operand EZ interface do not map
  directly to case lines but the same effect can be achieved.

  In cases 14-17, an operand value is translated into an \c any of some
  type T. In the EZ interface this is accomplished by the value<T>
  parameter as long as the given operand string value can be translated
  into an instance of type T.

  Suppose however that no translation is needed and the provided string
  value is ignored and the vm is populated with the same instance of T
  always. This is achieved by providing an \c implicit value to \c
  value<T>. That is, the given operand's string value is ignored and the
  \c any value is always set to \c value<T>.implicit().

  In cases 18-21, an operand's given string value is always ignored and
  a default constructed \c any is the value of the \c mapped key.
*/

/*
  Case line 14
*/
template<typename CharT, typename T>
inline basic_option_description<CharT>
make_operand(const std::basic_string<CharT> &extended_desc, const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<CharT> desc;

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_operand_value(val,desc);

  detail::add_operand_key(default_operand_key,cnts._position,
    cnts._argument,desc);

  detail::add_operand_constraints(cnts,default_operand_key,desc);

  return desc;
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_operand(const CharT *extended_desc, const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(std::basic_string<CharT>(extended_desc),val,cnts);

}

/*
  Case line 15
*/
template<typename CharT, typename T>
inline basic_option_description<CharT>
make_hidden_operand(const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<CharT> desc;

  detail::add_operand_value(val,desc);

  detail::add_operand_key(default_operand_key,cnts._position,
    cnts._argument,desc);

  detail::add_operand_constraints(cnts,default_operand_key,desc);

  return desc;
}

/*
  Case line 16
*/

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_operand(const std::basic_string<CharT> &mapped_key,
  const std::basic_string<CharT> &extended_desc, const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<char> desc;

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_option_value(val,desc);

  detail::add_operand_key(mapped_key,cnts._position,cnts._argument,desc);

  detail::add_operand_constraints(cnts,mapped_key,desc);

  return desc;
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_operand(const CharT *mapped_key, const CharT *extended_desc,
  const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(std::basic_string<CharT>(mapped_key),
    std::basic_string<CharT>(extended_desc),val,cnts);
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_operand(const CharT *mapped_key,
  const std::basic_string<CharT> &extended_desc, const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(std::basic_string<CharT>(mapped_key),
    extended_desc,val,cnts);
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_operand(const std::basic_string<CharT> &mapped_key,
  const CharT *extended_desc, const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(mapped_key,std::basic_string<CharT>(extended_desc),
    val,cnts);
}


/*
  Case line 17
*/
template<typename CharT, typename T>
inline basic_option_description<CharT>
make_hidden_operand(const std::basic_string<CharT> &mapped_key,
  const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<char> desc;

  detail::add_option_value(val,desc);

  detail::add_operand_key(mapped_key,cnts._position,cnts._argument,desc);

  detail::add_operand_constraints(cnts,mapped_key,desc);

  return desc;
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_hidden_operand(const CharT *mapped_key, const value<T> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_hidden_operand(std::basic_string<CharT>(mapped_key),val,cnts);
}

/*
  Case line 18
*/
template<typename CharT>
inline basic_option_description<CharT>
make_operand(const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<CharT> desc;

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_operand_key(default_operand_key,cnts._position,
    cnts._argument,desc);

  detail::add_operand_constraints(cnts,default_operand_key,desc);

  return desc;
}

template<typename CharT>
inline basic_option_description<CharT>
make_operand(const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(std::basic_string<CharT>(extended_desc),cnts);
}

/*
  Case line 19
*/
template<typename CharT>
inline basic_option_description<CharT>
make_hidden_operand(
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<CharT> desc;

  detail::add_operand_key(default_operand_key,cnts._position,
    cnts._argument,desc);

  detail::add_operand_constraints(cnts,default_operand_key,desc);

  return desc;
}

/*
  Case line 20
*/
template<typename CharT>
inline basic_option_description<CharT>
make_operand(const std::basic_string<CharT> &mapped_key,
  const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<char>())
{
  basic_option_description<CharT> desc;

  desc.extended_description = [=](void) { return extended_desc; };

  detail::add_operand_key(mapped_key,cnts._position,cnts._argument,desc);

  detail::add_operand_constraints(cnts,mapped_key,desc);

  return desc;
}

template<typename CharT>
inline basic_option_description<CharT>
make_operand(const CharT *mapped_key, const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<char>())
{
  return make_operand(std::basic_string<CharT>(mapped_key),
    std::basic_string<CharT>(extended_desc),cnts);
}

template<typename CharT>
inline basic_option_description<CharT>
make_operand(const std::basic_string<CharT> &mapped_key,
  const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<char>())
{
  return make_operand(mapped_key,std::basic_string<CharT>(extended_desc),cnts);
}

template<typename CharT>
inline basic_option_description<CharT>
make_operand(const CharT *mapped_key,
  const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<char>())
{
  return make_operand(std::basic_string<CharT>(mapped_key),extended_desc,cnts);
}

/*
  Case line 21
*/
template<typename CharT>
inline basic_option_description<CharT>
make_hidden_operand(const std::basic_string<CharT> &mapped_key,
  const basic_constraint<CharT> &cnts = basic_constraint<char>())
{
  basic_option_description<CharT> desc;

  detail::add_operand_key(mapped_key,cnts._position,cnts._argument,desc);

  detail::add_operand_constraints(cnts,mapped_key,desc);

  return desc;
}

template<typename CharT>
inline basic_option_description<CharT>
make_hidden_operand(const CharT *mapped_key,
  const basic_constraint<CharT> &cnts = basic_constraint<char>())
{
  return make_hidden_operand(std::basic_string<CharT>(mapped_key),cnts);
}

/*
  Convenience function to intercept options to make them errors. This is
  only needed in the case where there are only operands and
  you don't want to accept anything that looks like a option.

  For example, If there are _any_ descriptions that have uppack_option
  set and the option is unrecognized, then parsing will complain. For
  example, on my machine, the command 'echo' takes one option, '-n'
  (nonconforming to POSIX actually). If the command 'echo -f' is given,
  the string '-f' is printed meaning that '-f' is interpreted as a
  operand and not an invalid option. If when using this
  library you would rather an error be generated that states '-f' was
  unrecognized, use the \c make_options_error function and do not
  include any other non-operand descriptions.
*/
template<typename CharT>
inline basic_option_description<CharT> make_options_error(void)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map_type;

  return basic_option_description<CharT>{unpack_gnu<false,CharT>,
    [](const string_type &raw_key,int,int,const variable_map_type &) {
      return std::make_pair(false,string_type());
    }};
}








/*
  Convenience specializations of parse_incremental_arguments for appropriate
  character classes
*/
inline basic_variable_map<char>
parse_incremental_arguments(std::size_t _argc, const char *_argv[],
  const basic_options_group<char> &grp, const basic_variable_map<char> &vm)
{
  return parse_incremental_arguments(_argc,_argv,grp,vm,
    std::basic_string<char>("--"));
}

inline basic_variable_map<char>
parse_incremental_arguments(std::size_t _argc, const char *_argv[],
  const basic_options_group<char> &grp)
{
  return parse_incremental_arguments(_argc,_argv,grp,
    basic_variable_map<char>(),std::basic_string<char>("--"));
}

inline basic_variable_map<char>
parse_arguments(std::size_t _argc, const char *_argv[],
  const basic_options_group<char> &grp, const basic_variable_map<char> &vm)
{
  return parse_incremental_arguments(_argc,_argv,grp,vm,
    std::basic_string<char>("--"));
}

inline basic_variable_map<char>
parse_arguments(std::size_t _argc, const char *_argv[],
  const basic_options_group<char> &grp)
{
  return parse_arguments(_argc,_argv,grp,
    basic_variable_map<char>(),std::basic_string<char>("--"));
}








#if 0
/*
  Convenience default parser

  Parse the arguments contained in \c argv with size \c argc. Options
  are returned in a new variable map. Long options are prefaced with a
  '--' and short options (single character) are prefaced with a '-'. In
  each case, the option is stored as a key in the variable_map_type. If
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

  option_desc.unpack_option = unpack_gnu<CharT,false>;

  basic_options_group<CharT> grp{
    option_desc
  };

  return parse_arguments(argv,argc,endc,grp,basic_variable_map<CharT>(),false);
}

#endif


























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



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
#include <tuple>
#include <utility>
#include <vector>
#include <string>
#include <cassert>
#include <codecvt>
#include <locale>
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
    unknown_option_error(std::size_t posn, std::size_t argn)
      :parse_error("unknown_option_error",posn,argn) {}
};

class missing_argument_error : public parse_error {
  public:
    missing_argument_error(std::size_t posn, std::size_t argn)
      :parse_error("missing_argument_error",posn,argn) {}
};

class unexpected_argument_error : public parse_error {
  public:
    unexpected_argument_error(std::size_t posn, std::size_t argn)
        :parse_error("unexpected_argument_error",posn,argn) {}
};

class unexpected_operand_error : public parse_error {
  public:
    unexpected_operand_error(std::size_t posn, std::size_t argn)
        :parse_error("unexpected_operand_error",posn,argn) {}
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


/*
  Convenience function to translate CharT -> char for use in formatting
  exception messages. It is assumed that chars, char16, char32 are
  UTF-encoded and wchar_t has fixed width encoding (possibly UCS2 or
  UTF32 depending on platform.
*/
inline std::string asUTF8(const std::basic_string<char> &str)
{
  return str;
}

/*
  CharT either char16_t, char32_t, or wchar_t
*/
inline std::string asUTF8(const std::basic_string<char16_t> &str)
{
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(str);
}

inline std::string asUTF8(const std::basic_string<char32_t> &str)
{
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
  return convert.to_bytes(str);
}

inline std::string asUTF8(const std::basic_string<wchar_t> &str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t> > convert;
  return convert.to_bytes(str);
}





template<typename CharT>
inline const std::basic_string<CharT> & default_operand_key(void)
{
  static const auto cvt = [](const char *str) -> std::basic_string<CharT> {
    std::wstring_convert<std::codecvt_utf8_utf16<CharT> > convert;
    return convert.from_bytes(str);
  };

  static const std::basic_string<CharT> str =
    cvt(CMD_OPTIONS_DEFAULT_OPERAND_KEY_STR);

  return str;
}

template<>
inline const std::basic_string<char> & default_operand_key<char>(void)
{
  static const std::string str(CMD_OPTIONS_DEFAULT_OPERAND_KEY_STR);

  return str;
}












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

#if 0
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
#endif

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

    If \c key_description is not set, then the option is considered to
    be hidden.
  */
  std::function<string_type(void)> key_description;

  /*
    Return a human-readable concise description of this option's
    functionality.
  */
  std::function<string_type(void)> extended_description;

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

  // use this form to automatically convert single byte ASCII (UTF) encoding
  // to wider equivalent. Works because each is part of the basic source
  // character set.
  static const string_type sprefix{'-'};

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

  // use this form to automatically convert single byte ASCII (UTF) encoding
  // to wider equivalent. Works because each is part of the basic source
  // character set.
  static const string_type lprefix{'-','-'};
  static const string_type assignment{'='};

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
            throw unexpected_argument_error(option_count,arg_count);

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
              throw missing_argument_error(option_count,arg_count);
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
                throw missing_argument_error(option_count,arg_count);
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
        throw unknown_option_error(option_count,arg_count);
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

          std::pair<bool,string_type> operand_key{false,
            default_operand_key<CharT>()};

          if(desc->mapped_key) {
            operand_key =
              desc->mapped_key(default_operand_key<CharT>(),operand_count,
                arg_count,_vm);
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
          throw unexpected_operand_error(operand_count,arg_count);

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

/*
  Convenience specializations of parse_incremental_arguments
*/
template<typename CharT>
inline basic_variable_map<CharT>
parse_incremental_arguments(std::size_t _argc, const CharT *_argv[],
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm)
{
  return parse_incremental_arguments(_argc,_argv,grp,vm,
    std::basic_string<CharT>{'-','-'});
}

template<typename CharT>
inline basic_variable_map<CharT>
parse_incremental_arguments(std::size_t _argc, const CharT *_argv[],
  const basic_options_group<CharT> &grp)
{
  return parse_incremental_arguments(_argc,_argv,grp,
    basic_variable_map<CharT>(),std::basic_string<CharT>{'-','-'});
}

template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(std::size_t _argc, const CharT *_argv[],
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm)
{
  return parse_incremental_arguments(_argc,_argv,grp,vm,
    std::basic_string<CharT>{'-','-'});
}

template<typename CharT>
inline basic_variable_map<CharT>
parse_arguments(std::size_t _argc, const CharT *_argv[],
  const basic_options_group<CharT> &grp)
{
  return parse_arguments(_argc,_argv,grp,
    basic_variable_map<CharT>(),std::basic_string<CharT>{'-','-'});
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
  using namespace std;

  return to_string(t);
}


template <typename CharT, typename T>
inline
typename std::enable_if<std::is_same<CharT, wchar_t>::value, std::wstring>::type
    to_xstring(const T &t)
{
  using namespace std;

  return to_wstring(t);
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
        return string_type{'-','-','*',',','-'};
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
        return string_type{'-','-'} + long_opt + delim
          + string_type{'-'} + short_opt;
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
        return string_type{'-','-'} + long_opt;
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
        return string_type{'-'} + short_opt;
      };
    }
  }

  return std::make_pair(long_opt,short_opt);
}




template<typename T>
struct convert {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    typedef std::basic_istringstream<CharT> istream_type;
    T _val;
    istream_type in(val);
    in >> _val;
    if(in.peek() != istream_type::traits_type::eof())
      throw std::invalid_argument(asUTF8(val));

    return any(_val);
  }
};

template<typename CharT>
struct convert<std::basic_string<CharT> > {
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    return any(val);
  }
};

template<>
struct convert<bool> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    bool _val;
    std::basic_istringstream<CharT> in(val);
    if(!(in >> std::noboolalpha >> _val)) {
      in.clear();
      if(!(in >> std::boolalpha >> _val))
        throw std::invalid_argument(asUTF8(val));
    }

    return any(static_cast<bool>(_val));
  }
};

template<>
struct convert<char> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    if(val.size() != 1)
      throw std::invalid_argument(asUTF8(val));

    int _i = val.front();
    if(_i < std::numeric_limits<char>::min()
      || _i > std::numeric_limits<char>::max())
    {
      throw std::out_of_range(asUTF8(val));
    }

    return any(static_cast<char>(_i));
  }
};

template<>
struct convert<signed char> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    if(val.size() != 1)
      throw std::invalid_argument(asUTF8(val));

    int _i = val.front();
    if(_i < std::numeric_limits<signed char>::min()
      || _i > std::numeric_limits<signed char>::max())
    {
      throw std::out_of_range(asUTF8(val));
    }

    return any(static_cast<signed char>(_i));
  }
};

template<>
struct convert<wchar_t> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    if(val.size() != 1)
      throw std::invalid_argument(asUTF8(val));

    int _i = val.front();
    if(_i < std::numeric_limits<wchar_t>::min()
      || _i > std::numeric_limits<wchar_t>::max())
    {
      throw std::out_of_range(asUTF8(val));
    }

    return any(static_cast<wchar_t>(_i));
  }
};

template<>
struct convert<short> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    int _i = std::stoi(val);
    if(_i < std::numeric_limits<short>::min()
      || _i > std::numeric_limits<short>::max())
    {
      throw std::out_of_range(asUTF8(val));
    }

    return any(static_cast<short>(_i));
  }
};

template<>
struct convert<int> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &key,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &vm)
  {
    return any(std::stoi(val));
  }
};

template<>
struct convert<long> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &key,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &vm)
  {
    return any(std::stol(val));
  }
};

template<>
struct convert<long long> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    return any(std::stoll(val));
  }
};

template<>
struct convert<unsigned char> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    if(val.size() != 1)
      throw std::invalid_argument(asUTF8(val));

    int _i = val.front();
    if(_i < std::numeric_limits<unsigned char>::min()
      || _i > std::numeric_limits<unsigned char>::max())
    {
      throw std::out_of_range(asUTF8(val));
    }

    return any(static_cast<unsigned char>(_i));
  }
};

template<>
struct convert<unsigned short> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &)
  {
    unsigned long _i = std::stoul(val);
    if(_i > std::numeric_limits<unsigned short>::max())
      throw std::out_of_range(asUTF8(val));

    return any(static_cast<unsigned short>(_i));
  }
};

template<>
struct convert<unsigned int> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &key,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &vm)
  {
    // unsigned long may be larger than unsigned int
    unsigned long _i = std::stoul(val);
    if(_i > std::numeric_limits<unsigned int>::max())
      throw std::out_of_range(asUTF8(val));

    return any(static_cast<unsigned int>(_i));
  }
};

template<>
struct convert<unsigned long> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &key,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &vm)
  {
    return any(std::stoul(val));
  }
};

template<>
struct convert<unsigned long long> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &key,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &vm)
  {
    return any(std::stoull(val));
  }
};

template<>
struct convert<float> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &key,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &vm)
  {
    return any(std::stof(val));
  }
};

template<>
struct convert<double> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &key,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &vm)
  {
    return any(std::stod(val));
  }
};

template<>
struct convert<long double> {
  template<typename CharT>
  static any from_value(const std::basic_string<CharT> &key,
    const std::basic_string<CharT> &val, const basic_variable_map<CharT> &vm)
  {
    return any(std::stold(val));
  }
};



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
    //return to_value<T>(mapped_key,value,vm);
    return convert<T>::from_value(mapped_key,value,vm);
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
      //return to_value<T>(mapped_key,value,vm);
      return convert<T>::from_value(mapped_key,value,vm);
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
      throw occurrence_error(asUTF8(mapped_key),cnts._min,cnts._max,
        occurrances);
    }

    for(auto &exclusive_key : cnts._mutually_exclusive) {
      if(vm.count(exclusive_key) != 0)
        throw mutually_exclusive_error(asUTF8(mapped_key),
          asUTF8(exclusive_key));
    }

    for(auto &inclusive_key : cnts._mutually_inclusive) {
      if(vm.count(inclusive_key) == 0)
        throw mutually_inclusive_error(asUTF8(mapped_key),
          asUTF8(inclusive_key));
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
        throw occurrence_error(asUTF8(mapped_key),cnts._min,cnts._max,
          occurrances);
    }

    for(auto &exclusive_key : cnts._mutually_exclusive) {
      if(vm.count(exclusive_key) != 0)
        throw mutually_exclusive_error(asUTF8(mapped_key),
          asUTF8(exclusive_key));
    }

    for(auto &inclusive_key : cnts._mutually_inclusive) {
      if(vm.count(inclusive_key) == 0)
        throw mutually_inclusive_error(asUTF8(mapped_key),
          asUTF8(inclusive_key));
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
  CharT delim = CharT{','})
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
  CharT delim = CharT{','})
{
  return make_option(std::basic_string<CharT>(opt_spec),
    std::basic_string<CharT>(extended_desc),cnts,delim);
}

template<typename CharT>
inline basic_option_description<CharT>
make_option(const std::basic_string<CharT> &opt_spec,
  const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  return make_option(opt_spec,std::basic_string<CharT>(extended_desc),
    cnts,delim);
}

template<typename CharT>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec,
  const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
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
  CharT delim = CharT{','})
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
  CharT delim = CharT{','})
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
  CharT delim = CharT{','})
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
  CharT delim = CharT{','})
{
  return make_option(std::basic_string<CharT>(opt_spec),val,
    std::basic_string<CharT>(extended_desc),cnts,delim);
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const std::basic_string<CharT> &opt_spec,
  const value<T> &val, const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  return make_option(opt_spec,val,std::basic_string<CharT>(extended_desc),
    cnts,delim);
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec,
  const value<T> &val, const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
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
  CharT delim = CharT{','})
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
  CharT delim = CharT{','})
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

  detail::add_operand_key(default_operand_key<CharT>(),cnts._position,
    cnts._argument,desc);

  detail::add_operand_constraints(cnts,default_operand_key<CharT>(),desc);

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

  detail::add_operand_key(default_operand_key<CharT>(),cnts._position,
    cnts._argument,desc);

  detail::add_operand_constraints(cnts,default_operand_key<CharT>(),desc);

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
  basic_option_description<CharT> desc;

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
  basic_option_description<CharT> desc;

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

  detail::add_operand_key(default_operand_key<CharT>(),cnts._position,
    cnts._argument,desc);

  detail::add_operand_constraints(cnts,default_operand_key<CharT>(),desc);

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

  detail::add_operand_key(default_operand_key<CharT>(),cnts._position,
    cnts._argument,desc);

  detail::add_operand_constraints(cnts,default_operand_key<CharT>(),desc);

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
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(std::basic_string<CharT>(mapped_key),
    std::basic_string<CharT>(extended_desc),cnts);
}

template<typename CharT>
inline basic_option_description<CharT>
make_operand(const std::basic_string<CharT> &mapped_key,
  const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(mapped_key,std::basic_string<CharT>(extended_desc),cnts);
}

template<typename CharT>
inline basic_option_description<CharT>
make_operand(const CharT *mapped_key,
  const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(std::basic_string<CharT>(mapped_key),extended_desc,cnts);
}

/*
  Case line 21
*/
template<typename CharT>
inline basic_option_description<CharT>
make_hidden_operand(const std::basic_string<CharT> &mapped_key,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<CharT> desc;

  detail::add_operand_key(mapped_key,cnts._position,cnts._argument,desc);

  detail::add_operand_constraints(cnts,mapped_key,desc);

  return desc;
}

template<typename CharT>
inline basic_option_description<CharT>
make_hidden_operand(const CharT *mapped_key,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
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

namespace detail {

template<typename CharT>
inline bool is_C_space(CharT c)
{
  static const std::array<CharT,6> C_space{{0x20,0x0c,0x0a,0x0d,0x09,0x0b}};

  return (std::find(C_space.begin(),C_space.end(),c) != C_space.end());
}


template<typename CharT>
std::basic_string<CharT>
wrap(const std::basic_string<CharT> &text, std::size_t max_width)
{
  typedef std::basic_string<CharT> string_type;

  string_type wrapped;
  string_type word;

  std::size_t width = 0;
  bool ignore_ws = false;
  auto cur = text.begin();
  while(cur != text.end()) {
    if(ignore_ws) {
      // eat all whitespace until find non-whitespace or newline
      while(cur != text.end() && is_C_space(*cur) && *cur != '\n')
        ++cur;

      if(*cur == '\n') {
        wrapped += *cur++;
        width = 0;
        ignore_ws = false;
        continue;
      }

      // read complete word
      while(cur != text.end() && !is_C_space(*cur))
        word += *cur++;

      if(width + word.size() + 1 > max_width) {
        wrapped += '\n';
        width = 0;
      }
      else if(width != 0) {
        wrapped += ' ';
        ++width;
      }
      wrapped += word;
      width += word.size();
      word.clear();

      if(*cur == '\n') {
        wrapped += *cur++;
        width = 0;
        ignore_ws = false;
      }
    }
    else {
      // do not ignore whitespace
      while(cur != text.end() && is_C_space(*cur)) {
        if(width+1 > max_width) {
          wrapped += '\n';
          width = 0;
        }
        wrapped += *cur++;
        ++width;
      }

      if(cur != text.end())
        ignore_ws = true;
    }
  }

  return wrapped;
}

}

/*
  Format an option description into a basic_string<CharT>

  \c typeset_option Calls do_typeset_option(desc). Return a string
  containing the fully formatted option or the empty string if the
  option should be hidden.

  \c compare [OPTIONAL] Calls do_compare(). May return an empty
  compare_type to indicate sorting should not be performed
*/
template<typename CharT>
class basic_option_formatter {
  public:
    typedef std::basic_string<CharT> string_type;
    typedef basic_option_description<CharT> description_type;

    typedef std::function<bool(const description_type &,
      const description_type &)> compare_type;

    virtual ~basic_option_formatter(void) {}

    string_type typeset_option(const description_type &desc) const {
      return do_typeset_option(desc);
    }

    compare_type compare(void) const {
      return do_compare();
    }

  protected:
    virtual string_type
      do_typeset_option(const description_type &desc) const = 0;

    virtual compare_type do_compare(void) const = 0;
};

/*
  Default option formatter

  Formats the options wrapped to \c max_width() (default 72 characters)
  into two columns (default 30 characters). If the first column is too
  wide, then the second column is started on a new line.

  First form:

  key_description [column_pad] extended_description

  Second Form:

  key_desciption [newline]
  [key_column_width] [column_pad] extended_description [newline]

  Sorts the option_descriptions by key_description() if both are
  present. If only rhs.key_description is present, returns true,
  otherwise returns false.

  key_description is one of the following forms (if each is present):
  --long_name,-short_name _arg_
  --long_name,-short_name [=_arg_<implicit>]

  Where _arg_ is the argument string provided in the constructor
  (default CharT "arg"). To use a different value, provide it in the
  constructor according to type CharT
*/
template<typename CharT>
class basic_default_formatter : public basic_option_formatter<CharT> {
  public:
    basic_default_formatter(void) :_arg{'a','r','g'} {}
    template<typename ForwardIterator>
    basic_default_formatter(ForwardIterator first, ForwardIterator last)
      :_arg(first,last) {}
    basic_default_formatter(const CharT *arg) :_arg(arg) {}

  protected:
    using typename basic_option_formatter<CharT>::string_type;
    using typename basic_option_formatter<CharT>::description_type;
    using typename basic_option_formatter<CharT>::compare_type;

    string_type do_typeset_option(const description_type &desc) const;

    compare_type do_compare(void) const {
      return [](const description_type &lhs, const description_type &rhs)
        {
          if(!rhs.key_description)
            return true;

          if(!lhs.key_description)
            return false;

          return lhs.key_description() < rhs.key_description();
        };
    }

    virtual std::size_t key_column_width(void) const {
      return 24;
    }

    virtual std::size_t column_pad(void) const {
      return 2;
    }

    virtual std::size_t max_width(void) const {
      return 72;
    }

    string_type arg_str(void) const {
      return _arg;
    }

  private:
    string_type _arg;
};

struct u32string_identity_cvt {
  const std::u32string & to_bytes(const std::u32string &str) const {
    return str;
  }

  const std::u32string & from_bytes(const std::u32string &str) const {
    return str;
  }
};


/*
  Ensure we are always operating on code points and not the underlying
  char_type.

  Helper traits to transform (if necessary) a CharT string into a code
  point string. That is, if CharT string is a multibyte string,
  transform it to a string where each element is a code point. Since
  each element in a UTF32 string is a single unicode code point, then
  convert the UTF{8,16} strings into UTF32. Since wchar_t is platform-
  and local-dependent, then assume it represents a single code point and
  ensure documentation states this. Specifically, wchar_t is NOT UTF16.
*/
template<typename CharT>
struct code_point_traits;

// UTF8
template<>
struct code_point_traits<char> {
  typedef char char_type;
  typedef char32_t code_point;

  static std::basic_string<code_point>
  convert_from(const std::basic_string<char_type> &str)
  {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    return cvt.from_bytes(str);
  }

  static std::basic_string<char_type>
  convert_to(const std::basic_string<code_point> &str)
  {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    return cvt.to_bytes(str);
  }
};

// UTF16
template<>
struct code_point_traits<char16_t> {
  typedef char16_t char_type;
  typedef char32_t code_point;

  // Run conversion through UTF8. Appears like it is possible to just
  // use std::codecvt_utf16 but not clear that this works with
  // std::wstring_convert and generally poorly documented. May need to
  // revisit if there is an unreasonable performance impact
  static std::basic_string<code_point>
  convert_from(const std::basic_string<char_type> &str)
  {
    std::wstring_convert<std::codecvt_utf8_utf16<char_type>, char_type> cvt16_8;
    std::string utf8 = cvt16_8.to_bytes(str);

    std::wstring_convert<std::codecvt_utf8<code_point>, code_point> cvt8_32;
    return cvt8_32.from_bytes(utf8);
  }

  static std::basic_string<char_type>
  convert_to(const std::basic_string<code_point> &str)
  {
    std::wstring_convert<std::codecvt_utf8<code_point>, code_point> cvt32_8;
    std::string utf8 = cvt32_8.to_bytes(str);

    std::wstring_convert<std::codecvt_utf8_utf16<char_type>, char_type> cvt8_16;
    return cvt8_16.from_bytes(utf8);
  }
};

// UTF32
template<>
struct code_point_traits<char32_t> {
  typedef char32_t char_type;
  typedef char32_t code_point;

  static std::basic_string<code_point>
  convert_from(const std::basic_string<char_type> &str)
  {
    return str;
  }

  static std::basic_string<char_type>
  convert_to(const std::basic_string<code_point> &str)
  {
    return str;
  }
};

// wchar_t
template<>
struct code_point_traits<wchar_t> {
  typedef wchar_t char_type;
  typedef wchar_t code_point;

  static std::basic_string<code_point>
  convert_from(const std::basic_string<char_type> &str)
  {
    return str;
  }

  static std::basic_string<char_type>
  convert_to(const std::basic_string<code_point> &str)
  {
    return str;
  }
};


/*
  Default formatter provides a comparison function that sorts the
  entries by long names (mapped_keys) and typsets the option description
  into two columns. The first is the summary of the option in one of the
  following forms:
    --foo,f arg
    --foo,f [arg=<implicit>]

  The second column containing the option description starts of the same
  line as the option summary if it fits or on the next line if it does
  not.

  Indent is the amount of space from the left edge that the text will
  start to be drawn. That is, in two column format, the column width is
  (width-indent) whereas in a single column (or paragraph-mode), the
  first line starts at indent and is (width-indent) wide.

  All text processing is done as UTF32 by converting the given CharT for
  wrapping and indenting and then converting back again. Although this
  adds a conversion and copy step, it is cleaner than multiple
  implementations that handle the different UTF{8,16,32} and wchar_t
  possibilities.
*/
template<typename CharT>
typename basic_default_formatter<CharT>::string_type
basic_default_formatter<CharT>::
  do_typeset_option(const description_type &desc) const
{
  typedef code_point_traits<CharT> cpoint_traits;
  typedef typename cpoint_traits::code_point code_point;
  typedef std::basic_string<typename cpoint_traits::code_point>
        cpstring_type;

  if(!desc.key_description)
    return string_type();

  string_type key_col = desc.key_description();

  if(desc.make_value) {
    if(desc.implicit_value && desc.implicit_value_description) {
      key_col.append({' ','['});
      key_col.append(arg_str());
      key_col.append({'=','<'});
      key_col.append(desc.implicit_value_description());
      key_col.append({'>',']'});
    }
    else {
      key_col.push_back(' ');
      key_col.append(arg_str());
    }

    if(key_col.size() > key_column_width()) {
      key_col.push_back('\n');
      key_col.append(key_column_width()+column_pad(),' ');
    }
    else {
      std::size_t per_pad = key_column_width()+column_pad()-key_col.size();
      key_col.append(per_pad,' ');
    }

    std::size_t indent = key_column_width()+column_pad();
    string_type extended_col;
    if(desc.extended_description) {
      cpstring_type cpextended_desc =
        cpoint_traits::convert_from(desc.extended_description());

      cpstring_type wrapped_text = detail::wrap(cpextended_desc,
        max_width()-indent);

      cpextended_desc.clear();
      for(auto c : wrapped_text) {
        cpextended_desc += c;
        if(c == code_point('\n'))
          cpextended_desc.append(indent,code_point(' '));
      }

      extended_col = cpoint_traits::convert_to(cpextended_desc);

    }

    key_col.append(extended_col);
  }

  return key_col;
}


#if 0
template<typename CharT>
typename basic_default_formatter<CharT>::string_type
basic_default_formatter<CharT>::
  do_typeset_option(const description_type &desc) const
{
  if(!desc.key_description)
    return string_type();

  string_type key_col = desc.key_description();

  if(desc.make_value) {
    if(desc.implicit_value && desc.implicit_value_description) {
      key_col.append({' ','['});
      key_col.append(arg_str());
      key_col.append({'=','<'});
      key_col.append(desc.implicit_value_description());
      key_col.append({'>',']'});
    }
    else {
      key_col.push_back(' ');
      key_col.append(arg_str());
    }

    if(key_col.size() > key_column_width()) {
      key_col.push_back('\n');
      key_col.append(key_column_width()+column_pad(),' ');
    }
    else {
      std::size_t per_pad = key_column_width()+column_pad()-key_col.size();
      key_col.append(per_pad,' ');
    }

    std::size_t indent = key_column_width()+column_pad();
    string_type extended_col;
    if(desc.extended_description) {
      // CharT - UTF32 - CharT conversion
      typename wstring_convert_to_UTF32<CharT>::converter cvt;

      std::u32string utf32_desc = cvt.from_bytes(desc.extended_description());
      std::u32string wrapped_text = detail::wrap(utf32_desc,
        max_width()-indent);

      utf32_desc.clear();
      for(char32_t c : wrapped_text) {
        utf32_desc += c;
        if(c == char32_t('\n'))
          utf32_desc.append(indent,char32_t(' '));
      }

      extended_col = cvt.to_bytes(utf32_desc);
    }

    key_col.append(extended_col);
  }

  return key_col;
}

#endif


typedef basic_option_formatter<char> cmd_option_fmt;
typedef basic_option_formatter<wchar_t> wcmd_option_fmt;

template<typename CharT>
std::basic_string<CharT>
to_string(const std::vector<basic_option_description<CharT> > &grp,
  const basic_option_formatter<CharT> &fmt = basic_default_formatter<CharT>())
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_option_description<CharT> description_type;

  string_type out;

  typedef std::pair<const description_type *,string_type> output_pair_type;
  std::vector<output_pair_type> output_grp;
  output_grp.reserve(grp.size());

  std::size_t extent = 0;
  for(auto &desc : grp) {
    // hidden means not long or short key descriptions
    if((desc.key_description)) {
      output_grp.emplace_back(std::make_pair(&desc,fmt.typeset_option(desc)));
      extent = std::max(extent,output_grp.back().second.size());
    }
  }

  // Sort the results if requested
  auto &&compare = fmt.compare();
  if(compare) {
    std::sort(output_grp.begin(),output_grp.end(),
      [&](const output_pair_type &lhs, const output_pair_type &rhs) {
        return compare(*lhs.first,*rhs.first);
      });
  }

  for(auto &pair : output_grp) {
    out.append(pair.second);
    out.push_back('\n');
  }

  return out;
}


}

}




#endif



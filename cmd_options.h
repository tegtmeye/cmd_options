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

#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

#if __cplusplus < 201703L
#include <boost/any.hpp>
#else
#include <any>
#endif

#include <map>
#include <tuple>
#include <array>
#include <utility>
#include <vector>
#include <string>
#include <cassert>
#include <codecvt>
#include <locale>
#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>

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

  /*
    Checking if any has a value is different between boost and C++17.

    Provide an abstraction
  */
  inline bool is_empty(const any &_val)
  {
    return _val.has_value();
  }

  /*
    Convenience syntax for any_cast between boost::any and std::any

    Would rather use a "using any_cast = boost::any_cast" syntax but aliases
    are not available for functions
  */
  template<typename T, typename AnyT>
  inline T any_cast(AnyT && _val)
  {
    return std::any_cast<T>(std::forward<AnyT>(_val));
  }

  /*
    Convenience for bad_any_cast between boost::any and std::any
  */
  using bad_any_cast = std::bad_any_cast;
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

class invalid_argument_error : public parse_error {
  public:
    invalid_argument_error(std::size_t posn, std::size_t argn)
        :parse_error("invalid_argument_error",posn,argn) {}
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

    std::size_t min(void) const noexcept {
      return _min;
    }

    std::size_t max(void) const noexcept {
      return _max;
    }

    std::size_t occurrences(void) const noexcept {
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
  Used for parsing errors during formatting
*/
class formatter_error : public std::logic_error {
  public:
    formatter_error(const std::string &what, std::size_t posn)
      :std::logic_error(what), _position(posn) {}

    std::size_t position(void) const {
      return _position;
    }

  private:
    std::size_t _position;
};

namespace detail {
/*
  Convenience function to translate CharT -> char -> CharT for use in
  formatting exception messages. It is assumed that chars, char16,
  char32 are UTF-encoded and wchar_t has fixed width encoding (possibly
  UCS2 or UTF32 depending on platform.
*/
inline std::string asUTF8(const std::basic_string<char> &str)
{
  return str;
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT, char>::value,
  std::basic_string<char> >::type
fromUTF8(const std::string &str)
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

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT, char16_t>::value,
  std::basic_string<char16_t> >::type
fromUTF8(const std::string &str)
{
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.from_bytes(str);
}

inline std::string asUTF8(const std::basic_string<char32_t> &str)
{
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
  return convert.to_bytes(str);
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT, char32_t>::value,
  std::basic_string<char32_t> >::type
fromUTF8(const std::string &str)
{
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
  return convert.from_bytes(str);
}

inline std::string asUTF8(const std::basic_string<wchar_t> &str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t> > convert;
  return convert.to_bytes(str);
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT, wchar_t>::value,
  std::basic_string<wchar_t> >::type
fromUTF8(const std::string &str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t> > convert;
  return convert.from_bytes(str);
}

}










template<typename CharT>
using basic_variable_map = std::multimap<std::basic_string<CharT>,any>;

typedef std::multimap<std::basic_string<char>,any> variable_map;
typedef std::multimap<std::basic_string<wchar_t>,any> wvariable_map;
typedef std::multimap<std::basic_string<char16_t>,any> variable_map16;
typedef std::multimap<std::basic_string<char32_t>,any> variable_map32;

/*
  Structure describing an unpacked argument returned from an unpack
  function. Its important to understand that this structure is intended
  to represent a the result of unpacking an argument, not the argument's
  semantic value. That is to say, the function that returns an object of
  this type should not determine if the _value_ of the option is valid,
  for example whether `--foo` is a valid option for this application,
  but rather whether or not the character sequence `--foo` is valid
  option syntax and can be decomposed into an option, packed options,
  and a value if so specified. The reason for an intermediate return
  value here is a byproduct of the many different ways that an option
  argument can be packed together. Having a single function that unpacks
  an option without actually determining if the option has a valid value
  for the current application removes the complexities of determining
  syntax validity from the code that determines its value validity.

  The elements represent the following

  - `did_unpack`

    A boolean value indicating that the structure contains the result of
    unpacking the given argument into its constitute properties. Reasons
    that this value would be set to false include an
    `option_description` that does not use this option syntax. For
    example, if an `option_description` has the `unpack_GNU` unpacking
    function set for a `--foo` option but the `-fno-bar` string was
    parsed.

  - `value_provided`

    A boolean value indicating that the option string contained a packed
    value. This is necessary because it is possible to explicitly
    provide an empty value. Examples of packed values include GNU long
    option syntax such as `--foo=bar`. Do not set `value_provided` to
    true if the option expects a later argument--only if it is provided
    as part of the given string.

  - `raw_key`

    A string containing the raw option key parsed in the option. This is
    not the same as the mapped key stored in the `variable_map`.
    Determining that is handled by `option_description.mapped_key`. For
    example, if the `unpack_GNU` unpacking function is set and `--foo`
    is provided, the `raw_key` is `foo`.

  - `packed_arguments`

    A vector of strings containing any remaining packed arguments if
    present. If the arguments require a prefix, they must be prepended
    to each argument otherwise it will be considered an operand
    argument. For example, if you wish to allow `-abcd` to be equivalent
    to `-a -b --cat=dog -d`, then the raw key is `a` and the packed
    options are: `{"-b","--cat=dog","-d"}`. An appropriate unpack method
    could add packed arguments for other reasons. For example, if `-b`
    always implies a hidden (deprecated)`--bar_old`, then `raw_key` is
    `b` and include `--bar_old` in the packed arguments;

  - `value`

    A string containing the value if present. For example, if the
    `unpack_GNU` unpacking function is set and `--foo=bar` is provided,
    the `value` is `bar`.


  For example: here are some known ways that option arguments can be
  represented:

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

  8) -fbar        = long flag, key=bar, value=true
                  : prefix = "-"
                  : raw_key = "bar"
                  : packed_arguments = ""
                  : value = "true"
                  : value_provided = true

  8) -fno-bar     = long flag, key=bar, value=false
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
  string_type raw_key;
  packed_arg_seq packed_arguments;
  string_type value;
};

typedef basic_option_pack<char> option_pack;
typedef basic_option_pack<wchar_t> woption_pack;
typedef basic_option_pack<char16_t> option_pack16;
typedef basic_option_pack<char32_t> option_pack32;

/*
  This structure is the fundamental description of how an option or
  operand should be parsed by `parse_arguments`. A sequence of
  `option_description` structures are provided to `parse_arguments` and
  each one is evaluated in-order until a suitable `option_description`
  is found that can handle the input string. The presence or absence of
  appropriately set fields determines how the argument string is parsed.
  A default constructed `option_description` or one with no fields set
  is ignored by `parse_arguments` completely.
*/
template<typename CharT>
struct basic_option_description {
  typedef CharT                     char_type;
  typedef std::basic_string<CharT>  string_type;
  typedef basic_variable_map<CharT> variable_map_type;
  typedef basic_option_pack<CharT>  option_pack;

  /*
    Unpack the raw option pack and return an object of `basic_option_pack`.
    If `unpack_option` is not provided, then this description is assumed to
    describe an operand argument.

    The `option` argument is raw argument string received by `parse_arguments`.

    If the option cannot be unpacked by this description, then return
    false in the `did_unpack` field (or an empty option_pack). See
    `basic_option_pack` for information on the return type.

    This function is intended to essentially parse the option into its
    constitute properties. Its important to understand that the function set
    in this field should unpack or determine if the argument has a valid
    syntax for this option, not validate its semantic value. That is to say,
    this function should not determine if the _value_ of the option is
    valid. For example whether `--foo` is a valid option for this
    application, but rather whether or not `--foo` is a valid option syntax
    and can be decomposed into an option, packed options, and a value if so
    specified. The reason for an intermediate return value here is a
    byproduct of the many different ways that an option argument can be
    packed together. Having a single function that unpacks an option without
    actually determining if the option has a valid value for the current
    application removes the complexities of determining syntax validity from
    the code that determines its value validity.

    Beyond determining the validity of the option syntax, this function
    can also be used to inject hidden options that can be useful. For
    example, if `--foo` is given implies `--bar`, even if `--bar` was
    not provided, then the unpacked option for `foo` can return `bar` in
    the `packed_arguments` field.

    A note on operand values... If the options group _only_ contains
    operands--or descriptions that do not have the `unpack_option`
    set--then every argument is considered an operand---even if looks
    like an option. For example, if there are no `option_descriptions`
    with `unpack_option` set, then `--foo`, which looks like a option
    flag, is considered a operand. This may not be what was intended.
    Reasonable machinery in this case is to include a 'dummy' option
    that has `unpack_option` set and where `mapped_key` throws an
    `unknown_option_error` indicating the unknown option. The EZ
    Interface includes such a 'dummy' option `make_options_error`.
  */
  std::function<option_pack(const string_type &option)> unpack_option;

  /*
    If this description can handle the given argument, return true and
    the key that will be used in the variable map to represent this
    option or operand depending on the presence or absence of
    `unpack_option`. That is, this `option_description` should process
    the _semantic value_ of the given raw key. If it does not, then
    return `false` as the first value. For example, suppose this option
    description handles `--foo` options and the string `--foo` was
    parsed. The `unpack_GNU` unpack function would return `foo` as the
    raw string. If you would like to access this option in the
    `variable_map` using the key `my_foo_key`, return
    `{true,"my_foo_key"}`. Likewise, if the string `--bar` was parsed
    and this `option_description` only handles `--foo` options, then
    return `{false,{}}`.

    - The `raw_key` argument is the `option_pack.raw_key` string
    returned by `unpack_option`.

    - The `posn` argument is the nth option or operand parsed so far by
    `parse_arguments`.

    - The `argn` argument is the nth argument parsed so far by
    `parse_arguments`.

    - The `vm` argument is the current state of the `variable_map` based
    on the previous n-1 arguments parsed so far by `parse_arguments`.

    - The return value is a pair where `first` indicates whether or not
    this `option_description` can accept this argument. The `second`
    field is a string representing the key to be used in the
    `variable_map`.


    If `unpack_option` is set and can successfully unpack the argument,
    then the argument is an option otherwise the argument is considered
    an operand. See `unpack_option`.

    Regardless of whether or not the argument is an option or an
    operand, the implementation of `mapped_key_fn` can selectively
    choose to handle or not to handle the argument. For example, if this
    `option_description` represents the `--foo` option, it should return
    `true` if the argument's `raw_key` equals `foo` and return false for
    everything else. It can additionally discriminate based on the
    option's or operand's position in the argument list. A common use
    case is for some utility:

      > foo infile outfile

    Where `infile` is always first and `outfile` is always second.
  */
  std::function<std::pair<bool,string_type>(
    const string_type &raw_key, std::size_t posn, std::size_t argn,
    const variable_map_type &vm)> mapped_key;

  /*
    Return the human-readable description for this option. The value is
    not used for options parsing but intended to be used with [help
    formatting](EZ-Interface-Help-Formatting).

    * The return value is an appropriately formatted string

    This does not necessarily need to be the result provided in
    `mapped_key` but can be any meaningful string based on the need. For
    example, suppose for some nonstandard syntax the key `--foo1bar
    --foo2bar` is possible, a reasonable response from this function
    could be: `foo[integer]bar`. Another example is in the case of long
    and short options. Typically the short option is hidden and the long
    option is fully specified with a description containing the
    associated short option. That is: `--foo, -f`.

    In the EZ Interface, the default formatter considers the description
    hidden if the `key_description` field is not set. Likewise,
    `set_default_option_spec` sets a function that returns a string with
    identifiers that participate in substitution of values handled by
    the default formatter namely `%?V{ <%V%?I{=%I}{}>}{}`.
  */
  std::function<string_type(void)> key_description;

  /*
    Return the human-readable extended description for this option. The
    value is not used for options parsing but intended to be used with help
    formatting.

    * The return value is an appropriately formatted string

    This can be any meaningful string based on the need.

    In the EZ Interface, the default formatter provides simple
    substitution and wrapping of this result string.
  */
  std::function<string_type(void)> extended_description;

  /*
    If set return the semantic value of the argument as an `any` object as
    determined by the contents of the other parameter.

    If the description represents an option, then the parser requires an
    argument string either through a packed option string or through an
    argument to be parsed next. If not set, the parser interprets the
    option as explicitly forbiding option values. I.e. `--foo=bar` or
    `--foo bar` will return an error unless the `bar` is a valid operand
    in the later case.

    If set and the description is an operand, then convert the operand
    to a semantic value. If not set, then the parser will store a
    default-constructed `any` object in the `variable_map`.

    - The `mapped_key` argument is the value returned by
    `option_pack.mapped_key`. This can be useful for checking the state
    of the `variable_map` and throwing an errors or if the semantic
    value depends on the current state of the `variable_map`.

    - The `posn` argument is the nth option or operand parsed so far by
    `parse_arguments`.

    - The `argn` argument is the nth argument parsed so far by
    `parse_arguments`.

    - The `value` argument is the raw argument string provided either by
    the packed value or the subsequent parsed argument. This function
    should interpret this value and return the semantic meaning in an
    `any` object. N.B., it is possible this string may be empty as it is
    possible to explicitly provide an empty string argument.

    - The `vm` argument is the current state of the `variable_map` based
    on the previous n-1 arguments parsed so far by `parse_arguments`.

    - The return value is an `any` object representing the interpreted
    value of `value` to be stored in the `variable_map` under the key
    `mapped_key`.

    If conversion of the `value` string to an `any` obejct fails, thow
    the appropriate exception indicating the failure. It an
    application-specific exception is desired, it is suggested that
    `invalid_argument_error` be thrown with a nested
    application-specific exception but it is not required. ie

      any my_make_value_fn(const string_type &mapped_key, std::size_t posn,
        std::size_t argn, const string_type &value, const variable_map_type &vm)
      {
        try {
          // try to convert val

          return any(result);
        }
        catch (...) {
          std::throw_with_nested(invalid_argument_error{posn,argn});
        }
      }

    Likewise, although there are no restrictions on what the
    `make_value` function performs, it is recommended that exceptions
    should only be thrown for cases where the option is valid but the
    value of the option is not. That is, it shouldn't be used to handle
    mutual exclusivity or other cases where the option itself is
    improper. A good example of where to throw an exception would be an
    option `--foo` with a numeric argument. For example `--foo 5`.
    Suppose that you would also like to support the ability to spell out
    the number: `--foo five`. The `make_value` function needs to parse
    the string `five` and return the integer `5` in a `any` object. If
    the given string doesn't correspond to a number, for example `--foo
    bar` as `bar` is not a number, then it is reasonable for
    `make_value` to throw an `co::invalid_argument_error` exception.
  */

  std::function<
    any(const string_type &mapped_key, std::size_t posn, std::size_t argn,
      const string_type &value, const variable_map_type &vm)> make_value;

  /*
    Return the human-readable description for this option's value. The
    value is not used for options parsing but intended to be used with
    help formatting. For example, suppose the option `--foo` expects an
    integer argument type. A reasonable return value could be `integer`.

    * The return value is an appropriately formatted string

    This can be any meaningful string based on the need.

    In the EZ Interface, the default formatter provides simple
    substitution and wrapping of this result string.
  */
  std::function<string_type(void)> value_description;

  /*
    If set return the implicit semantic value as an `any` object for the
    given `mapped_key` to be stored in the given `variable_map`. When
    parsing, setting the `make_implicit_value` field indicates that an
    option-argument is optional. If it is not set, the an option argument is
    explicitly forbidden.

    For example suppose the `--foo` option accepts optional arguments.
    If `--foo arg` was given, the `arg` argument is converted to its
    semantic value using `make_value`. If an argument was not provided,
    then `make_implicit_value` is called and the result is stored in the
    `variable_map`. Likewise, if the `--bar` option does not permit
    arguments, do not set `make_implicit_value`.


    - The `mapped_key` argument is the value returned by
    `option_pack.mapped_key`. This can be useful for checking the state
    of the `variable_map` and throwing an errors or if the semantic
    value depends on the current state of the `variable_map`.

    - The `vm` argument is the current state of the `variable_map` based on
    the previous n-1 arguments parsed so far by `parse_arguments`.

    - The return value is an `any` object representing the desired implicit
    valueto be stored in the `variable_map` under the key `mapped_key`.
  */
  std::function<any(const string_type &mapped_key,
    const variable_map_type &vm)> make_implicit_value;

  /*
    Return the human-readable description for this option's implicit
    value. The value is not used for options parsing but intended to be
    used with help formatting. For example, suppose the option `--foo`
    has a default value of `5`. A reasonable return value could be `5`.

    * The return value is an appropriately formatted string

    This can be any meaningful string based on the need.

    In the EZ Interface, the default formatter provides simple
    substitution and wrapping of this result string.
  */
  std::function<string_type(void)> implicit_value_description;

  /*
    If set, this function is called for each option_description by the
    `parse_arguments` function after all options are parsed. This
    function is ignored by `parse_incremental_arguments`. A typical use
    of this function is to inspect the state of the `variable_map`
    argument for option-specific inconsistencies or illegal
    combinations. For example, if a minimum number of options are
    expected, the variable_map_type can be queried to see if it has been
    satisfied and throw an error if not. There is no restriction on what
    this function can or cannot do.
  */
  std::function<void(const variable_map_type &vm)> finalize;

};

typedef basic_option_description<char> option_description;
typedef basic_option_description<wchar_t> woption_description;
typedef basic_option_description<char16_t> option_description16;
typedef basic_option_description<char32_t> option_description32;

template<typename CharT>
using basic_options_group = std::vector<basic_option_description<CharT> >;

typedef std::vector<basic_option_description<char> > options_group;
typedef std::vector<basic_option_description<wchar_t> > woptions_group;
typedef std::vector<basic_option_description<char16_t> > options_group16;
typedef std::vector<basic_option_description<char32_t> > options_group32;


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
    return option_pack{false,{},{},{},{}};

  if(res.second+1 == str.end())
    return option_pack{true,false,{res.second,res.second+1},{},{}};

  if(uses_packed_flags) {
    string_type raw_key{res.second,++res.second};
    packed_arg_seq packed_arguments;
    while(res.second != str.end())
      packed_arguments.push_back(sprefix+(*(res.second++)));

    return option_pack{true,false,raw_key,packed_arguments,{}};
  }

  return option_pack{true,true,{res.second,res.second+1},{},
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
    return option_pack{true,false,{res.second,str.end()},{},{}};

  return option_pack{true,true,{res.second,assign_loc},{},
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
template<typename BidirectionalIterator, typename CharT =
  typename std::remove_pointer<
    typename std::iterator_traits<BidirectionalIterator>::value_type>::type>
basic_variable_map<CharT>
parse_incremental_arguments(BidirectionalIterator first,
  BidirectionalIterator last, const basic_options_group<CharT> &grp,
  const basic_variable_map<CharT> &vm,
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
      std::reverse_iterator<BidirectionalIterator>(last),
      std::reverse_iterator<BidirectionalIterator>(first)
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
          if(desc->make_implicit_value)
            _vm.emplace(mapped_key,desc->make_implicit_value(mapped_key,_vm));
          else
            _vm.emplace(mapped_key,any());
        }
        else {
          // required or optional value
          // is it embedded in the option pack?
          if(option_pack.value_provided) {
            // handle the provided value
            _vm.emplace(mapped_key,
              desc->make_value(mapped_key,option_count,arg_count,
                option_pack.value,_vm));
          }
          else if(desc->make_implicit_value) {
            _vm.emplace(mapped_key,desc->make_implicit_value(mapped_key,_vm));
          }
          else if(current_cmdlist.empty()) {
            // no more items in the current pack
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
              throw missing_argument_error(option_count,arg_count);
            }
            else {
              _vm.emplace(mapped_key,
                desc->make_value(mapped_key,option_count,arg_count++,
                  current_cmdlist.back(),_vm));
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
          if(desc->unpack_option || !desc->mapped_key)
            continue;

          std::pair<bool,string_type> operand_key =
            desc->mapped_key(string_type(),operand_count,arg_count,_vm);
          if(!operand_key.first)
            continue;

          // handle this operand
          if(desc->make_value) {
            _vm.emplace(operand_key.second,
              desc->make_value(operand_key.second,operand_count,arg_count,
                arg,_vm));
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


template<typename BidirectionalIterator,
  typename CharT = typename std::remove_pointer<
    typename std::iterator_traits<BidirectionalIterator>::value_type>::type>
inline basic_variable_map<CharT>
parse_incremental_arguments(BidirectionalIterator first,
  BidirectionalIterator last, const basic_options_group<CharT> &grp,
  const std::basic_string<CharT> &end_of_options)
{
  return parse_incremental_arguments(first,last,grp,
    basic_variable_map<CharT>(),end_of_options);
}

template<typename BidirectionalIterator,
  typename CharT = typename std::remove_pointer<
    typename std::iterator_traits<BidirectionalIterator>::value_type>::type>
inline basic_variable_map<CharT>
parse_arguments(BidirectionalIterator first, BidirectionalIterator last,
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm,
  const std::basic_string<CharT> &end_of_options)
{
  basic_variable_map<CharT> _vm =
    std::move(parse_incremental_arguments(first,last,grp,vm,end_of_options));

  for(auto &desc : grp) {
    if(desc.finalize)
      desc.finalize(_vm);
  }

  return _vm;
}

template<typename BidirectionalIterator,
  typename CharT = typename std::remove_pointer<
    typename std::iterator_traits<BidirectionalIterator>::value_type>::type>
inline basic_variable_map<CharT>
parse_arguments(BidirectionalIterator first, BidirectionalIterator last,
  const basic_options_group<CharT> &grp,
  const std::basic_string<CharT> &end_of_options)
{
  return parse_arguments(first,last,grp,basic_variable_map<CharT>(),
    end_of_options);
}

/*
  Convenience specializations of parse_incremental_arguments
*/
template<typename BidirectionalIterator,
  typename CharT = typename std::remove_pointer<
    typename std::iterator_traits<BidirectionalIterator>::value_type>::type>
inline basic_variable_map<CharT>
parse_incremental_arguments(BidirectionalIterator first,
  BidirectionalIterator last, const basic_options_group<CharT> &grp,
  const basic_variable_map<CharT> &vm)
{
  return parse_incremental_arguments(first,last,grp,vm,
    std::basic_string<CharT>{'-','-'});
}

template<typename BidirectionalIterator,
  typename CharT = typename std::remove_pointer<
    typename std::iterator_traits<BidirectionalIterator>::value_type>::type>
inline basic_variable_map<CharT>
parse_incremental_arguments(BidirectionalIterator first,
  BidirectionalIterator last, const basic_options_group<CharT> &grp)
{
  return parse_incremental_arguments(first,last,grp,
    basic_variable_map<CharT>(),std::basic_string<CharT>{'-','-'});
}

template<typename BidirectionalIterator,
  typename CharT = typename std::remove_pointer<
    typename std::iterator_traits<BidirectionalIterator>::value_type>::type>
inline basic_variable_map<CharT>
parse_arguments(BidirectionalIterator first, BidirectionalIterator last,
  const basic_options_group<CharT> &grp, const basic_variable_map<CharT> &vm)
{
  basic_variable_map<CharT> _vm =
    std::move(parse_incremental_arguments(first,last,grp,vm,
      std::basic_string<CharT>{'-','-'}));

  for(auto &desc : grp) {
    if(desc.finalize)
      desc.finalize(_vm);
  }

  return _vm;
}

template<typename BidirectionalIterator,
  typename CharT = typename std::remove_pointer<
    typename std::iterator_traits<BidirectionalIterator>::value_type>::type>
inline basic_variable_map<CharT>
parse_arguments(BidirectionalIterator first, BidirectionalIterator last,
  const basic_options_group<CharT> &grp)
{
  return parse_arguments(first,last,grp,
    basic_variable_map<CharT>(),std::basic_string<CharT>{'-','-'});
}









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
class basic_constraint {
  public:
    typedef std::basic_string<CharT> string_type;

    int at_position(void) const {
      return _position;
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

    int at_argument(void) const {
      return _argument;
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

    std::size_t min_occurrence(void) const {
      return _min;
    }

    std::size_t max_occurrence(void) const {
      return _max;
    }

    /*
      Constrain the number of occurrences to exactly \c n
    */
    basic_constraint<CharT> & occurrences(std::size_t n) {
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

    const std::vector<string_type> & mutually_exclusive(void) const {
      return _mutually_exclusive;
    }

    basic_constraint<CharT> &
    mutually_exclusive(const std::vector<string_type> &mapped_key_vec)
    {
      _mutually_exclusive = mapped_key_vec;
      return *this;
    }

    const std::vector<string_type> & mutually_inclusive(void) const {
      return _mutually_inclusive;
    }

    basic_constraint<CharT> &
    mutually_inclusive(const std::vector<string_type> &mapped_key_vec)
    {
      _mutually_inclusive = mapped_key_vec;
      return *this;
    }

  private:
    int _position = -1;
    int _argument = -1;

    std::size_t _min = 0;
    std::size_t _max = std::numeric_limits<std::size_t>::max();
    std::vector<string_type> _mutually_exclusive;
    std::vector<string_type> _mutually_inclusive;
};

typedef basic_constraint<char> constrain;
typedef basic_constraint<wchar_t> wconstrain;



template<typename T, typename CharT>
class basic_value {
  public:
    typedef T value_type;

    basic_value(void) = default;
    basic_value(T *_val) :_callback([=](const T &val){*_val = val;}) {}
    basic_value(const std::function<void(const T &)> &callback)
      :_callback(callback) {}

    const std::function<void(const T &)> & callback(void) const {
      return _callback;
    }

    std::shared_ptr<value_type> implicit(void) const {
      return _implicit;
    }

    basic_value<T,CharT> & implicit(const T &val) {
      _implicit = std::make_shared<T>(val);

      return *this;
    }

    const std::basic_string<CharT> description(void) const {
      return _description;
    }

    basic_value<T,CharT> & description(const std::basic_string<CharT> &str) {
      _description = str;

      return *this;
    }

    basic_value<T,CharT> & validate(const std::function<void(const T &)> &fn) {
      _validate = fn;

      return *this;
    }

    const std::function<void(const T &)> & validate(void) const {
      return _validate;
    }

    basic_value<T,CharT> &
    transform(const std::function<
      std::basic_string<CharT>(const std::basic_string<CharT> &str)> &fn)
    {
      _transform = fn;

      return *this;
    }

    const std::function<
      std::basic_string<CharT>(const std::basic_string<CharT> &)> &
    transform(void) const
    {
      return _transform;
    }


  private:
    std::function<
      std::basic_string<CharT>(const std::basic_string<CharT> &)> _transform;
    std::function<void(const T &)> _callback;
    std::function<void(const T &)> _validate;
    std::shared_ptr<value_type> _implicit;
    std::basic_string<CharT> _description{'a','r','g'};
};

template<typename T>
using value = basic_value<T,char>;

template<typename T>
using wvalue = basic_value<T,wchar_t>;

template<typename T>
using value16 = basic_value<T,char16_t>;

template<typename T>
using value32 = basic_value<T,char32_t>;

/*
  Intermediary conversion class to handle conversion to and from
  basic_string<CharT> and value T.

  If CharT is char or wchar, basically calls operator<< and operator>>
  for type T as is. If CharT is char16_t or char32_t, then the stream is
  of type char and is considered UTF8. The correct string type is then
  transcoded from this.

  Deals with two issues:
    - Lack of support in C++11 for formatted stream of char16_t and char32_t
    - The case where a user needs finer grained handling of string to
      value conversions.

  For item (1), it is what it is. If another way of dealing with this
  emerges, it is worth rethinking this design.

  For item (2), it is a a real possibility that a userdefined type needs
  separate formatted IO handling. That is, if operator{<<,>>} is already
  overloaded for things like serialization or human readable input or
  debugging, then that IO probably doesn't make very good command
  argument IO. In this case, simply specialize convert_value for your
  own type similar to convert_value<bool>.
*/

template<typename T>
struct convert_value {
  template<typename CharT>
  static T from_string(const std::basic_string<CharT> &str)
  {
    typedef std::basic_istringstream<CharT> istream_type;

    T _val;
    istream_type in(str);
    in >> _val;
    if(!in || in.peek() != istream_type::traits_type::eof())
      throw std::invalid_argument(detail::asUTF8(str));

    return _val;
  }

  static T from_string(const std::u16string &str)
  {
    return from_string(detail::asUTF8(str));
  }

  static T from_string(const std::u32string &str)
  {
    return from_string(detail::asUTF8(str));
  }

  template<typename CharT>
  static void to_string(std::basic_string<CharT> &str, const T &val)
  {
    std::basic_ostringstream<CharT> out;
    out << val;

    str = out.str();
  }

  static void to_string(std::u16string &str, const T &val)
  {
    std::string _str;
    to_string(_str,val);
    str = detail::fromUTF8<char16_t>(_str);
  }

  static void to_string(std::u32string &str, const T &val)
  {
    std::string _str;
    to_string(_str,val);
    str = detail::fromUTF8<char32_t>(_str);
  }
};

template<typename CharT>
struct convert_value<std::basic_string<CharT> > {
  static const std::basic_string<CharT> &
  from_string(const std::basic_string<CharT> &str)
  {
    return str;
  }

  template<typename OtherCharT>
  static void to_string(std::basic_string<OtherCharT> &str,
    const std::basic_string<CharT> &val)
  {
    str = val;
  }
};

template<>
struct convert_value<bool> {
  template<typename CharT>
  static bool from_string(const std::basic_string<CharT> &str)
  {
    typedef std::basic_istringstream<CharT> istream_type;

    bool _val;
    istream_type in(str);
    if(!(in >> std::noboolalpha >> _val)) {
      in.clear();
      in.str(str);
      if(!(in >> std::boolalpha >> _val))
        throw std::invalid_argument(detail::asUTF8(str));
    }

    if(in.peek() != istream_type::traits_type::eof())
      throw std::invalid_argument(detail::asUTF8(str));

    return _val;
  }

  static bool from_string(const std::u16string &str)
  {
    return from_string(detail::asUTF8(str));
  }

  static bool from_string(const std::u32string &str)
  {
    return from_string(detail::asUTF8(str));
  }

  template<typename CharT>
  static void to_string(std::basic_string<CharT> &str, bool val)
  {
    std::basic_ostringstream<CharT> out;
    out << std::boolalpha << val;

    str = out.str();
  }

  static void to_string(std::u16string &str, const bool &val)
  {
    std::string _str;
    to_string(_str,val);
    str = detail::fromUTF8<char16_t>(_str);
  }

  static void to_string(std::u32string &str, const bool &val)
  {
    std::string _str;
    to_string(_str,val);
    str = detail::fromUTF8<char32_t>(_str);
  }
};

template<>
struct convert_value<char16_t> {
  static char16_t from_string(const std::basic_string<char16_t> &str)
  {
    if(str.size() != 1)
      throw std::invalid_argument(detail::asUTF8(str));

    return str.front();
  }

  static void to_string(std::u16string &str, char16_t val)
  {
    str = val;
  }
};

template<>
struct convert_value<char32_t> {
  static char32_t from_string(const std::basic_string<char32_t> &str)
  {
    if(str.size() != 1)
      throw std::invalid_argument(detail::asUTF8(str));

    return str.front();
  }

  static void to_string(std::u32string &str, char32_t val)
  {
    str = val;
  }
};




namespace detail {


/*
  Ensure we are always operating on code points and not the underlying
  char_type.

  Helper traits to transform (if necessary) a CharT string into a code
  point string. That is, if CharT string is a multibyte string,
  transform it to a string where each element is a code point. Since
  each element in a UTF32 string is a single unicode code point, then
  convert the UTF{8,16} strings into UTF32. Since wchar_t is platform-
  and local-dependent, then assume it represents a single code point and
  ensure documentation states this. Specifically, wchar_t is only UTF16
  if the code point fits into 2 bytes if sizeof(wchar_t) == 2.
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

}


template<typename CharT>
inline bool is_portable(CharT c)
{
  return (c == 0x00) || (c >= 0x07 && c <= 0x0D) || (c >= 0x20 && c < 0x7F);
}

/*
  Split a format string of the form:
    [long_name][delim][short_name][delim][map_key]
  where everything after a missing delim is optional. The following is ok:

    long
    long,
    long,,
    long,short
    long,short,
    long,short,key
    long,,key
    ,short
    ,short,
    ,short,key
    ,,key

  Assertion failure for trailing delims. That is,

    long,short,key,*.*
*/
template<typename CharT>
inline std::array<std::basic_string<CharT>,3>
split_opt_spec(const std::basic_string<CharT> &str, CharT delim)
{
  typedef std::basic_string<CharT> string_type;

  std::array<std::basic_string<CharT>,3> result{};

  auto sloc = std::find(str.begin(),str.end(),delim);
  result[0] = string_type(str.begin(),sloc);

  if(sloc != str.end()) {
    auto kloc = std::find(sloc+1,str.end(),delim);
    result[1] = string_type(sloc+1,kloc);

    if(kloc != str.end()) {
      auto eloc = std::find(kloc+1,str.end(),delim);
      assert(eloc == str.end()); // extra delim
      result[2] = string_type(kloc+1,str.end());
    }
  }

  return result;
}

/*
  If opt_spec is empty, then 'accept all options' and the mapped key is
  the given option verbatim.

  If a long or short opt is given, then enforce POSIX and it must be
  from the portable character set (subset of ASCII). If given the short
  opt must be a single character. The mapped_key can be of any length or
  value.

  If the mapped_key is not given, then it is the long_opt if it is given
  otherwise it is the short_opt.

  Return the mapped_key used for contraint violation exceptions only
*/
template<bool uses_packed_flags, typename CharT>
std::basic_string<CharT>
set_default_option_spec(const std::basic_string<CharT> &opt_spec, CharT delim,
  basic_option_description<CharT> &desc, bool hidden)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map_type;

  // %?V{ <%V%?I{=%I}{}>}{}

  static const string_type arg_suffix{'%','?','V','{',' ','<','%','V','%',
    '?','I','{','=','%','I','}','{','}','>','}','{','}'};

  string_type long_opt;
  string_type short_opt;
  string_type mapped_key;
  std::tie(long_opt,short_opt,mapped_key) = split_opt_spec(opt_spec,delim);

  assert(short_opt.size() < 2);
  // long and short_opt must be from the portable character set
  assert(std::all_of(long_opt.begin(),long_opt.end(),is_portable<CharT>));
  assert(std::all_of(short_opt.begin(),short_opt.end(),is_portable<CharT>));

  if(mapped_key.empty()) {
    if(long_opt.empty())
      mapped_key = short_opt;
    else
      mapped_key = long_opt;
  }

  if(!long_opt.empty() && !short_opt.empty()) {
    desc.unpack_option = unpack_gnu<uses_packed_flags,CharT>;
    desc.mapped_key = [=](const string_type &_opt, std::size_t, std::size_t,
      const variable_map_type &)
      {
        if(_opt == long_opt || _opt == short_opt)
          return std::make_pair(true,mapped_key);

        return std::make_pair(false,string_type());
      };

    if(!hidden) {
      desc.key_description = [=](void) {
        return string_type{'-','-'} + long_opt + delim
          + string_type{'-'} + short_opt + arg_suffix;
      };
    }
  }
  else if(!long_opt.empty()) {
    desc.unpack_option = unpack_gnu<uses_packed_flags,CharT>;
    desc.mapped_key = [=](const string_type &_opt, std::size_t, std::size_t,
      const variable_map_type &)
      {
        if(_opt == long_opt)
          return std::make_pair(true,mapped_key);

        return std::make_pair(false,string_type());
      };

    if(!hidden) {
      desc.key_description = [=](void) {
        return string_type{'-','-'} + long_opt + arg_suffix;
      };
    }
  }
  else if(!short_opt.empty()){
    desc.unpack_option = unpack_posix<uses_packed_flags,CharT>;
    desc.mapped_key = [=](const string_type &_opt, std::size_t, std::size_t,
      const variable_map_type &)
      {
        if(_opt == short_opt)
          return std::make_pair(true,mapped_key);

        return std::make_pair(false,string_type());
      };

    if(!hidden) {
      desc.key_description = [=](void) {
        return string_type{'-'} + short_opt + arg_suffix;
      };
    }
  }
  else {
    desc.unpack_option = unpack_gnu<uses_packed_flags,CharT>;
    if(!hidden) {
      desc.key_description = [=](void) {
        return string_type{'-','-','*'} + delim + string_type{'-','*'} +
          arg_suffix;
      };
    }
  }

  return mapped_key;
}

template<typename T, typename CharT>
inline void set_default_option_value(const basic_value<T,CharT> &val,
  basic_option_description<CharT> &desc)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map_type;

  if(!val.description().empty()) {
    desc.value_description = [=](void) -> string_type {
      return val.description();
    };
  }

  if(val.implicit()) {
    desc.make_implicit_value = [=](const string_type &,
      const variable_map_type &)
    {
      if(val.callback())
        val.callback()(*(val.implicit()));

      return any(*(val.implicit()));
    };
    desc.implicit_value_description = [=](void) {
      string_type str;
      convert_value<T>::to_string(str,*(val.implicit()));
      return str;
    };
  }

  if(val.transform()) {
    desc.make_value = [=](const string_type &, std::size_t posn,
      std::size_t argn, const string_type &in_val, const variable_map_type &)
    {
      try {
        const T &result =
          convert_value<T>::from_string(val.transform()(in_val));

        if(val.validate())
          val.validate()(result);

        if(val.callback())
          val.callback()(result);

        return any(result);
      }
      catch (...) {
        std::throw_with_nested(invalid_argument_error{posn,argn});
      }

      return any(); // should never get here, used to avoid compiler warnings
    };
  }
  else {
    desc.make_value = [=](const string_type &, std::size_t posn,
      std::size_t argn, const string_type &in_val, const variable_map_type &)
    {
      try {
        const T &result = convert_value<T>::from_string(in_val);

        if(val.validate())
          val.validate()(result);

        if(val.callback())
          val.callback()(result);

        return any(result);
      }
      catch (...) {
        std::throw_with_nested(invalid_argument_error{posn,argn});
      }

      return any(); // should never get here, used to avoid compiler warnings
    };
  }
}

template<typename T, typename CharT>
inline void set_default_operand_value(const basic_value<T,CharT> &val,
  basic_option_description<CharT> &desc)
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_variable_map<CharT> variable_map_type;

  if(val.implicit()) {
    desc.make_value = [=](const string_type &, std::size_t, std::size_t,
    const string_type &, const variable_map_type &)
    {
      if(val.callback())
        val.callback()(*(val.implicit()));

      return any(*(val.implicit()));
    };
  }
  else if(val.transform()) {
    desc.make_value = [=](const string_type &, std::size_t posn,
      std::size_t argn, const string_type &in_val, const variable_map_type &)
    {
      try {
        const T &result =
          convert_value<T>::from_string(val.transform()(in_val));

        if(val.validate())
          val.validate()(result);

        if(val.callback())
          val.callback()(result);

        return any(result);
      }
      catch (...) {
        std::throw_with_nested(invalid_argument_error{posn,argn});
      }

      return any(); // should never get here, used to avoid compiler warnings
    };
  }
  else {
    desc.make_value = [=](const string_type &, std::size_t posn,
      std::size_t argn, const string_type &in_val, const variable_map_type &)
    {
      try {
        const T &result = convert_value<T>::from_string(in_val);

        if(val.validate())
          val.validate()(result);

        if(val.callback())
          val.callback()(result);

        return any(result);
      }
      catch (...) {
        std::throw_with_nested(invalid_argument_error{posn,argn});
      }

      return any(); // should never get here, used to avoid compiler warnings
    };
  }
}

template<typename CharT>
inline void set_default_operand_key(const std::basic_string<CharT> &key,
  int posn, int argn, basic_option_description<CharT> &desc)
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
void set_default_constraints(const basic_constraint<CharT> &cnts,
  basic_option_description<CharT> &desc,
  const std::basic_string<CharT> &mapped_key)
{
  desc.finalize = [=](const basic_variable_map<CharT> &vm) {
    std::size_t occurrances = vm.count(mapped_key);
    if(occurrances > cnts.max_occurrence() ||
      occurrances < cnts.min_occurrence())
    {
      throw occurrence_error(detail::asUTF8(mapped_key),cnts.min_occurrence(),
        cnts.max_occurrence(),occurrances);
    }

    if(occurrances) {
      for(auto &exclusive_key : cnts.mutually_exclusive()) {
        if(vm.count(exclusive_key) != 0)
          throw mutually_exclusive_error(detail::asUTF8(mapped_key),
            detail::asUTF8(exclusive_key));
      }

      for(auto &inclusive_key : cnts.mutually_inclusive()) {
        if(vm.count(inclusive_key) == 0)
          throw mutually_inclusive_error(detail::asUTF8(mapped_key),
            detail::asUTF8(inclusive_key));
      }
    }
  };
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

  basic_option_description<CharT> desc;

  string_type mapped_key =
    set_default_option_spec<true>(opt_spec,delim,desc,false);

  desc.extended_description = [=](void) { return extended_desc; };

  set_default_constraints(cnts,desc,mapped_key);

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
make_option(const std::basic_string<CharT> &opt_spec,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  typedef std::basic_string<CharT> string_type;

  basic_option_description<CharT> desc;

  string_type mapped_key =
    set_default_option_spec<true>(opt_spec,delim,desc,true);

  set_default_constraints(cnts,desc,mapped_key);

  return desc;
}

template<typename CharT>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  return make_option(std::basic_string<CharT>(opt_spec),cnts,delim);
}


/*
  Case line 6, 8, 10, 12
*/
template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const std::basic_string<CharT> &opt_spec,
  const basic_value<T,CharT> &val,
  const std::basic_string<CharT> &extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  typedef std::basic_string<CharT> string_type;

  basic_option_description<CharT> desc;

  string_type mapped_key =
    set_default_option_spec<false>(opt_spec,delim,desc,false);

  set_default_option_value(val,desc);

  desc.extended_description = [=](void) { return extended_desc; };

  set_default_constraints(cnts,desc,mapped_key);

  return desc;
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec,
  const basic_value<T,CharT> &val, const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  return make_option(std::basic_string<CharT>(opt_spec),val,
    std::basic_string<CharT>(extended_desc),cnts,delim);
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const std::basic_string<CharT> &opt_spec,
  const basic_value<T,CharT> &val, const CharT *extended_desc,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  return make_option(opt_spec,val,std::basic_string<CharT>(extended_desc),
    cnts,delim);
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec, const basic_value<T,CharT> &val,
  const std::basic_string<CharT> &extended_desc,
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
make_option(const std::basic_string<CharT> &opt_spec,
  const basic_value<T,CharT> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  typedef std::basic_string<CharT> string_type;

  basic_option_description<CharT> desc;

  string_type mapped_key =
    set_default_option_spec<false>(opt_spec,delim,desc,true);

  set_default_option_value(val,desc);

  set_default_constraints(cnts,desc,mapped_key);

  return desc;
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_option(const CharT *opt_spec, const basic_value<T,CharT> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>(),
  CharT delim = CharT{','})
{
  return make_option(std::basic_string<CharT>(opt_spec),val,cnts,delim);
}

/*
  The \c make_operand and \c make_operand EZ interface do not map
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
make_operand(const std::basic_string<CharT> &mapped_key,
  const basic_value<T,CharT> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<CharT> desc;

  set_default_operand_value(val,desc);

  set_default_operand_key(mapped_key,cnts.at_position(),cnts.at_argument(),
    desc);

  set_default_constraints(cnts,desc,mapped_key);

  return desc;
}

template<typename CharT, typename T>
inline basic_option_description<CharT>
make_operand(const CharT *mapped_key, const basic_value<T,CharT> &val,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(std::basic_string<CharT>(mapped_key),val,cnts);
}


/*
  Case line 15
*/
template<typename CharT>
inline basic_option_description<CharT>
make_operand(const std::basic_string<CharT> &mapped_key,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  basic_option_description<CharT> desc;

  set_default_operand_key(mapped_key,cnts.at_position(),cnts.at_argument(),
    desc);

  set_default_constraints(cnts,desc,mapped_key);

  return desc;
}

template<typename CharT>
inline basic_option_description<CharT>
make_operand(const CharT *mapped_key,
  const basic_constraint<CharT> &cnts = basic_constraint<CharT>())
{
  return make_operand(std::basic_string<CharT>(mapped_key),cnts);
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
    [](const string_type &,int,int,const variable_map_type &) {
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

/*
  Simple substitution parser

  The EBNF is:
    STRING: REP_STR
          : REP_STR string
          : string REP_STR string
          : string REP_STR
          : string
          : empty

    REP_STR: % OPT_SUBS
             % SUBS

    OPT_SUBS: % ? SUBS { true_STRING } { false_STRING }

    SUBS: one of 'K', 'V', 'I', 'E'

  Where:
    - string is any sequence of characters within the range represented
    by the character size.

    - empty means the empty string

    - 'K' = substitute the value of option_description::key_description

    - 'V' = substitute the value of option_description::value_description

    - 'I' = substitute the value of
            option_description::implicit_value_description

    - 'E' = substitute the value of extended_description::extended_description

    OPT_SUBS evaluates the presence of the given SUBS and if it exists,
    OPT_SUBS is replaces with true_STRING otherwise it is replaced with
    false_STRING.

  Substitution continues until no more take place. Direct or transitive
  recursion is not allowed. That is, each of the following is a error:

    - if A includes a substitution with B and B includes a substitution with A

    - if A incudes a substitution with B and B includes a substitution
    with C and C includes a substitution with A

  Examples, given:
    - key_description is: '--foo%?V{<=%V>%I{[%I]}{}}{}'
    - value_description is: 'arg'
    - implicit value_description is: '42'
    - extended_description is: 'The default value of this option is %I'

  Formatting would result in:

    --foo=<arg>[42]         The default value of this option is 42

  Discussion:

  The purpose of the substitutions is to accomodate other languages
  while easily reformatting the output.

  do_expand parses up to last or '}' expanding along the way
*/
template<typename CharT>
std::pair<typename std::basic_string<CharT>::const_iterator,
  std::basic_string<CharT> >
do_expand(typename std::basic_string<CharT>::const_iterator first,
  typename std::basic_string<CharT>::const_iterator cur,
  typename std::basic_string<CharT>::const_iterator last,
  const std::function<std::basic_string<CharT>(void)> &key_desc,
  const std::function<std::basic_string<CharT>(void)> &value_desc,
  const std::function<std::basic_string<CharT>(void)> &imp_value_desc,
  const std::function<std::basic_string<CharT>(void)> &ext_desc,
  std::vector<CharT> &stack)
{
  typedef std::basic_string<CharT> string_type;

  string_type result;

  while(cur != last) {
    // find first occurrence of non-escaped '%' checking for valid
    // escape sequences and incorrect brackets along the way
    while(cur != last && *cur != '%') {
      if(*cur == '\\') {
        ++cur;

        if(cur == last ||
          (*cur != '{' && *cur != '}' && *cur != '\\' && *cur != '%'))
        {
          throw formatter_error("invalid formatter escape sequence",
            std::distance(first,cur));
        }
      }
      else if(*cur == '{') {
        throw formatter_error(
          "formatter clause brackets can only be used with %?",
          std::distance(first,cur));
      }
      else if(*cur == '}')
        return {cur,std::move(result)};

      result.push_back(*cur++);
    }

    if(cur == last)
      return {cur,std::move(result)};

    // cur == '%', if ++cur is end, then invalid replacement seq
    if(++cur == last) {
      throw formatter_error(
        "unexpected end during formatter substitution",
        std::distance(first,cur));
    }

//     std::cerr << "Expanding: '%" << *cur << "'\n";
    if(*cur == '?') {
      if(++cur == last ||
        (*cur != 'K' && *cur != 'V' && *cur != 'I' && *cur != 'E'))
      {
        throw formatter_error(
          "invalid formatter substitution (one of K,V,I,E)",
          std::distance(first,cur));
      }

      bool exists =
        (*cur == 'K' && key_desc) ||
        (*cur == 'V' && value_desc) ||
        (*cur == 'I' && imp_value_desc) ||
        (*cur == 'E' && ext_desc);

      if(++cur == last || *cur != '{') {
        throw formatter_error(
          "%? expansions must be followed by '{' true_expansion '}{'"
            " false_expansion '}'",
          std::distance(first,cur));
      }


      string_type true_clause;
      std::tie(cur,true_clause) = do_expand(first,++cur,last,key_desc,
        value_desc,imp_value_desc,ext_desc,stack);

      if(cur == last || *(++cur) != '{') {
        throw formatter_error(
          "%? expansions must be followed by '{' true_expansion '}{'"
            " false_expansion '}'",
          std::distance(first,cur));
      }

      string_type false_clause;
      std::tie(cur,false_clause) = do_expand(first,++cur,last,key_desc,
        value_desc,imp_value_desc,ext_desc,stack);

      if(cur == last) {
        throw formatter_error(
          "unexpected end during formatter substitution",
          std::distance(first,cur));
      }

      if(exists)
        result.append(true_clause);
      else
        result.append(false_clause);

      ++cur;
//       std::cerr << "Done Expanding: '%" << *cur << "'\n";

    }
    else {
      if(*cur != 'K' && *cur != 'V' && *cur != 'I' && *cur != 'E') {
        throw formatter_error(
          "invalid formatter substitution (one of K,V,I,E)",
          std::distance(first,cur));
      }

      if(std::find(stack.begin(),stack.end(),*cur) != stack.end()) {
        throw formatter_error(
          "recursive formatter_expansion",
          std::distance(first,cur));
      }

      stack.push_back(*cur);

      string_type str;
      if(*cur == 'K' && key_desc)
        str = key_desc();
      else if(*cur == 'V' && value_desc)
        str = value_desc();
      else if(*cur == 'I' && imp_value_desc)
        str = imp_value_desc();
      else if(*cur == 'E' && ext_desc)
        str = ext_desc();

      string_type subs_str;
      try {
        typename string_type::const_iterator subs_end;

        std::tie(subs_end,subs_str) =
          do_expand(str.begin(),str.begin(),str.end(),key_desc,
            value_desc,imp_value_desc,ext_desc,stack);

        if(subs_end != str.end()) {
          throw formatter_error(
            "formatter clause brackets can only be used with %?",
            std::distance(str.cbegin(),subs_end));
        }
      }
      catch (...) {
        std::throw_with_nested(formatter_error("expansion failed",
          std::distance(first,cur)));
      }


      result.append(subs_str);
      stack.pop_back();

//       std::cerr << "Done Expanding: '%" << *cur << "'\n";
      ++cur;
    }

  }

  return {cur,result};
}

/*
  Top-level expansion function

  See do_expand for grammar

  any errors are thrown as formatter_error exceptions
*/
template<typename CharT>
inline std::basic_string<CharT>
expand(const std::basic_string<CharT> &in,
  const basic_option_description<CharT> &desc)
{
  std::vector<CharT> stack;
  auto &&result = do_expand(in.begin(),in.begin(),in.end(),desc.key_description,
    desc.value_description,desc.implicit_value_description,
    desc.extended_description,stack);

  if(result.first != in.end()) {
    throw formatter_error(
      "formatter clause brackets can only be used with %?",
      std::distance(in.begin(),result.first));
  }

  return result.second;
}

}

/*
  Format an option description into a basic_string<CharT>

  \c typeset_description Calls do_typeset_option(desc). Return a string
  containing the fully formatted option or the empty string if the
  option should be hidden.

  \c compare [OPTIONAL] Calls do_compare(). May return an empty
  compare_type to indicate sorting should not be performed
*/
template<typename CharT>
class basic_description_formatter {
  public:
    typedef std::basic_string<CharT> string_type;
    typedef basic_option_description<CharT> description_type;

    typedef std::function<bool(const description_type &,
      const description_type &)> compare_type;

    virtual ~basic_description_formatter(void) {}

    virtual string_type
      typeset_description(const description_type &desc) const = 0;

    virtual compare_type compare(void) const {
      return compare_type();
    }
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
*/
template<typename CharT>
class basic_default_formatter : public basic_description_formatter<CharT> {
  public:
    using typename basic_description_formatter<CharT>::string_type;
    using typename basic_description_formatter<CharT>::description_type;
    using typename basic_description_formatter<CharT>::compare_type;

    string_type typeset_description(const description_type &desc) const;

    void sort_entries(bool val) {
      _should_sort = val;
    }

    bool sort_entries(void) const {
      return _should_sort;
    }

    compare_type compare(void) const {
      if(_should_sort) {
        return [](const description_type &lhs, const description_type &rhs)
          {
            if(!rhs.key_description)
              return true;

            if(!lhs.key_description)
              return false;

            return lhs.key_description() < rhs.key_description();
          };
      }

      return compare_type();
    }

    virtual void key_column_indent(std::size_t n) {
      _key_indent = n;
    }

    virtual std::size_t key_column_indent(void) const {
      return _key_indent;
    }

    virtual void key_column_width(std::size_t n) {
      _key_col_width = n;
    }

    virtual std::size_t key_column_width(void) const {
      return _key_col_width;
    }

    virtual void column_pad(std::size_t n) {
      _col_pad = n;
    }

    virtual std::size_t column_pad(void) const {
      return _col_pad;
    }

    virtual void max_width(std::size_t n) {
      _max_width = n;
    }

    virtual std::size_t max_width(void) const {
      return _max_width;
    }

    virtual string_type wrap(const string_type &str,
      std::size_t width) const
    {
      typedef detail::code_point_traits<CharT> cpoint_traits;
      typedef std::basic_string<typename cpoint_traits::code_point>
            cpstring_type;

      cpstring_type cpextended_desc = cpoint_traits::convert_from(str);
      cpstring_type wrapped_text = detail::wrap(cpextended_desc,width);

      return cpoint_traits::convert_to(wrapped_text);
    }

  private:
    bool _should_sort = false;
    std::size_t _key_indent = 2;
    std::size_t _key_col_width = 24;
    std::size_t _col_pad = 2;
    std::size_t _max_width = 72;
};

/*
  Default formatter provides a comparison function that sorts the
  entries by long names (mapped_keys) and typsets the option description
  into two columns. The first is the summary of the option in one of the
  following forms:
    --foo,f <arg>
    --foo,f <arg>=[implicit]

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
  typeset_description(const description_type &desc) const
{
  if(!desc.key_description)
    return string_type();

  string_type key_col =
    string_type(key_column_indent(),static_cast<CharT>(' ')) +
    detail::expand(desc.key_description(),desc);

  if(key_col.size() > key_column_width()) {
    key_col.push_back('\n');
    key_col.append(key_column_width()+column_pad(),static_cast<CharT>(' '));
  }
  else {
    std::size_t per_pad = key_column_width()+column_pad()-key_col.size();
    key_col.append(per_pad,' ');
  }

  std::size_t indent = key_column_width()+column_pad();
  string_type wrapped_desc;
  if(desc.extended_description) {
    string_type ext_desc_col =
      detail::expand(desc.extended_description(),desc);

    wrapped_desc = wrap(ext_desc_col,max_width()-indent);

    typename string_type::size_type pos =
      wrapped_desc.find(static_cast<CharT>('\n'));
    while(pos != string_type::npos) {
      wrapped_desc.insert(++pos,indent,static_cast<CharT>(' '));
      pos = wrapped_desc.find(static_cast<CharT>('\n'),pos+indent);
    }
  }

  key_col.append(wrapped_desc);

  return key_col;
}

template<typename CharT>
std::basic_string<CharT>
to_string(const std::vector<basic_option_description<CharT> > &grp,
  const basic_description_formatter<CharT> &fmt =
    basic_default_formatter<CharT>())
{
  typedef std::basic_string<CharT> string_type;
  typedef basic_option_description<CharT> description_type;

  string_type out;

  typedef std::pair<const description_type *,string_type> output_pair_type;
  std::vector<output_pair_type> output_grp;
  output_grp.reserve(grp.size());

  for(auto &desc : grp) {
    // hidden means no long or short key descriptions
    if((desc.key_description)) {
      output_grp.emplace_back(
        std::make_pair(&desc,fmt.typeset_description(desc)));
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





#endif

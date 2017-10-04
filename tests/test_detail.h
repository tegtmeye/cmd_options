#ifndef CMD_OPTIONS_TEST_TEST_DETAIL_H
#define CMD_OPTIONS_TEST_TEST_DETAIL_H

#include "cmd_options.h"

namespace lemma {
namespace cmd_options {

template<typename CharT>
bool operator==(const basic_option_pack<CharT> &lhs,
  const basic_option_pack<CharT> &rhs)
{
  return lhs.cease_processing == rhs.cease_processing
    && lhs.value_provided == rhs.value_provided
    && lhs.prefix == rhs.prefix
    && lhs.raw_key == rhs.raw_key
    && lhs.packed_key == rhs.packed_key
    && lhs.value == rhs.value;
}

}
}

namespace co = lemma::cmd_options;

namespace detail {

template<typename CharT>
inline bool is_empty(const co::basic_option_description<CharT> &desc)
{
  return (
    !desc.unpack_option && !desc.mapped_key &&
    !desc.key_description && !desc.extended_description &&
    !desc.implicit_key & !desc.implicit_value &&
    !desc.implicit_value_description && !desc.make_value &&
    !desc.finalize);
}

/*
  Aligned to cmd_options.xlsx starting at row 2
*/
template<typename CharT>
inline bool is_mapped_isolated_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && desc.key_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_hidden_mapped_isolated_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && !desc.key_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_raw_isolated_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && desc.key_description
    && !desc.make_value);
}

/*
  Row 5
*/
template<typename CharT>
inline bool is_hidden_raw_isolated_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && !desc.key_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_mapped_reqired_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && desc.key_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_hidden_mapped_reqired_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && !desc.key_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_raw_reqired_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && desc.key_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_raw_mapped_reqired_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && !desc.key_description
    && desc.make_value && !desc.implicit_value);
}

/*
  Row 10
*/
template<typename CharT>
inline bool is_mapped_optional_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && desc.key_description
    && desc.make_value && desc.implicit_value);
}

template<typename CharT>
inline bool is_hidden_mapped_optional_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && desc.mapped_key && !desc.key_description
    && desc.make_value && desc.implicit_value);
}

template<typename CharT>
inline bool is_raw_optional_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && desc.key_description
    && desc.make_value && desc.implicit_value);
}

template<typename CharT>
inline bool is_raw_mapped_optional_option(
  const co::basic_option_description<CharT> &desc)
{
  return (desc.unpack_option && !desc.mapped_key && !desc.key_description
    && desc.make_value && desc.implicit_value);
}

template<typename CharT>
inline bool is_interpret_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.extended_description
    && desc.make_value && !desc.implicit_value && !desc.implicit_key);
}

/*
  Row 15
*/
template<typename CharT>
inline bool is_hidden_interpret_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.extended_description
    && desc.make_value && !desc.implicit_value && !desc.implicit_key);
}

template<typename CharT>
inline bool is_keyed_interpret_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.extended_description
    && desc.make_value && !desc.implicit_value && desc.implicit_key);
}

template<typename CharT>
inline bool is_hidden_keyed_interpret_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.extended_description
    && desc.make_value && !desc.implicit_value && desc.implicit_key);
}

template<typename CharT>
inline bool is_replace_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.extended_description
    && desc.make_value && desc.implicit_value && !desc.implicit_key);
}

template<typename CharT>
inline bool is_hidden_replace_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.extended_description
    && desc.make_value && desc.implicit_value && !desc.implicit_key);
}

/*
  Row 20
*/
template<typename CharT>
inline bool is_keyed_replace_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.extended_description
    && desc.make_value && desc.implicit_value && desc.implicit_key);
}

template<typename CharT>
inline bool is_hidden_keyed_replace_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.extended_description
    && desc.make_value && desc.implicit_value && desc.implicit_key);
}

template<typename CharT>
inline bool is_empty_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.extended_description
    && !desc.make_value && !desc.implicit_key);
}

template<typename CharT>
inline bool is_hidden_empty_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.extended_description
    && !desc.make_value && !desc.implicit_key);
}

template<typename CharT>
inline bool is_empty_keyed_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.extended_description
    && !desc.make_value && desc.implicit_key);
}

/*
  Row 25
*/
template<typename CharT>
inline bool is_hidden_empty_keyed_positional(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.extended_description
    && !desc.make_value && desc.implicit_key);
}






inline std::string to_string(const co::basic_option_description<char> &desc)
{
  std::stringstream out;
  out
    << "unpack_option: " << bool(desc.unpack_option) << "\n"
    << "mapped_key: " << bool(desc.mapped_key) << "\n"
    << "key_description: " << bool(desc.key_description) << "\n"
    << "extended_description: " << bool(desc.extended_description) << "\n"
    << "implicit_key: " << bool(desc.implicit_key) << "\n"
    << "implicit_value: " << bool(desc.implicit_value) << "\n"
    << "implicit_value_description: "
      << bool(desc.implicit_value_description) << "\n"
    << "make_value: " << bool(desc.make_value) << "\n"
    << "finalize: " << bool(desc.finalize) << "\n";

  return out.str();
}

template<typename T>
std::string to_string(const co::variable_map &vm)
{
  std::stringstream out;
  for(auto && pair : vm) {
    out << pair.first << " -> ";
    if(co::is_empty(pair.second))
      out << "[empty]\n";
    else
      out << co::any_cast<T>(pair.second) << "\n";
  }

  return out.str();
}



template<typename CharT>
bool check_exclusive(const co::basic_option_description<CharT> &desc,
  bool(*fn)(const co::basic_option_description<CharT> &))
{
  typedef bool(*fn_t)(const co::basic_option_description<CharT> &);
  typedef std::pair<std::string,fn_t> pair_type;

  static const std::vector<pair_type> cases{
    {"is_mapped_isolated_option",is_mapped_isolated_option<CharT>},
    {"is_hidden_mapped_isolated_option",
      is_hidden_mapped_isolated_option<CharT>},
    {"is_raw_isolated_option",is_raw_isolated_option<CharT>},
    {"is_hidden_raw_isolated_option",is_hidden_raw_isolated_option<CharT>},
    {"is_mapped_reqired_option",is_mapped_reqired_option<CharT>},
    {"is_hidden_mapped_reqired_option",is_hidden_mapped_reqired_option<CharT>},
    {"is_raw_reqired_option",is_raw_reqired_option<CharT>},
    {"is_raw_mapped_reqired_option",is_raw_mapped_reqired_option<CharT>},
    {"is_mapped_optional_option",is_mapped_optional_option<CharT>},
    {"is_hidden_mapped_optional_option",
      is_hidden_mapped_optional_option<CharT>},
    {"is_raw_optional_option",is_raw_optional_option<CharT>},
    {"is_raw_mapped_optional_option",is_raw_mapped_optional_option<CharT>},
    {"is_interpret_positional",is_interpret_positional<CharT>},
    {"is_hidden_interpret_positional",is_hidden_interpret_positional<CharT>},
    {"is_keyed_interpret_positional",is_keyed_interpret_positional<CharT>},
    {"is_hidden_keyed_interpret_positional",
      is_hidden_keyed_interpret_positional<CharT>},
    {"is_replace_positional",is_replace_positional<CharT>},
    {"is_hidden_replace_positional",is_hidden_replace_positional<CharT>},
    {"is_keyed_replace_positional",is_keyed_replace_positional<CharT>},
    {"is_hidden_keyed_replace_positional",
      is_hidden_keyed_replace_positional<CharT>},
    {"is_empty_positional",is_empty_positional<CharT>},
    {"is_hidden_empty_positional",is_hidden_empty_positional<CharT>},
    {"is_empty_keyed_positional",is_empty_keyed_positional<CharT>},
    {"is_hidden_empty_keyed_positional",is_hidden_empty_keyed_positional<CharT>}
  };

  std::vector<pair_type> exclusive = cases;
  typename std::vector<pair_type>::iterator loc =
    std::remove_if(exclusive.begin(),exclusive.end(),
      [&](const pair_type &val) {
        return val.second == fn;
      }
    );

  assert(loc != exclusive.end());

  exclusive.erase(loc,exclusive.end());

#if 1
  if(!fn(desc)) {
    std::cerr << "Given function returned false\n" << to_string(desc) << "\n";
    return false;
  }

  for(std::size_t i=0; i<exclusive.size(); ++i) {
    if(exclusive[i].second(desc)) {
      std::cerr << "check_exclusive is true for : " << exclusive[i].first
        << "\n" << to_string(desc) << "\n";
      return false;
    }
  }
  return true;
#endif

  return fn(desc) && std::none_of(exclusive.begin(),exclusive.end(),
    [&](const pair_type &val) {return val.second(desc);});
}




}

#endif

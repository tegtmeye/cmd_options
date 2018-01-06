#ifndef CMD_OPTIONS_TEST_TEST_DETAIL_H
#define CMD_OPTIONS_TEST_TEST_DETAIL_H

#include "cmd_options.h"

namespace lemma {
namespace cmd_options {

template<typename CharT>
bool operator==(const basic_option_pack<CharT> &lhs,
  const basic_option_pack<CharT> &rhs)
{
  return lhs.value_provided == rhs.value_provided
    && lhs.prefix == rhs.prefix
    && lhs.raw_key == rhs.raw_key
    && lhs.packed_arguments == rhs.packed_arguments
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
    !desc.implicit_value &&
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

/*
  In the EZ interface for row 14, 15, 18, and 19 mapped key is set to
  allow for position and argument number constraints. That is row 14 and
  row 15 are actually another occurrences of row 16 and row 17 and row
  18 and 19 are another occurrences of row 20 and 21. The mapped key
  simply provides the raw key which is the default behavior if it wasn't
  present. So this check is not used and here for completeness only.
*/
template<typename CharT>
inline bool is_interpret_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.mapped_key && desc.extended_description
    && desc.make_value && !desc.implicit_value);
}

/*
  Row 15
*/
template<typename CharT>
inline bool is_hidden_interpret_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.mapped_key && !desc.extended_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_keyed_interpret_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.mapped_key && desc.extended_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_hidden_keyed_interpret_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.mapped_key && !desc.extended_description
    && desc.make_value && !desc.implicit_value);
}

template<typename CharT>
inline bool is_empty_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.mapped_key && desc.extended_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_hidden_empty_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && !desc.mapped_key && !desc.extended_description
    && !desc.make_value);
}

/*
  Row 20
*/
template<typename CharT>
inline bool is_empty_keyed_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.mapped_key && desc.extended_description
    && !desc.make_value);
}

template<typename CharT>
inline bool is_hidden_empty_keyed_operand(
  const co::basic_option_description<CharT> &desc)
{
  return (!desc.unpack_option && desc.mapped_key && !desc.extended_description
    && !desc.make_value);
}













inline std::string to_string(const co::basic_option_description<char> &desc)
{
  std::stringstream out;
  out
    << "unpack_option: " << bool(desc.unpack_option) << "\n"
    << "mapped_key: " << bool(desc.mapped_key) << "\n"
    << "key_description: " << bool(desc.key_description) << "\n"
    << "extended_description: " << bool(desc.extended_description) << "\n"
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
    {"is_interpret_operand",is_interpret_operand<CharT>},
    {"is_hidden_interpret_operand",is_hidden_interpret_operand<CharT>},
    {"is_keyed_interpret_operand",is_keyed_interpret_operand<CharT>},
    {"is_hidden_keyed_interpret_operand",
      is_hidden_keyed_interpret_operand<CharT>},
    {"is_empty_operand",is_empty_operand<CharT>},
    {"is_hidden_empty_operand",is_hidden_empty_operand<CharT>},
    {"is_empty_keyed_operand",is_empty_keyed_operand<CharT>},
    {"is_hidden_empty_keyed_operand",
      is_hidden_empty_keyed_operand<CharT>}
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

template<typename T, typename VariableMap>
bool contents_equal(const VariableMap &lhs, const VariableMap &rhs)
{
  typename VariableMap::const_iterator lcur = lhs.begin();
  typename VariableMap::const_iterator rcur = rhs.begin();

  while(lcur != lhs.end() && rcur != rhs.end()) {
    if(lcur->first != rcur->first) {
       std::cerr << "lhs vm key '" << lcur->first << "' != rhs vm key '"
         << rcur->first << "'\n";
      return false;
    }

    if(!(co::is_empty(lcur->second) && co::is_empty(rcur->second))) {
      if(co::is_empty(lcur->second) && !co::is_empty(rcur->second)) {
         std::cerr << "lhs vm value for key '" << lcur->first
           << "' is empty but rhs vm value for key '" << rcur->first
           << "' is not\n";
        return false;
      }

      if(!co::is_empty(lcur->second) && co::is_empty(rcur->second)) {
         std::cerr << "lhs vm value for key '" << lcur->first
           << "' is not empty but rhs vm value for key '" << rcur->first
           << "' is\n";
        return false;
      }

      try {
        if(co::any_cast<T>(lcur->second) != co::any_cast<T>(rcur->second))
          return false;
      }
      catch(const co::bad_any_cast &ex) {
        std::cerr << "vm values are not given type T: " << ex.what() << "\n";
        throw;
      }
    }

    ++lcur;
    ++rcur;
  }

  return lcur == lhs.end() && rcur == rhs.end();
}

}

#endif

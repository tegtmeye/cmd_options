#ifndef CMD_OPTIONS_TEST_TEST_DETAIL_H
#define CMD_OPTIONS_TEST_TEST_DETAIL_H

#include "cmd_options.h"

namespace co = lemma::cmd_options;

namespace detail {

template<typename CharT>
inline bool is_empty(const co::basic_option_description<CharT> &desc)
{
  return (
    !desc.map_long_key && !desc.long_key_description &&
    !desc.map_short_key && !desc.short_key_description &&
    !desc.extended_description & !desc.implicit_key &&
    !desc.implicit_value && !desc.implicit_value_description &&
    !desc.make_value && !desc.finalize);
}

template<typename CharT>
inline bool has_visible_long(const co::basic_option_description<CharT> &desc)
{
  return (desc.map_long_key && desc.long_key_description);
}

template<typename CharT>
inline bool has_hidden_long(const co::basic_option_description<CharT> &desc)
{
  return (desc.map_long_key && !desc.long_key_description);
}
template<typename CharT>
inline bool has_visible_short(const co::basic_option_description<CharT> &desc)
{
  return (desc.map_short_key && desc.short_key_description);
}

template<typename CharT>
inline bool has_hidden_short(const co::basic_option_description<CharT> &desc)
{
  return (desc.map_short_key && !desc.short_key_description);
}

template<typename CharT>
inline bool has_implicit_value(const co::basic_option_description<CharT> &desc)
{
  return desc.implicit_value && desc.implicit_value_description;
}

/*
  Aligned to cmd_options.xlsx starting at row 2
*/
template<typename CharT>
inline bool
is_strict_arg_long(const co::basic_option_description<CharT> &desc)
{
  return has_visible_long(desc) && desc.extended_description &&
    !desc.map_short_key && !desc.make_value;
}

template<typename CharT>
inline bool
is_strict_arg_long_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_long(desc) && !desc.map_short_key && !desc.make_value;
}

template<typename CharT>
inline bool
is_strict_arg_short(const co::basic_option_description<CharT> &desc)
{
  return has_visible_short(desc) && desc.extended_description &&
    !desc.map_long_key && !desc.make_value;
}

/*
  Row 5
*/

template<typename CharT>
inline bool
is_strict_arg_short_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_short(desc) && !desc.map_long_key && !desc.make_value;
}

template<typename CharT>
inline bool
is_strict_arg_both(const co::basic_option_description<CharT> &desc)
{
  return has_visible_long(desc) && has_visible_short(desc) &&
    desc.extended_description && !desc.make_value;
}

template<typename CharT>
inline bool
is_strict_arg_both_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_long(desc) && has_hidden_short(desc) &&
    !desc.make_value;
}

template<typename CharT>
inline bool
is_optional_arg_long(const co::basic_option_description<CharT> &desc)
{
  return has_visible_long(desc) && desc.extended_description &&
    !desc.map_short_key && has_implicit_value(desc) && desc.make_value;
}

template<typename CharT>
inline bool
is_optional_arg_long_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_long(desc) && !desc.map_short_key &&
    has_implicit_value(desc) && desc.make_value;
}

/*
  Row 10
*/
template<typename CharT>
inline bool
is_optional_arg_short(const co::basic_option_description<CharT> &desc)
{
  return has_visible_short(desc) && desc.extended_description &&
    !desc.map_long_key && has_implicit_value(desc) && desc.make_value;
}

template<typename CharT>
inline bool
is_optional_arg_short_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_short(desc) && !desc.map_long_key &&
    has_implicit_value(desc) && desc.make_value;
}

template<typename CharT>
inline bool
is_optional_arg_both(const co::basic_option_description<CharT> &desc)
{
  return has_visible_long(desc) && has_visible_short(desc) &&
    desc.extended_description && has_implicit_value(desc) && desc.make_value;
}

template<typename CharT>
inline bool
is_optional_arg_both_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_long(desc) && has_hidden_short(desc) &&
    has_implicit_value(desc) && desc.make_value;
}

template<typename CharT>
inline bool
is_required_arg_long(const co::basic_option_description<CharT> &desc)
{
  return has_visible_long(desc) && desc.extended_description &&
    !desc.map_short_key && !desc.implicit_value && desc.make_value;
}

/*
  Row 15
*/
template<typename CharT>
inline bool
is_required_arg_long_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_long(desc) && !desc.map_short_key &&
    !desc.implicit_value && desc.make_value;
}

template<typename CharT>
inline bool
is_required_arg_short(const co::basic_option_description<CharT> &desc)
{
  return has_visible_short(desc) && desc.extended_description &&
    !desc.map_long_key && !desc.implicit_value && desc.make_value;
}

template<typename CharT>
inline bool
is_required_arg_short_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_short(desc) && !desc.map_long_key &&
    !desc.implicit_value && desc.make_value;
}

template<typename CharT>
inline bool
is_required_arg_both(const co::basic_option_description<CharT> &desc)
{
  return has_visible_long(desc) && has_visible_short(desc) &&
    desc.extended_description && !desc.implicit_value && desc.make_value;
}

template<typename CharT>
inline bool
is_required_arg_both_hidden(const co::basic_option_description<CharT> &desc)
{
  return has_hidden_long(desc) && has_hidden_short(desc) &&
    !desc.implicit_value && desc.make_value;
}

/*
  Row 20
*/
template<typename CharT>
inline bool
is_simple_positional(const co::basic_option_description<CharT> &desc)
{
  return !desc.map_long_key && !desc.map_short_key
    && desc.extended_description && !desc.implicit_key
    && !desc.implicit_value && !desc.make_value;
}

template<typename CharT>
inline bool
is_interpret_positional(const co::basic_option_description<CharT> &desc)
{
  return !desc.map_long_key && !desc.map_short_key
    && desc.extended_description && !desc.implicit_key
    && !desc.implicit_value && desc.make_value;
}

template<typename CharT>
inline bool
is_simple_keyed_positional(const co::basic_option_description<CharT> &desc)
{
  return !desc.map_long_key && !desc.map_short_key
    && desc.extended_description && desc.implicit_key
    && !desc.implicit_value && !desc.make_value;
}

template<typename CharT>
inline bool
is_interpret_keyed_positional(const co::basic_option_description<CharT> &desc)
{
  return !desc.map_long_key && !desc.map_short_key
    && desc.extended_description && desc.implicit_key && !desc.implicit_value
    && desc.make_value;
}

/*
  This is functionally equivalent to the default constructed case
*/
template<typename CharT>
inline bool
is_simple_positional_hidden(const co::basic_option_description<CharT> &desc)
{
  return !desc.map_long_key && !desc.map_short_key
    && !desc.extended_description && !desc.implicit_key
    && !desc.implicit_value && !desc.make_value;
}

/*
  Row 25
*/
template<typename CharT>
inline bool
is_interpret_positional_hidden(const co::basic_option_description<CharT> &desc)
{
  return !desc.map_long_key && !desc.map_short_key
    && !desc.extended_description && !desc.implicit_key
    && !desc.implicit_value && desc.make_value;
}

template<typename CharT>
inline bool
is_simple_keyed_positional_hidden(
  const co::basic_option_description<CharT> &desc)
{
  return !desc.map_long_key && !desc.map_short_key
    && !desc.extended_description && desc.implicit_key
    && !desc.implicit_value && !desc.make_value;
}

template<typename CharT>
inline bool
is_interpret_keyed_positional_hidden(
  const co::basic_option_description<CharT> &desc)
{
  return !desc.map_long_key && !desc.map_short_key
    && !desc.extended_description && desc.implicit_key && !desc.implicit_value
    && desc.make_value;
}





inline std::string to_string(const co::basic_option_description<char> &desc)
{
  std::stringstream out;
  out
    << "map_long_key: " << bool(desc.map_long_key) << "\n"
    << "map_short_key: " << bool(desc.map_short_key) << "\n"
    << "extended_description: " << bool(desc.extended_description) << "\n"
    << "implicit_key: " << bool(desc.implicit_key) << "\n"
    << "implicit_value: " << bool(desc.implicit_value) << "\n"
    << "make_value: " << bool(desc.make_value) << "\n";

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
    {"is_strict_arg_long",is_strict_arg_long<CharT>},
    {"is_strict_arg_long_hidden",is_strict_arg_long_hidden<CharT>},
    {"is_strict_arg_short",is_strict_arg_short<CharT>},
    {"is_strict_arg_short_hidden",is_strict_arg_short_hidden<CharT>},
    {"is_strict_arg_both",is_strict_arg_both<CharT>},
    {"is_strict_arg_both_hidden",is_strict_arg_both_hidden<CharT>},
    {"is_optional_arg_long",is_optional_arg_long<CharT>},
    {"is_optional_arg_long_hidden",is_optional_arg_long_hidden<CharT>},
    {"is_optional_arg_short",is_optional_arg_short<CharT>},
    {"is_optional_arg_short_hidden",is_optional_arg_short_hidden<CharT>},
    {"is_optional_arg_both",is_optional_arg_both<CharT>},
    {"is_optional_arg_both_hidden",is_optional_arg_both_hidden<CharT>},
    {"is_required_arg_long",is_required_arg_long<CharT>},
    {"is_required_arg_long_hidden",is_required_arg_long_hidden<CharT>},
    {"is_required_arg_short",is_required_arg_short<CharT>},
    {"is_required_arg_short_hidden",is_required_arg_short_hidden<CharT>},
    {"is_required_arg_both",is_required_arg_both<CharT>},
    {"is_required_arg_both_hidden",is_required_arg_both_hidden<CharT>},
    {"is_simple_positional",is_simple_positional<CharT>},
    {"is_interpret_positional",is_interpret_positional<CharT>},
    {"is_simple_keyed_positional",is_simple_keyed_positional<CharT>},
    {"is_interpret_keyed_positional",is_interpret_keyed_positional<CharT>},
    {"is_simple_positional_hidden",is_simple_positional_hidden<CharT>},
    {"is_interpret_positional_hidden",is_interpret_positional_hidden<CharT>},
    {"is_simple_keyed_positional_hidden",
      is_simple_keyed_positional_hidden<CharT>},
    {"is_interpret_keyed_positional_hidden",
      is_interpret_keyed_positional_hidden<CharT>}
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

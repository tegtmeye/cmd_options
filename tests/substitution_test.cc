#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

/**
  substitution parser test
 */

BOOST_AUTO_TEST_SUITE( substitution_test_suite )

namespace co = cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;

/**
  option no restriction given 0
 */
BOOST_AUTO_TEST_CASE( empty_substitution_test )
{
  string_type str = "";
  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),_LIT_FN("VALUE"),_LIT_FN("IMPLICIT"),_LIT_FN("EXTENDED"),
    stack);

  BOOST_REQUIRE(result.first == str.end() && result.second == "");
}

BOOST_AUTO_TEST_CASE( no_substitution_test )
{
  string_type str = "abcdefghijklmnopqrstuvwxyz";
  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),_LIT_FN("VALUE"),_LIT_FN("IMPLICIT"),_LIT_FN("EXTENDED"),
    stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "abcdefghijklmnopqrstuvwxyz");
}

BOOST_AUTO_TEST_CASE( leaf_substitution_test )
{
  string_type str = "%Kabcde%Vfghijklmno%Ipqrstuvwxyz%E";
  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),_LIT_FN("VALUE"),_LIT_FN("IMPLICIT"),_LIT_FN("EXTENDED"),
    stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "KEYabcdeVALUEfghijklmnoIMPLICITpqrstuvwxyzEXTENDED");
}

BOOST_AUTO_TEST_CASE( nested_substitution_test )
{
  string_type str = "%Kabcde%Vfghijklmno%Ipqrstuvwxyz%E";
  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),_LIT_FN("VALUE__%K__"),_LIT_FN("IMPLICIT__%V__"),
    _LIT_FN("EXTENDED__%I__"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "KEYabcdeVALUE__KEY__fghijklmnoIMPLICIT__VALUE__KEY____"
      "pqrstuvwxyzEXTENDED__IMPLICIT__VALUE__KEY______");
}

BOOST_AUTO_TEST_CASE( empty_conditional_substitution_test )
{
  string_type str = "%?V{}{}%?I{}{}";
  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),_LIT_FN("VALUE"),{nullptr},_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second.empty());
}

BOOST_AUTO_TEST_CASE( conditional_substitution_test )
{
  string_type str = "%?V{TRUE}{FALSE}%?I{TRUE}{FALSE}";
  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),_LIT_FN("VALUE"),{nullptr},_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "TRUEFALSE");
}

BOOST_AUTO_TEST_CASE( conditional_nested_leaf_substitution_test )
{
  string_type str = "%?V{true_%K}{%E}%?I{%E}{false_%K}";
  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),_LIT_FN("VALUE"),{nullptr},_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "true_KEYfalse_KEY");
}

BOOST_AUTO_TEST_CASE( nested_conditional_substitution_test1 )
{
  string_type str =
    "%?V{true_VALUE_%?K{true_KEY}{false_KEY}}"
    "{false_VALUE_%?I{true_IMPLICIT}{false_IMPLICIT}}";

  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),_LIT_FN("VALUE"),{nullptr},_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "true_VALUE_true_KEY");
}

BOOST_AUTO_TEST_CASE( nested_conditional_substitution_test2 )
{
  string_type str =
    "%?V{true_VALUE_%?K{true_KEY}{false_KEY}}"
    "{false_VALUE_%?I{true_IMPLICIT}{false_IMPLICIT}}";

  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    {nullptr},_LIT_FN("VALUE"),{nullptr},_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "true_VALUE_false_KEY");
}

BOOST_AUTO_TEST_CASE( nested_conditional_substitution_test3 )
{
  string_type str =
    "%?V{true_VALUE_%?K{true_KEY}{false_KEY}}"
    "{false_VALUE_%?I{true_IMPLICIT}{false_IMPLICIT}}";

  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),{nullptr},_LIT_FN("IMPLICIT"),_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "false_VALUE_true_IMPLICIT");
}

BOOST_AUTO_TEST_CASE( nested_conditional_substitution_test4 )
{
  string_type str =
    "%?V{true_VALUE_%?K{true_KEY}{false_KEY}}"
    "{false_VALUE_%?I{true_IMPLICIT}{false_IMPLICIT}}";

  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),{nullptr},{nullptr},_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "false_VALUE_false_IMPLICIT");
}

BOOST_AUTO_TEST_CASE( nested_conditional_empty_substitution_test )
{
  string_type str =
    "%?V{true_VALUE_%?K{}{}}"
    "{false_VALUE_%?I{}{}}";

  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),{nullptr},{nullptr},_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "false_VALUE_");
}

BOOST_AUTO_TEST_CASE( nested_conditional_nested_leaf_substitution_test )
{
  string_type str =
    "%?V{true_VALUE_%?K{true_KEY_(%E)}{false_KEY_(%E)}}"
    "{false_VALUE_%?I{true_IMPLICIT_(%E)}{false_IMPLICIT_(%E)}}";

  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("KEY"),{nullptr},{nullptr},_LIT_FN("EXTENDED"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "false_VALUE_false_IMPLICIT_(EXTENDED)");
}

BOOST_AUTO_TEST_CASE( recursive_substitution_test1 )
{
  string_type str = "%E";
  std::vector<detail::check_char_t> stack;

  BOOST_CHECK_EXCEPTION(
    co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("%E"),_LIT_FN("VALUE__%K__"),_LIT_FN("IMPLICIT__%V__"),
    _LIT_FN("EXTENDED__%E__"),stack),

  co::formatter_error, [](const co::formatter_error &ex) {
      return (ex.position() == 1);
    }
  );
}

BOOST_AUTO_TEST_CASE( recursive_substitution_test2 )
{
  string_type str = "abcdefghijk%Elmnopqrstuvwxyz";
  std::vector<detail::check_char_t> stack;

  BOOST_CHECK_EXCEPTION(
    co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("%E"),_LIT_FN("VALUE__%K__"),_LIT_FN("IMPLICIT__%V__"),
    _LIT_FN("EXTENDED__%I__"),stack),

  co::formatter_error, [](const co::formatter_error &ex) {
      return (ex.position() == 12);
    }
  );
}

BOOST_AUTO_TEST_CASE( default_EZ_substitution_test )
{
  string_type str = "%K  %E";
  std::vector<detail::check_char_t> stack;

  auto &&result = co::detail::do_expand(str.begin(),str.begin(),str.end(),
    _LIT_FN("--foo%?V{=<%V>%?I{[%I]}{}}{}"),_LIT_FN("arg"),_LIT_FN("implicit"),
    _LIT_FN("Extended description of --foo"),stack);

  BOOST_REQUIRE(result.first == str.end() &&
    result.second == "--foo=<arg>[implicit]  Extended description of --foo");
}



BOOST_AUTO_TEST_SUITE_END()


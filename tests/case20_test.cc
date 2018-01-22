#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 20, Degenerate operand with implicit key
 */

BOOST_AUTO_TEST_SUITE( case18_test_suite )


namespace co = lemma::cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;

/**
  All keys values
 */
BOOST_AUTO_TEST_CASE( all_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("-f"),
    _LIT("-f42"),
    _LIT("--bar"),
    _LIT("--bar=43")
  };

  options = options_group_type{
    co::make_operand(_LIT("operand_key"),_LIT("case 20"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("operand_key"),{}},
      {_LIT("operand_key"),{}},
      {_LIT("operand_key"),{}},
      {_LIT("operand_key"),{}}
    }));
}


/**
  keys values with embedded operand
 */
BOOST_AUTO_TEST_CASE( key_value_with_embedded_operand_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("-f"),
    _LIT("f_arg"),
    _LIT("operand1"),
    _LIT("-f42"),
    _LIT("operand2"),
    _LIT("operand3"),
    _LIT("--bar"),
    _LIT("bar_arg"),
    _LIT("operand4"),
    _LIT("--bar=43"),
    _LIT("operand5"),
  };

  options = options_group_type{
    co::make_option(_LIT(""),
      co::value<string_type>().implicit(_LIT("floo")),_LIT("case 20")),
    co::make_operand(_LIT("operand_key"),_LIT("case 20"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("operand_key"),{}},
      {_LIT("operand_key"),{}},
      {_LIT("operand_key"),{}},
      {_LIT("operand_key"),{}},
      {_LIT("operand_key"),{}},
      {_LIT("f"),{string_type(_LIT("f_arg"))}},
      {_LIT("f"),{string_type(_LIT("42"))}},
      {_LIT("bar"),{string_type(_LIT("bar_arg"))}},
      {_LIT("bar"),{string_type(_LIT("43"))}}
    }));
}



BOOST_AUTO_TEST_SUITE_END()


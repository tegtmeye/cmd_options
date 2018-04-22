#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  Accept all options and operands, need to check these before
  individual cases.
 */

BOOST_AUTO_TEST_SUITE( accept_test_suite )


namespace co = cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;

/**
  All flags
 */
BOOST_AUTO_TEST_CASE( all_flags_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("--foo"),
    _LIT("-f")
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}},
      {_LIT("foo"),{}}
    }));
}

/**
  Callback Assignment
 */
BOOST_AUTO_TEST_CASE( callback_assignment_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("--foo"),
    _LIT("bar")
  };

  string_type arg{_LIT("*****")};
  options = options_group_type{
    co::make_option(_LIT("foo,f"),co::value<string_type>(&arg),
      _LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{string_type(_LIT("bar"))}}
    })
    && arg == _LIT("bar"));
}

/**
  Callback function
 */
BOOST_AUTO_TEST_CASE( callback_function_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("--foo"),
    _LIT("42")
  };

  int n = 99;
  options = options_group_type{
    co::make_option(_LIT("foo,f"),co::value<int>([&](int _n){n=_n+1;}),
      _LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::contents_equal<int>(vm,
    variable_map_type{
      {_LIT("foo"),{42}}
    })
    && n == 43);
}

BOOST_AUTO_TEST_SUITE_END()

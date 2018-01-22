#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 9, Raw keys with strictly required value
 */

BOOST_AUTO_TEST_SUITE( case9_test_suite )


namespace co = lemma::cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;

/**
  All POSIX key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_POSIX_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("-f"),
    _LIT("bar"),
    _LIT("-frab"),
    _LIT("-f bar")
  };

  options =   {
    co::make_hidden_option(_LIT(""),co::value<string_type>())
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("f"),{string_type(_LIT("bar"))}},
      {_LIT("f"),{string_type(_LIT("rab"))}},
      {_LIT("f"),{string_type(_LIT(" bar"))}}
    }));
}

/**
  All GNU key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_GNU_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo=bar"),
    _LIT("--foo"),
    _LIT("rab")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT(""),co::value<string_type>())
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{string_type(_LIT("bar"))}},
      {_LIT("foo"),{string_type(_LIT("rab"))}}
    }));
}

/**
  All invalid POSIX key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_invalid_POSIX_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("-f bar"),
    _LIT("-f"),
    _LIT("-b")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT(""),co::value<string_type>())
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::missing_argument_error);
}

/**
  Invalid GNU key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_invalid_GNU_key_value_test1 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo bar")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT(""),co::value<string_type>())
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::missing_argument_error);
}

/**
  Invalid GNU key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_invalid_GNU_key_value_test2 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("--bar")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT(""),co::value<string_type>())
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::missing_argument_error);
}




BOOST_AUTO_TEST_SUITE_END()


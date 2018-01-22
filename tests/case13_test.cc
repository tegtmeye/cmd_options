#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 13, Hidden raw keys with optional value
 */

BOOST_AUTO_TEST_SUITE( case13_test_suite )


namespace co = lemma::cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;

/**
  All POSIX keys
 */
BOOST_AUTO_TEST_CASE( all_POSIX_key_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("-f"),
    _LIT("-b")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT(""),
      co::value<string_type>().implicit(_LIT("floo")))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("f"),{string_type(_LIT("floo"))}},
      {_LIT("b"),{string_type(_LIT("floo"))}}
    }));
}

/**
  All GNU keys
 */
BOOST_AUTO_TEST_CASE( all_GNU_key_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("--bar")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT(""),co::value<string_type>().implicit(_LIT("floo")))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{string_type(_LIT("floo"))}},
      {_LIT("bar"),{string_type(_LIT("floo"))}}
    }));
}

/**
  All POSIX keys with values
 */
BOOST_AUTO_TEST_CASE( all_POSIX_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("-ffloopy"),
    _LIT("-b"),
    _LIT("bloppy")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT(""),
      co::value<string_type>().implicit(_LIT("floo")))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("f"),{string_type(_LIT("floopy"))}},
      {_LIT("b"),{string_type(_LIT("bloppy"))}}
    }));
}

/**
  All GNU keys with values
 */
BOOST_AUTO_TEST_CASE( all_GNU_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo=floopy"),
    _LIT("--bar"),
    _LIT("bloppy")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT(""),
      co::value<string_type>().implicit(_LIT("floo")))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{string_type(_LIT("floopy"))}},
      {_LIT("bar"),{string_type(_LIT("bloppy"))}}
    }));
}



BOOST_AUTO_TEST_SUITE_END()


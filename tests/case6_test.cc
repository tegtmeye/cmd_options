#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 6, Raw keys with strictly no value
 */

BOOST_AUTO_TEST_SUITE( case6_test_suite )


namespace co = lemma::cmd_options;

/**
  All POSIX key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_POSIX_key_value_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-f",
    "bar",
    "-frab",
    "-f bar"
  };

  options = co::options_group{
    co::make_option("foo,f",co::value<std::string>(),"case 6")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"foo",{std::string("rab")}},
      {"foo",{std::string(" bar")}}
    }));
}

/**
  All GNU key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_GNU_key_value_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo=bar",
    "--foo",
    "rab"
  };

  options = co::options_group{
    co::make_option("foo,f",co::value<std::string>(),"case 6")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"foo",{std::string("rab")}}
    }));
}

/**
  All invalid POSIX key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_invalid_POSIX_key_value_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-f bar",
    "-f",
    "-b"
  };

  options = co::options_group{
    co::make_option("foo,f",co::value<std::string>(),"case 6")
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
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo bar"
  };

  options = co::options_group{
    co::make_option("foo,f",co::value<std::string>(),"case 6")
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::unknown_option_error);
}

/**
  Invalid GNU key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_invalid_GNU_key_value_test2 )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "--bar"
  };

  options = co::options_group{
    co::make_option("foo,f",co::value<std::string>(),"case 6")
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::missing_argument_error);
}




BOOST_AUTO_TEST_SUITE_END()


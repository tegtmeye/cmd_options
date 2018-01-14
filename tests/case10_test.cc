#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 10, Raw keys with strictly required value
 */

BOOST_AUTO_TEST_SUITE( case10_test_suite )


namespace co = lemma::cmd_options;

/**
  All POSIX keys
 */
BOOST_AUTO_TEST_CASE( all_POSIX_key_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-f",
    "-b"
  };

  options = co::options_group{
    co::make_option(",f",co::value<std::string>().implicit("floo"),"case 10"),
    co::make_option(",b",co::value<std::string>().implicit("blar"),"case 10")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"f",{std::string("floo")}},
      {"b",{std::string("blar")}}
    }));
}

/**
  All GNU keys
 */
BOOST_AUTO_TEST_CASE( all_GNU_key_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "--bar"
  };

  options = co::options_group{
    co::make_option("foo,f",co::value<std::string>().implicit("floo"),
      "case 10"),
    co::make_option("bar,b",co::value<std::string>().implicit("blar"),"case 10")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("floo")}},
      {"bar",{std::string("blar")}}
    }));
}

/**
  All POSIX keys with values
 */
BOOST_AUTO_TEST_CASE( all_POSIX_key_value_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-ffloopy",
    "-b",
    "bloppy"
  };

  options = co::options_group{
    co::make_option("foo,f",co::value<std::string>().implicit("floo"),
      "case 10"),
    co::make_option("bar,b",co::value<std::string>().implicit("blar"),"case 10")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("floopy")}},
      {"bar",{std::string("bloppy")}}
    }));
}

/**
  All GNU keys with values
 */
BOOST_AUTO_TEST_CASE( all_GNU_key_value_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo=floopy",
    "--bar",
    "bloppy"
  };

  options = co::options_group{
    co::make_option("foo,f",co::value<std::string>().implicit("floo"),
      "case 10"),
    co::make_option("bar,b",co::value<std::string>().implicit("blar"),"case 10")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("floopy")}},
      {"bar",{std::string("bloppy")}}
    }));
}



BOOST_AUTO_TEST_SUITE_END()


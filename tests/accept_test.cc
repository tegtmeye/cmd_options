#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  Accept all options and positionals, need to check these before
  individual cases.
 */

BOOST_AUTO_TEST_SUITE( accept_test_suite )


namespace co = lemma::cmd_options;

/**
  All options
 */
BOOST_AUTO_TEST_CASE( all_flags_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f"
  };

  options = co::options_group{
    co::make_option("foo,f","case 2")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{}},
      {"foo",{}}
    }));
}

BOOST_AUTO_TEST_SUITE_END()

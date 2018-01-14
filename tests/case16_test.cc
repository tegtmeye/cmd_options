#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 16, operand
 */

BOOST_AUTO_TEST_SUITE( case16_test_suite )


namespace co = lemma::cmd_options;

/**
  All keys values
 */
BOOST_AUTO_TEST_CASE( all_key_value_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-f",
    "-f42",
    "--bar",
    "--bar=43"
  };

  options = co::options_group{
    co::make_operand("key","case 14",co::value<std::string>())
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key",{std::string("-f")}},
      {"key",{std::string("-f42")}},
      {"key",{std::string("--bar")}},
      {"key",{std::string("--bar=43")}},
    }));
}


/**
  keys values with embedded operand
 */
BOOST_AUTO_TEST_CASE( key_value_with_embedded_operand_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-f",
    "f_arg",
    "operand1",
    "-f42",
    "operand2",
    "operand3",
    "--bar",
    "bar_arg",
    "operand4",
    "--bar=43",
    "operand5",
  };

  options = co::options_group{
    co::make_option("",co::value<std::string>().implicit("floo"),"case 14"),
    co::make_operand("key","case 14",co::value<std::string>())
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key",{std::string("operand1")}},
      {"key",{std::string("operand2")}},
      {"key",{std::string("operand3")}},
      {"key",{std::string("operand4")}},
      {"key",{std::string("operand5")}},
      {"f",{std::string("f_arg")}},
      {"f",{std::string("42")}},
      {"bar",{std::string("bar_arg")}},
      {"bar",{std::string("43")}}
    }));
}



BOOST_AUTO_TEST_SUITE_END()


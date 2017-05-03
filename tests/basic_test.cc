#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( basic_test_suite )

namespace co = lemma::cmd_options;



/**

 */
BOOST_AUTO_TEST_CASE( default_description_test )
{
  co::option_description desc;
  BOOST_REQUIRE(detail::is_empty(desc));

  BOOST_REQUIRE(
    detail::check_exclusive(desc,detail::is_simple_positional_hidden<char>));
}

BOOST_AUTO_TEST_CASE( bo_constraint_description_construction_test )
{
  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo","line 2"),
    detail::is_strict_arg_long<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo"), // line 3
    detail::is_strict_arg_long_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(",f","line 4"),
    detail::is_strict_arg_short<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option(",f"), // line 5
    detail::is_strict_arg_short_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo,f","line 6"),
    detail::is_strict_arg_both<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo,f"), // line 7
    detail::is_strict_arg_both_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo",co::value<std::string>("bar"),"line 8"),
    detail::is_optional_arg_long<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo",co::value<std::string>("bar")), // line 9
    detail::is_optional_arg_long_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(",f",co::value<std::string>("bar"),"line 10"),
    detail::is_optional_arg_short<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option(",f",co::value<std::string>("bar")), // line 11
    detail::is_optional_arg_short_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo,f",co::value<std::string>("bar"),"line 12"),
    detail::is_optional_arg_both<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo,f",co::value<std::string>("bar")), // line 13
    detail::is_optional_arg_both_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo",co::value<std::string>(),"line 14"),
    detail::is_required_arg_long<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo",co::value<std::string>()), // line 15
    detail::is_required_arg_long_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(",f",co::value<std::string>(),"line 16"),
    detail::is_required_arg_short<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option(",f",co::value<std::string>()), // line 17
    detail::is_required_arg_short_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo,f",co::value<std::string>(),"line 18"),
    detail::is_required_arg_both<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo,f",co::value<std::string>()), // line 19
    detail::is_required_arg_both_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_positional("positional"), // line 20
    detail::is_simple_positional<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_positional("positional",co::value<std::string>()), // line 21
    detail::is_interpret_positional<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_positional("positional","implicit_key"), // line 22
    detail::is_simple_keyed_positional<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_positional("positional",co::value<std::string>(), // line 23
      "implicit_key"),
    detail::is_interpret_keyed_positional<char>));

  BOOST_REQUIRE(detail::check_exclusive(co::make_hidden_positional(), // line 24
    detail::is_simple_positional_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_positional(co::value<std::string>()), // line 25
    detail::is_interpret_positional_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_positional("implicit_key"), // line 26
    detail::is_simple_keyed_positional_hidden<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_positional(
      co::value<std::string>(),"implicit_key"), // line 27
    detail::is_interpret_keyed_positional_hidden<char>));
}

/**

 */
#if 0
BOOST_AUTO_TEST_CASE( is_strict_arg_long )
{
  try {
    co::options_group main_options{
      co::make_option(",f",co::value<std::string>().occurrences(2),
        "Requires an argument and must be specified exactly twice."),
      co::make_option("bar/b",co::value<std::string>(),
        "Not required but if given, must have an argument.",'/'),
      co::make_option("foob","o",co::value<std::string>().occurrences(2,-1),
        "Requires an argument and must be specified at least twice."),
      co::make_option("bar","b",
        co::value<std::string>().occurrences(1).implicit_value("_bar_"),
        "Must be specified once but the argument is optional."),
      co::make_option("zap","z",co::value<std::string>(),
        "Not required but if given, must have an argument."),
      co::make_positional(co::value<std::string>(),"positional"),
      co::make_option("no,n","Must not have an argument.")
    };



    //std::cerr << "\n\n" << to_string_debug(main_options.back()) << "\n\n";
    std::cout << "Usage: " << argv[0] << "[OPTIONS]" << "\n"
      << "Main Options:\n\n"
      << to_string(main_options) << "\n\n";


    co::variable_map vm = co::parse_arguments(argv+1,argc-1,main_options);

    for(auto &pair : vm) {
      std::cerr << "key: [" << pair.first << "] value: ";
      if(!pair.second.empty())
        std::cerr << "[" << boost::any_cast<std::string>(pair.second) << "]\n";
      else
        std::cerr << "[empty]\n";
    }
  }
  catch(const co::command_option_error &ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    BOOST_REQUIRE(false);
  }
}
#endif

BOOST_AUTO_TEST_SUITE_END()


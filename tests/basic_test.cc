#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( basic_test_suite )

namespace co = lemma::cmd_options;



/**
  Test to ensure a default constructed option_description is empty
 */
BOOST_AUTO_TEST_CASE( default_description_test )
{
  co::option_description desc;
  BOOST_REQUIRE(detail::is_empty(desc));
}

/**
  Check to ensure that a appropriately formed 'make_option' call produces a
  correctly formed option_description
 */
BOOST_AUTO_TEST_CASE( constraint_description_construction_test )
{
  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo","line 2"),
    detail::is_mapped_isolated_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo"), // line 3
    detail::is_hidden_mapped_isolated_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("","line 4"),
    detail::is_raw_isolated_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option(""), // line 5
    detail::is_hidden_raw_isolated_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo",co::value<std::string>(),"line 6"),
    detail::is_mapped_reqired_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo",co::value<std::string>()), // line 7
    detail::is_hidden_mapped_reqired_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("",co::value<std::string>(),"line 8"),
    detail::is_raw_reqired_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("",co::value<std::string>()), // line 9
    detail::is_raw_mapped_reqired_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("foo",co::value<std::string>("bar"),"line 10"),
    detail::is_mapped_optional_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("foo",co::value<std::string>("bar")), // line 11
    detail::is_hidden_mapped_optional_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option("",co::value<std::string>("bar"),"line 12"),
    detail::is_raw_optional_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option("",co::value<std::string>("bar")), // line 13
    detail::is_raw_mapped_optional_option<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand<std::string>("line 14"),
    detail::is_interpret_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand<std::string>(), // line 15
    detail::is_hidden_interpret_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand<std::string>("key","line 16"),
    detail::is_keyed_interpret_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand<std::string>("key"), // line 17
    detail::is_hidden_keyed_interpret_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand("line 18"),
    detail::is_empty_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand(), // line 19
    detail::is_hidden_empty_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand("key","line 20"),
    detail::is_empty_keyed_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand("key"), // line 21
    detail::is_hidden_empty_keyed_operand<char>));
}

/**
  Check for proper POSIX option flag unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_posix_flag_test )
{
  typedef co::basic_option_pack<char> option_pack;

  BOOST_REQUIRE_MESSAGE((co::unpack_posix<true,char>("") == option_pack()),
    "unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE((co::unpack_posix<true,char>("-f") ==
    option_pack{true,false,"-","f",{},{}}));

  // orphan flag
  BOOST_REQUIRE((co::unpack_posix<true,char>("-") == option_pack()));

  // packed flags
  BOOST_REQUIRE((co::unpack_posix<true,char>("-fbar") ==
    option_pack{true,false,"-","f",{"-b","-a","-r"}}));

  // packed flag with trailing extra
  BOOST_REQUIRE((co::unpack_posix<true,char>("-f bar") ==
    option_pack{true,false,"-","f",{"- ","-b","-a","-r"}}));

  // cease flag
  BOOST_REQUIRE((co::unpack_posix<true,char>("--") ==
    option_pack{true,false,"-","-"}));

  // cease flag with extra chars
  BOOST_REQUIRE((co::unpack_posix<true,char>("--blah") ==
    option_pack{true,false,"-","-",{"-b","-l","-a","-h"}}));

  // packed with embedded 'end of options'
  BOOST_REQUIRE((co::unpack_posix<true,char>("-fb--ar") ==
    option_pack{true,false,"-","f",{"-b","--","--","-a","-r"}}));
}

/**
  Check for proper POSIX option with argument unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_posix_arg_test )
{
  typedef co::basic_option_pack<char> option_pack;

  BOOST_REQUIRE_MESSAGE((co::unpack_posix<false,char>("") == option_pack()),
    "unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE((co::unpack_posix<false,char>("-f") ==
    option_pack{true,false,"-","f",{},{}}));

  // orphan flag
  BOOST_REQUIRE((co::unpack_posix<false,char>("-") == option_pack()));

  // packed flags
  BOOST_REQUIRE((co::unpack_posix<false,char>("-fbar") ==
    option_pack{true,true,"-","f",{},{"bar"}}));

  // packed flag with trailing extra
  BOOST_REQUIRE((co::unpack_posix<false,char>("-f bar") ==
    option_pack{true,true,"-","f",{},{" bar"}}));

  // cease flag
  BOOST_REQUIRE((co::unpack_posix<false,char>("--") ==
    option_pack{true,false,"-","-"}));

  // cease flag with extra chars
  BOOST_REQUIRE((co::unpack_posix<false,char>("--blah") ==
    option_pack{true,true,"-","-",{},{"blah"}}));
}

/**
  Check for proper GNU option flag unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_gnu_flag_test )
{
  typedef co::basic_option_pack<char> option_pack;

  BOOST_REQUIRE_MESSAGE((co::unpack_gnu<true,char>("") == option_pack()),
    "unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE((co::unpack_gnu<true,char>("-f") ==
    option_pack{true,false,"-","f",{},{}}));

  // orphan flag
  BOOST_REQUIRE((co::unpack_gnu<true,char>("-") == option_pack()));

  // packed flags
  BOOST_REQUIRE((co::unpack_gnu<true,char>("-fbar") ==
    option_pack{true,false,"-","f",{"-b","-a","-r"}}));

  // packed flag with trailing extra
  BOOST_REQUIRE((co::unpack_gnu<true,char>("-f bar") ==
    option_pack{true,false,"-","f",{"- ","-b","-a","-r"}}));

  // cease flag
  BOOST_REQUIRE((co::unpack_gnu<true,char>("--") ==
    option_pack{true,false,"-","-"}));

  // long flag
  BOOST_REQUIRE((co::unpack_gnu<true,char>("--foo") ==
    option_pack{true,false,"--","foo",{},{}}));

  // long flag with embedded cease
  BOOST_REQUIRE((co::unpack_gnu<true,char>("--foo--bar") ==
    option_pack{true,false,"--","foo--bar",{},{}}));

  // long flag w/value
  BOOST_REQUIRE((co::unpack_gnu<true,char>("--foo=bar") ==
    option_pack{true,true,"--","foo",{},"bar"}));

  // long flag with leading extra w/value
  BOOST_REQUIRE((co::unpack_gnu<true,char>("--foo =bar  ") ==
    option_pack{true,true,"--","foo ",{},"bar  "}));

  // long flag with trailing extra w/value
  BOOST_REQUIRE((co::unpack_gnu<true,char>("--foo= bar  ") ==
    option_pack{true,true,"--","foo",{}," bar  "}));

  // long flag with leading and trailing extra w/value
  BOOST_REQUIRE((co::unpack_gnu<true,char>("--foo = bar  ") ==
    option_pack{true,true,"--","foo ",{}," bar  "}));

  // long flag w/cease value
  BOOST_REQUIRE((co::unpack_gnu<true,char>("--foo=--") ==
    option_pack{true,true,"--","foo",{},"--"}));
}

/**
  Check for proper GNU option with argument unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_gnu_arg_test )
{
  typedef co::basic_option_pack<char> option_pack;

  BOOST_REQUIRE_MESSAGE((co::unpack_gnu<false,char>("") == option_pack()),
    "unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE((co::unpack_gnu<false,char>("-f") ==
    option_pack{true,false,"-","f",{},{}}));

  // orphan flag
  BOOST_REQUIRE((co::unpack_gnu<false,char>("-") == option_pack()));

  // packed flags
  BOOST_REQUIRE((co::unpack_gnu<false,char>("-fbar") ==
    option_pack{true,true,"-","f",{},{"bar"}}));

  // packed flag with trailing extra
  BOOST_REQUIRE((co::unpack_gnu<false,char>("-f bar") ==
    option_pack{true,true,"-","f",{},{" bar"}}));

  // cease flag
  BOOST_REQUIRE((co::unpack_gnu<false,char>("--") ==
    option_pack{true,false,"-","-"}));

  // long flag w/value
  BOOST_REQUIRE((co::unpack_gnu<false,char>("--foo=bar") ==
    option_pack{true,true,"--","foo",{},"bar"}));

  // long flag with leading extra w/value
  BOOST_REQUIRE((co::unpack_gnu<false,char>("--foo =bar  ") ==
    option_pack{true,true,"--","foo ",{},"bar  "}));

  // long flag with trailing extra w/value
  BOOST_REQUIRE((co::unpack_gnu<false,char>("--foo= bar  ") ==
    option_pack{true,true,"--","foo",{}," bar  "}));

  // long flag with leading and trailing extra w/value
  BOOST_REQUIRE((co::unpack_gnu<false,char>("--foo = bar  ") ==
    option_pack{true,true,"--","foo ",{}," bar  "}));

  // long flag w/cease value
  BOOST_REQUIRE((co::unpack_gnu<false,char>("--foo=--") ==
    option_pack{true,true,"--","foo",{},"--"}));
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
      co::make_operand(co::value<std::string>(),"operand"),
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


#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( basic_test_suite )

namespace co = lemma::cmd_options;

typedef std::basic_string<char> string_type;
typedef co::basic_option_pack<char> option_pack;
typedef co::basic_option_description<char> option_description;
typedef co::basic_variable_map<char> variable_map;


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

  /*
    In the EZ interface for row 14 and row 15, mapped key is set to allow
    for position and argument number constraints. That is row 14 and row
    15 are actually another occurrences of row 16 and row 17. The mapped
    key simply provides the raw key which is the default behavior if it
    wasn't present.
  */
  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand<std::string>("line 14"),
    detail::is_keyed_interpret_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand<std::string>(), // line 15
    detail::is_hidden_keyed_interpret_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand<std::string>("key","line 16"),
    detail::is_keyed_interpret_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand<std::string>("key"), // line 17
    detail::is_hidden_keyed_interpret_operand<char>));

  /*
    In the EZ interface for row 18 and row 19, mapped key is set to allow
    for position and argument number constraints. That is row 18 and row
    19 are actually another occurrences of row 20 and row 21. The mapped
    key simply provides the raw key which is the default behavior if it
    wasn't present.
  */
  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand("line 18"),
    detail::is_empty_keyed_operand<char>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand(), // line 19
    detail::is_hidden_empty_keyed_operand<char>));

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


BOOST_AUTO_TEST_SUITE_END()


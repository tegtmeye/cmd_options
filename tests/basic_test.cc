#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( basic_test_suite )

namespace co = lemma::cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack;
typedef co::basic_option_description<detail::check_char_t> option_description;
typedef co::basic_variable_map<detail::check_char_t> variable_map;


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
    co::make_option(_LIT("foo"),_LIT("line 2")),
    detail::is_mapped_isolated_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option(_LIT("foo")), // line 3
    detail::is_hidden_mapped_isolated_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT(""),_LIT("line 4")),
    detail::is_raw_isolated_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option(_LIT("")), // line 5
    detail::is_hidden_raw_isolated_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT("foo"),co::value<string_type>(),
      _LIT("line 6")),
    detail::is_mapped_reqired_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 7
    co::make_hidden_option(_LIT("foo"),co::value<string_type>()),
    detail::is_hidden_mapped_reqired_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT(""),co::value<string_type>(),
      _LIT("line 8")),
    detail::is_raw_reqired_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_option(_LIT(""),co::value<string_type>()), // line 9
    detail::is_raw_mapped_reqired_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT("foo"),
      co::value<string_type>().implicit(_LIT("bar")),
      _LIT("line 10")),
    detail::is_mapped_optional_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 11
    co::make_hidden_option(_LIT("foo"),
      co::value<string_type>().implicit(_LIT("bar"))),
    detail::is_hidden_mapped_optional_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT(""),
      co::value<string_type>().implicit(_LIT("bar")),
      _LIT("line 12")),
    detail::is_raw_optional_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 13
    co::make_hidden_option(_LIT(""),
      co::value<string_type>().implicit(_LIT("bar"))),
    detail::is_raw_mapped_optional_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand(_LIT("line 14"),co::value<string_type>()),
    detail::is_keyed_interpret_operand<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 15
    co::make_hidden_operand<detail::check_char_t>(co::value<string_type>()),
    detail::is_hidden_keyed_interpret_operand<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand(_LIT("key"),_LIT("line 16"),
      co::value<string_type>()),
    detail::is_keyed_interpret_operand<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 17
    co::make_hidden_operand(_LIT("key"),co::value<string_type>()),
    detail::is_hidden_keyed_interpret_operand<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand(_LIT("line 18")),
    detail::is_empty_keyed_operand<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand<detail::check_char_t>(), // line 19
    detail::is_hidden_empty_keyed_operand<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand(_LIT("key"),_LIT("line 20")),
    detail::is_empty_keyed_operand<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_hidden_operand(_LIT("key")), // line 21
    detail::is_hidden_empty_keyed_operand<detail::check_char_t>));
}

/**
  Check for proper POSIX option flag unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_posix_flag_test )
{
  typedef co::basic_option_pack<detail::check_char_t> option_pack;

  BOOST_REQUIRE_MESSAGE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("")) ==
      option_pack()),"unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-f")) ==
      option_pack{true,false,_LIT("-"),_LIT("f"),{},{}}));

  // orphan flag
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-")) ==
      option_pack()));

  // packed flags
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-fbar")) ==
    option_pack{true,false,_LIT("-"),_LIT("f"),
      {_LIT("-b"),_LIT("-a"),_LIT("-r")}}));

  // packed flag with trailing extra
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-f bar")) ==
    option_pack{true,false,_LIT("-"),_LIT("f"),
      {_LIT("- "),_LIT("-b"),_LIT("-a"),
        _LIT("-r")}}));

  // cease flag
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("--")) ==
    option_pack{true,false,_LIT("-"),_LIT("-")}));

  // cease flag with extra chars
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("--blah")) ==
    option_pack{true,false,_LIT("-"),_LIT("-"),
      {_LIT("-b"),_LIT("-l"),_LIT("-a"),
        _LIT("-h")}}));

  // packed with embedded 'end of options'
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-fb--ar")) ==
    option_pack{true,false,_LIT("-"),_LIT("f"),
      {_LIT("-b"),_LIT("--"),_LIT("--"),
        _LIT("-a"),_LIT("-r")}}));
}

/**
  Check for proper POSIX option with argument unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_posix_arg_test )
{
  typedef co::basic_option_pack<detail::check_char_t> option_pack;

  BOOST_REQUIRE_MESSAGE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("")) ==
      option_pack()),"unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("-f")) ==
    option_pack{true,false,_LIT("-"),_LIT("f"),{},{}}));

  // orphan flag
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("-")) ==
      option_pack()));

  // packed flags
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("-fbar")) ==
    option_pack{true,true,_LIT("-"),_LIT("f"),{},
      {_LIT("bar")}}));

  // packed flag with trailing extra
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("-f bar")) ==
    option_pack{true,true,_LIT("-"),_LIT("f"),{},
      {_LIT(" bar")}}));

  // cease flag
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("--")) ==
    option_pack{true,false,_LIT("-"),_LIT("-")}));

  // cease flag with extra chars
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("--blah")) ==
    option_pack{true,true,_LIT("-"),_LIT("-"),{},
      {_LIT("blah")}}));
}

/**
  Check for proper GNU option flag unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_gnu_flag_test )
{
  typedef co::basic_option_pack<detail::check_char_t> option_pack;

  BOOST_REQUIRE_MESSAGE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("")) ==
      option_pack()),"unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("-f")) ==
    option_pack{true,false,_LIT("-"),_LIT("f"),{},{}}));

  // orphan flag
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("-")) ==
      option_pack()));

  // packed flags
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("-fbar")) ==
    option_pack{true,false,_LIT("-"),_LIT("f"),
      {_LIT("-b"),_LIT("-a"),_LIT("-r")}}));

  // packed flag with trailing extra
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("-f bar")) ==
    option_pack{true,false,_LIT("-"),_LIT("f"),
      {_LIT("- "),_LIT("-b"),_LIT("-a"),
        _LIT("-r")}}));

  // cease flag
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--")) ==
    option_pack{true,false,_LIT("-"),_LIT("-")}));

  // long flag
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo")) ==
    option_pack{true,false,_LIT("--"),_LIT("foo"),{},{}}));

  // long flag with embedded cease
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo--bar")) ==
      option_pack{true,false,_LIT("--"),
        _LIT("foo--bar"),{},{}}));

  // long flag w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo=bar")) ==
    option_pack{true,true,_LIT("--"),_LIT("foo"),{},
      _LIT("bar")}));

  // long flag with leading extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo =bar  ")) ==
      option_pack{true,true,_LIT("--"),_LIT("foo "),{},
        _LIT("bar  ")}));

  // long flag with trailing extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo= bar  ")) ==
    option_pack{true,true,_LIT("--"),_LIT("foo"),{},
      _LIT(" bar  ")}));

  // long flag with leading and trailing extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo = bar  ")) ==
    option_pack{true,true,_LIT("--"),_LIT("foo "),{},
      _LIT(" bar  ")}));

  // long flag w/cease value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo=--")) ==
    option_pack{true,true,_LIT("--"),_LIT("foo"),{},
      _LIT("--")}));
}

/**
  Check for proper GNU option with argument unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_gnu_arg_test )
{
  typedef co::basic_option_pack<detail::check_char_t> option_pack;

  BOOST_REQUIRE_MESSAGE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("")) ==
      option_pack()),"unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("-f")) ==
    option_pack{true,false,_LIT("-"),_LIT("f"),{},{}}));

  // orphan flag
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("-")) ==
      option_pack()));

  // packed flags
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("-fbar")) ==
    option_pack{true,true,_LIT("-"),_LIT("f"),{},
      {_LIT("bar")}}));

  // packed flag with trailing extra
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("-f bar")) ==
    option_pack{true,true,_LIT("-"),_LIT("f"),{},
      {_LIT(" bar")}}));

  // cease flag
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--")) ==
      option_pack{true,false,_LIT("-"),_LIT("-")}));

  // long flag w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo=bar")) ==
    option_pack{true,true,_LIT("--"),_LIT("foo"),{},
      _LIT("bar")}));

  // long flag with leading extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo =bar  ")) ==
      option_pack{true,true,_LIT("--"),_LIT("foo "),{},
        _LIT("bar  ")}));

  // long flag with trailing extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo= bar  ")) ==
      option_pack{true,true,_LIT("--"),_LIT("foo"),{},
        _LIT(" bar  ")}));

  // long flag with leading and trailing extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo = bar  "))
      == option_pack{true,true,_LIT("--"),_LIT("foo "),{},
        _LIT(" bar  ")}));

  // long flag w/cease value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo=--")) ==
    option_pack{true,true,_LIT("--"),_LIT("foo"),{},
      _LIT("--")}));
}


BOOST_AUTO_TEST_SUITE_END()


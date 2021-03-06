#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( basic_test_suite )

namespace co = cmd_options;
namespace utf = boost::unit_test;

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
  Check for proper option spec decompilation
 */
BOOST_AUTO_TEST_CASE( option_full_spec_split_test )
{
  string_type long_opt;
  string_type short_opt;
  string_type map_key;
  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT("foo,f,key"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt == _LIT("foo"));
  BOOST_REQUIRE(short_opt == _LIT("f"));
  BOOST_REQUIRE(map_key == _LIT("key"));
}

BOOST_AUTO_TEST_CASE( option_empty_split_test )
{
  string_type long_opt;
  string_type short_opt;
  string_type map_key;
  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT(""),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt.empty());
  BOOST_REQUIRE(short_opt.empty());
  BOOST_REQUIRE(map_key.empty());

  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT(","),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt.empty());
  BOOST_REQUIRE(short_opt.empty());
  BOOST_REQUIRE(map_key.empty());

  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT(",,"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt.empty());
  BOOST_REQUIRE(short_opt.empty());
  BOOST_REQUIRE(map_key.empty());
}

BOOST_AUTO_TEST_CASE( option_long_spec_split_test )
{
  string_type long_opt;
  string_type short_opt;
  string_type map_key;
  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT("foo"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt == _LIT("foo"));
  BOOST_REQUIRE(short_opt.empty());
  BOOST_REQUIRE(map_key.empty());

  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT("foo,"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt == _LIT("foo"));
  BOOST_REQUIRE(short_opt.empty());
  BOOST_REQUIRE(map_key.empty());

  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT("foo,,"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt == _LIT("foo"));
  BOOST_REQUIRE(short_opt.empty());
  BOOST_REQUIRE(map_key.empty());
}

BOOST_AUTO_TEST_CASE( option_short_spec_split_test )
{
  string_type long_opt;
  string_type short_opt;
  string_type map_key;
  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT(",f"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt.empty());
  BOOST_REQUIRE(short_opt == _LIT("f"));
  BOOST_REQUIRE(map_key.empty());

  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT(",f,"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt.empty());
  BOOST_REQUIRE(short_opt == _LIT("f"));
  BOOST_REQUIRE(map_key.empty());
}

BOOST_AUTO_TEST_CASE( option_long_short_spec_split_test )
{
  string_type long_opt;
  string_type short_opt;
  string_type map_key;
  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT("foo,f"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt == _LIT("foo"));
  BOOST_REQUIRE(short_opt == _LIT("f"));
  BOOST_REQUIRE(map_key.empty());

  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT("foo,f,"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt == _LIT("foo"));
  BOOST_REQUIRE(short_opt == _LIT("f"));
  BOOST_REQUIRE(map_key.empty());
}

BOOST_AUTO_TEST_CASE( option_map_spec_split_test )
{
  string_type long_opt;
  string_type short_opt;
  string_type map_key;
  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT(",,key"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt.empty());
  BOOST_REQUIRE(short_opt.empty());
  BOOST_REQUIRE(map_key == _LIT("key"));

  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT(",f,key"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt.empty());
  BOOST_REQUIRE(short_opt == _LIT("f"));
  BOOST_REQUIRE(map_key == _LIT("key"));

  std::tie(long_opt,short_opt,map_key) =
    co::split_opt_spec<detail::check_char_t>(
      _LIT("foo,f,key"),detail::check_char_t(','));

  BOOST_REQUIRE(long_opt == _LIT("foo"));
  BOOST_REQUIRE(short_opt == _LIT("f"));
  BOOST_REQUIRE(map_key == _LIT("key"));
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
    co::make_option(_LIT("foo")), // line 3
      detail::is_hidden_mapped_isolated_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT(""),_LIT("line 4")),
      detail::is_raw_isolated_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT("")), // line 5
      detail::is_hidden_raw_isolated_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT("foo"),
      co::basic_value<string_type,detail::check_char_t>(),
      _LIT("line 6")),
      detail::is_mapped_reqired_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 7
    co::make_option(_LIT("foo"),
      co::basic_value<string_type,detail::check_char_t>()),
      detail::is_hidden_mapped_reqired_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT(""),
      co::basic_value<string_type,detail::check_char_t>(),
      _LIT("line 8")),
      detail::is_raw_reqired_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT(""),
      co::basic_value<string_type,detail::check_char_t>()), // line 9
      detail::is_raw_mapped_reqired_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT("foo"),
      co::basic_value<string_type,detail::check_char_t>().implicit(_LIT("bar")),
      _LIT("line 10")),
      detail::is_mapped_optional_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 11
    co::make_option(_LIT("foo"),
      co::basic_value<string_type,
        detail::check_char_t>().implicit(_LIT("bar"))),
      detail::is_hidden_mapped_optional_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    co::make_option(_LIT(""),
      co::basic_value<string_type,detail::check_char_t>().implicit(_LIT("bar")),
      _LIT("line 12")),
      detail::is_raw_optional_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 13
    co::make_option(_LIT(""),
      co::basic_value<string_type,detail::check_char_t>()
        .implicit(_LIT("bar"))),
      detail::is_raw_mapped_optional_option<detail::check_char_t>));

  BOOST_REQUIRE(detail::check_exclusive(
    // line 14
    co::make_operand(_LIT("key"),
      co::basic_value<string_type,detail::check_char_t>()),
      detail::is_operand_with_key_value<detail::check_char_t>));

    // line 15
  BOOST_REQUIRE(detail::check_exclusive(
    co::make_operand(_LIT("key")),
    detail::is_operand_with_key<detail::check_char_t>));
}

/**
  Check for proper POSIX option flag unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_posix_flag_test )
{
  BOOST_REQUIRE_MESSAGE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("")) ==
      option_pack(false)),
        "unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-f")) ==
      option_pack(true,false,_LIT("f"))));

  // orphan flag
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-")) ==
      option_pack(false)));

  // packed flags
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-fbar")) ==
    option_pack(true,false,_LIT("f"),{_LIT("-b"),_LIT("-a"),_LIT("-r")})));

  // packed flag with trailing extra
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-f bar")) ==
    option_pack(true,false,_LIT("f"),
      {_LIT("- "),_LIT("-b"),_LIT("-a"),
        _LIT("-r")})));

  // cease flag
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("--")) ==
    option_pack(true,false,_LIT("-"))));

  // cease flag with extra chars
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("--blah")) ==
    option_pack(true,false,_LIT("-"),{_LIT("-b"),_LIT("-l"),_LIT("-a"),
        _LIT("-h")})));

  // packed with embedded 'end of options'
  BOOST_REQUIRE(
    (co::unpack_posix<true,detail::check_char_t>(_LIT("-fb--ar")) ==
    option_pack(true,false,_LIT("f"),{_LIT("-b"),_LIT("--"),_LIT("--"),
      _LIT("-a"),_LIT("-r")})));
}

/**
  Check for proper POSIX option with argument unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_posix_arg_test )
{
  BOOST_REQUIRE_MESSAGE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("")) ==
      option_pack(false)),
        "unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("-f")) ==
    option_pack(true,false,_LIT("f"))));

  // orphan flag
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("-")) ==
      option_pack(false)));

  // packed flags
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("-fbar")) ==
    option_pack(true,true,_LIT("f"),{},{_LIT("bar")})));

  // packed flag with trailing extra
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("-f bar")) ==
    option_pack(true,true,_LIT("f"),{},{_LIT(" bar")})));

  // cease flag
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("--")) ==
    option_pack(true,false,_LIT("-"))));

  // cease flag with extra chars
  BOOST_REQUIRE(
    (co::unpack_posix<false,detail::check_char_t>(_LIT("--blah")) ==
    option_pack(true,true,_LIT("-"),{},{_LIT("blah")})));
}

/**
  Check for proper GNU option flag unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_gnu_flag_test )
{
  BOOST_REQUIRE_MESSAGE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("")) ==
      option_pack(false)),
        "unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("-f")) ==
    option_pack(true,false,_LIT("f"),{},{})));

  // orphan flag
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("-")) ==
      option_pack(false)));

  // packed flags
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("-fbar")) ==
    option_pack(true,false,_LIT("f"),{_LIT("-b"),_LIT("-a"),_LIT("-r")})));

  // packed flag with trailing extra
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("-f bar")) ==
    option_pack(true,false,_LIT("f"),{_LIT("- "),_LIT("-b"),_LIT("-a"),
        _LIT("-r")})));

  // cease flag
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--")) ==
    option_pack(true,false,_LIT("-"))));

  // long flag
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo")) ==
    option_pack(true,false,_LIT("foo"),{},{})));

  // long flag with embedded cease
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo--bar")) ==
      option_pack(true,false,_LIT("foo--bar"))));

  // long flag w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo=bar")) ==
    option_pack(true,true,_LIT("foo"),{},_LIT("bar"))));

  // long flag with leading extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo =bar  ")) ==
      option_pack(true,true,_LIT("foo "),{},_LIT("bar  "))));

  // long flag with trailing extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo= bar  ")) ==
    option_pack(true,true,_LIT("foo"),{},_LIT(" bar  "))));

  // long flag with leading and trailing extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo = bar  ")) ==
    option_pack(true,true,_LIT("foo "),{},_LIT(" bar  "))));

  // long flag w/cease value
  BOOST_REQUIRE(
    (co::unpack_gnu<true,detail::check_char_t>(_LIT("--foo=--")) ==
    option_pack(true,true,_LIT("foo"),{},_LIT("--"))));
}

/**
  Check for proper GNU option with argument unpacking
 */
BOOST_AUTO_TEST_CASE( unpack_gnu_arg_test )
{
  BOOST_REQUIRE_MESSAGE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("")) ==
      option_pack(false)),
        "unpack on empty string does not yield empty option_pack");

  // simple flag
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("-f")) ==
    option_pack(true,false,_LIT("f"),{},{})));

  // orphan flag
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("-")) ==
      option_pack(false)));

  // packed flags
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("-fbar")) ==
    option_pack(true,true,_LIT("f"),{},{_LIT("bar")})));

  // packed flag with trailing extra
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("-f bar")) ==
    option_pack(true,true,_LIT("f"),{},{_LIT(" bar")})));

  // cease flag
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--")) ==
      option_pack(true,false,_LIT("-"))));

  // long flag w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo=bar")) ==
    option_pack(true,true,_LIT("foo"),{},_LIT("bar"))));

  // long flag with leading extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo =bar  ")) ==
      option_pack(true,true,_LIT("foo "),{},_LIT("bar  "))));

  // long flag with trailing extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo= bar  ")) ==
      option_pack(true,true,_LIT("foo"),{},_LIT(" bar  "))));

  // long flag with leading and trailing extra w/value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo = bar  "))
      == option_pack(true,true,_LIT("foo "),{},_LIT(" bar  "))));

  // long flag w/cease value
  BOOST_REQUIRE(
    (co::unpack_gnu<false,detail::check_char_t>(_LIT("--foo=--")) ==
    option_pack(true,true,_LIT("foo"),{},_LIT("--"))));
}

BOOST_AUTO_TEST_SUITE_END()


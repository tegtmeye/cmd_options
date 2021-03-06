#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 7, Hidden keys with strictly required value
 */

BOOST_AUTO_TEST_SUITE( case7_test_suite )


namespace co = cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;


/**
  All POSIX key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_POSIX_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("-f"),
    _LIT("bar"),
    _LIT("-frab"),
    _LIT("-f bar")
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),
      co::basic_value<string_type,detail::check_char_t>())
  };

  std::tie(std::ignore,vm) =
    co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{string_type(_LIT("bar"))}},
      {_LIT("foo"),{string_type(_LIT("rab"))}},
      {_LIT("foo"),{string_type(_LIT(" bar"))}}
    }));
}

/**
  All GNU key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_GNU_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo=bar"),
    _LIT("--foo"),
    _LIT("rab")
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),
      co::basic_value<string_type,detail::check_char_t>())
  };

  std::tie(std::ignore,vm) =
    co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{string_type(_LIT("bar"))}},
      {_LIT("foo"),{string_type(_LIT("rab"))}}
    }));
}

/**
  All invalid POSIX key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_invalid_POSIX_key_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("-f bar"),
    _LIT("-f"),
    _LIT("-b")
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),
      co::basic_value<string_type,detail::check_char_t>())
  };

  BOOST_REQUIRE_THROW(
    ( std::tie(std::ignore,vm) =
        co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
      co::missing_argument_error);
}

/**
  Invalid GNU key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_invalid_GNU_key_value_test1 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo bar")
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),
      co::basic_value<string_type,detail::check_char_t>())
  };

  BOOST_REQUIRE_THROW(
    ( std::tie(std::ignore,vm) =
        co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
      co::unknown_option_error);
}

/**
  Invalid GNU key/value pairs
 */
BOOST_AUTO_TEST_CASE( all_invalid_GNU_key_value_test2 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("--bar")
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),
      co::basic_value<string_type,detail::check_char_t>())
  };

  BOOST_REQUIRE_THROW(
    ( std::tie(std::ignore,vm) =
        co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
      co::missing_argument_error);
}
















#if 0

/**
  All flags embedded cease
 */
BOOST_AUTO_TEST_CASE( all_flags_cease_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("--"),
    _LIT("--bar"),
    _LIT("blah")
  };

  options = options_group_type{
    co::make_option(_LIT("")),
    co::make_operand<string_type>("accept all operands")
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT(""),string_type(_LIT("--bar"))},
      {_LIT(""),string_type(_LIT("blah"))},
      {_LIT("foo"),{}},
      {_LIT("f"),{}}
    }));
}

/**
  Packed flags
 */
BOOST_AUTO_TEST_CASE( packed_flags_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("-abcd")
  };

  options = options_group_type{
    co::make_option(_LIT(""))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}},
      {_LIT("f"),{}},
      {_LIT("a"),{}},
      {_LIT("b"),{}},
      {_LIT("c"),{}},
      {_LIT("d"),{}}
    }));
}

/**
  Packed flags embedded cease
 */
BOOST_AUTO_TEST_CASE( packed_flags_cease_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("-abcd"),
    _LIT("--"),
    _LIT("--bar"),
    _LIT("blah")
  };

  options = options_group_type{
    co::make_option(_LIT("")),
    co::make_operand<string_type>("accept all operands")
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT(""),string_type(_LIT("--bar"))},
      {_LIT(""),string_type(_LIT("blah"))},
      {_LIT("foo"),{}},
      {_LIT("f"),{}},
      {_LIT("a"),{}},
      {_LIT("b"),{}},
      {_LIT("c"),{}},
      {_LIT("d"),{}}
    }));
}

/**
  Flags w/invalid packed argument
 */
BOOST_AUTO_TEST_CASE( flags_packed_arg_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo=bar"),
    _LIT("-f"),
    _LIT("-abcd")
  };

  options = options_group_type{
    co::make_option(_LIT(""))
  };


  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
      co::unexpected_argument_error);
}

/**
  Flags w/operand as invalid separate argument
 */
BOOST_AUTO_TEST_CASE( flags_invalid_posarg_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("bar")
  };

  options = options_group_type{
    co::make_option(_LIT(""))
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
      co::unexpected_operand_error);
}

/**
  Flags w/invalid packed cease
 */
BOOST_AUTO_TEST_CASE( flags_invalid_packed_cease_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("-a--")
  };

  options = options_group_type{
    co::make_option(_LIT(""))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}},
      {_LIT("f"),{}},
      {_LIT("a"),{}},
      {_LIT("-"),{}},
      {_LIT("-"),{}}
    }));
}

/**
  Multi packed flags
 */
BOOST_AUTO_TEST_CASE( multi_packed_flags_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-abc"),
    _LIT("-def")
  };

  options = options_group_type{
    co::make_option(_LIT(""))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("a"),{}},
      {_LIT("b"),{}},
      {_LIT("c"),{}},
      {_LIT("d"),{}},
      {_LIT("e"),{}},
      {_LIT("f"),{}},
      {_LIT("foo"),{}}
    }));
}
#endif





BOOST_AUTO_TEST_SUITE_END()


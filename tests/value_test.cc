#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  Check for fundmental and STL value handling
 */

namespace co = cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;


struct userdef_struct {
  userdef_struct(const string_type &str=string_type()) :_str(str) {}

  bool operator==(const userdef_struct &rhs) const {
    return rhs._str == _str;
  }

  string_type _str;
};

/*
  operator>> and operator << are only going to be called in one of two
  ways, char in the case where CharT is one of {char,char16_t,char32_t}
  which means UTF and wchar_t which may be UCS2 or something else. Use
  SFINAE so that this class be be used with all types for testing
*/
template<typename CharT>
inline typename std::enable_if<std::is_same<CharT,char>::value,
  std::basic_ostream<CharT> &>::type
operator<<(std::basic_ostream<CharT> &out, const userdef_struct &ts)
{
  return (out << co::detail::asUTF8(ts._str));
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT,wchar_t>::value,
  std::basic_ostream<CharT> &>::type
operator<<(std::basic_ostream<CharT> &out, const userdef_struct &ts)
{
  return (out << ts._str);
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT,char>::value,
  std::basic_istream<CharT> &>::type
operator>>(std::basic_istream<CharT> &in, userdef_struct &rhs)
{
  std::istreambuf_iterator<CharT> first(in);
  std::istreambuf_iterator<CharT> last;

  rhs._str = co::detail::fromUTF8<detail::check_char_t>({first,last});

  return in;
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT,wchar_t>::value,
  std::basic_istream<CharT> &>::type
operator>>(std::basic_istream<CharT> &in, userdef_struct &rhs)
{
  std::istreambuf_iterator<CharT> first(in);
  std::istreambuf_iterator<CharT> last;

  rhs._str.assign(first,last);

  return in;
}


struct userdef_convert_struct {
  userdef_convert_struct(const string_type &str=string_type()) :_str(str) {}

  bool operator==(const userdef_convert_struct &rhs) const {
    return rhs._str == _str;
  }

  string_type _str;
};


/*
  These 4 functions should never be chosen by the compiler, the EZ
  interface should pick up the specialized
  'co::convert_value<userdef_convert_struct>' structure instead of
  choosing these.
*/
template<typename CharT>
inline typename std::enable_if<std::is_same<CharT,char>::value,
  std::basic_ostream<CharT> &>::type
operator<<(std::basic_ostream<CharT> &out, const userdef_convert_struct &ts)
{
  throw 1;
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT,wchar_t>::value,
  std::basic_ostream<CharT> &>::type
operator<<(std::basic_ostream<CharT> &out, const userdef_convert_struct &ts)
{
  throw 1;
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT,char>::value,
  std::basic_istream<CharT> &>::type
operator>>(std::basic_istream<CharT> &in, userdef_convert_struct &rhs)
{
  throw 1;
}

template<typename CharT>
inline typename std::enable_if<std::is_same<CharT,wchar_t>::value,
  std::basic_istream<CharT> &>::type
operator>>(std::basic_istream<CharT> &in, userdef_convert_struct &rhs)
{
  throw 1;
}



template<>
struct co::convert_value<userdef_convert_struct> {
  static userdef_convert_struct from_string(const string_type &str)
  {
    return userdef_convert_struct(str);
  }

  static void to_string(string_type &str, const userdef_convert_struct &val)
  {
    str = val._str;
  }
};



BOOST_AUTO_TEST_SUITE( value_test_suite )

/**
  Check fundamental values
 */
BOOST_AUTO_TEST_CASE( bool_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--bool=1"),
    _LIT("--bool=0"),
    _LIT("--bool=true"),
    _LIT("--bool=false"),
  };

  options = options_group_type{
    co::make_option(_LIT("bool"),co::basic_value<bool,detail::check_char_t>(),
      _LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("bool"),static_cast<bool>(1)),
      detail::check_value(_LIT("bool"),static_cast<bool>(0)),
      detail::check_value(_LIT("bool"),static_cast<bool>(1)),
      detail::check_value(_LIT("bool"),static_cast<bool>(0)),
    }
  ));

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--bool=11"),
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("11"));
            }
        );
        return true;
      }
  );

  std::vector<const detail::check_char_t *> argv3{
    _LIT("--bool=foobar"),
  };


  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv3.data(),
    argv3.data()+argv3.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foobar"));
            }
        );
        return true;
      }
  );

  std::vector<const detail::check_char_t *> argv4{
    _LIT("--bool=truefoo"),
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv4.data(),
    argv4.data()+argv4.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("truefoo"));
            }
        );
        return true;
      }
  );
}


BOOST_AUTO_TEST_CASE( CharT_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--CharT=a"),
  };

  options = options_group_type{
    co::make_option(_LIT("CharT"),
      co::basic_value<detail::check_char_t,detail::check_char_t>(),
      _LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("CharT"),static_cast<detail::check_char_t>('a'))
    }
  ));

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--CharT=aa"),
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("aa"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( short_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--short=11"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--short=foo"),
  };

  std::vector<const detail::check_char_t *> argv3{
    _LIT("--short=3.14"),
  };

  std::vector<const detail::check_char_t *> argv4{
    _LIT("--short=11 foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("short"),
      co::basic_value<short,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("short"),static_cast<short>(11)),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv3.data(),
    argv3.data()+argv3.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("3.14"));
            }
        );
        return true;
      }
  );

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv4.data(),
    argv4.data()+argv4.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("11 foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( ushort_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--ushort=21"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--ushort=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("ushort"),
      co::basic_value<unsigned short,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("ushort"),static_cast<unsigned short>(21))
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( int_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--int=12"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--int=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("int"),
      co::basic_value<int,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("int"),static_cast<int>(12)),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( uint_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--uint=22"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--uint=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("uint"),
      co::basic_value<unsigned int,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("uint"),static_cast<unsigned int>(22)),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( long_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--long=13"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--long=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("long"),
      co::basic_value<long,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("long"),static_cast<long>(13)),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( ulong_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--ulong=23"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--ulong=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("ulong"),
      co::basic_value<unsigned long,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("ulong"),static_cast<unsigned long>(23)),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( longlong_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--longlong=14"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--longlong=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("longlong"),
      co::basic_value<long long,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("longlong"),static_cast<long long>(14)),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( ulonglong_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--ulonglong=24"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--ulonglong=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("ulonglong"),
      co::basic_value<unsigned long long,detail::check_char_t>(),
      _LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("ulonglong"),
        static_cast<unsigned long long>(24)),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( float_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--float=5.1"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--float=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("float"),
      co::basic_value<float,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("float"),static_cast<float>(5.1),
        detail::essentiallyEqual<float>()),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( double_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--double=6.1"),
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--double=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("double"),
      co::basic_value<double,detail::check_char_t>(),_LIT("case 6")),
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("double"),static_cast<double>(6.1),
        detail::essentiallyEqual<double>()),
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

BOOST_AUTO_TEST_CASE( long_double_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--longdouble=7.1")
  };

  std::vector<const detail::check_char_t *> argv2{
    _LIT("--longdouble=foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("longdouble"),
      co::basic_value<long double,detail::check_char_t>(),
      _LIT("case 6"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("longdouble"),static_cast<long double>(7.1),
        detail::essentiallyEqual<long double>())
    }
  ));

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv2.data(),
    argv2.data()+argv2.size(),options),
      co::invalid_argument_error, [](const co::invalid_argument_error &ex) {
        BOOST_CHECK_EXCEPTION(std::rethrow_if_nested(ex),
          std::invalid_argument, [](const std::invalid_argument &e) {
              return (e.what() == std::string("foo"));
            }
        );
        return true;
      }
  );
}

/**
  Check all fundamental values for clash
 */
BOOST_AUTO_TEST_CASE( fundamental_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--CharT=a"),
    _LIT("--short=11"),
    _LIT("--ushort=21"),
    _LIT("--int=12"),
    _LIT("--uint=22"),
    _LIT("--long=13"),
    _LIT("--ulong=23"),
    _LIT("--longlong=14"),
    _LIT("--ulonglong=24"),
    _LIT("--float=5.1"),
    _LIT("--double=6.1"),
    _LIT("--longdouble=7.1")
  };

  options = options_group_type{
    co::make_option(_LIT("CharT"),
      co::basic_value<detail::check_char_t,detail::check_char_t>(),
      _LIT("case 6")),
    co::make_option(_LIT("short"),
      co::basic_value<short,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("ushort"),
      co::basic_value<unsigned short,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("int"),
      co::basic_value<int,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("uint"),
      co::basic_value<unsigned int,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("long"),
      co::basic_value<long,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("ulong"),
      co::basic_value<unsigned long,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("longlong"),
      co::basic_value<long long,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("ulonglong"),
      co::basic_value<unsigned long long,detail::check_char_t>(),
      _LIT("case 6")),
    co::make_option(_LIT("float"),
      co::basic_value<float,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("double"),
      co::basic_value<double,detail::check_char_t>(),_LIT("case 6")),
    co::make_option(_LIT("longdouble"),
      co::basic_value<long double,detail::check_char_t>(),
      _LIT("case 6"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("CharT"),static_cast<detail::check_char_t>('a')),
      detail::check_value(_LIT("double"),static_cast<double>(6.1),
        detail::essentiallyEqual<double>()),
      detail::check_value(_LIT("float"),static_cast<float>(5.1),
        detail::essentiallyEqual<float>()),
      detail::check_value(_LIT("int"),static_cast<int>(12)),
      detail::check_value(_LIT("long"),static_cast<long>(13)),
      detail::check_value(_LIT("longdouble"),static_cast<long double>(7.1),
        detail::essentiallyEqual<long double>()),
      detail::check_value(_LIT("longlong"),static_cast<long long>(14)),
      detail::check_value(_LIT("short"),static_cast<short>(11)),
      detail::check_value(_LIT("uint"),static_cast<unsigned int>(22)),
      detail::check_value(_LIT("ulong"),static_cast<unsigned long>(23)),
      detail::check_value(_LIT("ulonglong"),
        static_cast<unsigned long long>(24)),
      detail::check_value(_LIT("ushort"),static_cast<unsigned short>(21))
    }
  ));
}

BOOST_AUTO_TEST_CASE( string_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--string=Hello World")
  };

  options = options_group_type{
    co::make_option(_LIT("string"),
      co::basic_value<string_type,detail::check_char_t>(),
      _LIT("case 6"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("string"),
        static_cast<string_type>(_LIT("Hello World"))),
    }
  ));
}

BOOST_AUTO_TEST_CASE( userdef_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--userdef=Hello World")
  };

  options = options_group_type{
    co::make_option(_LIT("userdef"),
      co::basic_value<userdef_struct,detail::check_char_t>(),
      _LIT("case 6"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("userdef"),
        static_cast<userdef_struct>(_LIT("Hello World"))),
    }
  ));
}

BOOST_AUTO_TEST_CASE( userdef_convert_value_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--userdef=Hello World")
  };

  options = options_group_type{
    co::make_option(_LIT("userdef"),
      co::basic_value<userdef_convert_struct,detail::check_char_t>(),
      _LIT("case 6"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

  BOOST_REQUIRE(detail::vm_check(vm,{
      detail::check_value(_LIT("userdef"),
        static_cast<userdef_convert_struct>(_LIT("Hello World"))),
    }
  ));
}

BOOST_AUTO_TEST_SUITE_END()


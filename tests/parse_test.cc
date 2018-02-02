#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( parse_test_suite )

namespace co = lemma::cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;


option_description_type check_pos_arg(const option_description_type &desc,
  std::size_t posn, std::size_t argn)
{
    assert(desc.mapped_key);

  option_description_type result = desc;

  result.mapped_key = [=](const string_type &_key, std::size_t _posn,
    std::size_t _argn, const variable_map_type &_vm)
  {
    auto mapped_key = desc.mapped_key;

    assert(mapped_key);
    std::pair<bool,string_type> base = desc.mapped_key(_key,_posn,_argn,_vm);
    if(base.first && !(posn == _posn && argn == _argn)) {
      std::stringstream err;
      err << "check_pos_arg failed: posn: " << _posn << " and argn: "
        << _argn << " required posn: " << posn << " and argn: " << argn
        << "\n";
      throw std::runtime_error(err.str());
    }

    return base;
  };

  return result;
}

option_description_type nested1{
  [](const string_type &option) -> option_pack_type {
    if(option == _LIT("-foo"))
      return option_pack_type{true,false,_LIT("-"),_LIT("foo"),
        {_LIT("-a"),_LIT("-bar"),_LIT("-c")}};
    return option_pack_type{false};
  },
  [](const string_type &raw_key, std::size_t, std::size_t,
    const variable_map_type &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return _LIT("test nested");
  }
};

option_description_type nested2{
  [](const string_type &option) -> option_pack_type {
    if(option == _LIT("-bar"))
      return option_pack_type{true,false,_LIT("-"),_LIT("bar"),
        {_LIT("-d"),_LIT("-e"),_LIT("-f")}};
    return option_pack_type{false};
  },
  [](const string_type &raw_key, std::size_t, std::size_t,
    const variable_map_type &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return _LIT("test nested2");
  }
};

option_description_type nested3{
  [](const string_type &option) -> option_pack_type {
    if(option == _LIT("-bar"))
      return option_pack_type{true,false,_LIT("-"),_LIT("bar"),
        {_LIT("-d"),_LIT("pos"),_LIT("-f")}};
    return option_pack_type{false};
  },
  [](const string_type &raw_key, std::size_t, std::size_t,
    const variable_map_type &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return _LIT("test nested3");
  }
};

option_description_type make_operand_at(std::size_t posn, std::size_t argn)
{
  return option_description_type{
    {},
    [=](const string_type &_key, std::size_t _posn, std::size_t _argn,
      const variable_map_type &)
    {
      if(_posn == posn && _argn == argn)
        return std::make_pair(true,_key);
      return std::make_pair(false,string_type());
    },
    {},{},
    [](const string_type &mapped_key, const string_type &value,
      const variable_map_type &) -> co::any
    {
      return co::any(value);
    }
  };
}
/*
  Fall through accept that simply throws. Must be last in list of descriptions
*/
option_description_type throw_operand{
  {},
  [](const string_type &, std::size_t _posn, std::size_t _argn,
    const variable_map_type &) -> std::pair<bool,string_type>
  {
    std::stringstream err;
    err << "throw_operand: posn: " << _posn << " and argn: "
      << _argn << "\n";
    throw std::runtime_error(err.str());
  }
};




/*
  Check for proper option and argument numbering
 */
BOOST_AUTO_TEST_CASE( option_numbering_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("--foo1"),
    _LIT("--foo2"),
    _LIT("--foo3"),
  };

  options = options_group_type{
    check_pos_arg(co::make_option(_LIT("foo1"),_LIT("case 2")),0,0),
    check_pos_arg(co::make_option(_LIT("foo2"),_LIT("case 2")),1,1),
    check_pos_arg(co::make_option(_LIT("foo3"),_LIT("case 2")),2,2),
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);
}

BOOST_AUTO_TEST_CASE( operand_numbering_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("foo1"),
    _LIT("foo2"),
    _LIT("foo3"),
  };

  options = options_group_type{
    check_pos_arg(make_operand_at(0,0),0,0),
    check_pos_arg(make_operand_at(1,1),1,1),
    check_pos_arg(make_operand_at(2,2),2,2),
    throw_operand
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);
}

BOOST_AUTO_TEST_CASE( option_operand_numbering_test1 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("--foo0"),
    _LIT("--foo1"),
    _LIT("operand0"),
    _LIT("--foo2"),
    _LIT("operand1"),
    _LIT("--foo3"),
    _LIT("operand2"),
    _LIT("operand3"),
  };

  options = options_group_type{
    check_pos_arg(co::make_option(_LIT("foo0"),_LIT("case 2")),0,0),
    check_pos_arg(co::make_option(_LIT("foo1"),_LIT("case 2")),1,1),
    check_pos_arg(co::make_option(_LIT("foo2"),_LIT("case 2")),2,3),
    check_pos_arg(co::make_option(_LIT("foo3"),_LIT("case 2")),3,5),
    check_pos_arg(make_operand_at(0,2),0,2),
    check_pos_arg(make_operand_at(1,4),1,4),
    check_pos_arg(make_operand_at(2,6),2,6),
    check_pos_arg(make_operand_at(3,7),3,7),
    throw_operand
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);
}

BOOST_AUTO_TEST_CASE( option_operand_numbering_test2 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("operand0"),
    _LIT("operand1"),
    _LIT("--foo0"),
    _LIT("operand2"),
    _LIT("--foo1"),
    _LIT("operand3"),
    _LIT("--foo2"),
    _LIT("--foo3"),
  };

  options = options_group_type{
    check_pos_arg(co::make_option(_LIT("foo0"),_LIT("case 2")),0,2),
    check_pos_arg(co::make_option(_LIT("foo1"),_LIT("case 2")),1,4),
    check_pos_arg(co::make_option(_LIT("foo2"),_LIT("case 2")),2,6),
    check_pos_arg(co::make_option(_LIT("foo3"),_LIT("case 2")),3,7),
    check_pos_arg(make_operand_at(0,0),0,0),
    check_pos_arg(make_operand_at(1,1),1,1),
    check_pos_arg(make_operand_at(2,3),2,3),
    check_pos_arg(make_operand_at(3,5),3,5),
    throw_operand
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);
}



/**
  Nested option test
 */
BOOST_AUTO_TEST_CASE( parse_nested_option_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("-foo")
  };

  options = options_group_type{
    check_pos_arg(nested1,0,0),
    check_pos_arg(co::make_option(_LIT(",a"),_LIT("case 2")),1,1),
    check_pos_arg(nested2,2,2),
    check_pos_arg(co::make_option(_LIT(",d"),_LIT("case 2")),3,3),
    check_pos_arg(co::make_option(_LIT(",e"),_LIT("case 2")),4,4),
    check_pos_arg(co::make_option(_LIT(",f"),_LIT("case 2")),5,5),
    check_pos_arg(co::make_option(_LIT(",c"),_LIT("case 2")),6,6)
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("a"),{}},
      {_LIT("bar"),{}},
      {_LIT("c"),{}},
      {_LIT("d"),{}},
      {_LIT("e"),{}},
      {_LIT("f"),{}},
      {_LIT("foo"),{}}
    }));
}

/**
  Nested operand_test
 */
BOOST_AUTO_TEST_CASE( parse_nested_operand_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv;

  argv = std::vector<const detail::check_char_t *>{
    _LIT("-foo")
  };

  options = options_group_type{
    check_pos_arg(nested1,0,0),
    check_pos_arg(co::make_option(_LIT(",a"),_LIT("case 2")),1,1),
    check_pos_arg(nested3,2,2),
    check_pos_arg(co::make_option(_LIT(",d"),_LIT("case 2")),3,3),
    check_pos_arg(make_operand_at(0,4),0,4),
    check_pos_arg(co::make_option(_LIT(",f"),_LIT("case 2")),4,5),
    check_pos_arg(co::make_option(_LIT(",c"),_LIT("case 2")),5,6),
    throw_operand
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("alt_key"),{string_type(_LIT("pos"))}},
      {_LIT("a"),{}},
      {_LIT("bar"),{}},
      {_LIT("c"),{}},
      {_LIT("d"),{}},
      {_LIT("f"),{}},
      {_LIT("foo"),{}},
    }));
}


BOOST_AUTO_TEST_SUITE_END()


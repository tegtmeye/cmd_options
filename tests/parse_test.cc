#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( parse_test_suite )

namespace co = lemma::cmd_options;

typedef std::basic_string<char> string_type;
typedef co::basic_option_pack<char> option_pack;
typedef co::basic_option_description<char> option_description;
typedef co::basic_variable_map<char> variable_map;


option_description check_pos_arg(const option_description &desc,
  std::size_t posn, std::size_t argn)
{
    assert(desc.mapped_key);

  option_description result = desc;

  result.mapped_key = [=](const string_type &_key, std::size_t _posn,
    std::size_t _argn, const variable_map &_vm)
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

option_description nested1{
  [](const string_type &option) -> option_pack {
    if(option == "-foo")
      return option_pack{true,false,"-","foo",{"-a","-bar","-c"}};
    return option_pack{false};
  },
  [](const string_type &raw_key, std::size_t, std::size_t, const variable_map &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return "test nested";
  }
};

option_description nested2{
  [](const string_type &option) -> option_pack {
    if(option == "-bar")
      return option_pack{true,false,"-","bar",{"-d","-e","-f"}};
    return option_pack{false};
  },
  [](const string_type &raw_key, std::size_t, std::size_t, const variable_map &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return "test nested2";
  }
};

option_description nested3{
  [](const string_type &option) -> option_pack {
    if(option == "-bar")
      return option_pack{true,false,"-","bar",{"-d","pos","-f"}};
    return option_pack{false};
  },
  [](const string_type &raw_key, std::size_t, std::size_t, const variable_map &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return "test nested3";
  }
};

option_description make_operand_at(std::size_t posn, std::size_t argn)
{
  return option_description{
    {},
    [=](const string_type &_key, std::size_t _posn, std::size_t _argn,
      const variable_map &)
    {
      if(_posn == posn && _argn == argn)
        return std::make_pair(true,_key);
      return std::make_pair(false,string_type());
    },
    {},{},{},{},
    [](const string_type &mapped_key, const string_type &value,
      const variable_map &) -> co::any
    {
      return co::any(value);
    }
  };
}
/*
  Fall through accept that simply throws. Must be last in list of descriptions
*/
option_description throw_operand{
  {},
  [](const string_type &, std::size_t _posn, std::size_t _argn,
    const variable_map &) -> std::pair<bool,string_type>
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
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo1",
    "--foo2",
    "--foo3",
  };

  options = co::options_group{
    check_pos_arg(co::make_option("foo1","case 2"),0,0),
    check_pos_arg(co::make_option("foo2","case 2"),1,1),
    check_pos_arg(co::make_option("foo3","case 2"),2,2),
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);
}

BOOST_AUTO_TEST_CASE( operand_numbering_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "foo1",
    "foo2",
    "foo3",
  };

  options = co::options_group{
    check_pos_arg(make_operand_at(0,0),0,0),
    check_pos_arg(make_operand_at(1,1),1,1),
    check_pos_arg(make_operand_at(2,2),2,2),
    throw_operand
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);
}

BOOST_AUTO_TEST_CASE( option_operand_numbering_test1 )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo0",
    "--foo1",
    "operand0",
    "--foo2",
    "operand1",
    "--foo3",
    "operand2",
    "operand3",
  };

  options = co::options_group{
    check_pos_arg(co::make_option("foo0","case 2"),0,0),
    check_pos_arg(co::make_option("foo1","case 2"),1,1),
    check_pos_arg(co::make_option("foo2","case 2"),2,3),
    check_pos_arg(co::make_option("foo3","case 2"),3,5),
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
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "operand0",
    "operand1",
    "--foo0",
    "operand2",
    "--foo1",
    "operand3",
    "--foo2",
    "--foo3",
  };

  options = co::options_group{
    check_pos_arg(co::make_option("foo0","case 2"),0,2),
    check_pos_arg(co::make_option("foo1","case 2"),1,4),
    check_pos_arg(co::make_option("foo2","case 2"),2,6),
    check_pos_arg(co::make_option("foo3","case 2"),3,7),
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
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-foo"
  };

  options = co::options_group{
    check_pos_arg(nested1,0,0),
    check_pos_arg(co::make_option(",a","case 2"),1,1),
    check_pos_arg(nested2,2,2),
    check_pos_arg(co::make_option(",d","case 2"),3,3),
    check_pos_arg(co::make_option(",e","case 2"),4,4),
    check_pos_arg(co::make_option(",f","case 2"),5,5),
    check_pos_arg(co::make_option(",c","case 2"),6,6)
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"a",{}},
      {"bar",{}},
      {"c",{}},
      {"d",{}},
      {"e",{}},
      {"f",{}},
      {"foo",{}}
    }));
}

/**
  Nested operand_test
 */
BOOST_AUTO_TEST_CASE( parse_nested_operand_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-foo"
  };

  options = co::options_group{
    check_pos_arg(nested1,0,0),
    check_pos_arg(co::make_option(",a","case 2"),1,1),
    check_pos_arg(nested3,2,2),
    check_pos_arg(co::make_option(",d","case 2"),3,3),
    check_pos_arg(make_operand_at(0,4),0,4),
    check_pos_arg(co::make_option(",f","case 2"),4,5),
    check_pos_arg(co::make_option(",c","case 2"),5,6),
    throw_operand
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"alt_key",{string_type("pos")}},
      {"a",{}},
      {"bar",{}},
      {"c",{}},
      {"d",{}},
      {"f",{}},
      {"foo",{}},
    }));
}


BOOST_AUTO_TEST_SUITE_END()


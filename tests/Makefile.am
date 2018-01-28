#  Copyright (c) 2016-2018, Mike Tegtmeyer
#  All rights reserved.
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of the author nor the names of its contributors may
#        be used to endorse or promote products derived from this software
#        without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
#  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
#  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

master_suite=$(top_srcdir)/tests/master_suite.cc

additional_cppflags= \
	-pedantic -Wall -I.. \
	-DCMD_OPTIONS_DEFAULT_OPERAND_KEY=alt_key \
	$(SYSTEM_BOOST_CPPFLAGS)

char_cpp_flags= \
	-DCMD_OPT_TEST_CHART=char -DLITERAL_PREFIX=

wchar_cpp_flags= \
	-DCMD_OPT_TEST_CHART=wchar_t -DLITERAL_PREFIX=L

additional_cflags=

additional_libs= \
	$(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS)

additional_ldflags= \
        $(BOOST_LDFLAGS) \
        $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS) \
        $(BOOST_SYSTEM_LDFLAGS) \
        $(BOOST_FILESYSTEM_LDFLAGS)


check_PROGRAMS=\
	basic_test \
	wbasic_test \
	parse_test \
	wparse_test \
	accept_test \
	waccept_test \
	case2_test \
	wcase2_test \
	case3_test \
	wcase3_test \
	case4_test \
	wcase4_test \
	case5_test \
	wcase5_test \
	case6_test \
	wcase6_test \
	case7_test \
	wcase7_test \
	case8_test \
	wcase8_test \
	case9_test \
	wcase9_test \
	case10_test \
	wcase10_test \
	case11_test \
	wcase11_test \
	case12_test \
	wcase12_test \
	case13_test \
	wcase13_test \
	case14_test \
	wcase14_test \
	case15_test \
	wcase15_test \
	case16_test \
	wcase16_test \
	case17_test \
	wcase17_test \
	case18_test \
	wcase18_test \
	case19_test \
	wcase19_test \
	case20_test \
	wcase20_test \
	case21_test \
	wcase21_test \
	constraints_test \
	wconstraints_test \
	value_test \
	wvalue_test

basic_test_SOURCES=$(master_suite) \
	test_detail.h \
	basic_test.cc
basic_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
basic_test_LDFLAGS=$(additional_ldflags)
basic_test_LDADD=$(additional_libs)

wbasic_test_SOURCES=$(master_suite) \
	test_detail.h \
	basic_test.cc
wbasic_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wbasic_test_LDFLAGS=$(additional_ldflags)
wbasic_test_LDADD=$(additional_libs)

parse_test_SOURCES=$(master_suite) \
	test_detail.h \
	parse_test.cc
parse_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
parse_test_LDFLAGS=$(additional_ldflags)
parse_test_LDADD=$(additional_libs)

wparse_test_SOURCES=$(master_suite) \
	test_detail.h \
	parse_test.cc
wparse_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wparse_test_LDFLAGS=$(additional_ldflags)
wparse_test_LDADD=$(additional_libs)

accept_test_SOURCES=$(master_suite) \
	test_detail.h \
	accept_test.cc
accept_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
accept_test_LDFLAGS=$(additional_ldflags)
accept_test_LDADD=$(additional_libs)

waccept_test_SOURCES=$(master_suite) \
	test_detail.h \
	accept_test.cc
waccept_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
waccept_test_LDFLAGS=$(additional_ldflags)
waccept_test_LDADD=$(additional_libs)

case2_test_SOURCES=$(master_suite) \
	test_detail.h \
	case2_test.cc
case2_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case2_test_LDFLAGS=$(additional_ldflags)
case2_test_LDADD=$(additional_libs)

wcase2_test_SOURCES=$(master_suite) \
	test_detail.h \
	case2_test.cc
wcase2_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase2_test_LDFLAGS=$(additional_ldflags)
wcase2_test_LDADD=$(additional_libs)

case3_test_SOURCES=$(master_suite) \
	test_detail.h \
	case3_test.cc
case3_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case3_test_LDFLAGS=$(additional_ldflags)
case3_test_LDADD=$(additional_libs)

wcase3_test_SOURCES=$(master_suite) \
	test_detail.h \
	case3_test.cc
wcase3_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase3_test_LDFLAGS=$(additional_ldflags)
wcase3_test_LDADD=$(additional_libs)

case4_test_SOURCES=$(master_suite) \
	test_detail.h \
	case4_test.cc
case4_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case4_test_LDFLAGS=$(additional_ldflags)
case4_test_LDADD=$(additional_libs)

wcase4_test_SOURCES=$(master_suite) \
	test_detail.h \
	case4_test.cc
wcase4_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase4_test_LDFLAGS=$(additional_ldflags)
wcase4_test_LDADD=$(additional_libs)

case5_test_SOURCES=$(master_suite) \
	test_detail.h \
	case5_test.cc
case5_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case5_test_LDFLAGS=$(additional_ldflags)
case5_test_LDADD=$(additional_libs)

wcase5_test_SOURCES=$(master_suite) \
	test_detail.h \
	case5_test.cc
wcase5_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase5_test_LDFLAGS=$(additional_ldflags)
wcase5_test_LDADD=$(additional_libs)

case6_test_SOURCES=$(master_suite) \
	test_detail.h \
	case6_test.cc
case6_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case6_test_LDFLAGS=$(additional_ldflags)
case6_test_LDADD=$(additional_libs)

wcase6_test_SOURCES=$(master_suite) \
	test_detail.h \
	case6_test.cc
wcase6_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase6_test_LDFLAGS=$(additional_ldflags)
wcase6_test_LDADD=$(additional_libs)

case7_test_SOURCES=$(master_suite) \
	test_detail.h \
	case7_test.cc
case7_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case7_test_LDFLAGS=$(additional_ldflags)
case7_test_LDADD=$(additional_libs)

wcase7_test_SOURCES=$(master_suite) \
	test_detail.h \
	case7_test.cc
wcase7_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase7_test_LDFLAGS=$(additional_ldflags)
wcase7_test_LDADD=$(additional_libs)

case8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case8_test.cc
case8_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case8_test_LDFLAGS=$(additional_ldflags)
case8_test_LDADD=$(additional_libs)

wcase8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case8_test.cc
wcase8_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase8_test_LDFLAGS=$(additional_ldflags)
wcase8_test_LDADD=$(additional_libs)

case9_test_SOURCES=$(master_suite) \
	test_detail.h \
	case9_test.cc
case9_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case9_test_LDFLAGS=$(additional_ldflags)
case9_test_LDADD=$(additional_libs)

wcase9_test_SOURCES=$(master_suite) \
	test_detail.h \
	case9_test.cc
wcase9_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase9_test_LDFLAGS=$(additional_ldflags)
wcase9_test_LDADD=$(additional_libs)

case10_test_SOURCES=$(master_suite) \
	test_detail.h \
	case10_test.cc
case10_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case10_test_LDFLAGS=$(additional_ldflags)
case10_test_LDADD=$(additional_libs)

wcase10_test_SOURCES=$(master_suite) \
	test_detail.h \
	case10_test.cc
wcase10_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase10_test_LDFLAGS=$(additional_ldflags)
wcase10_test_LDADD=$(additional_libs)

case11_test_SOURCES=$(master_suite) \
	test_detail.h \
	case11_test.cc
case11_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case11_test_LDFLAGS=$(additional_ldflags)
case11_test_LDADD=$(additional_libs)

wcase11_test_SOURCES=$(master_suite) \
	test_detail.h \
	case11_test.cc
wcase11_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase11_test_LDFLAGS=$(additional_ldflags)
wcase11_test_LDADD=$(additional_libs)

case12_test_SOURCES=$(master_suite) \
	test_detail.h \
	case12_test.cc
case12_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case12_test_LDFLAGS=$(additional_ldflags)
case12_test_LDADD=$(additional_libs)

wcase12_test_SOURCES=$(master_suite) \
	test_detail.h \
	case12_test.cc
wcase12_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase12_test_LDFLAGS=$(additional_ldflags)
wcase12_test_LDADD=$(additional_libs)

case13_test_SOURCES=$(master_suite) \
	test_detail.h \
	case13_test.cc
case13_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case13_test_LDFLAGS=$(additional_ldflags)
case13_test_LDADD=$(additional_libs)

wcase13_test_SOURCES=$(master_suite) \
	test_detail.h \
	case13_test.cc
wcase13_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase13_test_LDFLAGS=$(additional_ldflags)
wcase13_test_LDADD=$(additional_libs)

case14_test_SOURCES=$(master_suite) \
	test_detail.h \
	case14_test.cc
case14_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case14_test_LDFLAGS=$(additional_ldflags)
case14_test_LDADD=$(additional_libs)

wcase14_test_SOURCES=$(master_suite) \
	test_detail.h \
	case14_test.cc
wcase14_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase14_test_LDFLAGS=$(additional_ldflags)
wcase14_test_LDADD=$(additional_libs)

case15_test_SOURCES=$(master_suite) \
	test_detail.h \
	case15_test.cc
case15_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case15_test_LDFLAGS=$(additional_ldflags)
case15_test_LDADD=$(additional_libs)

wcase15_test_SOURCES=$(master_suite) \
	test_detail.h \
	case15_test.cc
wcase15_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase15_test_LDFLAGS=$(additional_ldflags)
wcase15_test_LDADD=$(additional_libs)

case16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case16_test.cc
case16_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case16_test_LDFLAGS=$(additional_ldflags)
case16_test_LDADD=$(additional_libs)

wcase16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case16_test.cc
wcase16_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase16_test_LDFLAGS=$(additional_ldflags)
wcase16_test_LDADD=$(additional_libs)

case17_test_SOURCES=$(master_suite) \
	test_detail.h \
	case17_test.cc
case17_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case17_test_LDFLAGS=$(additional_ldflags)
case17_test_LDADD=$(additional_libs)

wcase17_test_SOURCES=$(master_suite) \
	test_detail.h \
	case17_test.cc
wcase17_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase17_test_LDFLAGS=$(additional_ldflags)
wcase17_test_LDADD=$(additional_libs)

case18_test_SOURCES=$(master_suite) \
	test_detail.h \
	case18_test.cc
case18_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case18_test_LDFLAGS=$(additional_ldflags)
case18_test_LDADD=$(additional_libs)

wcase18_test_SOURCES=$(master_suite) \
	test_detail.h \
	case18_test.cc
wcase18_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase18_test_LDFLAGS=$(additional_ldflags)
wcase18_test_LDADD=$(additional_libs)

case19_test_SOURCES=$(master_suite) \
	test_detail.h \
	case19_test.cc
case19_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case19_test_LDFLAGS=$(additional_ldflags)
case19_test_LDADD=$(additional_libs)

wcase19_test_SOURCES=$(master_suite) \
	test_detail.h \
	case19_test.cc
wcase19_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase19_test_LDFLAGS=$(additional_ldflags)
wcase19_test_LDADD=$(additional_libs)

case20_test_SOURCES=$(master_suite) \
	test_detail.h \
	case20_test.cc
case20_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case20_test_LDFLAGS=$(additional_ldflags)
case20_test_LDADD=$(additional_libs)

wcase20_test_SOURCES=$(master_suite) \
	test_detail.h \
	case20_test.cc
wcase20_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase20_test_LDFLAGS=$(additional_ldflags)
wcase20_test_LDADD=$(additional_libs)

case21_test_SOURCES=$(master_suite) \
	test_detail.h \
	case21_test.cc
case21_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
case21_test_LDFLAGS=$(additional_ldflags)
case21_test_LDADD=$(additional_libs)

wcase21_test_SOURCES=$(master_suite) \
	test_detail.h \
	case21_test.cc
wcase21_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wcase21_test_LDFLAGS=$(additional_ldflags)
wcase21_test_LDADD=$(additional_libs)


constraints_test_SOURCES=$(master_suite) \
	test_detail.h \
	constraints_test.cc
constraints_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
constraints_test_LDFLAGS=$(additional_ldflags)
constraints_test_LDADD=$(additional_libs)

wconstraints_test_SOURCES=$(master_suite) \
	test_detail.h \
	constraints_test.cc
wconstraints_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wconstraints_test_LDFLAGS=$(additional_ldflags)
wconstraints_test_LDADD=$(additional_libs)

value_test_SOURCES=$(master_suite) \
	test_detail.h \
	value_test.cc
value_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
value_test_LDFLAGS=$(additional_ldflags)
value_test_LDADD=$(additional_libs)

wvalue_test_SOURCES=$(master_suite) \
	test_detail.h \
	value_test.cc
wvalue_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wvalue_test_LDFLAGS=$(additional_ldflags)
wvalue_test_LDADD=$(additional_libs)

TESTS=\
	basic_test \
	wbasic_test \
	parse_test \
	wparse_test \
	accept_test \
	waccept_test \
	case2_test \
	wcase2_test \
	case3_test \
	wcase3_test \
	case4_test \
	wcase4_test \
	case5_test \
	wcase5_test \
	case6_test \
	wcase6_test \
	case7_test \
	wcase7_test \
	case8_test \
	wcase8_test \
	case9_test \
	wcase9_test \
	case10_test \
	wcase10_test \
	case11_test \
	wcase11_test \
	case12_test \
	wcase12_test \
	case13_test \
	wcase13_test \
	case14_test \
	wcase14_test \
	case15_test \
	wcase15_test \
	case16_test \
	wcase16_test \
	case17_test \
	wcase17_test \
	case18_test \
	wcase18_test \
	case19_test \
	wcase19_test \
	case20_test \
	wcase20_test \
	case21_test \
	wcase21_test \
	constraints_test \
	wconstraints_test \
	value_test \
	wvalue_test



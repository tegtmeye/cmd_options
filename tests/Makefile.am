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

char8_cpp_flags= \
	-DCMD_OPT_TEST_CHART=char -DLITERAL_PREFIX=u8

char16_cpp_flags= \
	-DCMD_OPT_TEST_CHART=char16_t -DLITERAL_PREFIX=u

char32_cpp_flags= \
	-DCMD_OPT_TEST_CHART=char32_t -DLITERAL_PREFIX=U

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
	basic_test basic8_test basic16_test basic32_test wbasic_test \
	parse_test parse8_test parse16_test parse32_test wparse_test \
	accept_test accept8_test accept16_test accept32_test waccept_test \
	case2_test case2_8_test case2_16_test case2_32_test wcase2_test \
	case3_test case3_8_test case3_16_test case3_32_test wcase3_test \
	case4_test case4_8_test case4_16_test case4_32_test wcase4_test \
	case5_test case5_8_test case5_16_test case5_32_test wcase5_test \
	case6_test case6_8_test case6_16_test case6_32_test wcase6_test \
	case7_test case7_8_test case7_16_test case7_32_test wcase7_test \
	case8_test case8_8_test case8_16_test case8_32_test wcase8_test \
	case9_test case9_8_test case9_16_test case9_32_test wcase9_test \
	case10_test case10_8_test case10_16_test case10_32_test wcase10_test \
	case11_test case11_8_test case11_16_test case11_32_test wcase11_test \
	case12_test case12_8_test case12_16_test case12_32_test wcase12_test \
	case13_test case13_8_test case13_16_test case13_32_test wcase13_test \
	case14_test case14_8_test case14_16_test case14_32_test wcase14_test \
	case15_test case15_8_test case15_16_test case15_32_test wcase15_test \
	case16_test case16_8_test case16_16_test case16_32_test wcase16_test \
	case17_test case17_8_test case17_16_test case17_32_test wcase17_test \
	case18_test case18_8_test case18_16_test case18_32_test wcase18_test \
	case19_test case19_8_test case19_16_test case19_32_test wcase19_test \
	case20_test case20_8_test case20_16_test case20_32_test wcase20_test \
	case21_test case21_8_test case21_16_test case21_32_test wcase21_test \
	constraints_test constraints8_test constraints16_test constraints32_test \
	  wconstraints_test \
	value_test value8_test value16_test value32_test wvalue_test \
	format_test format8_test format16_test format32_test wformat_test

basic_test_SOURCES=$(master_suite) \
	test_detail.h \
	basic_test.cc
basic_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
basic_test_LDFLAGS=$(additional_ldflags)
basic_test_LDADD=$(additional_libs)

basic8_test_SOURCES=$(master_suite) \
	test_detail.h \
	basic_test.cc
basic8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
basic8_test_LDFLAGS=$(additional_ldflags)
basic8_test_LDADD=$(additional_libs)

basic16_test_SOURCES=$(master_suite) \
	test_detail.h \
	basic_test.cc
basic16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
basic16_test_LDFLAGS=$(additional_ldflags)
basic16_test_LDADD=$(additional_libs)

basic32_test_SOURCES=$(master_suite) \
	test_detail.h \
	basic_test.cc
basic32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
basic32_test_LDFLAGS=$(additional_ldflags)
basic32_test_LDADD=$(additional_libs)

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

parse8_test_SOURCES=$(master_suite) \
	test_detail.h \
	parse_test.cc
parse8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
parse8_test_LDFLAGS=$(additional_ldflags)
parse8_test_LDADD=$(additional_libs)

parse16_test_SOURCES=$(master_suite) \
	test_detail.h \
	parse_test.cc
parse16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
parse16_test_LDFLAGS=$(additional_ldflags)
parse16_test_LDADD=$(additional_libs)

parse32_test_SOURCES=$(master_suite) \
	test_detail.h \
	parse_test.cc
parse32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
parse32_test_LDFLAGS=$(additional_ldflags)
parse32_test_LDADD=$(additional_libs)

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

accept8_test_SOURCES=$(master_suite) \
	test_detail.h \
	accept_test.cc
accept8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
accept8_test_LDFLAGS=$(additional_ldflags)
accept8_test_LDADD=$(additional_libs)

accept16_test_SOURCES=$(master_suite) \
	test_detail.h \
	accept_test.cc
accept16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
accept16_test_LDFLAGS=$(additional_ldflags)
accept16_test_LDADD=$(additional_libs)

accept32_test_SOURCES=$(master_suite) \
	test_detail.h \
	accept_test.cc
accept32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
accept32_test_LDFLAGS=$(additional_ldflags)
accept32_test_LDADD=$(additional_libs)

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

case2_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case2_test.cc
case2_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case2_8_test_LDFLAGS=$(additional_ldflags)
case2_8_test_LDADD=$(additional_libs)

case2_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case2_test.cc
case2_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case2_16_test_LDFLAGS=$(additional_ldflags)
case2_16_test_LDADD=$(additional_libs)

case2_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case2_test.cc
case2_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case2_32_test_LDFLAGS=$(additional_ldflags)
case2_32_test_LDADD=$(additional_libs)

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

case3_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case3_test.cc
case3_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case3_8_test_LDFLAGS=$(additional_ldflags)
case3_8_test_LDADD=$(additional_libs)

case3_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case3_test.cc
case3_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case3_16_test_LDFLAGS=$(additional_ldflags)
case3_16_test_LDADD=$(additional_libs)

case3_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case3_test.cc
case3_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case3_32_test_LDFLAGS=$(additional_ldflags)
case3_32_test_LDADD=$(additional_libs)

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

case4_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case4_test.cc
case4_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case4_8_test_LDFLAGS=$(additional_ldflags)
case4_8_test_LDADD=$(additional_libs)

case4_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case4_test.cc
case4_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case4_16_test_LDFLAGS=$(additional_ldflags)
case4_16_test_LDADD=$(additional_libs)

case4_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case4_test.cc
case4_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case4_32_test_LDFLAGS=$(additional_ldflags)
case4_32_test_LDADD=$(additional_libs)

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

case5_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case5_test.cc
case5_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case5_8_test_LDFLAGS=$(additional_ldflags)
case5_8_test_LDADD=$(additional_libs)

case5_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case5_test.cc
case5_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case5_16_test_LDFLAGS=$(additional_ldflags)
case5_16_test_LDADD=$(additional_libs)

case5_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case5_test.cc
case5_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case5_32_test_LDFLAGS=$(additional_ldflags)
case5_32_test_LDADD=$(additional_libs)

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

case6_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case6_test.cc
case6_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case6_8_test_LDFLAGS=$(additional_ldflags)
case6_8_test_LDADD=$(additional_libs)

case6_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case6_test.cc
case6_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case6_16_test_LDFLAGS=$(additional_ldflags)
case6_16_test_LDADD=$(additional_libs)

case6_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case6_test.cc
case6_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case6_32_test_LDFLAGS=$(additional_ldflags)
case6_32_test_LDADD=$(additional_libs)

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

case7_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case7_test.cc
case7_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case7_8_test_LDFLAGS=$(additional_ldflags)
case7_8_test_LDADD=$(additional_libs)

case7_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case7_test.cc
case7_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case7_16_test_LDFLAGS=$(additional_ldflags)
case7_16_test_LDADD=$(additional_libs)

case7_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case7_test.cc
case7_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case7_32_test_LDFLAGS=$(additional_ldflags)
case7_32_test_LDADD=$(additional_libs)

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

case8_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case8_test.cc
case8_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case8_8_test_LDFLAGS=$(additional_ldflags)
case8_8_test_LDADD=$(additional_libs)

case8_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case8_test.cc
case8_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case8_16_test_LDFLAGS=$(additional_ldflags)
case8_16_test_LDADD=$(additional_libs)

case8_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case8_test.cc
case8_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case8_32_test_LDFLAGS=$(additional_ldflags)
case8_32_test_LDADD=$(additional_libs)

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

case9_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case9_test.cc
case9_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case9_8_test_LDFLAGS=$(additional_ldflags)
case9_8_test_LDADD=$(additional_libs)

case9_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case9_test.cc
case9_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case9_16_test_LDFLAGS=$(additional_ldflags)
case9_16_test_LDADD=$(additional_libs)

case9_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case9_test.cc
case9_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case9_32_test_LDFLAGS=$(additional_ldflags)
case9_32_test_LDADD=$(additional_libs)

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

case10_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case10_test.cc
case10_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case10_8_test_LDFLAGS=$(additional_ldflags)
case10_8_test_LDADD=$(additional_libs)

case10_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case10_test.cc
case10_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case10_16_test_LDFLAGS=$(additional_ldflags)
case10_16_test_LDADD=$(additional_libs)

case10_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case10_test.cc
case10_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case10_32_test_LDFLAGS=$(additional_ldflags)
case10_32_test_LDADD=$(additional_libs)

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

case11_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case11_test.cc
case11_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case11_8_test_LDFLAGS=$(additional_ldflags)
case11_8_test_LDADD=$(additional_libs)

case11_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case11_test.cc
case11_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case11_16_test_LDFLAGS=$(additional_ldflags)
case11_16_test_LDADD=$(additional_libs)

case11_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case11_test.cc
case11_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case11_32_test_LDFLAGS=$(additional_ldflags)
case11_32_test_LDADD=$(additional_libs)

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

case12_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case12_test.cc
case12_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case12_8_test_LDFLAGS=$(additional_ldflags)
case12_8_test_LDADD=$(additional_libs)

case12_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case12_test.cc
case12_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case12_16_test_LDFLAGS=$(additional_ldflags)
case12_16_test_LDADD=$(additional_libs)

case12_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case12_test.cc
case12_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case12_32_test_LDFLAGS=$(additional_ldflags)
case12_32_test_LDADD=$(additional_libs)

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

case13_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case13_test.cc
case13_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case13_8_test_LDFLAGS=$(additional_ldflags)
case13_8_test_LDADD=$(additional_libs)

case13_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case13_test.cc
case13_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case13_16_test_LDFLAGS=$(additional_ldflags)
case13_16_test_LDADD=$(additional_libs)

case13_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case13_test.cc
case13_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case13_32_test_LDFLAGS=$(additional_ldflags)
case13_32_test_LDADD=$(additional_libs)

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

case14_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case14_test.cc
case14_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case14_8_test_LDFLAGS=$(additional_ldflags)
case14_8_test_LDADD=$(additional_libs)

case14_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case14_test.cc
case14_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case14_16_test_LDFLAGS=$(additional_ldflags)
case14_16_test_LDADD=$(additional_libs)

case14_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case14_test.cc
case14_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case14_32_test_LDFLAGS=$(additional_ldflags)
case14_32_test_LDADD=$(additional_libs)

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

case15_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case15_test.cc
case15_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case15_8_test_LDFLAGS=$(additional_ldflags)
case15_8_test_LDADD=$(additional_libs)

case15_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case15_test.cc
case15_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case15_16_test_LDFLAGS=$(additional_ldflags)
case15_16_test_LDADD=$(additional_libs)

case15_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case15_test.cc
case15_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case15_32_test_LDFLAGS=$(additional_ldflags)
case15_32_test_LDADD=$(additional_libs)

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

case16_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case16_test.cc
case16_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case16_8_test_LDFLAGS=$(additional_ldflags)
case16_8_test_LDADD=$(additional_libs)

case16_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case16_test.cc
case16_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case16_16_test_LDFLAGS=$(additional_ldflags)
case16_16_test_LDADD=$(additional_libs)

case16_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case16_test.cc
case16_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case16_32_test_LDFLAGS=$(additional_ldflags)
case16_32_test_LDADD=$(additional_libs)

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

case17_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case17_test.cc
case17_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case17_8_test_LDFLAGS=$(additional_ldflags)
case17_8_test_LDADD=$(additional_libs)

case17_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case17_test.cc
case17_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case17_16_test_LDFLAGS=$(additional_ldflags)
case17_16_test_LDADD=$(additional_libs)

case17_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case17_test.cc
case17_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case17_32_test_LDFLAGS=$(additional_ldflags)
case17_32_test_LDADD=$(additional_libs)

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

case18_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case18_test.cc
case18_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case18_8_test_LDFLAGS=$(additional_ldflags)
case18_8_test_LDADD=$(additional_libs)

case18_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case18_test.cc
case18_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case18_16_test_LDFLAGS=$(additional_ldflags)
case18_16_test_LDADD=$(additional_libs)

case18_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case18_test.cc
case18_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case18_32_test_LDFLAGS=$(additional_ldflags)
case18_32_test_LDADD=$(additional_libs)

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

case19_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case19_test.cc
case19_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case19_8_test_LDFLAGS=$(additional_ldflags)
case19_8_test_LDADD=$(additional_libs)

case19_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case19_test.cc
case19_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case19_16_test_LDFLAGS=$(additional_ldflags)
case19_16_test_LDADD=$(additional_libs)

case19_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case19_test.cc
case19_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case19_32_test_LDFLAGS=$(additional_ldflags)
case19_32_test_LDADD=$(additional_libs)

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

case20_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case20_test.cc
case20_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case20_8_test_LDFLAGS=$(additional_ldflags)
case20_8_test_LDADD=$(additional_libs)

case20_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case20_test.cc
case20_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case20_16_test_LDFLAGS=$(additional_ldflags)
case20_16_test_LDADD=$(additional_libs)

case20_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case20_test.cc
case20_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case20_32_test_LDFLAGS=$(additional_ldflags)
case20_32_test_LDADD=$(additional_libs)

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

case21_8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case21_test.cc
case21_8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
case21_8_test_LDFLAGS=$(additional_ldflags)
case21_8_test_LDADD=$(additional_libs)

case21_16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case21_test.cc
case21_16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
case21_16_test_LDFLAGS=$(additional_ldflags)
case21_16_test_LDADD=$(additional_libs)

case21_32_test_SOURCES=$(master_suite) \
	test_detail.h \
	case21_test.cc
case21_32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
case21_32_test_LDFLAGS=$(additional_ldflags)
case21_32_test_LDADD=$(additional_libs)

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

constraints8_test_SOURCES=$(master_suite) \
	test_detail.h \
	constraints_test.cc
constraints8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
constraints8_test_LDFLAGS=$(additional_ldflags)
constraints8_test_LDADD=$(additional_libs)

constraints16_test_SOURCES=$(master_suite) \
	test_detail.h \
	constraints_test.cc
constraints16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
constraints16_test_LDFLAGS=$(additional_ldflags)
constraints16_test_LDADD=$(additional_libs)

constraints32_test_SOURCES=$(master_suite) \
	test_detail.h \
	constraints_test.cc
constraints32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
constraints32_test_LDFLAGS=$(additional_ldflags)
constraints32_test_LDADD=$(additional_libs)

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

value8_test_SOURCES=$(master_suite) \
	test_detail.h \
	value_test.cc
value8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
value8_test_LDFLAGS=$(additional_ldflags)
value8_test_LDADD=$(additional_libs)

value16_test_SOURCES=$(master_suite) \
	test_detail.h \
	value_test.cc
value16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
value16_test_LDFLAGS=$(additional_ldflags)
value16_test_LDADD=$(additional_libs)

value32_test_SOURCES=$(master_suite) \
	test_detail.h \
	value_test.cc
value32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
value32_test_LDFLAGS=$(additional_ldflags)
value32_test_LDADD=$(additional_libs)

wvalue_test_SOURCES=$(master_suite) \
	test_detail.h \
	value_test.cc
wvalue_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wvalue_test_LDFLAGS=$(additional_ldflags)
wvalue_test_LDADD=$(additional_libs)

format_test_SOURCES=$(master_suite) \
	test_detail.h \
	format_test.cc
format_test_CPPFLAGS=$(char_cpp_flags) $(additional_cppflags)
format_test_LDFLAGS=$(additional_ldflags)
format_test_LDADD=$(additional_libs)

format8_test_SOURCES=$(master_suite) \
	test_detail.h \
	format_test.cc
format8_test_CPPFLAGS=$(char8_cpp_flags) $(additional_cppflags)
format8_test_LDFLAGS=$(additional_ldflags)
format8_test_LDADD=$(additional_libs)

format16_test_SOURCES=$(master_suite) \
	test_detail.h \
	format_test.cc
format16_test_CPPFLAGS=$(char16_cpp_flags) $(additional_cppflags)
format16_test_LDFLAGS=$(additional_ldflags)
format16_test_LDADD=$(additional_libs)

format32_test_SOURCES=$(master_suite) \
	test_detail.h \
	format_test.cc
format32_test_CPPFLAGS=$(char32_cpp_flags) $(additional_cppflags)
format32_test_LDFLAGS=$(additional_ldflags)
format32_test_LDADD=$(additional_libs)

wformat_test_SOURCES=$(master_suite) \
	test_detail.h \
	format_test.cc
wformat_test_CPPFLAGS=$(wchar_cpp_flags) $(additional_cppflags)
wformat_test_LDFLAGS=$(additional_ldflags)
wformat_test_LDADD=$(additional_libs)

TESTS=\
	basic_test basic8_test basic16_test basic32_test wbasic_test \
	parse_test parse8_test parse16_test parse32_test wparse_test \
	accept_test accept8_test accept16_test accept32_test waccept_test \
	case2_test case2_8_test case2_16_test case2_32_test wcase2_test
	case3_test case3_8_test case3_16_test case3_32_test wcase3_test
	case4_test case4_8_test case4_16_test case4_32_test wcase4_test \
	case5_test case5_8_test case5_16_test case5_32_test wcase5_test \
	case6_test case6_8_test case6_16_test case6_32_test wcase6_test \
	case7_test case7_8_test case7_16_test case7_32_test wcase7_test \
	case8_test case8_8_test case8_16_test case8_32_test wcase8_test \
	case9_test case9_8_test case9_16_test case9_32_test wcase9_test \
	case10_test case10_8_test case10_16_test case10_32_test wcase10_test \
	case11_test case11_8_test case11_16_test case11_32_test wcase11_test \
	case12_test case12_8_test case12_16_test case12_32_test wcase12_test \
	case13_test case13_8_test case13_16_test case13_32_test wcase13_test \
	case14_test case14_8_test case14_16_test case14_32_test wcase14_test \
	case15_test case15_8_test case15_16_test case15_32_test wcase15_test \
	case16_test case16_8_test case16_16_test case16_32_test wcase16_test \
	case17_test case17_8_test case17_16_test case17_32_test wcase17_test \
	case18_test case18_8_test case18_16_test case18_32_test wcase18_test \
	case19_test case19_8_test case19_16_test case19_32_test wcase19_test \
	case20_test case20_8_test case20_16_test case20_32_test wcase20_test \
	case21_test case21_8_test case21_16_test case21_32_test wcase21_test \
	constraints_test constraints8_test constraints16_test \
		constraints32_test wconstraints_test \
	value_test value8_test value16_test value32_test wvalue_test \
	format_test format8_test format16_test format32_test wformat_test

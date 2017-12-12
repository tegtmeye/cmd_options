#  Copyright (c) 2017, Mike Tegtmeyer
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
	$(SYSTEM_BOOST_CPPFLAGS)

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
	parse_test \
	accept_test \
	case2_test \
	case3_test \
	case4_test \
	case5_test \
	case6_test \
	case7_test \
	case8_test \
	case9_test \
	case10_test \
	case11_test \
	case12_test \
	case13_test \
	case14_test \
	case15_test \
	case16_test \
	case17_test \
	case18_test \
	case19_test \
	case20_test \
	case21_test

basic_test_SOURCES=$(master_suite) \
	test_detail.h \
	basic_test.cc 
basic_test_CPPFLAGS=$(additional_cppflags)
basic_test_LDFLAGS=$(additional_ldflags)
basic_test_LDADD=$(additional_libs)

parse_test_SOURCES=$(master_suite) \
	test_detail.h \
	parse_test.cc 
parse_test_CPPFLAGS=$(additional_cppflags)
parse_test_LDFLAGS=$(additional_ldflags)
parse_test_LDADD=$(additional_libs)

accept_test_SOURCES=$(master_suite) \
	test_detail.h \
	accept_test.cc 
accept_test_CPPFLAGS=$(additional_cppflags)
accept_test_LDFLAGS=$(additional_ldflags)
accept_test_LDADD=$(additional_libs)

case2_test_SOURCES=$(master_suite) \
	test_detail.h \
	case2_test.cc 
case2_test_CPPFLAGS=$(additional_cppflags)
case2_test_LDFLAGS=$(additional_ldflags)
case2_test_LDADD=$(additional_libs)

case3_test_SOURCES=$(master_suite) \
	test_detail.h \
	case3_test.cc 
case3_test_CPPFLAGS=$(additional_cppflags)
case3_test_LDFLAGS=$(additional_ldflags)
case3_test_LDADD=$(additional_libs)

case4_test_SOURCES=$(master_suite) \
	test_detail.h \
	case4_test.cc 
case4_test_CPPFLAGS=$(additional_cppflags)
case4_test_LDFLAGS=$(additional_ldflags)
case4_test_LDADD=$(additional_libs)

case5_test_SOURCES=$(master_suite) \
	test_detail.h \
	case5_test.cc 
case5_test_CPPFLAGS=$(additional_cppflags)
case5_test_LDFLAGS=$(additional_ldflags)
case5_test_LDADD=$(additional_libs)

case6_test_SOURCES=$(master_suite) \
	test_detail.h \
	case6_test.cc 
case6_test_CPPFLAGS=$(additional_cppflags)
case6_test_LDFLAGS=$(additional_ldflags)
case6_test_LDADD=$(additional_libs)

case7_test_SOURCES=$(master_suite) \
	test_detail.h \
	case7_test.cc 
case7_test_CPPFLAGS=$(additional_cppflags)
case7_test_LDFLAGS=$(additional_ldflags)
case7_test_LDADD=$(additional_libs)

case8_test_SOURCES=$(master_suite) \
	test_detail.h \
	case8_test.cc 
case8_test_CPPFLAGS=$(additional_cppflags)
case8_test_LDFLAGS=$(additional_ldflags)
case8_test_LDADD=$(additional_libs)

case9_test_SOURCES=$(master_suite) \
	test_detail.h \
	case9_test.cc 
case9_test_CPPFLAGS=$(additional_cppflags)
case9_test_LDFLAGS=$(additional_ldflags)
case9_test_LDADD=$(additional_libs)

case10_test_SOURCES=$(master_suite) \
	test_detail.h \
	case10_test.cc 
case10_test_CPPFLAGS=$(additional_cppflags)
case10_test_LDFLAGS=$(additional_ldflags)
case10_test_LDADD=$(additional_libs)

case11_test_SOURCES=$(master_suite) \
	test_detail.h \
	case11_test.cc 
case11_test_CPPFLAGS=$(additional_cppflags)
case11_test_LDFLAGS=$(additional_ldflags)
case11_test_LDADD=$(additional_libs)

case12_test_SOURCES=$(master_suite) \
	test_detail.h \
	case12_test.cc 
case12_test_CPPFLAGS=$(additional_cppflags)
case12_test_LDFLAGS=$(additional_ldflags)
case12_test_LDADD=$(additional_libs)

case13_test_SOURCES=$(master_suite) \
	test_detail.h \
	case13_test.cc 
case13_test_CPPFLAGS=$(additional_cppflags)
case13_test_LDFLAGS=$(additional_ldflags)
case13_test_LDADD=$(additional_libs)

case14_test_SOURCES=$(master_suite) \
	test_detail.h \
	case14_test.cc 
case14_test_CPPFLAGS= \
	-DCMD_OPTIONS_DEFAULT_OPERAND_KEY=alt_key \
	$(additional_cppflags)
case14_test_LDFLAGS=$(additional_ldflags)
case14_test_LDADD=$(additional_libs)

case15_test_SOURCES=$(master_suite) \
	test_detail.h \
	case15_test.cc 
case15_test_CPPFLAGS=$(additional_cppflags)
case15_test_LDFLAGS=$(additional_ldflags)
case15_test_LDADD=$(additional_libs)

case16_test_SOURCES=$(master_suite) \
	test_detail.h \
	case16_test.cc 
case16_test_CPPFLAGS=$(additional_cppflags)
case16_test_LDFLAGS=$(additional_ldflags)
case16_test_LDADD=$(additional_libs)

case17_test_SOURCES=$(master_suite) \
	test_detail.h \
	case17_test.cc 
case17_test_CPPFLAGS=$(additional_cppflags)
case17_test_LDFLAGS=$(additional_ldflags)
case17_test_LDADD=$(additional_libs)

case18_test_SOURCES=$(master_suite) \
	test_detail.h \
	case18_test.cc 
case18_test_CPPFLAGS=$(additional_cppflags)
case18_test_LDFLAGS=$(additional_ldflags)
case18_test_LDADD=$(additional_libs)

case19_test_SOURCES=$(master_suite) \
	test_detail.h \
	case19_test.cc 
case19_test_CPPFLAGS=$(additional_cppflags)
case19_test_LDFLAGS=$(additional_ldflags)
case19_test_LDADD=$(additional_libs)

case20_test_SOURCES=$(master_suite) \
	test_detail.h \
	case20_test.cc 
case20_test_CPPFLAGS=$(additional_cppflags)
case20_test_LDFLAGS=$(additional_ldflags)
case20_test_LDADD=$(additional_libs)

case21_test_SOURCES=$(master_suite) \
	test_detail.h \
	case21_test.cc 
case21_test_CPPFLAGS=$(additional_cppflags)
case21_test_LDFLAGS=$(additional_ldflags)
case21_test_LDADD=$(additional_libs)

TESTS=\
	basic_test \
	parse_test \
	accept_test \
	case2_test \
	case3_test \
	case4_test \
	case5_test \
	case6_test \
	case7_test \
	case8_test \
	case9_test \
	case10_test \
	case11_test \
	case12_test \
	case13_test \
	case14_test \
	case15_test \
	case16_test \
	case17_test \
	case18_test \
	case19_test \
	case20_test \
	case21_test

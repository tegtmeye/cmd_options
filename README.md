cmd_options
=======

I wrote this header-only options parser to specifically address limitations of other common parsers in an easy-to-use syntax while taking advantage of language features available in c++11.

Current support is for \*nix although there should be no reason it cannot support Windows as well. If there are interested folks in this capability, please contact me.

The library provides large amounts of standard and custom functionality to include strict POSIX or GNU-style argument syntax, constraints on the number of argument or operands, option and operand mutual inclusion and exclusion, implied options and operands (i.e. if --foo implies --bar). Additionally, a low-level API is exposed that allows significant customization of option and operand handling while still integrating into the standard interface.

Take a look at the wiki for additional documentation on advantages, limitations, and sample code.

Licensing is permissive BSD 3-clause. If this bothers you and you would like to use it under a different licensing model, let me know. I generally don't care as long as someone doesn't claim they wrote something that they didn't.

# License

BSD 3-Clause

Copyright (c) 2012-2018, Mike Tegtmeyer
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may
      be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


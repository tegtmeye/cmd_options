cmd_options
=======

I wrote this header-only options parser to specifically address limitations of other common parsers in an easy-to-use syntax while taking advantage of language features available in c++11.

Current support is for \*nix although there should be no reason it cannot support Windows as well. If there are interested folks in this capability, please contact me.

Licensing is permissive BSD 3-clause. If this bothers you and you would like to use it under a different licensing model, let me know. I generally don't care as long as someone doesn't claim they wrote something that they didn't.

## For the impatient

1. Ensure you are using at least a c++11 compliant compiler. If using c++11 then you must compile against a recent version of boost::any (>1.55). If you are compiling with c++17 or greater then there are no external dependancies. All code lives in the `cmd_options` namespace.
1. Do one of two things:
   1. Drop the cmd_options.h file into your project
   1. Install the header file in the standard auto tool fashion and make sure your include path references this location. If you need boost and it is installed in a non-standard location, add the option`--with-boost=DIR` to `./configure` where `DIR` is the correct path to your boost installation.
```
./configure --prefix=/path/to/installation/dir
make
make install
```
3. In your code, create an `options_group` that describes how you want the options to be parsed. This is simply a `std::vector` of `option_descriptions`---one for each option or operand. Function's assist in the creation for the most common use cases. The following accepts: `--foo (or -f) -b <arg> -i [arg] operand`.

```
namespace co = cmd_options;

co::option_group grp{
  co::make_option("foo,f","Description of this no-arg option"), // accepts --foo and -f
  co::make_option(",b",co::value<std::string>(),
    "Description of option that requires a string argument"), // accepts -b <arg>
  co::make_option(",i",co::value<int>().implicit(42),
    "Description of option with an optional integer argument"), // accepts -i <arg> or -i with default 42
  co::make_operand("Description of this operand that takes a string", co::value<std::string>())
};
```

4. Once the option group has been created, simply pass it to the parse function along with the array of strings to parse. If no errors exist, the result will be returned in a std::multimap<key_string,{boost-or-std}::any>. For convenience this has been typedefed to `variable_map` which you can use if you want to. The results can be examined using standard std::multimap access.

```
namespace co = cmd_options;

co::option_group grp{...};
co::variable_map vm = co::parse_options(argc,argv,grp);
if(vm.count("foo"))
  std::cout << "--foo or -f was given\n";
```

The library provides much more functionality to include strict POSIX or GNU-style argument syntax, constraints on the number of argument or operands, option and operand mutual inclusion and exclusion, implied options and operands (i.e. if --foo implies --bar). Additionally, a low-level API is exposed that allows significant customization of option and operand handling while still integrating into the standard interface.

Take a look at the wiki for additional documentation on advantages, limitations, and sample code.

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


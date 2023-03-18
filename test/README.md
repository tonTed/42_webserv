# Doctest (version is "1.2.0")

## Summary:
- [Getting started](#getting-started)
  * [Configuring the project](#configuring-the-project)
  * [Writing tests](#writing-tests)
    + [Verbose asserts](#verbose-asserts)
    + [Binary and unary asserts](#binary-and-unary-asserts)
    + [More details on the asserts](#more-details-on-the-asserts)
    + [Example:](#example)
- [Context Class](#context-class)
  * [Class Methods](#class-methods)
  * [Context Options](#context-options)

## Getting started
### Configuring the project
```c++
// this tells doctest to provide a main() - only do this in one cpp file
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

int main(int argc, char** argv)
{
	doctest::Context context;
	
	context.applyCommandLine(argc, argv); // this parses the command line
	
	context.setOption("no-breaks", true); // don't break in the debugger when assertions fail
		//more options can be found in the docs
		
	int res = context.run(); // run
	
	if(context.shouldExit())
        return res;
	return res; // the result from doctest is propagated here as well
}
```

### Writing tests

There are 3 levels of assert severity for all assertion macros:

- ```REQUIRE``` - this level will immediately quit the test case if the assert fails and will mark the test case as failed.
- ```CHECK``` - this level will mark the test case as failed if the assert fails but will continue with the test case.
- ```WARN``` - this level will only print a message if the assert fails but will not mark the test case as failed.

The ```CHECK``` level is mostly useful if you have a series of essentially orthogonal assertions and it is useful to see all the results rather than stopping at the first failure.


#### Verbose asserts
- These asserts also have a ```_MESSAGE``` form - like ```CHECK_MESSAGE(expression, message)``` which is basically a code block ```{}``` with a scoped [**```INFO()```**](logging.md#info) logging macro together with the ```CHECK``` macro - that way the message will be relevant only to that assert. The binary/unary asserts don't have this variation yet.

Examples:

```c++
INFO("this is relevant to all asserts, and here is some var: ", local);

CHECK_MESSAGE(a < b, "relevant only to this assert ", other_local, " more text!");

CHECK(b < c); // here only the first INFO() will be relevant
```

#### Binary and unary asserts

These asserts don't use templates to decompose the comparison expressions for the left and right parts.

These have the same guarantees as the expression decomposing ones but [**57-68% faster**](benchmarks.md#cost-of-an-assertion-macro) for compilation.

```<LEVEL>``` is one of 3 possible: ```REQUIRE```/```CHECK```/```WARN```.

- ```<LEVEL>_EQ(left, right)``` - same as ```<LEVEL>(left == right)```
- ```<LEVEL>_NE(left, right)``` - same as ```<LEVEL>(left != right)```
- ```<LEVEL>_GT(left, right)``` - same as ```<LEVEL>(left >  right)```
- ```<LEVEL>_LT(left, right)``` - same as ```<LEVEL>(left <  right)```
- ```<LEVEL>_GE(left, right)``` - same as ```<LEVEL>(left >= right)```
- ```<LEVEL>_LE(left, right)``` - same as ```<LEVEL>(left <= right)```
- ```<LEVEL>_UNARY(expr)``` - same as ```<LEVEL>(expr)```
- ```<LEVEL>_UNARY_FALSE(expr)``` - same as ```<LEVEL>_FALSE(expr)```

#### More details on the asserts
- Exceptions
- Floating point
- Strings
- NaN

https://github.com/doctest/doctest/blob/master/doc/markdown/assertions.md

#### Example

Asserts can be used outside of a testing context (in code not called from a TEST_CASE()) instead of assert().

```c++
TEST_CASE("some test")
{
	CHECK(1 == 1);
	CHECK_EQ(1, 1);
	CHECK(1.0f == doctest::Approx(1.0));
	CHECK("hello" == std::string("hello"));
	CHECK(std::string("hello") == "hello");
	CHECK(std::string("hello") != std::string("world"));
	
	CHECK_THROWS_AS(throw std::runtime_error("hello"), std::runtime_error);
	CHECK_MESSAGE(1 == 2, "this is a message");
	INFO("this is an info message");
	...
}

```

## Context Class

### Class Methods

```c++
void Context::applyCommandLine(int, const char* const*) {}
void Context::addFilter(const char*, const char*) {}
void Context::clearFilters() {}
void Context::setOption(const char*, int) {}
void Context::setOption(const char*, const char*) {}
bool Context::shouldExit() { return false; }
int  Context::run() { return 0; }
```

### Context Options
- boolean values: "1/on/yes/true" or "0/off/no/false"
- filter  values: "str1,str2,str3" (comma separated strings)
- filters use wildcards for matching strings something passes a filter if any of the strings in a filter matches
- ALL FLAGS, OPTIONS AND FILTERS ALSO AVAILABLE WITH A "dt-" PREFIX!!!

#### Query flags - the program quits after them. Available:
<table>
<tbody>
  <tr>
    <td class="tg-0lax">-?</td>
    <td class="tg-0lax">--help, -h</td>
    <td class="tg-0lax">prints this message</td>
  </tr>
  <tr>
    <td class="tg-0lax">-v</td>
    <td class="tg-0lax">--version</td>
    <td class="tg-0lax">prints the version</td>
  </tr>
  <tr>
    <td class="tg-0lax">-c</td>
    <td class="tg-0lax">--count</td>
    <td class="tg-0lax">prints the number of matching tests</td>
  </tr>
  <tr>
    <td class="tg-0lax">-ltc</td>
    <td class="tg-0lax">--list-test-cases</td>
    <td class="tg-0lax">lists all matching tests by name</td>
  </tr>
  <tr>
    <td class="tg-0lax">-lts</td>
    <td class="tg-0lax">--list-test-suites</td>
    <td class="tg-0lax">lists all matching test suites</td>
  </tr>
</tbody>
</table>

#### The available \<int>/\<string> options/filters are:
<table>
<tbody>
  <tr>
    <td class="tg-0pky">-tc</td>
    <td class="tg-0pky">--test-case=&lt;filters&gt;</td>
    <td class="tg-0pky">filters&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;tests by their name</td>
  </tr>
  <tr>
    <td class="tg-0pky">-tce</td>
    <td class="tg-0pky">--test-case-exclude=&lt;filters&gt;</td>
    <td class="tg-0pky">filters OUT tests by their name</td>
  </tr>
  <tr>
    <td class="tg-0pky">-sf</td>
    <td class="tg-0pky">--source-file=&lt;filters&gt;</td>
    <td class="tg-0pky">filters&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;tests by their file</td>
  </tr>
  <tr>
    <td class="tg-0pky">-sfe</td>
    <td class="tg-0pky">--source-file-exclude=&lt;filters&gt;</td>
    <td class="tg-0pky">filters OUT tests by their file</td>
  </tr>
  <tr>
    <td class="tg-0pky">-ts</td>
    <td class="tg-0pky">--test-suite=&lt;filters&gt;</td>
    <td class="tg-0pky">filters&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;tests by their test suite</td>
  </tr>
  <tr>
    <td class="tg-0pky">-tse</td>
    <td class="tg-0pky">--test-suite-exclude=&lt;filters&gt;</td>
    <td class="tg-0pky">filters OUT tests by their test suite</td>
  </tr>
  <tr>
    <td class="tg-0pky">-sc</td>
    <td class="tg-0pky">--subcase=&lt;filters&gt;</td>
    <td class="tg-0pky">filters&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;subcases by their name</td>
  </tr>
  <tr>
    <td class="tg-0pky">-sce</td>
    <td class="tg-0pky">--subcase-exclude=&lt;filters&gt;</td>
    <td class="tg-0pky">filters OUT subcases by their name</td>
  </tr>
  <tr>
    <td class="tg-0pky">-ob</td>
    <td class="tg-0pky">--order-by=&lt;string&gt;</td>
    <td class="tg-0pky">how the tests should be ordered &lt;string&gt; - by [file/suite/name/rand]</td>
  </tr>
  <tr>
    <td class="tg-0pky">-rs</td>
    <td class="tg-0pky">--rand-seed=&lt;int&gt;</td>
    <td class="tg-0pky">seed for random ordering</td>
  </tr>
  <tr>
    <td class="tg-0pky">-f</td>
    <td class="tg-0pky">--first=&lt;int&gt;</td>
    <td class="tg-0pky">the first test passing the filters to execute - for range-based execution</td>
  </tr>
  <tr>
    <td class="tg-0pky">-l</td>
    <td class="tg-0pky">--last=&lt;int&gt;</td>
    <td class="tg-0pky">the last test passing the filters to execute - for range-based execution</td>
  </tr>
  <tr>
    <td class="tg-0pky">-aa</td>
    <td class="tg-0pky">--abort-after=&lt;int&gt;</td>
    <td class="tg-0pky">stop after &lt;int&gt; failed assertions</td>
  </tr>
  <tr>
    <td class="tg-0pky">-scfl</td>
    <td class="tg-0pky">--subcase-filter-levels=&lt;int&gt;</td>
    <td class="tg-0pky">apply filters for the first &lt;int&gt; levels</td>
  </tr>
</tbody>
</table>

#### Bool options - can be used like flags and true is assumed. Available:
<table>
<tbody>
  <tr>
    <td class="tg-0lax">-s</td>
    <td class="tg-0lax">--success=&lt;bool&gt;</td>
    <td class="tg-0lax">include successful assertions in output</td>
  </tr>
  <tr>
    <td class="tg-0lax">-cs</td>
    <td class="tg-0lax">--case-sensitive=&lt;bool&gt;</td>
    <td class="tg-0lax">filters being treated as case sensitive</td>
  </tr>
  <tr>
    <td class="tg-0lax">-e</td>
    <td class="tg-0lax">--exit=&lt;bool&gt;</td>
    <td class="tg-0lax">exits after the tests finish</td>
  </tr>
  <tr>
    <td class="tg-0lax">-d</td>
    <td class="tg-0lax">--duration=&lt;bool&gt;</td>
    <td class="tg-0lax">prints the time duration of each test</td>
  </tr>
  <tr>
    <td class="tg-0lax">-nt</td>
    <td class="tg-0lax">--no-throw=&lt;bool&gt;</td>
    <td class="tg-0lax">skips exceptions-related assert checks</td>
  </tr>
  <tr>
    <td class="tg-0lax">-ne</td>
    <td class="tg-0lax">--no-exitcode=&lt;bool&gt;</td>
    <td class="tg-0lax">returns (or exits) always with success</td>
  </tr>
  <tr>
    <td class="tg-0lax">-nr</td>
    <td class="tg-0lax">--no-run=&lt;bool&gt;</td>
    <td class="tg-0lax">skips all runtime doctest operations</td>
  </tr>
  <tr>
    <td class="tg-0lax">-nv</td>
    <td class="tg-0lax">--no-version=&lt;bool&gt;</td>
    <td class="tg-0lax">omit the framework version in the output</td>
  </tr>
  <tr>
    <td class="tg-0lax">-nc</td>
    <td class="tg-0lax">--no-colors=&lt;bool&gt;</td>
    <td class="tg-0lax">disables colors in output</td>
  </tr>
  <tr>
    <td class="tg-0lax">-fc</td>
    <td class="tg-0lax">--force-colors=&lt;bool&gt;</td>
    <td class="tg-0lax">use colors even when not in a tty</td>
  </tr>
  <tr>
    <td class="tg-0lax">-nb</td>
    <td class="tg-0lax">--no-breaks=&lt;bool&gt;</td>
    <td class="tg-0lax">disables breakpoints in debuggers</td>
  </tr>
  <tr>
    <td class="tg-0lax">-ns</td>
    <td class="tg-0lax">--no-skip=&lt;bool&gt;</td>
    <td class="tg-0lax">don't skip test cases marked as skip</td>
  </tr>
  <tr>
    <td class="tg-0lax">-npf</td>
    <td class="tg-0lax">--no-path-filenames=&lt;bool&gt;</td>
    <td class="tg-0lax">only filenames and no paths in output</td>
  </tr>
  <tr>
    <td class="tg-0lax">-nln</td>
    <td class="tg-0lax">--no-line-numbers=&lt;bool&gt;</td>
    <td class="tg-0lax">0 instead of real line numbers in output</td>
  </tr>
</tbody>
</table>

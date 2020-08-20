# Console++
A header-only cross-platform console application framework for writing command line tools easily.
## Features
 - Easy to use API:
```cpp
ConsoleApp app("TestApp"); // create an application
app.Description("Just for testing, does nothing") // set description
    .UsageDesc("testapp [options...]") // usage description
    .Version("v1.0.0") // version
    .CommandLineArgs() // start setting command line arguments
    .AddArg2Hyphens<int>() // add an option --some_int <number>
        .Name("some_int")
        .Required(true)
        .Help("Any integer")
        .Build()
    // ...
    .AddArg2Hyphens<NoType>() // add --help flag
        .Name("help")
        .Help("Print help message")
        .Build()
    .Parse(argc, argv); // parse arguments from argc, argv
// ...
if (app.CommandLineArgs().HasArg("help")) { // if has '--help' option
    app.PrintHelp(true); // print generated help message; true - with header (version, description)
    // Prints this:
    // 
    // TestApp v1.0.0
    // Just for testing, does nothing
    // Usage: testapp [options...]
    // Arguments:
    //     --some_int (required) Any integer
    //     --help     (optional) Print help message
    return 0;
}
if (app.PrintErrors()) { // if some error detected (not all required options passed, etc.)
    app.PrintHelp(false); // print generated help message; false - without header
    // Prints this:
    // 
    // Usage: testapp [options...]
    // Arguments:
    //     --some_int (required) Any integer
    //     --help     (optional) Print help message
    return 0;
}
// get value of an argument
if (int* val = app.CommandLineArgs().GetArg<int>("some_int"); val) {
    app.Log(" - {:<20} = {}", "Some Int", *val);
}
```
 - Python-like string formatting:
```cpp
FormatStr("{} + {} = {}", 2, 2, 5);  // 2 + 2 = 5
FormatStr("{2} {0} {1}", 1, 2, 3);   // 3 1 2
FormatStr("A:{:>15.3f}", 2.8888);    // A:          2.889
FormatStr("B:{:>15.1f}", 2.8888);    // B:            2.9
FormatStr("C:{:>15.9f}", 2.8888);    // C:         2.8888
```
# Building
No build tools required, just copy the folder 'conpp' to your project and `#include "conpp/conpp.h"`.
# Usage
See an example with comments in 'test.cpp'.

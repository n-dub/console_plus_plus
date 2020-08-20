#include "conpp/conpp.h"

using namespace conpp;

int main(int argc, char** argv) {
	ConsoleApp app("TestApp");
	app.Description("Just for testing, prints options")
		.UsageDesc("testapp [options...]")
		.Version("v1.0.0")
		.CommandLineArgs()
		.AddArg2Hyphens<int>()
			.Name("some_int")
			.Required(true)
			.Help("Any integer")
			.Build()
		.AddArg2Hyphens<NoType>()
			.Name("some_flag")
			.Help("A flag")
			.Build()
		.AddArg2Hyphens<float>()
			.Name("some_optional_float")
			.Required(false)
			.Build()
		.AddArg2Hyphens<NoType>()
			.Name("help")
			.Help("Print help message")
			.Build()
		.Parse(argc, argv);

	if (app.CommandLineArgs().HasArg("help") || app.PrintErrors()) {
		app.PrintHelp(true);
		return 0;
	}

	if (int* val = app.CommandLineArgs().GetArg<int>("some_int"); val) {
		app.Log(" - {:<20} = {}", "Some Int", *val);
	}

	if (float* val = app.CommandLineArgs().GetArg<float>("some_optional_float"); val) {
		app.Log(" - {:<20} = {}", "Some Float", *val);
	}

	if (app.CommandLineArgs().HasArg("some_flag")) {
		app.Log(" - {:<20} = True", "Some Flag");
	}
}

#include "conpp/conpp.h"

using namespace conpp;

int main(int argc, char** argv) {
	ConsoleApp app("TestApp");
	app.Description("Just for testing, prints options")
		.UsageDesc("testapp [options...]")
		.Version("v1.0.0")
		.CommandLineArgs()
		.AddOption2Hyphens<int>()
			.Name("some_int")
			.Required(true)
			.Help("Any integer")
			.Build()
		.AddOption2Hyphens<NoType>()
			.Name("some_flag")
			.Help("A flag")
			.Build()
		.AddOption2Hyphens<float>()
			.Name("some_optional_float")
			.Required(false)
			.Build()
		.AddOptionHelp()
		.Parse(argc, argv);

	if (app.CommandLineArgs().HasOption("help") || app.PrintErrors()) {
		app.PrintHelp(false);
		return 0;
	}

	if (int* val = app.CommandLineArgs().GetOption<int>("some_int"); val) {
		app.Log(" - {:<20} = {}", "Some Int", *val);
	}

	if (float* val = app.CommandLineArgs().GetOption<float>("some_optional_float"); val) {
		app.Log(" - {:<20} = {}", "Some Float", *val);
	}

	if (app.CommandLineArgs().HasOption("some_flag")) {
		app.Log(" - {:<20} = True", "Some Flag");
	}
}

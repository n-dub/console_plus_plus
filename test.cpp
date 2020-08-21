//   Copyright 2020 Nikita Dubovikov
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//	   http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
#include "conpp/conpp.h"

using namespace conpp;
using namespace conpp::literals;

int main(int argc, char** argv) {
	ConsoleApp app("TestApp");
	app.Log("#{};Red#{};Green#{};Blue", Color::Red, Color::Green, Color::Blue);
	app.Description("Just for testing, prints options")
		.UsageDesc("testapp [options...]")
		.Version("v1.0.0")
		.CommandLineArgs()
		.AddArg2Hyphens<int>()
			.Name("some_int")
			.Required(true)
			.Help("Any integer: {}, {}, {} etc."s.format(1, 2, 3))
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
		app.Log(" - {:<20} = #{};True#{};", "Some Flag", Color::Aqua, Color::Def);
	}
}

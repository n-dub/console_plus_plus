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
#pragma once
#include <any>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>
#include <iomanip>

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <Windows.h>
#endif

#ifndef CONPP_ALLOW_EXCEPTIONS
#	ifdef NDEBUG
#		define CONPP_ALLOW_EXCEPTIONS 0
#	else
#		define CONPP_ALLOW_EXCEPTIONS 1
#	endif
#endif

namespace conpp
{
	enum class Color
	{
		Def = 0xff,
#ifdef _WIN32
		Black = 0x0,
		Navy = 0x1,
		Green = 0x2,
		Teal = 0x3,
		Maroon = 0x4,
		Purple = 0x5,
		Olive = 0x6,
		Silver = 0x7,
		Gray = 0x8,
		Blue = 0x9,
		Lime = 0xa,
		Aqua = 0xb,
		Red = 0xc,
		Fuchsia = 0xd,
		Yellow = 0xe,
		White = 0xf
#else
		Black = 0,
		Navy = 4,
		Green = 2,
		Teal = 6,
		Maroon = 1,
		Purple = 5,
		Olive = 3,
		Silver = 7,
		Gray = 8,
		Blue = 12,
		Lime = 10,
		Aqua = 14,
		Red = 9,
		Fuchsia = 13,
		Yellow = 11,
		White = 15
#endif
	};
}

namespace std
{
	inline ostream& operator<<(ostream& stream, ::conpp::Color col) {
		stream << int(col);
		return stream;
	}
}

namespace conpp
{
	namespace detail
	{
		inline constexpr bool Exceptions = bool(CONPP_ALLOW_EXCEPTIONS);

		inline bool IsDigit(char c, int base) {
			int result = 0;
			if (!isalnum(c)) {
				return false;
			}
			if (c >= '0' && c <= '9') {
				result = c - '0';
			}
			else {
				result = tolower(c) - 'a' + 10;
			}
			return result < base;
		}

		template<class T>
		inline T ParseInteger(std::string_view src, size_t& i, int base = 10) {
			static_assert(std::is_integral<T>::value);
			T result = 0;
			while (i < src.length() && IsDigit(src[i], base)) {
				auto c = (unsigned char)(src[i]);
				result *= base;
				if (c >= '0' && c <= '9') {
					result += c - '0';
				}
				else {
					result += T(tolower(c)) - T('a') + T(10);
				}
				++i;
			}
			return result;
		}

		template<class ArgType, class... ArgTypes>
		inline void FormatSStreamOneValueImpl(std::stringstream& ss, std::initializer_list<std::any>& list, size_t idx, size_t currIdx) {
			if (currIdx < idx) {
				if constexpr (sizeof...(ArgTypes) > 0) {
					FormatSStreamOneValueImpl<ArgTypes...>(ss, list, idx, currIdx + 1);
				}
			}
			else {
				ss << *std::any_cast<typename std::decay<ArgType>::type>(list.begin() + idx);
			}
		}

		template<class... ArgTypes>
		inline void FormatSStreamOneValue(std::stringstream& ss, std::initializer_list<std::any>& list, size_t idx) {
			if (idx >= list.size()) {
				throw std::runtime_error("index out of range");
			}
			FormatSStreamOneValueImpl<ArgTypes...>(ss, list, idx, 0);
		}

		template<class... Args>
		inline void FormatSStreamImpl(std::stringstream& ss, std::string_view fmt, std::initializer_list<std::any>& list) {
			size_t start = 0;
			int fieldSpec = 0; // 1-manual, 2-automatic
			size_t autoIdx = 0;
			for (size_t i = 0; i < fmt.length(); ++i) {
				if (fmt[i] == '{') {
					if (i + 1 < fmt.length()) {
						if (fmt[i + 1] == '{') {
							ss << fmt.substr(start, i - start) << '{';
							++i;
							start = i + 1;
						}
						else {
							int idx = 0;
							if (fmt[i + 1] == '}' || fmt[i + 1] == ':') {
								if (fieldSpec == 1 && detail::Exceptions) {
									throw std::runtime_error("cannot switch from manual field specification to automatic field numbering");
								}
								fieldSpec = 2;
								ss << fmt.substr(start, i - start);
								++i;
								idx = autoIdx++;
							}
							else {
								if (fieldSpec == 2 && detail::Exceptions) {
									throw std::runtime_error("cannot switch from automatic field numbering to manual field specification");
								}
								fieldSpec = 1;
								ss << fmt.substr(start, i - start);
								idx = ParseInteger<int>(fmt, ++i);
							}
							if (fmt[i] == ':' && i + 1 < fmt.length()) {
								++i;
								if (fmt[i] == '<') {
									int space = ParseInteger<int>(fmt, ++i);
									ss << std::left;
									ss << std::setw(space);
								}
								else if (fmt[i] == '>') {
									int space = ParseInteger<int>(fmt, ++i);
									ss << std::right;
									ss << std::setw(space);
								}

								if (fmt[i] == 'X' || fmt[i] == 'x') {
									++i;
									ss << std::setbase(16);
								}
								else if (fmt[i] == 'D' || fmt[i] == 'd') {
									++i;
									ss << std::setbase(10);
								}
								else if (fmt[i] == 'B' || fmt[i] == 'b') {
									++i;
									ss << std::setbase(2);
								}
								else if (fmt[i] == 'O' || fmt[i] == 'o') {
									++i;
									ss << std::setbase(8);
								}

								if (fmt[i] == '.') {
									ss << std::setprecision(ParseInteger<std::streamsize>(fmt, ++i) + 1);
									if (fmt[i] == 'f') {
										++i;
									}
								}

								if (fmt[i] != '}') {
									if constexpr (detail::Exceptions) {
										throw std::runtime_error("invalid format string");
									}
									else {
										while (i < fmt.length() && fmt[i] != '}') {
											++i;
										}
									}
								}
							}
							else {
								ss << std::setprecision(3);
								ss << std::setbase(10);
							}
							start = i + 1;
							FormatSStreamOneValue<Args...>(ss, list, idx);
						}
					}
					else {
						ss << '{';
						return;
					}
				}
			}

			ss << fmt.substr(start);
		}
	}

	/**
	 * @brief Python-like string formatting.
	 * 
	 * Automatically detects field numbering (automatic or manual).\
	 * Example of manual field numbering:\
	 * ```cpp
	 * FormatStr("{1} {0} {0}", 1, "qwerty"); // returns "qwerty 1 1"
	 * ```
	 * 
	 * Example of automatic field numbering:\
	 * ```cpp
	 * FormatStr("{} {} {}", 1, 2, "qwerty"); // returns "1 2 qwerty"
	 * ```
	 * 
	 * @tparam ...Args Argument types.
	 * @param fmt Format string.
	 * @param ...args Arguments.
	 * @return The formatted string.
	*/
	template<class... Args>
	inline std::string FormatStr(std::string_view fmt, Args&&... args) {
		if constexpr (sizeof...(Args) > 0) {
			std::initializer_list<std::any> list{ std::any(args)... };
			std::stringstream ss;
			detail::FormatSStreamImpl<Args...>(ss, fmt, list);
			return ss.str();
		}
		else {
			return std::string(fmt.data());
		}
	}

	class String : public std::string
	{
	public:
		String(const char* str) : std::string(str) {}
		String(const char* str, size_t len) : std::string(str, len) {}
		String(const std::string& str) : std::string(str) {}
		String(std::string&& str) : std::string(std::move(str)) {}

		operator std::string() {
			return std::string(*(std::string*)this);
		}

		template<class... Args>
		String format(Args&&... args) {
			return FormatStr(data(), args...);
		}
	};

	namespace literals {
		inline ::conpp::String operator""s(const char* str, ::std::size_t len) {
			return ::conpp::String(str, len);
		}
	}

	inline void SetConsoleColor(Color fg) {
#ifdef _WIN32
		if (fg == Color::Def) {
			fg = Color::White;
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WORD(fg));
#else
		if (fg == Color::Def) {
			std::cout << "\033[0m";
		}
		else {
			std::cout << FormatStr("\033[38;5;{}m", int(fg));
		}
#endif
	}

	/**
	 * @brief This is used for command line arguments.
	*/
	enum class NoType {};

	class ConsoleApp;
	class CommandLineConfig;

	/**
	 * @brief Base class for command line arguments.
	*/
	class CommandLineArgBase
	{
		friend class CommandLineConfig;
		friend class ConsoleApp;

	protected:
		bool m_req = false;
		bool m_exists = false;
		std::string m_name;
		std::string m_help;
		virtual void Parse(std::string_view src) {}
	};

	/**
	 * @brief A command line argument.
	 * @tparam T Type of argument's value. Use `NoType` if the value is not needed (e.g. for flags).
	*/
	template<class T>
	class CommandLineArg final : public CommandLineArgBase
	{
		T m_value{};
		CommandLineConfig& m_config;

		friend class CommandLineConfig;

		CommandLineArg(CommandLineConfig& cfg) : m_config(cfg) {

		}

	protected:
		virtual void Parse(std::string_view src) final override {
			if constexpr (std::is_same<T, std::string>()) {
				m_value = src.data();
			}
			else if constexpr (std::is_same<T, std::string_view>()) {
				m_value = src;
			}
			else {
				std::stringstream ss;
				ss << src;
				ss >> m_value;
			}
		}

	public:
		/**
		 * @brief Set name of argument.
		 * @param val The name.
		 * @return This.
		*/
		CommandLineArg<T>& Name(std::string_view val) {
			m_name = val;
			return *this;
		}

		/**
		 * @brief Set help message for the argument.
		 * @param val The message.
		 * @return This.
		*/
		CommandLineArg<T>& Help(std::string_view val) {
			m_help = val;
			return *this;
		}

		/**
		 * @brief Set if the argument is required.
		 * @param val True is required.
		 * @return This.
		*/
		CommandLineArg<T>& Required(bool val) {
			m_req = val;
			return *this;
		}

		/**
		 * @brief Finish the argument.
		 * @return Instance of `CommandLineConfig`.
		*/
		CommandLineConfig& Build() {
			if (detail::Exceptions && m_name.length() <= 0) {
				throw std::runtime_error("Name of the argument was not specified");
			}
			return m_config;
		}
	};

	template<>
	class CommandLineArg<NoType> final : public CommandLineArgBase
	{
		CommandLineConfig& m_config;

		friend class CommandLineConfig;

		CommandLineArg(CommandLineConfig& cfg) : m_config(cfg) {

		}

	public:
		CommandLineArg<NoType>& Name(std::string_view val) {
			m_name = val;
			return *this;
		}

		CommandLineArg<NoType>& Help(std::string_view val) {
			m_help = val;
			return *this;
		}

		CommandLineConfig& Build() {
			if (detail::Exceptions && m_name.length() <= 0) {
				throw std::runtime_error("Name of the argument was not specified");
			}
			return m_config;
		}
	};

	/**
	 * @brief Command line arguments configuration.
	*/
	class CommandLineConfig
	{
		std::vector<std::unique_ptr<CommandLineArgBase>> m_options;

		friend class ConsoleApp;

		ConsoleApp& m_app;

		struct
		{
			bool NoArguments = false;
		} m_parsingResults;

		void PrintHelp() {
			size_t maxLen = 0;
			for (auto& opt : m_options) {
				if (opt->m_name.length() > maxLen) {
					maxLen = opt->m_name.length();
				}
			}

			auto fmt1 = FormatStr("    --{{:<{}} (required) {{}\n", maxLen);
			auto fmt2 = FormatStr("    --{{:<{}} (optional) {{}\n", maxLen);
			for (auto& opt : m_options) {
				if (opt->m_req) {
					std::cout << FormatStr(fmt1,
						opt->m_name, opt->m_help);
				}
				else {
					std::cout << FormatStr(fmt2,
						opt->m_name, opt->m_help);
				}
			}
		}

		CommandLineConfig(ConsoleApp& app) : m_app(app) {

		}

	public:
		/**
		 * @brief Add an argument with two hyphens (e.g. --arg)
		 * @tparam T Type of argument's value, use NoType if it's not needed.
		 * @return 
		*/
		template<class T>
		CommandLineArg<T>& AddArg2Hyphens() {
			m_options.push_back(std::unique_ptr<CommandLineArg<T>>(new CommandLineArg<T>(*this)));
			return static_cast<CommandLineArg<T>&>(*m_options.back());
		}

		template<class T>
		T* GetArg(std::string_view name) {
			for (auto& opt : m_options) {
				if (opt->m_name == name && opt->m_exists) {
					CommandLineArg<T>* optT = (CommandLineArg<T>*)(opt.get());
					return &optT->m_value;
				}
			}

			return (T*)nullptr;
		}

		bool HasArg(std::string_view name) {
			for (auto& opt : m_options) {
				if (opt->m_name == name) {
					return opt->m_exists;
				}
			}
			return false;
		}

		CommandLineConfig& Parse(int argc, char** argv) {
			if (argc == 1) {
				m_parsingResults.NoArguments = true;
				return *this;
			}
			std::vector<std::string_view> args;

			for (int i = 1; i < argc; ++i) {
				args.push_back(argv[i]);
			}

			for (size_t i = 0; i < args.size(); ++i) {
				std::string_view arg = args[i];
				if (arg.length() > 2 && arg[0] == '-' && arg[1] == '-') {
					arg = arg.substr(2);

					for (auto& opt : m_options) {
						if (opt->m_name == arg) {
							opt->m_exists = true;
							if (i + 1 < args.size() && args[i + 1][0] != '-') {
								opt->Parse(args[i + 1]);
								++i;
							}
						}
					}
				}
			}

			return *this;
		}
	};

	/**
	 * @brief 
	*/
	class ConsoleApp
	{
		std::string m_name, m_version, m_desc, m_usage;
		CommandLineConfig m_args;

	public:
		explicit ConsoleApp(std::string_view name) : m_args(*this) {
			m_name = name;
		}

		~ConsoleApp() {
			SetConsoleColor(Color::Def);
		}

		ConsoleApp& Version(std::string_view ver) {
			m_version = ver;
			return *this;
		}

		ConsoleApp& Description(std::string_view desc) {
			m_desc = desc;
			return *this;
		}

		ConsoleApp& UsageDesc(std::string_view usage) {
			m_usage = usage;
			return *this;
		}

		void PrintHelp(bool printHeader = true) {
			if (printHeader) {
				Log("{} {}", m_name, m_version);
				if (m_desc.length()) {
					Log(m_desc);
				}
			}
			Log("Usage: {}", m_usage);

			Log("Arguments:");
			m_args.PrintHelp();
		}

		bool NoArguments() {
			return m_args.m_parsingResults.NoArguments;
		}

		bool PrintErrors() {
			bool r = false;
			for (auto& opt : m_args.m_options) {
				if (!opt->m_exists && opt->m_req) {
					LogErr("argument \"{}\" is required", opt->m_name);
					r = true;
				}
			}

			return r;
		}

		template<class... Args>
		void LogErr(std::string_view str, Args&&... args) {
			Log("#{};Error:#{}; {}", Color::Red, Color::Def, FormatStr(str, args...));
		}

		template<class... Args>
		void Log(std::string_view str, Args&&... args) {
			auto s = FormatStr(str, args...);
			str = s;
			size_t start = 0;
			for (size_t i = 0; i < str.length(); ++i) {
				if (i + 2 < str.length() && str[i] == '#') {
					std::cout << str.substr(start, i - start);
					++i;
					SetConsoleColor((Color)detail::ParseInteger<int>(str, i));
					start = i + 1;
				}
			}
			std::cout << str.substr(start) << std::endl;
			SetConsoleColor(Color::Def);
		}

		CommandLineConfig& CommandLineArgs() {
			return m_args;
		}
	};
}

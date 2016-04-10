/*
	Copyright (C) 2016 Federico Kircheis

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INIPARSER_HPP
#define INIPARSER_HPP

#include <map>
#include <string>
#include <vector>


//FIXME: copied from old project, should polish it, remove multiline comments and so on

namespace iniparser {
	class IniParser {
	public:
		typedef std::string section;
		typedef std::string property;
		typedef std::string value;
		std::map<section, std::map<property, value>> content;
	public:
		explicit IniParser(const std::string &iniFile);

		~IniParser();

		//
		bool HasSection(const section &section);

		bool HasProperty(const section &section, const property &property);

		std::string GetValue(const section &section, const property &property);

		std::string GetValue(const section &section, const property &property, const std::string &defaultValue);

		typedef std::map<section, std::map<property, value>>::const_iterator const_iterator;
		typedef std::map<property, value>::const_iterator const_section_iterator;

		const_iterator cbegin() const { return content.begin(); }
		const_iterator cend() const { return content.end(); }

		const_section_iterator cbegin(std::string& section) const { return content.at(section).begin(); }
		const_section_iterator cend(std::string& section) const { return content.at(section).end(); }

		void safetofile(const std::string& filename);
	private:
		section currentsection;
		bool loadcomments = false;
		bool enableescapecharacter = true;
		bool enablequotedvalues = true;
		bool casesensitive = true;
		std::vector<std::string> commentidentifiers = {"#", ";"};

		bool enablemultilineComment = false;


		void process(std::string &line);

		/// tells with inmultiline if a multilinecomment has begun but not ended, use as input to tell if multiline has already began
		void strip_comments(std::string &line, bool &inmultiline);

		void strip_spaces(std::string &line);

		std::pair<property, value> getPropVal(std::string &line, std::string::size_type sep);
	};
}
#endif // INIPARSER_HPP

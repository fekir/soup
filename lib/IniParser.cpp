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

#include "IniParser.hpp"

#include <cctype>
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <stdexcept>


namespace iniparser {
	IniParser::IniParser(const std::string &iniFile) {
		//http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
		std::ifstream f(iniFile);
		if (!f.is_open()) {
			throw std::runtime_error("error while opening file");
		}
		std::string line;
		while (getline(f, line)) {
			process(line);
		}
		if (f.bad()) {
			throw std::runtime_error("error while reading file");
		}
	}

	IniParser::~IniParser() = default;


	void IniParser::strip_comments(std::string &line, bool &inmultiline) {
		if (inmultiline) {// sono in un multiline
			auto pos_multilineend = line.find("*/");
			if (pos_multilineend == std::string::npos) { // ma non ho trovato la fine
				line.clear();
				inmultiline = true; // sono ancora in un multiline
				return;
			}
		}

		const auto pos_multilinebegin = line.find("/*");
		const auto pos_multilineend = line.find("*/");
		const auto pos_singleline = std::min(line.find(";"),std::min(line.find("#"),line.find("//")));

		if (pos_singleline <= pos_multilinebegin) { // se è vero, allora pos_singleline è stato trovato
			line = line.substr(0, pos_singleline);
			inmultiline = false;
			return;
		} else { // è stato trovato multiline!!
			if (pos_multilineend == std::string::npos) { // ma non ho trovato la fine
				line = "";
				inmultiline = true;
				return;
			} else { // ho trovato anche la fine del commento
				const auto tmpline1 = line.substr(0, pos_multilinebegin);
				const auto tmpline2 = line.substr(pos_multilineend, line.length() - pos_multilineend);
				line = tmpline1 + tmpline2;
				inmultiline = false;
				return;
			}
		}
	}

	void IniParser::strip_spaces(std::string &line) {
		const auto escbeg = line.find("\"");
		const auto escend = line.rfind("\"");
		if (escbeg == std::string::npos) { // non ho trovato ", rimuovi tutti gli spazi
			line.erase(
						std::remove_if(line.begin(),line.end(),[](char c) { return std::isspace(c); }),
						line.end()
						);
		} else {
			assert(escbeg != std::string::npos);
			assert(escend != std::string::npos);
		}
	}

	std::pair<std::string, std::string> IniParser::getPropVal(std::string &line, std::string::size_type pos) {
		// --> mettere in funzione getPropValue
		const property property = line.substr(0, pos);
		value value = line.substr(pos + 1, line.length() - (pos + 1));
		const auto escbeg = value.find("\"");
		const auto escend = value.rfind("\"");
		if (escbeg != std::string::npos && escbeg != escend) {
			assert(escend != std::string::npos); // altrimenti lo sarebbe anche escbeg...
			value = value.substr(escbeg + 1, escend - (escbeg + 1));
		}
		return {property, value};
	}

	void IniParser::process(std::string &line) {
		strip_comments(line, enablemultilineComment);

		strip_spaces(line);

		// if we are in a enablemultilineComment, check if we find the end
		if (enablemultilineComment) {
			const auto pos = line.find("*/");
			if (pos != std::string::npos) {
				line = line.substr(pos, line.length());
			}
		}

		if (line.empty()) {
			return; // skip line --> da miglioreare
		}


		if ((line[0] == '[') && (line.back() == ']')) {
			currentsection = line.substr(1, line.length() - 2);
			if (content.count(currentsection) == 0) { // section is not in the map
				content.insert({ currentsection, {}});
			} else { /// ??? we should not be here in an conformant file... should I just ignore it???
				std::clog << "Same section appears multiple times!!! Some value may be overwritten!" << std::endl;
			}
		} else { // read property and value
			const auto pos = line.find("=");
			if (pos != std::string::npos) {
				const auto prop_val = getPropVal(line, pos);
				assert((!currentsection.empty() && content.count(currentsection) > 0) ||
					   currentsection.empty()); // check, l'unico che dovrei default-agg è quello vuoto
				content[currentsection].insert(prop_val);// default-create if not present -> vedi assert
			}
		}
	}

	std::string IniParser::GetValue(const section &section, const property &property) {
		if (!HasSection(section)) {
			throw std::runtime_error("Section \"" +section+"\" not found");
		}
		if (!HasProperty(section, property)) {
			throw std::runtime_error("Property \"" +property+"\"not found");
		}
		return content.at(section).at(property);
	}

	bool IniParser::HasSection(const section &section) {
		return (content.count(section) > 0);
	}

	bool IniParser::HasProperty(const section &section, const property &property) {
		if (!HasSection(section)) {
			throw std::runtime_error("Section not found");
		}
		return content.at(section).count(property) > 0;
	}

}

// midikeyMapping.cpp
// Spontz Demogroup

// includes ////////////////////////////////////////////////////////////////////////////////////////

#include "midiKeyMapping.h"

#include <fstream>
#include <filesystem>
#include <io.h>

namespace Phoenix {

	

	MidiKeyMapping::MidiKeyMapping()
		:
		midiKeys({
			{"play",		0, 0.0	},
			{"pause",		0, 0.0	},
			{"fastforward",	0, 0.0	},
			{"rewind",		0, 0.0	},
			{"knob1",		0, 0.0	},
			{"knob2",		0, 0.0	},
			{"knob3",		0, 0.0	},
			{"knob4",		0, 0.0	},
			{"knob5",		0, 0.0	},
			{"knob6",		0, 0.0	},
			{"knob7",		0, 0.0	},
			{"knob8",		0, 0.0	},
			{"slider1",		0, 0.0	},
			{"slider2",		0, 0.0	},
			{"slider3",		0, 0.0	},
			{"slider4",		0, 0.0	},
			{"slider5",		0, 0.0	},
			{"slider6",		0, 0.0	},
			{"slider7",		0, 0.0	},
			{"slider8",		0, 0.0	},
			{"button1",		0, 0.0	},
			{"button2",		0, 0.0	},
			{"button3",		0, 0.0	}
		})
	{
	}

	MidiKeyMapping::~MidiKeyMapping()
	{
	}

	// To be replaced by UTILS in the engine
	std::string MidiKeyMapping::readASCIIFile(std::string_view fileName)
	{
		std::string result;
		std::ifstream file(fileName.data(), std::ios::in | std::ios::binary);
		if (file.good()) {
			file.seekg(0, std::ios::end);
			size_t size = file.tellg();
			if (size != -1) {
				result.resize(size);
				file.seekg(0, std::ios::beg);
				file.read(&result[0], size);
				file.close();
			}
			else {
				std::cout << "Could not read file: " << fileName;
			}
		}
		else {
			std::cout << "Could not open file: " << fileName;
		}
		return result;
	}

	std::vector<std::string> MidiKeyMapping::splitInMultipleLines(const std::string& line) {
		std::vector<std::string> strings;
		std::istringstream f(line);
		std::string s;
		// Split the string by spaces
		while (std::getline(f, s, ' ')) {
			strings.emplace_back(s);
		}
		return strings;
	}

	std::pair<std::string, std::string> MidiKeyMapping::splitIn2Lines(const std::string& line) {
		std::pair<std::string, std::string> ret("", "");

		std::vector<std::string> strings = splitInMultipleLines(line);

		// recompose the string: in the first we have the commmand, in the second we have all the string
		if (strings.size() >= 1) {
			ret.first = strings[0];
			for (int i = 1; i < strings.size(); i++) {
				ret.second += strings[i];
				if ((i + 1) < strings.size())
					ret.second += " ";
			}
		}
		return ret;
	}

	MidiKeyMapping::MidiKey* MidiKeyMapping::findMidiKeyByName(std::string_view keyName)
	{
		for (auto& midiKey : midiKeys) {
			if (keyName == midiKey.m_keyName)
				return &midiKey;
		}
		return nullptr;
	}

	MidiKeyMapping::MidiKey* MidiKeyMapping::findMidiKeyByNumber(const uint32_t keyNumber)
	{
		for (auto& midiKey : midiKeys) {
			if (keyNumber == midiKey.m_keyNumber)
				return &midiKey;
		}
		return nullptr;
	}

	bool MidiKeyMapping::loadMidiKeyMapping(std::string_view filepath)
	{

		std::string m_scriptData = readASCIIFile(filepath);

		std::istringstream f(m_scriptData);
		std::string line;
		int lineNum = 0; // Line counter
		bool loadingOK = true;

		while (std::getline(f, line)) {
			lineNum++;
			// Remove '\r' (if exists)
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);

			// Ignore comments or empty line
			if (line.empty() || (line[0] == '\n') || (line[0] == '\r')) {
				continue;
			}
			if ((line[0] == ';') || (line[0] == ' ') || (line[0] == '\t')) {
				continue;
			}

			std::pair<std::string, std::string> s_line = splitIn2Lines(line);
			// check if its a known key
			MidiKey* midiKey = findMidiKeyByName(s_line.first);
			if (!midiKey) {
				std::cout << "Could not find key: " << s_line.first << std::endl;
				loadingOK = false;
			}
			else {
				midiKey->m_keyNumber = atoi(s_line.second.c_str());
			}
		}
		return loadingOK;
	}

}
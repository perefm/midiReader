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

	MidiKeyMapping::MidiKey* MidiKeyMapping::findCurrentKey()
	{
		if (m_recordMappedKey < midiKeys.size())
			return &midiKeys[m_recordMappedKey];
		else
			return nullptr;
	}

	void MidiKeyMapping::recordKeyMapping()
	{
		char selection = -1;
		uint32_t key = 0;
		std::cout << "Record mapping started...\n"; 
		std::cout << "Press ENTER to move to next key.\n";
		std::cout << "Press Q to exit.\n";

		// Ignorar cualquier carácter sobrante en el buffer (como un ENTER anterior)
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		do {
			m_recordMappedKey = key;
			auto* midiKey = findCurrentKey();
			if (midiKey)
				std::cout << "Capturing key: " << midiKey->m_keyName << std::endl;
			else
				std::cout << "Error! Key not valid!" << std::endl;

			selection = std::cin.get();
			if (selection == '\n') {
				key++;
				if (key >= midiKeys.size())
					key = 0;
			}
		} while (selection != 'q' && selection != 'Q');
		std::cout << "Mapping finished!\n";
	}

	bool MidiKeyMapping::mapCurrentKey(uint32_t keyNumber)
	{
		if (m_recordMappedKey < midiKeys.size()) {
			midiKeys[m_recordMappedKey].m_keyNumber = keyNumber;
			return true;
		}
		return false;
	}

	bool MidiKeyMapping::mapKey(std::string_view keyName, uint32_t keyNumber)
	{
		auto* key = findMidiKeyByName(keyName);
		if (key) {
			key->m_keyNumber = keyNumber;
			return true;
		}
		return false;
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
			if (!mapKey(s_line.first, atoi(s_line.second.c_str())) ) {
				std::cout << "Could not find key: " << s_line.first << std::endl;
				loadingOK = false;
			}

		}
		return loadingOK;
	}

	bool MidiKeyMapping::saveMidiKeyMapping(std::string_view filepath) const
	{
		std::ofstream outFile(filepath.data());
		if (!outFile) {
			return false;
		}

		outFile << "; Auto-saved file with midi mapping\n";
		for (const auto& key : midiKeys) {
			outFile << key.m_keyName << " " << key.m_keyNumber << "\n";
		}

		outFile.close();
		return true;
	}

	void MidiKeyMapping::displayMidiKeyMapping() const
	{
		std::cout << "MIDI Key Mapping:" << std::endl;
		for (const auto& key : midiKeys) {
			std::cout << "Key: " << key.m_keyName <<
				", Number: " << key.m_keyNumber <<
				", Value: " << key.m_keyValue << std::endl;
		}
		std::cout << "Total keys: " << midiKeys.size() << std::endl;
	}

}
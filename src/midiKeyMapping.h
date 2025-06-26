// midiKeyMapping.h
// Spontz Demogroup

#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "rtmidi/rtmidi.h"
#include <wolf-midi/MidiFile.h>

namespace Phoenix {

	class MidiKeyMapping final {

		struct MidiKey final {
			std::string m_keyName;
			uint32_t m_keyNumber;
			float m_keyValue;
		};

	public:
		MidiKeyMapping();
		~MidiKeyMapping();

	public:

		bool loadMidiKeyMapping(std::string_view filepath);
		bool saveMidiKeyMapping(std::string_view filepath) const;
		void displayMidiKeyMapping() const;
		
		MidiKey* findMidiKeyByName(std::string_view keyName);
		MidiKey* findMidiKeyByNumber(const uint32_t keyNumber);
		MidiKey* findCurrentKey();
		
		void recordKeyMapping();
		bool mapCurrentKey(uint32_t keyNumber);
		bool mapKey(std::string_view keyName, uint32_t keyNumber);

	private:
		std::string readASCIIFile(std::string_view fileName); // To be replaced by UTILS in the engine
		std::pair<std::string, std::string> splitIn2Lines(const std::string& line);
		std::vector<std::string> splitInMultipleLines(const std::string& line);

		uint32_t	m_recordMappedKey = 0;
		
	public:
		std::vector<MidiKey> midiKeys;
	};
}

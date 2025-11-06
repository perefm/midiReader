// midi.h
// Spontz Demogroup

#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "rtmidi/rtmidi.h"
#include <wolf-midi/MidiFile.h>

// Key Mapping
#include "midiEvents.h"
// Key Mapping
#include "midiKeyMapping.h"

namespace Phoenix {

	class MidiDriver final {

	public:
		MidiDriver();
		~MidiDriver();

	public:

		void openAllPorts();
		void closeAllPorts();
		void clearMemory();
		void recordEventsStart();
		void recordEventsStop();
		void recordMappingStart();
		void recordMappingStop();
		void displayEvents();
		void triggerEvents();
		void storeSong(const std::string& filePath);
		void loadSong(const std::string& filePath);
		void clearDriver();
		void outputMessage();
		bool loadKeyMapping(const std::string& filePath);
		void updateKeyValue(unsigned char key, unsigned char value);
		uint32_t getNumDevices() { return static_cast<uint32_t>(m_deviceIn.size()); }
		std::string getDeviceName(uint32_t device);

		std::string getVersion();
		uint32_t getEventsSize() { return static_cast<uint32_t>(m_events.events.size()); };
		bool isRecording() { return m_isRecording; };

	private:
		// Midi Callbacks
		static void captureEvent(double deltatime, std::vector<unsigned char>* message, void* userData);
		static void captureKeyMapping(double deltatime, std::vector<unsigned char>* message, void* userData);
		
	private:
		std::vector<RtMidiIn*> m_deviceIn;
		bool m_isRecording = false;		// Used to capture midi events
		bool m_isTriggering = false;	// Used to trigger events
		bool m_isKeyMapping = false;	// Used to re-map midi keys
		
		const uint32_t BPM = 120;
		const uint32_t PPQN = 480;			// Pulses per Quarter Note
		const double TICKS_PER_SECOND = static_cast<double>(BPM * PPQN) / 60.0;

		using Clock = std::chrono::high_resolution_clock;
		std::chrono::high_resolution_clock::time_point m_startRecordingTime;	// Start recording time

		MidiEvents m_events;

		//std::vector<EventMessage*> events;

	public:
		std::string	m_version;
		uint32_t m_numPorts = 0;
		MidiKeyMapping keys;


	};
}

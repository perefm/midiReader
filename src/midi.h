// midi.h
// Spontz Demogroup

#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "rtmidi/rtmidi.h"
#include <wolf-midi/MidiFile.h>

// Key Mapping
#include "midiKeyMapping.h"

namespace Phoenix {

	class MidiDriver final {

		struct EventMessage final {
			unsigned char	type;		// 0xB0 + channel
			unsigned char	key;		// control number (key or note)
			unsigned char	value;		// value (for control) or velocty (for note)
			uint32_t		tick;		// absolute tick in the song
			double			absTime;	// Absolute time when the event was produced
		
			// Comparador para ordenar por tick (menor a mayor)
			static bool compareByTick(const EventMessage* a, const EventMessage* b) {
				return a->tick < b->tick;
			}
		};

	public:
		MidiDriver();
		~MidiDriver();

	public:

		void openMidiPort(uint32_t port);
		void clearMemory();
		void recordEventsStart();
		void recordEventsStop();
		void recordMappingStart();
		void recordMappingStop();
		void displayEvents() const;
		void storeSong(const std::string& filePath);
		void loadSong(const std::string& filePath);
		void clearDriver();
		void outputMessage();
		bool loadKeyMapping(const std::string& filePath);
		void updateKeyValue(unsigned char key, unsigned char value);


		std::string getVersion();
		uint32_t getEventsSize() { return static_cast<uint32_t>(events.size()); };
		bool isRecording() { return m_isRecording; };

	private:
		// Midi Callbacks
		static void captureEvent(double deltatime, std::vector<unsigned char>* message, void* userData);
		static void captureKeyMapping(double deltatime, std::vector<unsigned char>* message, void* userData);
		
	private:
		RtMidiIn* midiin = nullptr;
		bool m_isRecording = false;		// Used to capture midi events
		bool m_isKeyMapping = false;	// Used to re-map midi keys
		
		const double BPM = 120.0;
		const int PPQN = 480;			// Pulses per Quarter Note
		const double TICKS_PER_SECOND = (BPM * PPQN) / 60.0;

		using Clock = std::chrono::high_resolution_clock;
		std::chrono::high_resolution_clock::time_point m_startRecordingTime;	// Start recording time

		std::vector<EventMessage*> events;

	public:
		uint32_t nPorts = 0;
		MidiKeyMapping keys;


	};
}

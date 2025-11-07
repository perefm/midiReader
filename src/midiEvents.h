// midiEventPlayer.h
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

	class MidiEvents final {

	public:
		struct EventMessage final {
			unsigned char	type;		// 0xB0 + channel
			unsigned char	key;		// control number (key or note)
			unsigned char	value;		// value (for control) or velocty (for note)
			uint32_t		tick;		// absolute tick in the song
			double			absTime;	// Absolute time when the event was produced
			bool			triggered;	// The event Has been triggered?
		
			// Comparador para ordenar por tick (menor a mayor)
			static bool compareByTick(const EventMessage* a, const EventMessage* b) {
				return a->tick < b->tick;
			}
		};

	public:
		MidiEvents();
		~MidiEvents();

		void resetEventsTrigger();
		void clearEvents();
		void sortEvents();
		void displayEvents() const;
		uint32_t numEvents() { return static_cast<uint32_t>(events.size()); };
		void addEvent(unsigned char type, unsigned char key, unsigned char value, uint32_t tick, double absTime);

		std::vector<EventMessage*> events;

	public:

		bool triggerEvents();
		bool triggerEvents(double currentTime);

	private:
		using Clock = std::chrono::high_resolution_clock;
		size_t currentIndex = 0;  // Index of the next event to trigger, if we need to start payback agaiun, we need to reset this


	};
}

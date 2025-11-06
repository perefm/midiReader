// midiEventPlayer.cpp
// Spontz Demogroup

// includes ////////////////////////////////////////////////////////////////////////////////////////

#include "midiEvents.h"

#include <thread>   // Para sleep_for
#include <chrono>   // Para chrono::seconds
#include <algorithm>	// sts::sort

namespace Phoenix {

	MidiEvents::MidiEvents()
	{
	}

	MidiEvents::~MidiEvents()
	{
	}

	void MidiEvents::clearEvents()
	{
		events.clear();
	}

	void MidiEvents::sortEvents()
	{
		std::sort(events.begin(), events.end(), EventMessage::compareByTick);
	}

	void MidiEvents::displayEvents() const
	{
		std::cout << "MIDI Events:" << std::endl;
		for (const auto& event : events) {
			std::cout << "Event type: " << static_cast<uint32_t>(event->type) <<
				", time: " << std::format("{:.5f}", event->absTime) <<
				", tick: " << static_cast<uint32_t>(event->tick) <<
				", key: " << static_cast<uint32_t>(event->key) <<
				", data: " << static_cast<uint32_t>(event->value) << std::endl;

		}
		std::cout << "Total events: " << events.size() << std::endl;
	}

	void MidiEvents::addEvent(unsigned char type, unsigned char key, unsigned char value, uint32_t tick, double absTime)
	{
		EventMessage* eventMsg = new EventMessage(type, key, value, tick, absTime, false);
		events.push_back(eventMsg);
	}

	bool MidiEvents::triggerEvents()
	{
		auto ResetStart = Clock::now();
		std::cout << "Resetting all events in queue..." << std::endl;
		for (const auto& event : events) {
			event->triggered = false;
		}
		double ResetDuration = std::chrono::duration<double>(Clock::now() - ResetStart).count();

		std::cout << "All events reset in (s):" << ResetDuration << std::endl;
		std::cout << "Pres a key to trigger all events." << std::endl;

		std::system("pause");

		// Trigger events - to fix... quick and dirty
		auto TimeStart = Clock::now();
		bool exitLoop = false;
		while (!exitLoop) {
			auto CurrentTime = std::chrono::duration<double>(Clock::now() - TimeStart).count();

			for (const auto& event : events) {
				if (event->absTime > CurrentTime)
					break;
				if (!event->triggered && event->absTime <= CurrentTime) {
					//std::cout << "At time: " << CurrentTime << " - Event triggered!" << event->key << " event time is: " << event->absTime << std::endl;
					//if ((event->absTime - CurrentTime) > 0.00001)
					//	std::cout << "Too late! Current time:" << CurrentTime << " Event time is: " << event->absTime << std::endl;
					event->triggered = true;
				}
			}
			if (CurrentTime > 10)
				exitLoop = true;

		}
		return false;
	}

	

}
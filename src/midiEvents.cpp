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

	void MidiEvents::resetEventsTrigger()
	{
		for (auto& e : events)
			e->triggered = false;
		currentIndex = 0;
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
		for (auto& event : events) {
			event->triggered = false;
		}
		double ResetDuration = std::chrono::duration<double>(Clock::now() - ResetStart).count();

		std::cout << "All events reset in (s):" << ResetDuration << std::endl;
		std::cout << "Pres a key to trigger all events." << std::endl;

		std::system("pause");

		// Trigger events - to fix... quick and dirty
		auto TimeStart = Clock::now();
		
		// Keep track of where we are in the vector
		size_t currentIndex = 0;

		const size_t total = events.size();

		while (currentIndex < total) {
			double CurrentTime = std::chrono::duration<double>(Clock::now() - TimeStart).count();

			// Get the next event to trigger
			EventMessage* ev = events[currentIndex];

			// Wait until it's time to trigger this event
			if (CurrentTime >= ev->absTime && !ev->triggered) {
				std::cout << "At time: " << CurrentTime
					<< " - Event triggered! Key=" << (int)ev->key
					<< " event time=" << ev->absTime << std::endl;

				ev->triggered = true;
				currentIndex++;
			}
			else {
				// Sleep a bit to avoid busy waiting
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			// Example condition to stop after all events or 10 seconds
			if (CurrentTime > 10)
				break;
		}
		return true;
	}

	bool MidiEvents::triggerEvents(double currentTime)
	{
		// Assume events are already sorted by absTime
		while (currentIndex < events.size()) {
			EventMessage* ev = events[currentIndex];

			// If the next event is in the future, stop here (nothing else to trigger yet)
			if (ev->absTime > currentTime)
				break;

			// Trigger all events that should have happened by now
			if (!ev->triggered) {
				std::cout << "Trigger event (key=" << (int)ev->key
					<< ", time=" << ev->absTime
					<< ", current=" << currentTime << ")\n";
				ev->triggered = true;

				// Here you can actually send it to a MIDI output or event handler
				// midiOut->sendMessage(...);
			}

			// Move to next event
			++currentIndex;
		}

		// Return true when all events have been triggered
		return currentIndex >= events.size();
	}

	

}
// midi.cpp
// Spontz Demogroup

// includes ////////////////////////////////////////////////////////////////////////////////////////

#include "midi.h"

#include <thread>   // Para sleep_for
#include <chrono>   // Para chrono::seconds
#include <algorithm>	// sts::sort

namespace Phoenix {

	MidiDriver::MidiDriver()
	{
		// Create a device just to get the ports and few info
		RtMidiIn* midiin = new RtMidiIn();
		// Check available ports.
		m_numPorts = midiin->getPortCount();
		m_version = midiin->getVersion();
		delete midiin;
	}

	MidiDriver::~MidiDriver()
	{
		clearDriver();
	}

	void MidiDriver::openAllPorts()
	{
		// Create a device just to get the ports
		RtMidiIn* midiin = new RtMidiIn();
		// Check available ports.
		m_numPorts = midiin->getPortCount();
		delete midiin;

		// First close all ports
		closeAllPorts();

		// Open and configure all available IN ports
		for (uint32_t i = 0; i < m_numPorts; ++i) {
			RtMidiIn* deviceIn = new RtMidiIn();
			deviceIn->openPort(i);
			// Set the callback function for incoming MIDI messages.
			deviceIn->setCallback(&captureEvent, this);
			// Don't ignore sysex, timing, or active sensing messages.
			deviceIn->ignoreTypes(false, false, false);
			m_deviceIn.push_back(deviceIn);
		}
	}

	void MidiDriver::closeAllPorts()
	{
		// Stop all events recording or mapping
		recordEventsStop();
		recordMappingStop();
		// Close all ports
		for (auto* deviceIn : m_deviceIn) {
			deviceIn->cancelCallback();
			deviceIn->closePort();
		}
		m_deviceIn.clear();
	}

	void MidiDriver::clearMemory()
	{
		m_events.clearEvents();
	}

	void MidiDriver::recordEventsStart()
	{
		m_isRecording = true;
		m_isKeyMapping = false;
		for (auto* deviceIn : m_deviceIn) {
			deviceIn->cancelCallback();
			deviceIn->setCallback(&captureEvent, this); // Make sure we have the correct callback (captureEvent)
		}
		m_startRecordingTime = Clock::now();
	}

	void MidiDriver::recordEventsStop()
	{
		if (m_isRecording) {
			m_isRecording = false;
			std::cout << "Sorting events by tick..." << std::endl;
			m_events.sortEvents();
			std::cout << "Events sorted!" << std::endl;
		}		
	}

	void MidiDriver::recordMappingStart()
	{
		if (m_isRecording) {
			std::cout << "Event recording was enabled, so stopping it first..." << std::endl;
			recordEventsStop();
		}
		m_isKeyMapping = true;
		for (auto* deviceIn : m_deviceIn) {
			deviceIn->cancelCallback();
			deviceIn->setCallback(&captureKeyMapping, this); // Make sure we have the correct callback (keyMapping)
		}
		keys.recordKeyMapping();
		recordMappingStop();
	}

	void MidiDriver::recordMappingStop()
	{
		m_isKeyMapping = false;
	}

	void MidiDriver::displayEvents()
	{
		m_events.displayEvents();
	}

	void MidiDriver::triggerEvents()
	{
		//m_isTriggering = true;
		//m_events.triggerEvents();
		//m_isTriggering = false;

		m_isTriggering = true;

		m_events.resetEventsTrigger();

		auto start = std::chrono::high_resolution_clock::now();

		while (true) {
			double elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count();

			bool done = m_events.triggerEvents(elapsed);
			if (done)
				break;

			// Do other stuff (rendering, audio mixing, etc.)
		}

		m_isTriggering = false;
	}

	void MidiDriver::storeSong(const std::string& filePath)
	{
		Midi::MidiFile midi;
		midi.setResolution(PPQN);
		
		int track = midi.createTrack();  // Crear una pista nueva

		// TEST: Add note on on tick 0
		//midi.createNoteOnEvent(track, 0, /*canal=*/0, /*nota=*/60, /*velocidad=*/100);
		// TEST: Add note off on tick 480 (0.5s)
		//midi.createNoteOffEvent(track, 480, /*canal=*/0, /*nota=*/60, /*velocidad=*/0);

		// Before storing to midi, let's assure that all events are sorted by tick
		std::cout << "Sorting events by tick..." << std::endl;
		m_events.sortEvents();
		std::cout << "Events sorted!" << std::endl;


		for (const auto& event : m_events.events) {
			switch (event->type) {
			
			case 0xB0:
				midi.createControlChangeEvent(track, event->tick, 0, event->key, event->value);
				break;
			
			case 0x90:
				midi.createNoteOnEvent(track, event->tick, 0, event->key, event->value);
				break;
			case 0x80:
				midi.createNoteOffEvent(track, event->tick, 0, event->key, event->value);
				break;
			default:
				std::cout << "Unsupported event: " << static_cast<uint32_t>(event->type) << std::endl;
			}
		}

		if (!midi.save(filePath)) {
			std::cout << "Midi save failed." << std::endl;
			return;
		}

		std::cout << "Midi save succeed!" << std::endl;
	}

	void MidiDriver::loadSong(const std::string& filePath)
	{
		// load the song
		Midi::MidiFile midi;
		if (!midi.load(filePath)) {
			std::cout << "Midi load failed." << std::endl;
			return;
		}

		// If song has been properly loaded, clear all events
		clearMemory();

		double previousTime = 0;
		// load all events in memory
		for (const auto& event : midi.events()) {
			double eventTime = static_cast<float>(event->tick())/ TICKS_PER_SECOND; //midi.timeFromTick(event->tick());
			double deltaTime = eventTime - previousTime; // this does not work well unless the events are sorted in time.
			previousTime = eventTime;

			switch (event->type()) {
			case Midi::MidiEvent::EventType::ControlChange:
			{
				m_events.addEvent(0xB0, static_cast<unsigned char>(event->number()), static_cast<unsigned char>(event->value()), static_cast<uint32_t>(event->tick()), eventTime);
				break;
			}
			case Midi::MidiEvent::EventType::NoteOn:
			{
				m_events.addEvent(0x90, static_cast<unsigned char>(event->note()), static_cast<unsigned char>(event->velocity()), static_cast<uint32_t>(event->tick()), eventTime);
				break;
			}
			case Midi::MidiEvent::EventType::NoteOff:
			{
				m_events.addEvent(0x80, static_cast<unsigned char>(event->note()), static_cast<unsigned char>(event->velocity()), static_cast<uint32_t>(event->tick()), eventTime);
				break;
			}

			default:
				std::cout << "Unsupported event: " << static_cast<uint32_t>(event->type()) << std::endl;
				break;
			}

		}
		std::cout << "\nMidi file loaded.";
		std::cout << "\nEvents loaded: " << getEventsSize() << std::endl;
	}

	void MidiDriver::clearDriver()
	{
		recordEventsStop();
		closeAllPorts();
	}

	void MidiDriver::outputMessage()
	{
		RtMidiOut* midiout = new RtMidiOut();
		std::vector<unsigned char> message;

		// Check available ports.
		unsigned int m_numPorts = midiout->getPortCount();
		if (m_numPorts == 0) {
			std::cout << "No ports available!\n";
			delete midiout;
			return;
		}

		// Open first available port.
		midiout->openPort(m_numPorts-1);

		// Send out a series of MIDI messages.
		std::vector<unsigned char> keys;

		// Change the color of the 40 pads of the AKAI MK2 APC KEY25
		for (uint32_t i = 0; i < 40; i++) {
			keys.push_back(i);
		}

		message.push_back(0x00);
		message.push_back(0);
		message.push_back(5);

		// Blank all keys
		for (const auto& key : keys) {
			message[0] = 0x90;
			message[1] = key;
			message[2] = 0x00;
			midiout->sendMessage(&message);
		}

		// Turn on the keys in sequence
		for (const auto& key : keys) {
			message[0] = 0x90;
			message[1] = key;
			message[2] = rand()%256;// Random color
			midiout->sendMessage(&message);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		// Turn on the keys in sequence
		for (const auto& key : keys) {
			message[0] = 0x90;
			message[1] = key;
			message[2] = 0;
			midiout->sendMessage(&message);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		delete midiout;

		std::cout << "Messages sent!\n";

	}

	bool MidiDriver::loadKeyMapping(const std::string& filePath)
	{
		return keys.loadMidiKeyMapping(filePath);
	}

	void MidiDriver::updateKeyValue(unsigned char key, unsigned char value)
	{
		auto* keyToUpdate = keys.findMidiKeyByNumber(key);
		if (keyToUpdate) {
			keyToUpdate->m_keyValue = value; // Update the key value
		} else {
			std::cout << "Key not found in mapping! Value cannot be set" << std::endl;
		}
	}

	std::string MidiDriver::getDeviceName(uint32_t device)
	{
		if (device >= m_deviceIn.size())
			return "Device not valid";
		else
			return m_deviceIn[device]->getPortName(device);
	}

	std::string MidiDriver::getVersion()
	{
		return m_version;
	}

	void MidiDriver::captureEvent(double deltatime, std::vector<unsigned char>* message, void* userData)
    {
		auto* driver = static_cast<MidiDriver*>(userData);
		if (driver && driver->m_isRecording) {

			// Capturamos el tiempo real en segundos
			auto now = Clock::now();
			double timestamp = std::chrono::duration<double>(now - driver->m_startRecordingTime).count();
			uint32_t calculatedTick = static_cast<uint32_t>(timestamp * driver->TICKS_PER_SECOND);
			double calculatedTime = static_cast<float>(calculatedTick) / driver->TICKS_PER_SECOND;
			
			std::cout << "Real TimeStamp: " << std::format("{:.5f}s. ", timestamp) <<
				"Calc Time: " << std::format("{:.5f}s. ", calculatedTime) <<
				"Calc Tick: " << calculatedTick << ". ";

			uint32_t nBytes = static_cast<uint32_t>(message->size());
			if (nBytes >= 3) {
				driver->m_events.addEvent(message->at(0), message->at(1), message->at(2), calculatedTick, calculatedTime);
				driver->updateKeyValue(message->at(1), message->at(2)); // Update the key value in the mapping
			}

			// Display the captured MIDI message
			for (uint32_t i = 0; i < nBytes; i++)
				std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
				std::cout << std::endl;
		}
        
    }

	void MidiDriver::captureKeyMapping(double deltatime, std::vector<unsigned char>* message, void* userData)
	{
		auto* driver = static_cast<MidiDriver*>(userData);
		if (driver && driver->m_isKeyMapping) {
			if (driver->keys.mapCurrentKey(message->at(1)))
				std::cout << "Key: " << driver->keys.findCurrentKey()->m_keyName << " mapped to midi ID: " << driver->keys.findCurrentKey()->m_keyNumber << std::endl;
			else
				std::cout << "Current key cannot be mapped, out of bounds!" << std::endl;
				
		}

	}

}
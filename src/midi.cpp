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
		midiin = new RtMidiIn();
		// Check available ports.
		nPorts = midiin->getPortCount();
		if (nPorts != 0) {
			midiin->openPort(0); // Hardcoded to open the first available port.
		}
		// Set the callback function for incoming MIDI messages.
		midiin->setCallback(&captureEvent, this);
		// Don't ignore sysex, timing, or active sensing messages.
		midiin->ignoreTypes(false, false, false);
	}

	MidiDriver::~MidiDriver()
	{
		clearDriver();
	}

	void MidiDriver::openMidiPort(uint32_t port)
	{
		// Check available ports.
		nPorts = midiin->getPortCount();
		if (nPorts > port) {
			midiin->openPort(port);
		}
	}

	void MidiDriver::clearMemory()
	{
		events.clear();
	}

	void MidiDriver::recordEventsStart()
	{
		m_isRecording = true;
		m_startRecordingTime = Clock::now();
	}


	void MidiDriver::recordEventsStop()
	{
		m_isRecording = false;
		std::cout << "Sorting events by tick..." << std::endl;
		std::sort(events.begin(), events.end(), EventMessage::compareByTick);
		std::cout << "Events sorted!" << std::endl;
	}

	void MidiDriver::displayEvents() const
	{
		// TODO> hemos de reordenar todos los eventos por tick y luego mostrarlos ordenados.
		// en caso de que haya duplicados, se guarda el ultimo evento de ese tick.
		// esto se ha de hacer tambien antes de guardar la cancion.

		std::cout << "MIDI Events:" << std::endl;
		for (const auto& event : events) {
			std::cout << "Event type: " << static_cast<uint32_t>(event->type) <<
				", tick: " << static_cast<uint32_t>(event->tick) <<
				", key: " << static_cast<uint32_t>(event->key) <<
				", data: "  << static_cast<uint32_t>(event->value) <<
				", absTime: " << std::format("{:.5f}", event->absTime) << std::endl;
		}
		std::cout << "Total events: " << events.size() << std::endl;
	}

	void MidiDriver::storeSong(const std::string& filePath) const
	{
		Midi::MidiFile midi;
		midi.setResolution(PPQN);
		
		int track = midi.createTrack();  // Crear una pista nueva

		// Añadir Note On al tick 0
		midi.createNoteOnEvent(track, 0, /*canal=*/0, /*nota=*/60, /*velocidad=*/100);
		
		// Añadir Note Off manualmente al tick 480
		midi.createNoteOffEvent(track, 480, /*canal=*/0, /*nota=*/60, /*velocidad=*/0);

		for (const auto& event : events) {
			switch (event->type) {
			
			case 0xB0:
				// Añadir Control Change (p.ej. controlador 7 = volumen) al tick 240
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
				EventMessage* eventMsg = new EventMessage(0xB0, static_cast<unsigned char>(event->number()), static_cast<unsigned char>(event->value()), static_cast<uint32_t>(event->tick()), eventTime);
				events.push_back(eventMsg);
				break;
			}
			case Midi::MidiEvent::EventType::NoteOn:
			{
				EventMessage* eventMsg = new EventMessage(0x90, static_cast<unsigned char>(event->note()), static_cast<unsigned char>(event->velocity()), static_cast<uint32_t>(event->tick()), eventTime);
				events.push_back(eventMsg); 
				break;
			}
			case Midi::MidiEvent::EventType::NoteOff:
			{
				EventMessage* eventMsg = new EventMessage(0x80, static_cast<unsigned char>(event->note()), static_cast<unsigned char>(event->velocity()), static_cast<uint32_t>(event->tick()), eventTime);
				events.push_back(eventMsg); 
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
		midiin->closePort();

		// Clean up
		if (midiin != nullptr) {
			delete midiin;
			midiin = nullptr;
		}			
	}

	void MidiDriver::outputMessage()
	{
		RtMidiOut* midiout = new RtMidiOut();
		std::vector<unsigned char> message;

		// Check available ports.
		unsigned int nPorts = midiout->getPortCount();
		if (nPorts == 0) {
			std::cout << "No ports available!\n";
			delete midiout;
			return;
		}

		// Open first available port.
		midiout->openPort(0);

		// Send out a series of MIDI messages.

		// Control Change: 176, 7, 100 (volume)
		message.push_back(176);
		message.push_back(48);
		message.push_back(127);
		midiout->sendMessage(&message);

		
		std::this_thread::sleep_for(std::chrono::seconds(2)); // Espera 2 segundos

		// Control Change: 176, 7, 100 (volume)
		message[0] = 176;
		message[1] = 48;
		message[2] = 0;
		midiout->sendMessage(&message);

		delete midiout;

		std::cout << "Messages sent!\n";

	}

	std::string MidiDriver::getVersion()
	{
		return midiin->getVersion();
	}

    void MidiDriver::captureEvent(double deltatime, std::vector<unsigned char>* message, void* userData)
    {
		//Hey un bug: Si reseteamos la grabacion el deltaTime deberia valer cero de nuevo, pero no lo hace.


		auto* driver = static_cast<MidiDriver*>(userData);
		if (driver && driver->m_isRecording) {

			// Capturamos el tiempo real en segundos
			auto now = Clock::now();
			double timestamp = std::chrono::duration<double>(now - driver->m_startRecordingTime).count();
			uint32_t calculatedTick = static_cast<uint32_t>(timestamp * driver->TICKS_PER_SECOND);
			
			std::cout << "Abs Time: " << std::format("{:.5f}", timestamp) << "s. ";
			std::cout << "Tick: " << calculatedTick << ". ";

			uint32_t nBytes = static_cast<uint32_t>(message->size());
			if (nBytes >= 3) {
				EventMessage* event = new EventMessage(message->at(0), message->at(1), message->at(2), calculatedTick, timestamp);
				driver->events.push_back(event);
			}

			// Display the captured MIDI message
			for (uint32_t i = 0; i < nBytes; i++)
				std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";

			std::cout << std::endl;
		}
        
    }

}
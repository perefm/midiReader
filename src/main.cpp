#include <iostream>
#include <cstdlib>
//#include "rtmidi/rtmidi.h"
//#include <wolf-midi/MidiFile.h>
#include <midi.h>
#include "main.h"

using namespace Phoenix;

int main()
{
	MidiDriver* midiDriver = new MidiDriver();
    

    char selection;
    uint32_t devicesIn = 0;

    do {
		// Clear the console
		std::system("cls"); // "cls" for Windows, "clear" for Unix-like systems
        std::cout << "\n--- Spontz useless tools presents... the infamous MIDI reader! ---\n";
        std::cout << "\nRtMidi version: " << midiDriver->getVersion();
		std::cout << "\nAvailable MIDI ports: " << midiDriver->m_numPorts;
        devicesIn = midiDriver->getNumDevices();
        std::cout << "\nMIDI devices opened: " << devicesIn;
        for (uint32_t i=0; i< devicesIn; ++i) {
            std::cout << "\nMIDI device " << i << " - Name: " << midiDriver->getDeviceName(i);
        }
        std::cout << "\nEvents stored: " << midiDriver->getEventsSize() << std::endl;
        std::cout << "0. Show this menu :)\n";
        std::cout << "1. Open all available midi IN devices.\n";
        std::cout << "2. Clear memory (events, song, etc...).\n";
        std::cout << "3. Record midi events and store in memory.\n";
        std::cout << "4. Stop recording midi events.\n";
        std::cout << "5. Display captured events.\n";
        std::cout << "6. Store events to song (files/shitty.midi).\n"; 
        std::cout << "7. Load song (files/shitty.midi): CAUTION: This will delete the stored memory events.\n";
        std::cout << "8. Output message.\n";
        std::cout << "9. Trigger events (aka. play song).\n";
        std::cout << "H. Load key mapping file (files/keymapping.spmidi).\n";
        std::cout << "J. Re-Map midi keys.\n";
        std::cout << "K. Show keys mapping.\n";
        std::cout << "L. Store key mapping file (files/keymapping.spmidi).\n";
        std::cout << "Q. Exit.\n\n";
        std::cout << "Choose your destiny: " << std::endl;

        if (midiDriver->isRecording()) {
            std::cout << "--------> Midi events are being recorded!!\n";
		}


        //std::cin >> selection;
        selection = std::cin.get();

        switch (selection) {
        case '\n':
        case '0':
            break;
        case '1':
            midiDriver->openAllPorts();
            break;
        case '2':
            midiDriver->clearMemory();
            break;
        case '3':
            midiDriver->recordEventsStart();
            break;
        case '4':
            midiDriver->recordEventsStop();
            std::system("pause");
            break;
        case '5':
            midiDriver->displayEvents();
            std::cout << "--------------------------\n";
            std::cout << "Total events displayed: " << midiDriver->getEventsSize() << std::endl;
            std::system("pause");
            break;
        case '6':
            midiDriver->storeSong("files/shitty.mid");
            std::system("pause");
            break;
        case '7':
            midiDriver->loadSong("files/shitty.mid");
            std::system("pause");
            break;
        case '8':
            midiDriver->outputMessage();
            std::system("pause");
            break;
        case '9':
            midiDriver->triggerEvents();
            std::system("pause");
            break;
        case 'h':
        case 'H':
            if (!midiDriver->keys.loadMidiKeyMapping("files/keymapping.spmidi")) {
                std::cout << "Errors found in config file (spmidi file).\n";
                std::system("pause");
            }
            else {
                std::cout << "Mapping loaded OK.\n";
                std::system("pause");
            }
            break;
        case 'j':
        case 'J':
            midiDriver->recordMappingStart();
            std::system("pause");
            break;
        case 'k':
        case 'K':
            midiDriver->keys.displayMidiKeyMapping();
            std::system("pause");
            break;
        case 'l':
        case 'L':
            if (!midiDriver->keys.saveMidiKeyMapping("files/keymapping.spmidi")) {
                std::cout << "Errors found saving config file (spmidi file).\n";
                std::system("pause");
            }
            else {
                std::cout << "Mapping saved OK.\n";
                std::system("pause");
            }
            break;
        case 'q':
        case 'Q':
            midiDriver->clearDriver();
            std::cout << "Bye!\n";
            std::system("pause");
            break;
        default:
            std::cout << "Not a valid option, try again please!" << std::endl;
            std::system("pause");
        }

    } while (selection != 'q' && selection != 'Q');
	
    return 0;
}
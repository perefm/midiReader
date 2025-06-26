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
    char no_one_cares;

    do {
		// Clear the console
		std::system("clear || cls"); // "cls" for Windows, "clear" for Unix-like systems
        std::cout << "\n--- Spontz useless tools presents... the infamous MIDI reader! ---\n";
        std::cout << "\nRtMidi version: " << midiDriver->getVersion();
		std::cout << "\nAvailable MIDI ports: " << midiDriver->nPorts;
        std::cout << "\nEvents stored: " << midiDriver->getEventsSize() << std::endl;
        std::cout << "0. Show this menu :)\n";
        std::cout << "1. Re-open midi devices.\n";
        std::cout << "2. Clear memory (events, song, etc...).\n";
        std::cout << "3. Record midi events and store in memory.\n";
        std::cout << "4. Stop recording midi events.\n";
        std::cout << "5. Display captured events.\n";
        std::cout << "6. Store events to song (files/shitty.midi).\n"; 
        std::cout << "7. Load song (files/shitty.midi): CAUTION: This will delete the stored memory events.\n";
        std::cout << "8. Output message.\n";
        std::cout << "H. Load key mapping file (files/keymapping.spmidi).\n";
        std::cout << "J. Re-Map midi keys.\n";
        std::cout << "K. Show keys mapping.\n";
        std::cout << "L. Store key mapping file (files/keymapping.spmidi).\n";
        std::cout << "Q. Exit.\n\n";
        std::cout << "Choose your destiny: " << std::endl;

        if (midiDriver->isRecording()) {
            std::cout << "--------> Midi events are being recorded!!\n";
		}


        std::cin >> selection;

        switch (selection) {
        case '0':
            break;
        case '1':
            midiDriver->openMidiPort(0);
            break;
        case '2':
            midiDriver->clearMemory();
            break;
        case '3':
            midiDriver->recordEventsStart();
            break;
        case '4':
            midiDriver->recordEventsStop();
            std::cin >> no_one_cares;
            break;
        case '5':
            midiDriver->displayEvents();
            std::cout << "--------------------------\n";
            std::cout << "Total events displayed: " << midiDriver->getEventsSize() << std::endl;
            std::cout << "Press any key to get back to menu..." << std::endl;
            std::cin >> no_one_cares;
            break;
        case '6':
            midiDriver->storeSong("files/shitty.mid");

            std::cout << "Press any key to get back to menu..." << std::endl;
            std::cin >> no_one_cares;
            break;
        case '7':
            midiDriver->loadSong("files/shitty.mid");
            std::cout << "Press any key to get back to menu..." << std::endl;
            std::cin >> no_one_cares;
            break;
        case '8':
            midiDriver->outputMessage();
            std::cout << "Press any key to go back to menu.";
            std::cin >> no_one_cares;
            break;
        case 'h':
        case 'H':
            if (!midiDriver->keys.loadMidiKeyMapping("files/keymapping.spmidi")) {
                std::cout << "Errors found in config file (spmidi file), press any key to continue..." << std::endl;
                std::cin >> no_one_cares;
            }
            else {
                std::cout << "Mapping loaded OK, press any key to continue...";
                std::cin >> no_one_cares;
            }
            break;
        case 'j':
        case 'J':
             std::cout << "Re-Map not implemented yet, press any key to continue..." << std::endl;
             std::cin >> no_one_cares;
            break;
        case 'k':
        case 'K':
            midiDriver->keys.displayMidiKeyMapping();
            std::cin >> no_one_cares;
            break;
        case 'l':
        case 'L':
            if (!midiDriver->keys.saveMidiKeyMapping("files/keymapping.spmidi")) {
                std::cout << "Errors found saving config file (spmidi file), press any key to continue..." << std::endl;
                std::cin >> no_one_cares;
            }
            else {
                std::cout << "Mapping saved OK, press any key to continue...";
                std::cin >> no_one_cares;
            }
            break;
        case 'q':
        case 'Q':
            midiDriver->clearDriver();
            std::cout << "Bye!" << std::endl;
            break;
        default:
            std::cout << "Not a vlaid option, try again please!" << std::endl;
        }

    } while (selection != 'q' && selection != 'Q');
	
    return 0;
}
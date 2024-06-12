
#pragma once
#ifndef DSY_MIDIEXT_HPP
#define DSY_MIDIEXT_HPP

#include <stdint.h>
#include <stdlib.h>
#include <algorithm>
#include "../libDaisy/src/per/uart.h"
#include "../libDaisy/src/util/ringbuffer.h"
#include "../libDaisy/src/util/FIFO.h"
#include "../libDaisy/src/hid/midi_parser.h"
#include "../libDaisy/src/hid/usb_midi.h"
#include "../libDaisy/src/sys/dma.h"
#include "../libDaisy/src/sys/system.h"
#include "../libDaisy/src/hid/midi.h"
//The checks could be better perhaps (e.g. this wraps around note numbers and velocities > 127) but in this will get you started.
namespace daisy{
class MidiSender{
public: 
    MidiSender(MidiUsbHandler& midiHandler) : midi(midiHandler) {}
    
    void MIDISendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
        uint8_t data[3] = { 0 };
        
        data[0] = ((channel - 1)& 0x0F) + 0x90;  // limit channel byte, add status byte
        data[1] = note & 0x7F;              // remove MSB on data
        data[2] = velocity & 0x7F;

        midi.SendMessage(data, 3);
    }

    void MIDISendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
        uint8_t data[3] = { 0 };

        data[0] = ((channel - 1) & 0x0F) + 0x80;  // limit channel byte, add status byte
        data[1] = note & 0x7F;              // remove MSB on data
        data[2] = velocity & 0x7F;

        midi.SendMessage(data, 3);
    }

    void MIDISendControlChange(uint8_t channel, uint8_t control_number, uint8_t value) {

        uint8_t data[3] = { 0 };

        data[0] = ((channel - 1) & 0x0F) + 0xB0;  // limit channel byte, add status byte
        data[1] = control_number & 0x7F;        // remove MSB on data
        data[2] = value & 0x7F;

        midi.SendMessage(data, 3);
   }

private:
    MidiUsbHandler& midi;
};
}
#endif
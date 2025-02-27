#include "daisy_seed.h"
#include "daisysp.h"   
#include "MidiExtensions.hpp"
#include "SwitchesSend.cpp"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
//using namespace daisysp;


// Declare a DaisySeed object called hardware
DaisySeed hw;
MidiUsbHandler midi;
ShiftRegister4021<2> switch_sr;

int main(void)
{
    hw.Configure();
    hw.Init();

    MidiUsbHandler::Config midi_cfg;
    midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    midi.Init(midi_cfg);
    MidiSender send(midi);

    ShiftRegister4021<2>::Config switch_sr_cfg;
    switch_sr_cfg.latch = seed::D28; 
    switch_sr_cfg.clk = seed::D27;
    switch_sr_cfg.data[0] = seed::D14;
    switch_sr.Init(switch_sr_cfg);

    ShiftRegister595 led_sr;
    dsy_gpio_pin led_sr_pins[3];
    led_sr_pins[0] = seed::D28;
    led_sr_pins[1] = seed::D27;
    led_sr_pins[2] = seed::D13;
    led_sr.Init(led_sr_pins);

    //buttons
    Switch buttons[8];

    buttons[0].Init(seed::D17, 1000);
    buttons[1].Init(hw.GetPin(18), 1000);
    buttons[2].Init(hw.GetPin(19), 1000);
    buttons[3].Init(hw.GetPin(20), 1000);
    buttons[4].Init(hw.GetPin(21), 1000);
    buttons[5].Init(hw.GetPin(24), 1000);
    buttons[6].Init(hw.GetPin(25), 1000);
    buttons[7].Init(hw.GetPin(26), 1000);

    //pot
    AdcChannelConfig adcConfig[2];
    adcConfig[0].InitSingle(hw.GetPin(15)); 
    adcConfig[1].InitSingle(hw.GetPin(16)); 
    hw.adc.Init(adcConfig, 2);
    hw.adc.Start();

    //pot update parameters
    float pot1Value_old = 0.0f;
    float pot2Value_old = 0.0f;
    float threshold = 0.01f; 

    SwitchesSend switches(switch_sr,hw,send);
    bool ledState[8] = {false};
    

    for(;;){
    float pot1Value = hw.adc.GetFloat(0);
    float pot2Value = hw.adc.GetFloat(1);

    if(abs(pot1Value - pot1Value_old) > threshold){
    send.MIDISendControlChange(1,8,(pot1Value*127));
    pot1Value_old = pot1Value;
    }

    if(abs(pot2Value - pot2Value_old) > threshold){
    send.MIDISendControlChange(1,9,(pot2Value*127));
    pot2Value_old = pot2Value;
    }

    switches.Update();
    
    for(int i = 0; i < 8; i++) {
        buttons[i].Debounce();
        if(buttons[i].RisingEdge()) {
            send.MIDISendControlChange(1, i, 127);
           
        }
        else if(buttons[i].FallingEdge()) {
            send.MIDISendControlChange(1, i, 0);
        }
    }

    for(int i=0; i < 8; i++){
    if(ledState[i]){
        led_sr.Set(i, true);
     }
    else{
        led_sr.Set(i, false);
    }
    }
        led_sr.Write();
    /** Listen to MIDI for new changes */
    midi.Listen();

    /** When there are messages waiting in the queue... */
    while(midi.HasEvents())
    {
        /** Pull the oldest one from the list... */
        auto msg = midi.PopEvent();
        for(int i=0;i<8;i++){
            if(msg.data[0] == 27+i){
                if(msg.data[1]==127)
                ledState[i]=true;
                else
                ledState[i]=false;
            }
        }
        
     
       
    }

   

    //If the button is pressed, turn the LED on and send a MIDI Note On event
   
    }
      //wait 1 ms
       System::Delay(1);
    }
      




#include "daisy_seed.h"
#include "daisysp.h"   
#include "MidiExtensions.hpp"

using namespace daisy;

class SwitchesSend {
public: 
        static const int NUM_PINS = 16;
    SwitchesSend(ShiftRegister4021<2> & switch_sr, DaisySeed& hw, MidiSender& send)
        : switch_sr_(switch_sr), hw_(hw), send_(send) {
           for (int i = 0; i < NUM_PINS; i++) {
            lastDebounceTime_[i] = 0;
            lastState_[i] = false;
        }
        }

    void Update() {
          switch_sr_.Update();
        for (int i = 0; i < NUM_PINS; i++) {
            bool currentState = switch_sr_.State(i);
            if(currentState != lastState_[i]){
                if((hw_.system.GetUs() - lastDebounceTime_[i]) > debounceDelay_){
                    lastDebounceTime_[i] = hw_.system.GetUs();
                    if (currentState){
                        send_.MIDISendControlChange(1, (10 + i), 127);
                    }
                }
            }
            lastState_[i] = currentState;
        }
    }

private:
    ShiftRegister4021<2> switch_sr_;
    DaisySeed& hw_;
    MidiSender& send_;
    uint64_t lastDebounceTime_[NUM_PINS];
    bool lastState_[NUM_PINS];
    uint64_t debounceDelay_ = 1000;
};
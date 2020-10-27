#include "devices.h"


unsigned char ICPCON::TranslatefromASCII(unsigned char number){
    for(unsigned char i = 0; i < 16; ++i){
        if(number == hexx[i]){
            return i;
        }
    }
    return number;
}

unsigned char ICPCON::TranslatetoASCII(unsigned char number){
    for(unsigned char i = 0; i < 16; ++i){
        if(number == i){
            return hexx[i];
        }
}
  return number;
}

void ICPCON::ManageChannel(int channel, bool Enable){
    if(channel < 0 || channel > 7)return;
    if(channel <= 3){
        unsigned char current_state = ICPCONBUFFER[2];
        current_state = TranslatefromASCII(current_state);
        if(Enable)current_state = current_state | (1<<channel);
        else{
            if(current_state & (1<<channel))
            current_state = current_state ^ (1<<channel);
        }
        current_state = TranslatetoASCII(current_state);
        ICPCONBUFFER[2] = current_state;
    }
    else{
        unsigned char current_state = ICPCONBUFFER[1];
        if(Enable)current_state = TranslatefromASCII(current_state);
        else{
            if(current_state & (1<<(channel-4)))
            current_state = current_state ^ (1<<(channel-4));
        }
        current_state = TranslatetoASCII(current_state);
        ICPCONBUFFER[1] = current_state;
    }
}


void ICPCON::ProcessOutputsState(){
    if(Con_answer[0]==62){
        for(int cch = 0; cch<6; ++cch)ICPCONBUFFER[cch] = Con_answer[cch];
        int low = 0, high = 0;
        //std::cout<<" Procces ICPCON status \n";
        for(int i = 0; i < 16; ++i){
            if(Con_answer[2] == hexx[i]){
                low = i;
            }
            if(Con_answer[1] == hexx[i]){
                high = i;
            }
        }
        //std::cout<<" Low : "<<low<<" High : "<<high<<std::endl;


        for(int i = 0; i < 4; ++i){
            if((1<<i) & low) Outputs_State[i] = 1;
            else  Outputs_State[i] = 0;

        }

        for(int i = 4; i < 8; ++i){
            if((1<<(i-4)) & high)Outputs_State[i] = 1;
            else Outputs_State[i] = 0;
        }

        //for(int i = 0; i < 8; ++i)std::cout<<" Output : "<<i<<" state: "<<Outputs_State[i]<<std::endl;
    }
}

//cl /EHsc /Zi /Fe: midi.exe midi.cpp
//instructions: http://midi.teragonaudio.com/tech/stream.htm
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "User32.lib")
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include <mmeapi.h>
#include <Mmdeviceapi.h>
#include <dinput.h>
#include <thread> 

//Arturia MiniLab mapping - ABLETON (TAB 8, KEY MIDI CH 1)
#define ART_ML_MKII_STATUS_PITCH 0xE5
#define ART_ML_MKII_SLIDER_PITCH 0x00
#define ART_ML_MKII_SLIDER_PITCH_MAX 0x7F
#define ART_ML_MKII_STATUS_ARPP 0xB5
#define ART_ML_MKII_SLIDER_ARPP 0x01

#define ART_ML_MKII_STATUS_ROTENC 0xB1
#define ART_ML_MKII_STATUS_ROTENC_SHIFT 0xB0
#define ART_ML_MKII_ROTENC_1 0x70
#define ART_ML_MKII_ROTENC_1_SHIFT 0x7
#define ART_ML_MKII_ROTENC_2 0x17
#define ART_ML_MKII_ROTENC_3 0x18
#define ART_ML_MKII_ROTENC_4 0x19
#define ART_ML_MKII_ROTENC_5 0x1A
#define ART_ML_MKII_ROTENC_6 0x1B
#define ART_ML_MKII_ROTENC_7 0x1C
#define ART_ML_MKII_ROTENC_8 0x1D
#define ART_ML_MKII_ROTENC_9 0x72
#define ART_ML_MKII_ROTENC_9_SHIFT 0x74
#define ART_ML_MKII_ROTENC_10 0x1F
#define ART_ML_MKII_ROTENC_11 0x21
#define ART_ML_MKII_ROTENC_12 0x22
#define ART_ML_MKII_ROTENC_13 0x34
#define ART_ML_MKII_ROTENC_14 0x35
#define ART_ML_MKII_ROTENC_15 0x36
#define ART_ML_MKII_ROTENC_16 0x37

#define ART_ML_MKII_KEY_ON 0x90
#define ART_ML_MKII_KEY_OFF 0x80
#define ART_ML_MKII_KEY_DSH 0x3F
#define ART_ML_MKII_KEY_FSH 0x42

#define ART_ML_MKII_PAD_ON 0x9A
#define ART_ML_MKII_PAD_UPD 0xAA
#define ART_ML_MKII_PAD_OFF 0x8A
#define ART_ML_MKII_PAD_3 0x26
#define ART_ML_MKII_PAD_4 0x27
#define ART_ML_MKII_PAD_5 0x28
#define ART_ML_MKII_PAD_6 0x29
#define ART_ML_MKII_PAD_7 0x2A

UINT deviceListSize;
UINT selectedDevice;
UINT deviceId;

BOOL isRecording;

using namespace std;

void handleKey_SOUND_VOLTEX(UINT status, UINT obj, UINT value){
    const UINT KEYEVENT_KEYUP = 0x02;
    const UINT KEYEVENT_KEYDOWN = 0x00;

    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wVk = 0;
    ip.ki.dwExtraInfo = 0;
    
    switch (status){
        case ART_ML_MKII_STATUS_ROTENC:
            switch(obj){
                case ART_ML_MKII_ROTENC_11:
                    if(value > 0x7){ //left
                        ip.ki.wScan = DIK_A;
                    } else if(value > 0x00){ //right
                        ip.ki.wScan = DIK_S;
                    } else {
                        break;
                    }
                    //press
                    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &ip, sizeof(INPUT));
                    Sleep(20);
                    //release
                    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                    SendInput(1, &ip, sizeof(INPUT));
                    break;
                case ART_ML_MKII_ROTENC_15:
                    if(value > 0x7){ //left
                        ip.ki.wScan = DIK_L;
                    } else if(value > 0x00){ //right
                        ip.ki.wScan = DIK_SEMICOLON;
                    } else {
                        break;
                    }
                    //press
                    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &ip, sizeof(INPUT));
                    Sleep(20);
                    //release
                    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                    SendInput(1, &ip, sizeof(INPUT));
                    break;
            }
            break;
        case ART_ML_MKII_KEY_ON:
            switch(obj){
                case ART_ML_MKII_KEY_DSH:
                    ip.ki.wScan = DIK_V;
                    break;
                case ART_ML_MKII_KEY_FSH:
                    ip.ki.wScan = DIK_B;
                    break;
            }
            //press
            ip.ki.dwFlags = KEYEVENTF_SCANCODE;
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(20);
            break;
        case ART_ML_MKII_KEY_OFF:
             switch(obj){
                case ART_ML_MKII_KEY_DSH:
                    ip.ki.wScan = DIK_V;
                    break;
                case ART_ML_MKII_KEY_FSH:
                    ip.ki.wScan = DIK_B;
                    break;
            }
            //release
            ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(20);
            break;
        case ART_ML_MKII_PAD_ON:
            switch(obj){
                case ART_ML_MKII_PAD_3:
                    ip.ki.wScan = DIK_D;
                    break;
                case ART_ML_MKII_PAD_4:
                    ip.ki.wScan = DIK_F;
                    break;
                case ART_ML_MKII_PAD_5:
                    ip.ki.wScan = DIK_6;
                    break;
                case ART_ML_MKII_PAD_6:
                    ip.ki.wScan = DIK_J;
                    break;
                case ART_ML_MKII_PAD_7:
                    ip.ki.wScan = DIK_K;
                    break;
            }
            //press
            ip.ki.dwFlags = KEYEVENTF_SCANCODE;
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(20);
            break;
        case ART_ML_MKII_PAD_OFF:
            switch(obj){
                case ART_ML_MKII_PAD_3:
                    ip.ki.wScan = DIK_D;
                    break;
                case ART_ML_MKII_PAD_4:
                    ip.ki.wScan = DIK_F;
                    break;
                case ART_ML_MKII_PAD_5:
                    ip.ki.wScan = DIK_6;
                    break;
                case ART_ML_MKII_PAD_6:
                    ip.ki.wScan = DIK_J;
                    break;
                case ART_ML_MKII_PAD_7:
                    ip.ki.wScan = DIK_K;
                    break;
            }
            //release
            ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(20);
            break;
    }
    
}

void CALLBACK MidiInProc(
    HMIDIIN   hMidiIn,
    UINT      wMsg,
    DWORD_PTR dwInstance,
    DWORD_PTR dwParam1,
    DWORD_PTR dwParam2){

    //KEY/BTN STATE/AFFECTED
    printf("\"message\": %d", wMsg);
    if(wMsg == MIM_DATA || wMsg == MIM_MOREDATA){
        DWORD HIGH = dwParam1 >> 0x10;
        DWORD LOW = dwParam1 & 0x0000FFFF;
        WORD STATUS = LOW & 0x00FF;             //
        WORD DATA1 = ((LOW >> 0x08) & 0x00FF);  //key,button,knob
        WORD DATA2 = HIGH & 0x00FF;             //force

        printf(", \"data\": { \"dwI\": 0x%X, \"status\": 0x%X, \"data1\": 0x%X, \"data2\": 0x%X, \"ms\": %d }\n", dwInstance, STATUS, DATA1, DATA2, dwParam2);
        
        handleKey_SOUND_VOLTEX(STATUS, DATA1, DATA2);
    }
}

MMRESULT getInputDevice(UINT id){
    MIDIINCAPS midiInCapabilities;
    MMRESULT retStatus = midiInGetDevCaps(id, &midiInCapabilities, sizeof(midiInCapabilities));

    printf("{\n\t\"id\": \"%d\",\n\t\"product-id\": \"%X\",\n\t\"device\": \"%s\",\n\t\"by\": \"%X\"\n}", id, midiInCapabilities.wPid, midiInCapabilities.szPname, midiInCapabilities.wMid);

    return retStatus;
};

void reset(){
    deviceListSize = 0;
}

void selectDevice(){
    reset();
    deviceListSize = midiInGetNumDevs();
    printf("MIDI-IN Devices found:%d\n", deviceListSize);
    printf("\"Devices\": [");
    for(int deviceIndex = 0; deviceIndex < deviceListSize; deviceIndex++){
        if(getInputDevice(deviceIndex) == MMSYSERR_NOERROR){
            if(deviceIndex + 1 < deviceListSize){
                printf(",\n");
            } else {
                printf("]\n");
            }
        }
    }
    printf("select device: ");
    cin >> selectedDevice;
}

int checkKeyDown()
{
    int keyPressed = -1;

    if(GetAsyncKeyState(0x41) < 0) keyPressed = DIK_A;
    if(GetAsyncKeyState(0x42) < 0) keyPressed = DIK_B;
    if(GetAsyncKeyState(0x43) < 0) keyPressed = DIK_C;
    if(GetAsyncKeyState(0x44) < 0) keyPressed = DIK_D;
    if(GetAsyncKeyState(0x45) < 0) keyPressed = DIK_E;
    if(GetAsyncKeyState(0x46) < 0) keyPressed = DIK_F;
    if(GetAsyncKeyState(0x47) < 0) keyPressed = DIK_G;
    if(GetAsyncKeyState(0x48) < 0) keyPressed = DIK_H;
    if(GetAsyncKeyState(0x49) < 0) keyPressed = DIK_I;
    if(GetAsyncKeyState(0x4A) < 0) keyPressed = DIK_J;
    if(GetAsyncKeyState(0x4B) < 0) keyPressed = DIK_K;
    if(GetAsyncKeyState(0x4C) < 0) keyPressed = DIK_L;
    if(GetAsyncKeyState(0x4D) < 0) keyPressed = DIK_M;
    if(GetAsyncKeyState(0x4E) < 0) keyPressed = DIK_N;
    if(GetAsyncKeyState(0x4F) < 0) keyPressed = DIK_O;
    if(GetAsyncKeyState(0x50) < 0) keyPressed = DIK_P;
    if(GetAsyncKeyState(0x51) < 0) keyPressed = DIK_Q;
    if(GetAsyncKeyState(0x52) < 0) keyPressed = DIK_R;
    if(GetAsyncKeyState(0x53) < 0) keyPressed = DIK_S;
    if(GetAsyncKeyState(0x54) < 0) keyPressed = DIK_T;
    if(GetAsyncKeyState(0x55) < 0) keyPressed = DIK_U;
    if(GetAsyncKeyState(0x56) < 0) keyPressed = DIK_V;
    if(GetAsyncKeyState(0x57) < 0) keyPressed = DIK_W;
    if(GetAsyncKeyState(0x58) < 0) keyPressed = DIK_X;
    if(GetAsyncKeyState(0x59) < 0) keyPressed = DIK_Y;
    if(GetAsyncKeyState(0x5A) < 0) keyPressed = DIK_Z;

    return keyPressed;
}

void recordInput()
{
    int dInputKey = 0x00;
   
    while((dInputKey = checkKeyDown()) < 0)
    {
        if(GetAsyncKeyState(VK_RCONTROL) < 0)
        {
            isRecording = false;
            Sleep(200);
            break;
        }
    }
}

int main(){
    HMIDIIN inHandler;
    selectDevice();
    if(midiInOpen(&inHandler, selectedDevice, (DWORD)&MidiInProc, 0, CALLBACK_FUNCTION) == MMSYSERR_NOERROR){
        midiInStart(inHandler);
        for(;;){
            if(GetAsyncKeyState(VK_ESCAPE) < 0)
            {
                cout << "exiting...";
                midiInStop(inHandler);
                Sleep(5000);
                break;
            }
            
            if(GetAsyncKeyState(VK_RCONTROL) < 0)
            {
                isRecording = true;
                Sleep(200);
            }

            while(isRecording)
            {
                cout << "Flushing the output stream." << flush;
                recordInput();
            }

            //read buffer
            MIDIHDR inHeader;
            UINT size = 4096; //4k data
            if(midiInAddBuffer(inHandler, &inHeader, size) == MMSYSERR_NOERROR){
                printf("bytes of data recorded: %d", inHeader.dwBytesRecorded);
            }
        }
    } else {
        printf("device failed to load, press enter to continue...\n");
        cin;
    }
    return 0;
}
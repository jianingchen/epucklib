/*

embedded system library for e-puck

serial communication utility
--------------------------------------------------------------------------------

code distribution:
https://github.com/jianingchen/epucklib

online documentation:
http://jianingchen.github.io/epucklib/html/

--------------------------------------------------------------------------------

This file is released under the terms of the MIT license (see "el.h").

*/

#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include "Serial.h"

using namespace std;

#define CAMERA_IMAGE_DIM_X      40
#define CAMERA_IMAGE_DIM_Y      15
#define RX_BUFFER_DIM           1024
#define TX_BUFFER_DIM           64
#define PAGE_BUFFER_DIM         4096

#define TEXT_HIGHLIGHT_TITLE    {SetConsoleTextAttribute(hWnd,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);}
#define TEXT_HIGHLIGHT_PLAIN    {SetConsoleTextAttribute(hWnd,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);}
#define TEXT_HIGHLIGHT_INPUT    {SetConsoleTextAttribute(hWnd,FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);}
#define TEXT_HIGHLIGHT_PAGE     {SetConsoleTextAttribute(hWnd,FOREGROUND_GREEN);}
#define TEXT_HIGHLIGHT_FILE     {SetConsoleTextAttribute(hWnd,FOREGROUND_BLUE|FOREGROUND_INTENSITY);}

HANDLE hWnd; 
time_t StartTime;
char RxBuffer[RX_BUFFER_DIM];
size_t RxBufferLength;
char PageBuffer[PAGE_BUFFER_DIM];
size_t PageLength;
const char OutputDirectory[] = "CameraImages";
unsigned int FileCounter;
char Filename[80] = "";
uint8_t BmpData[CAMERA_IMAGE_DIM_Y][CAMERA_IMAGE_DIM_X][3];
CSerial SerialInterface;

void MainLoop();
int TranslatePage();
int SaveBmp24(const char*Filename,int Width,int Height,const uint8_t*Data);

int main(int argc,char*argv[]){
    string PortName;
    string commandline;
    int r;
    
    hWnd = GetStdHandle(STD_OUTPUT_HANDLE);
    StartTime = time(NULL);
    FileCounter = 0;
    
    TEXT_HIGHLIGHT_TITLE;
    printf("\nembedded system library for e-puck\n");
    TEXT_HIGHLIGHT_PLAIN;
    printf("serial communication utility\n");
    printf("________________________________________________________________\n\n");
    printf("This utility can communicate with an e-puck through a COM port.\n");
    printf("When a camera image sent by \"el_print_image\" is detected, \n");
    printf("the utility will save it as a BMP file.\n\n");
    printf("Press H to let the robot list available commands.\n\n");
    printf("Press ESC to exit when the communication is running.\n");
    printf("________________________________________________________________\n\n");
    printf("%u\n\n",StartTime);

ENTER_NAME:
    TEXT_HIGHLIGHT_INPUT;
    printf("enter COM port name:\n>>");
    cin>>PortName;
    TEXT_HIGHLIGHT_PLAIN
    
    if(PortName=="exit"){
        goto MAIN_RETURN;
    }
    
    printf("openning %s\n",PortName.c_str());

    r = SerialInterface.Open(PortName.c_str(),0,0,false);
    if(r!=NO_ERROR){
        printf("failed to open %s !\n",PortName.c_str());
        goto ENTER_NAME;
    }

    r = SerialInterface.Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
    if(r!=NO_ERROR){
        printf("failed to setup %s !\n",PortName.c_str());
        goto CLOSE_DOWN;
    }

    r = SerialInterface.SetupHandshaking(CSerial::EHandshakeHardware);
    if(r!=NO_ERROR){
        printf("failed to handshake %s !\n",PortName.c_str());
        goto CLOSE_DOWN;
    }

    printf("%s opened\n",PortName.c_str());

    SerialInterface.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
    
    commandline = "mkdir ";
    commandline += OutputDirectory;
    system(commandline.c_str());

    MainLoop();

CLOSE_DOWN:
    SerialInterface.Close();
    
    printf("%s closed\n",PortName.c_str());

MAIN_RETURN:
    system("pause");
    return 0;
}

void MainLoop(){
    char c,*p;
    int i;
    DWORD length;
    string temp;
    bool CapturePage;
    
    RxBufferLength = 0;
    CapturePage = false;
    PageLength = 0;

    TEXT_HIGHLIGHT_PLAIN;
    while(1){
        
        /** sending keyboard intput **/
        
        if(GetAsyncKeyState(VK_ESCAPE)){
            break;
        }
        if(GetAsyncKeyState(VK_F5)){
            system("cls");
        }
        
        if(_kbhit()){
            c = _getch();
            SerialInterface.Write(&c,1);
        }
        
        
        /** receiving from the port **/
        
        
        SerialInterface.Read(RxBuffer,RX_BUFFER_DIM,&length);
        
        RxBuffer[length] = '\0';
        
        if(length==0){
            continue;
        }
        
        /** analyze and display **/
        
        i = 0;
        p = RxBuffer;
        while(i<length){
            if(CapturePage){
                if(strncmp(p,"</p>",4)==0){
                    CapturePage = false;
                    TEXT_HIGHLIGHT_PLAIN;
                    PageBuffer[PageLength++] = '\n';
                    PageBuffer[PageLength] = '\0';
                    TranslatePage();
                }else{
                    if(PageLength < (PAGE_BUFFER_DIM - 4)){
                        PageBuffer[PageLength++] = *p;
                    }
                }
            }else{
                if(strncmp(p,"<p>",3)==0){
                    TEXT_HIGHLIGHT_PAGE;
                    CapturePage = true;
                    PageLength = 0;
                    PageBuffer[PageLength++] = *p;
                }
            }
            putchar(*p);
            ++p;
            ++i;
        }
        
    }

}

int TranslatePage(){
    const uint16_t RGB565_MASK_RED   = 0xF800;
    const uint16_t RGB565_MASK_GREEN = 0x07E0;
    const uint16_t RGB565_MASK_BLUE  = 0x001F;
    const char DELIMIT[] = "\r\n";
    uint32_t w;
    int X,Y,W,H,Number;
    int r;
    char *p,*q;
    /*
    FILE *Output;
    Output = fopen("DebugPage.txt","w");
    fwrite(PageBuffer,PageLength,1,Output);
    fclose(Output);
    */

    r = 0;
    p = strtok_s(PageBuffer,DELIMIT,&q);
    // skip one line

    p = strtok_s(NULL,DELIMIT,&q);
    r += sscanf(p,"NUM: %d",&Number);

    p = strtok_s(NULL,DELIMIT,&q);
    r += sscanf(p,"DIM: %d,%d",&X,&Y);

    p = strtok_s(NULL,DELIMIT,&q);
    r += sscanf(p,"LEN: %d,%d",&W,&H);
    
    if(r != 5){
        return -1;
    }
    if(W>CAMERA_IMAGE_DIM_X || H>CAMERA_IMAGE_DIM_Y){
        return -1;
    }
    
    for(Y=0;Y<H;Y++){

        p = strtok_s(NULL,DELIMIT,&q);
        if(p==NULL){
            break;
        }

        for(X=0;X<W;X++){
            if(sscanf(p,"%4X",&w)){
                BmpData[Y][X][2] = (w & RGB565_MASK_RED)>>8;
                BmpData[Y][X][1] = (w & RGB565_MASK_GREEN)>>3;
                BmpData[Y][X][0] = (w & RGB565_MASK_BLUE)<<3;
                p += 4;
            }
        }

    }

    ++FileCounter;
    _snprintf(Filename,80,"%s/%u_%u_%u_%ux%u.BMP",
        OutputDirectory,(unsigned int)StartTime,FileCounter,Number,W,H);
    
    r = SaveBmp24(Filename,W,H,(const uint8_t*)BmpData);
    if(r==NO_ERROR){
        TEXT_HIGHLIGHT_FILE;
        printf("[%s]",Filename);
        TEXT_HIGHLIGHT_PLAIN;
    }

    return NO_ERROR;
}

int SaveBmp24(const char*Filename,int Width,int Height,const uint8_t*Data){
    uint8_t BmpHeader[54] = {
        0x42, 0x4D, 0x3E, 0x07, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 
        0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x0F, 0x00, 
        0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x08, 0x07, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    FILE *Output;
    int line_size,line_pad;
    const uint8_t *p;
    int i;
    int Z = 0;
    
    Output = fopen(Filename,"wb");
    if(Output==NULL){
        return -1;
    }
    
    0[(int32_t*)(BmpHeader + 18)] = Width;
    1[(int32_t*)(BmpHeader + 18)] = Height;

    fwrite(BmpHeader,54,1,Output);
    
    line_size = (Width*3);
    line_pad = line_size%4;
    if(line_pad){
        line_pad = 4 - line_pad;
    };

    p = Data;
    for(i=0;i<Height;i++){
        fwrite(p,line_size,1,Output);
        p += line_size;
        if(line_pad){
            fwrite(&Z,line_pad,1,Output);
        }
    }

    fclose(Output);

    return NO_ERROR;
}


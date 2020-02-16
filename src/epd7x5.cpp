#include <node.h>
#include "epdif.h" 

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Boolean;
using v8::Object;
using v8::Value;

// Display resolution
#define EPD_WIDTH       800
#define EPD_HEIGHT      480

// EPD7IN5B V2 commands
#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_ON                                    0x04
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DISPLAY_REFRESH                             0x12
#define DATA_START_TRANSMISSION_2                   0x13
#define DUAL_SPI_MODE                               0x15
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define TCON_SETTING                                0x60
#define TCON_RESOLUTION                             0x61

void SendCommand(unsigned char command) {
    EpdIf::DigitalWrite(DC_PIN, LOW);
    EpdIf::SpiTransfer(command);
}
void SendData(unsigned char data) {
    EpdIf::DigitalWrite(DC_PIN, HIGH);
    EpdIf::SpiTransfer(data);
}


void WaitUntilIdle(void) {
    while(EpdIf::DigitalRead(BUSY_PIN) == 0) {      //0: busy, 1: idle
        EpdIf::DelayMs(200);
    }      
}

void Reset(void) {
    EpdIf::DigitalWrite(RST_PIN, LOW);                //module reset    
    EpdIf::DelayMs(200);
    EpdIf::DigitalWrite(RST_PIN, HIGH);
    EpdIf::DelayMs(200);    
}


void init(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	bool epdInterfaceInitSuccess = EpdIf::IfInit() == 0;
	
    if (epdInterfaceInitSuccess) {
		Reset();
        SendCommand(POWER_SETTING);
        SendData(0x07);
        SendData(0x07);    //VGH=20V,VGL=-20V
        SendData(0x3f);		//VDH=15V
        SendData(0x3f);		//VDL=-15V
        SendCommand(POWER_ON);
        EpdIf::DelayMs(100);
        WaitUntilIdle();
        SendCommand(PANEL_SETTING);
        SendData(0x0F);   //KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f
        SendCommand(TCON_RESOLUTION);
        SendData(0x03);		//source 800
        SendData(0x20);
        SendData(0x01);		//gate 480
        SendData(0xE0);
        SendCommand(DUAL_SPI_MODE);
        SendData(0x00);
        SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
        SendData(0x11);
        SendData(0x07);
        SendCommand(TCON_SETTING);
        SendData(0x22);
		WaitUntilIdle();
	}

	Local<Boolean> nodeReturnVal = Boolean::New(isolate, epdInterfaceInitSuccess);
	args.GetReturnValue().Set(nodeReturnVal);
}

void display(unsigned char* frame_buffer, unsigned char* frame_buffer_red) {
    SendCommand(DATA_START_TRANSMISSION_1);
	for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT; i++) {
        SendData(frame_buffer[i]);
    }

    SendCommand(DATA_START_TRANSMISSION_2);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT; i++) {
        SendData(frame_buffer_red[i]);
    }

    SendCommand(DISPLAY_REFRESH);
    EpdIf::DelayMs(100);
    WaitUntilIdle();
}


void displayFrame(const FunctionCallbackInfo<Value>& args) {
	v8::Local<v8::Uint8Array> view = args[0].As<v8::Uint8Array>();
    v8::Local<v8::Uint8Array> view_red = args[1].As<v8::Uint8Array>();
	void *data = view->Buffer()->GetContents().Data();
	void *data_red = view_red->Buffer()->GetContents().Data();
	unsigned char* imagedata = static_cast<unsigned char*>(data);
    unsigned char* imagereddata = static_cast<unsigned char*>(data_red);

	display(imagedata, imagereddata);
}

void deepSleep(const FunctionCallbackInfo<Value>& args) {
    SendCommand(POWER_OFF);
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);
    SendData(0xA5);
}

void InitAll(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "displayFrame", displayFrame);
  NODE_SET_METHOD(exports, "deepSleep", deepSleep);
}

NODE_MODULE(epd7x5, InitAll)
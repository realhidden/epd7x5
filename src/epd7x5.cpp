#include <node.h>
#include "epdif.h" 

// Modification based on diff:
// https://raw.githubusercontent.com/waveshare/e-Paper/master/RaspberryPi%26JetsonNano/c/lib/e-Paper/EPD_7in5b_V2.c
// https://raw.githubusercontent.com/waveshare/e-Paper/master/RaspberryPi%26JetsonNano/c/lib/e-Paper/EPD_7in5b_HD.c
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Boolean;
using v8::Object;
using v8::Value;

// Display resolution
#define EPD_WIDTH       880
#define EPD_HEIGHT      528

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
        EpdIf::DelayMs(10);
    }
    EpdIf::DelayMs(200);
}

void Reset(void) {
    EpdIf::DigitalWrite(RST_PIN, HIGH);
    EpdIf::DelayMs(200);
    EpdIf::DigitalWrite(RST_PIN, LOW);                //module reset    
    EpdIf::DelayMs(10);
    EpdIf::DigitalWrite(RST_PIN, HIGH);
    EpdIf::DelayMs(200);    
}


void init(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	bool epdInterfaceInitSuccess = EpdIf::IfInit() == 0;
	
    if (epdInterfaceInitSuccess) {
		Reset();
		SendCommand(0x12); //SWRESET
		WaitUntilIdle(); //waiting for the electronic paper IC to release the idle signal
		SendCommand(0x46); // Auto Write RAM
		SendData(0xF7);
		WaitUntilIdle(); //waiting for the electronic paper IC to release the idle signal

		SendCommand(0x47); // Auto Write RAM
        SendData(0xF7);
        WaitUntilIdle(); //waiting for the electronic paper IC to release the idle signal

        SendCommand(0x0C); // Soft start setting
        SendData(0xAE);
        SendData(0xC7);
        SendData(0xC3);
        SendData(0xC0);
        SendData(0x40);   
        SendCommand(0x01);  // Set MUX as 527
        SendData(0xAF);
        SendData(0x02);
        SendData(0x01);
        SendCommand(0x11);  // Data entry mode
        SendData(0x01);
        SendCommand(0x44);
        SendData(0x00); // RAM x address start at 0
        SendData(0x00);
        SendData(0x6F); // RAM x address end at 36Fh -> 879
        SendData(0x03);
        SendCommand(0x45);
        SendData(0xAF); // RAM y address start at 20Fh;
        SendData(0x02);
        SendData(0x00); // RAM y address end at 00h;
        SendData(0x00);
        SendCommand(0x3C); // VBD
        SendData(0x01); // LUT1, for white
        SendCommand(0x18);
        SendData(0x80);
        SendCommand(0x22);
        SendData(0xB1);	//Load Temperature and waveform setting.
        SendCommand(0x20);
        WaitUntilIdle();        //waiting for the electronic paper IC to release the idle signal

        SendCommand(0x4E); 
        SendData(0x00);
        SendData(0x00);
        SendCommand(0x4F); 
        SendData(0xAF);
        SendData(0x02);
	}

	Local<Boolean> nodeReturnVal = Boolean::New(isolate, epdInterfaceInitSuccess);
	args.GetReturnValue().Set(nodeReturnVal);
}

void display(unsigned char* frame_buffer, unsigned char* frame_buffer_red) {
    SendCommand(0x4F);
    SendData(0xAF);
    SendData(0x02);

    //send black data
    SendCommand(0x24);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT; i++) {
                SendData(frame_buffer[i]);
    }

    //send red data
    SendCommand(0x26);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT; i++) {
            SendData(frame_buffer_red[i]);
    }

    //Turn on Display
    SendCommand(0x22);
    SendData(0xC7);    //Load LUT from MCU(0x32)

    SendCommand(0x20);
    EpdIf::DelayMs(200);    //!!!The delay here is necessary, 200uS at least!!!
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
    SendCommand(0x10);
    SendData(0x01);
}

void InitAll(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "displayFrame", displayFrame);
  NODE_SET_METHOD(exports, "deepSleep", deepSleep);
}

NODE_MODULE(epd7x5, InitAll)
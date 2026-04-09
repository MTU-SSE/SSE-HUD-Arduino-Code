/*
  This file is meant to be an interrupt take on the original polling request set up used in the prototype vehicle.
  Interrupt: CAN notifies CPU when requires attention, can take place at any time, allows for concurrent events
    Advantage: Sleep Modes and paralell processing
    Disadvantages: Context switching, interrupt latency
  Polling Request: CPU Steadily checks if CAN needs attention
    Advantages: No context switching, Reduced interrupt overhead
    Disadvantages: Inefficiency, Increased power consumption

  File Created: 2/15/26
  File Revision Date: 3/26/26

  Updates: updated bitrate and clockrate to be compatable with custom Motor Controller (500Kbps, 8MHz respectively)
           rewired INT pin on MCP2515 to go to D2 on Mega 2560 because D49 does not support interrupts
           properly set up attaching interrupt to trigger
*/
#include <SPI.h>
#include <mcp2515.h>
#include <stdint.h>

struct can_frame CAN_MSG;
MCP2515 mcp2515(32);

volatile bool interrupt = false; // interrupt flag

void irqHandler() { interrupt = true; } // interrupt handler

void setup() {

  Serial.begin(9600);
  while (!Serial);
  delay(100); 

  // Error handling:
  mcp2515.reset();
  if(mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ) != MCP2515::ERROR_OK) { // mishandled bitrate set up
    
    Serial.println("Error: Something went wrong setting bitrate!");
  }

  // set up interrupts
  attachInterrupt(digitalPinToInterrupt(2), irqHandler, FALLING); // set up interrupt mode: falling edge interrupt

  // set extended frame masks
  mcp2515.setFilterMask(MCP2515::MASK0, true, 0);
  mcp2515.setFilterMask(MCP2515::MASK1, true, 0);

  // set filters for buffer registers
  mcp2515.setFilter(MCP2515::RXF0, true, 0);
  mcp2515.setFilter(MCP2515::RXF1, true, 0);
  mcp2515.setFilter(MCP2515::RXF2, true, 0);
  mcp2515.setFilter(MCP2515::RXF3, true, 0);
  mcp2515.setFilter(MCP2515::RXF4, true, 0);
  mcp2515.setFilter(MCP2515::RXF5, true, 0);

  if(mcp2515.setNormalMode() != MCP2515::ERROR_OK) { // mishandled mode set up

    Serial.println("Error: Something went wrong setting normal mode!");
  }

  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC  DATA");

  pinMode(13, OUTPUT);
}

void processFrame(struct can_frame &frame){
  // if for some reason a standard frame gets through ignore it
  if(!(frame.can_id & CAN_EFF_FLAG)) {return;}

  // get packet and controller IDs
  uint32_t raw_id = frame.can_id & CAN_EFF_MASK; // cannot use 
  uint8_t controller_id = raw_id & 0xFF;     // bits 0–7
  uint16_t packet_id    = raw_id >> 8;

  // print packet and controller IDs in HEX (make it human readable)

  //Serial.print("Packet ");
  //Serial.print(packet_id, HEX);
  //Serial.print(" From ID: ");
  //Serial.println(controller_id, HEX);

  Serial.print("[");
  Serial.print("2"); // send 2 to HUD app to represent the UC MOCO
  Serial.print("] ");
  Serial.print(packet_id, HEX);

  // print data from frame
  for(int i = 0; i < frame.can_dlc; i++){
          
    Serial.print(" ");
    Serial.print(frame.data[i], HEX);
     
  }

  // print new line
  Serial.print("\n");
    
}

void loop(){

  if (interrupt) {
    interrupt = false; // If an interrupt is triggered, reset the flag

    uint8_t irq = mcp2515.getInterrupts(); // get the current interrupt status

    if(irq & MCP2515::CANINTF_RX0IF) {

      if (mcp2515.readMessage(MCP2515::RXB0, &CAN_MSG) == MCP2515::ERROR_OK) {
        // CAN_MSG contains RXB0 buffer message

        processFrame(CAN_MSG);
      }
  
    }
    if(irq & MCP2515::CANINTF_RX1IF) {

      if(mcp2515.readMessage(MCP2515::RXB1, &CAN_MSG) == MCP2515::ERROR_OK) {
        // CAN_MSG contains RXB1 buffer message

        processFrame(CAN_MSG);
      }
    }
    //clear interrupt
    mcp2515.clearInterrupts();
  }

  // If all else fails this is polling request logic
  //if(mcp2515.readMessage(&CAN_MSG) == MCP2515::ERROR_OK) {
  //  Serial.print("ID: ");
  //  Serial.print(CAN_MSG.can_id, HEX);
  //  Serial.print("  DLC: ");
  //  Serial.print(CAN_MSG.can_dlc);
  //  Serial.print("  DATA: ");
  //  for(int i = 0; i < CAN_MSG.can_dlc; i++) {
  //    Serial.print(CAN_MSG.data[i], HEX);
  //    Serial.print(" ");
  //  }
  //  Serial.println();
  //}

}


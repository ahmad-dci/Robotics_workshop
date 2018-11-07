#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//
 
Servo myservo; 
 
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
RF24 radio(9,10);
 
//
// Topology
//
 
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
 
// Default Values
int servoPos = 92; // Steady point of the servo
 
void setup(void)
{
 
  //
  // Setup and configure rf radio
  //
 
  radio.begin();
 
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
 
  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(8);
 
  //
  // Open pipes to other nodes for communication
  //
 
  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
 
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
 
  //
  // Start listening
  //
 
  radio.startListening();
 
  //
  // Dump the configuration of the rf unit for debugging
  //
 
  // The servo is attached to pin 2
  myservo.attach(2);
  // Start with the steady point of the servo
  myservo.write(servoPos);
}
 
 
void receiveOrder()
{
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long message;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &message, sizeof(unsigned long) );
 
        // Use it
        // The servo can only ake a value fro 0 to 180
        if(message > 180) message = 180;
        else if(message < 0) message = 0;
        // Save the postion
        servoPos = message;
        // Write the position to the servo
        myservo.write(servoPos);
      }
 
      // First, stop listening so we can talk
      radio.stopListening();
 
      // Send the final one back.
      radio.write( &message, sizeof(unsigned long) );
 
      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
}
 
void loop(void)
{
  receiveOrder(); 
}
 

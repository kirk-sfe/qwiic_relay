/*
  This is a library for both the Qwiic Single and Qwiic Quad Relay. It gives basic
  functionality for turning on and off your relays and getting their statuses. 
  By: Elias Santistevan
  Date: July 2019 
  License: This code is public domain but you buy me a beer if you use this and 
  we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
 */

#include <stdio.h>
#include <string.h>
#include "qwiic_i2c.h"
#include "qwiic_relay.h"

Qwiic_I2C qwiic;

Qwiic_Relay::Qwiic_Relay(uint8_t address){

   _address = address;

}

bool Qwiic_Relay::begin( )
{
    // init I2C.

    if(!qwiic.init()){
        printf("Error: I2C subsystem failed to initialize.");
        return false;
    }
    return isConnected();

}

bool Qwiic_Relay::isConnected(void){

    return qwiic.pingDevice(_address);
}
//****----THE FOLLOWING FIVE FUNCTIONS ARE TO BE USED WITH THE SPARKFUN SINGLE RELAY-----****

// This function turns the single relay board on. 
void Qwiic_Relay::turnRelayOn()
{
  _writeCommandOn(TURN_RELAY_ON);
}

// This function turns the single relay board off. 
void Qwiic_Relay::turnRelayOff() 
{
  _writeCommandOff(TURN_RELAY_OFF);
}

// This function gives toggle functionality to the single relay. First the
// status of the relay is checked and is toggled according to what the status
// of the relay: on ---> off or off ----> on.
void Qwiic_Relay::toggleRelay()
{

  if (_readCommand(MYSTATUS) == SING_RELAY_ON)
    turnRelayOff();
  else  
    turnRelayOn();
}

// This function for the SparkFun Single Relay, gets the status of the relay:
// whether on: 1 or off: 0;
uint8_t Qwiic_Relay::getState()
{
  return _readCommand(MYSTATUS);

}

// This function gets the version number of the SparkFun Single Relay.
float Qwiic_Relay::singleRelayVersion()
{
  return _readVersion(FIRMWARE_VERSION);
}

//*****----THE FOLLOWING FUNCTIONS ARE TO BE USED WITH THE SPARKFUN QUAD RELAY------*****
 
// This function turns the given relay on.
void Qwiic_Relay::turnRelayOn(uint8_t relay)
{
  _writeCommandOn(relay);
}

// This function turns the given relay off.
void Qwiic_Relay::turnRelayOff(uint8_t relay)
{
  _writeCommandOff(relay);
}

// This function toggles the given relay. If the relay is on then it will turn
// it off, and if it is off then it will turn it on. 
void Qwiic_Relay::toggleRelay(uint8_t relay)
{
    // The RELAY_* and TOGGLE_RELAY_* enums are set to the same int values
    // so just make sure relay is in range, and pass on
    if( RELAY_ONE <= relay && relay <= RELAY_FOUR)
        qwiic.write(_address, relay);
}

// This function for the SparkFun Quad Relay, turns on all relays on the
// board. 
void Qwiic_Relay::turnAllRelaysOn()
{
  _writeCommandOn(TURN_ALL_ON);
}

// This function for the SparkFun Quad Relay, turns off all relays on the
// board. 
void Qwiic_Relay::turnAllRelaysOff()
{
  _writeCommandOff(TURN_ALL_OFF);
}

// This function for the SparkFun Quad Relay, turns off all relays on the
// board. 
void Qwiic_Relay::toggleAllRelays()
{
  _writeCommandOn(TOGGLE_ALL);
}

// This function for the SparkFun Quad Relay, gets the status of the relay:
// whether on: 1 or off: 0;
uint8_t Qwiic_Relay::getState(uint8_t relay)
{
    // The RELAY_* and TOGGLE_RELAY_* enums are increasing sequences, 
    // so just make sure relay is in range, and pass on 

    // NOTE: Using if for return value == QUAD_REALY_ON, return 1, else 0 

    if( RELAY_ONE <= relay && relay <= RELAY_FOUR)
       return _readCommand(relay + (RELAY_ONE_STATUS - RELAY_ONE)) == QUAD_RELAY_ON;

    // Param out of range
    return INCORR_PARAM;

}

// This function changes the I-squared-C address of the Qwiic RFID. The address
// is written to the memory location in EEPROM that determines its address.
bool Qwiic_Relay::changeAddress(uint8_t newAddress) 
{

    if (newAddress < 0x07 || newAddress > 0x78) // Range of legal addresses
        return false; 

    bool bStatus = qwiic.writeRegister(_address, ADDRESS_LOCATION, newAddress);

    if(bStatus)
        _address = newAddress;

    return bStatus;

}

// This function handles I-squared-C write commands for turning the relays on. 
// The quad relay relies on the current state of the relay to determine whether
// or not to turn the respective relay on (or off) and so the current state of
// the relay is checked before attempting to send a command. 
void Qwiic_Relay::_writeCommandOn(uint8_t _command)
{

    // note - RELAY_<N>_STATUS = _command + RELAY_ONE_STATUS - RELAY_ONE
    // check range of command 
    if( RELAY_ONE <= _command && _command <= RELAY_FOUR){
        if( _readCommand(_command + RELAY_ONE_STATUS - RELAY_ONE) != QUAD_RELAY_ON )
            qwiic.write(_address, _command + TOGGLE_RELAY_ONE - RELAY_ONE);
    }else // not a quad
        qwiic.write(_address, _command);
    
}

// This function handles I-squared-C write commands for turning the relays off. 
// The quad relay relies on the current state of the relay to determine whether
// or not to turn the respective relay off (or on) and so the current state of
// the relay is checked before attempting to toggle it.
void Qwiic_Relay::_writeCommandOff(uint8_t _command){

    // note - RELAY_<N>_STATUS = _command + RELAY_ONE_STATUS - RELAY_ONE
    // check range of command 
    if( RELAY_ONE <= _command && _command <= RELAY_FOUR){
        if( _readCommand(_command + RELAY_ONE_STATUS - RELAY_ONE) != QUAD_RELAY_OFF )
            qwiic.write(_address, _command + TOGGLE_RELAY_ONE - RELAY_ONE);
    }else // not a quad
        qwiic.write(_address, _command);
     
}

// This function requests information from the product with a simple
// I-squared-C transaction.
uint8_t Qwiic_Relay::_readCommand(uint8_t _command){
    return qwiic.readRegister(_address, _command);
}

// The function reads thee version number of the Single Quad Relay.
float Qwiic_Relay::_readVersion(uint8_t _command)
{
    uint16_t result;
    int rc = qwiic.readRegisterInt16(_address, _command, &result);

    if(rc == QWIIC_ERROR_GENERIC)
        return 0;

    return (float)(result & 0xFF) + (float)(result >> 8)/10.0;

}


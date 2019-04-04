#include "SSIradio.h"

uint8_t num_radios_initialized = 0;
SSIradio* radios[NUM_RADIOS];

//SSIradio:
//Constructs a new S6C/radio object
SSIradio::SSIradio(){}

//begin
//actually initializes a new radio on the specified serial port with the specified baud rate
void SSIradio::begin(uint16_t baud, HardwareSerial *serial){
  serial_port = serial;
  serial_port->begin(baud);

  swID = num_radios_initialized; // assign radio software ID in order it was initialized
  radios[swID] = this; // load reference to this radio into radios array
  num_radios_initialized++;
  min_init_context(&min_ctx, swID); // use swID as min port number

  //TODO get hardware ID from radio
}

//set_min_id
//sets message id in min
void SSIradio::set_min_id(uint8_t new_min_id){
  cur_min_id = new_min_id;
}

//rx
//checks for incoming bytes and processes them if available
uint8_t SSIradio::rx(){

  if (serial_port->available() > 0) {
    buf_len = serial_port->readBytes(buf, buf_size);
  } else {
    buf_len = 0;
  }
  min_poll(&min_ctx, (uint8_t *)buf, (uint8_t)buf_len);

  return buf_len;
}

//tx
//assumes message type SEND_MESSAGE, properly formed string input
void SSIradio::tx(const char* buffer_in){
  tx(MESSAGE_SEND, buffer_in, strlen(buffer_in));
}

//tx
//assumes message type SEND_MESSAGE, arbitrary input with specified length
void SSIradio::tx(const char* buffer_in, size_t length){
  tx(MESSAGE_SEND, buffer_in, length);
}

//tx
//generalized transmit function, allows you to set message message type
//transmits message, subject to buffer limits
//invoked by tx()
void SSIradio::tx(char msgtype, const char* buffer_in, size_t length){

  size_t cpylen = std::min(length, MAX_PAYLOAD_SIZE);

  for(uint8_t i = 0; i < cpylen; i++){
    buf[i+2] = buffer_in[i];
  }

  buf[0] = msgtype; // first char is message type
  buf[1] = cpylen + 1; // length of payload, including null terminator
  buf[cpylen + 2] = '\0'; // place null terminator at end of string

  min_send_frame(&min_ctx, cur_min_id++, (uint8_t *)buf, cpylen + 3); // +3 for msgtype, length, and terminator characters
}

void SSIradio::set_callback(std::function<void(char*)> callback_fn){
  callback = callback_fn;
}

// buf[0] = 0;
// buf[1] = 5;
// buf[2] = 'H';
// buf[3] = 'E';
// buf[4] = 'L';
// buf[5] = 'L';
// buf[6] = '\0';

// legitimate min functions

void min_application_handler(uint8_t min_id, uint8_t *min_payload,
    uint8_t len_payload, uint8_t port)
{

  if (len_payload == 0) {
			return;
	}

  // increment min_id to be sequential
  radios[port]->set_min_id(min_id + 1);

  // run callback function if set
  if(radios[port]->callback){
    radios[port]->callback((char *) min_payload);
  }

  // send acknowledgement message
  /*
  size_t ack_size = strlen(ack_message);
  char ack_buf[ack_size + 2];
  ack_buf[0] = 0;
  ack_buf[1] = strlen(ack_message);
  strncpy(ack_buf + 2, ack_message, ack_size);
  min_send_frame(&min_ctx, cur_min_id++, (uint8_t *)ack_buf, (uint8_t)ack_size + 2);
  */

  // Serial.println("Aw hell yeah");

  // Serial.write((char *) min_payload, len_payload);
  // Serial.println(*min_payload);
  // if (((char *)min_payload)[1] == 'X') {
  //   Serial.println("BOOM BOOM ACKA-LACKA BOOM BOOM");
  // }
  // if (((char *)min_payload)[1] == 'Z') {
  //   Serial.println("NO BOOM NO BOOM");
  // }
}

void min_tx_byte(uint8_t port, uint8_t byte)
{
  // // TODO fixme
  // S6C.write(&byte, 1U);
  // Serial.write(&byte, 1U);

  radios[port]->serial_port->write(&byte, 1U); // sends outgoing byte to radio
}

uint16_t min_tx_space(uint8_t port)
{
  return radios[port]->serial_port->availableForWrite();
}

// placeholder min functions

void min_tx_start(uint8_t port) {}

void min_tx_finished(uint8_t port) {}

uint32_t min_time_ms(void)
{
  return millis();
}



    /*
    // Send squib resistances on port 1
    snprintf(buf + header_size, buf_size, "R1A: %d/R1B: %d", stat->Squib_Stat1AResistance,
            stat->Squib_Stat1BResistance);
    buf_len = strlen(buf + header_size) + 1;
    buf[0] = 0;
    buf[1] = buf_len + header_size;
    min_send_frame(&min_ctx, cur_min_id++, (uint8_t *)buf, (uint8_t)buf_len + header_size);

    // Send squib resistances on port 2
    snprintf(buf + header_size, buf_size, "R2A: %d/R2B: %d", stat->Squib_Stat2AResistance,
            stat->Squib_Stat2BResistance);
    buf_len = strlen(buf + header_size) + 1;
    buf[0] = 0;
    buf[1] = buf_len + header_size;
    min_send_frame(&min_ctx, cur_min_id++, (uint8_t *)buf, (uint8_t)buf_len + header_size);
    */

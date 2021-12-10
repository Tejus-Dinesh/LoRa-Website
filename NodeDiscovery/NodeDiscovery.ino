#include <LoRa.h>
#define SS 15
#define RST 16
#define DIO0 2
extern "C" {
  #include <user_interface.h>
}

#define DATA_LENGTH           112
#define DISABLE 0
#define ENABLE  1
#define TYPE_MANAGEMENT       0x00
#define TYPE_CONTROL          0x01
#define TYPE_DATA             0x02addr
#define SUBTYPE_PROBE_REQUEST 0x04

struct RxControl {
 signed rssi:8; // signal intensity of packet
 unsigned rate:4;
 unsigned is_group:1;
 unsigned:1;
 unsigned sig_mode:2; // 0:is 11n packet; 1:is not 11n packet;
 unsigned legacy_length:12; // if not 11n packet, shows length of packet.
 unsigned damatch0:1;
 unsigned damatch1:1;
 unsigned bssidmatch0:1;
 unsigned bssidmatch1:1;
 unsigned MCS:7; // if is 11n packet, shows the modulation and code used (range from 0 to 76)
 unsigned CWB:1; // if is 11n packet, shows if is HT40 packet or not
 unsigned HT_length:16;// if is 11n packet, shows length of packet.
 unsigned Smoothing:1;
 unsigned Not_Sounding:1;
 unsigned:1;
 unsigned Aggregation:1;
 unsigned STBC:2;
 unsigned FEC_CODING:1; // if is 11n packet, shows if is LDPC packet or not.
 unsigned SGI:1;
 unsigned rxend_state:8;
 unsigned ampdu_cnt:8;
 unsigned channel:4; //which channel this packet in.
 unsigned:12;
};
char addr[] = "00:00:00:00:00:00";

struct Packet{
    struct RxControl rx_ctrl;
    uint8_t data[DATA_LENGTH];
    uint16_t cnt;
    uint16_t len;
};


static void printData(Packet *sniffedPacket);
static void ICACHE_FLASH_ATTR callback(uint8_t *buffer, uint16_t length);

static void getMAC(char *addr, uint8_t* data, uint16_t offset);
void channelSwitch();


static void printData(Packet *sniffedPacket) {

  unsigned int frameControl = ((unsigned int)sniffedPacket->data[1] << 8) + sniffedPacket->data[0];

  uint8_t version      = (frameControl & 0b0000000000000011) >> 0;
  uint8_t frameType    = (frameControl & 0b0000000000001100) >> 2;
  uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;
  uint8_t toDS         = (frameControl & 0b0000000100000000) >> 8;
  uint8_t fromDS       = (frameControl & 0b0000001000000000) >> 9;

  // Only look for probe request packets
  if (frameType != TYPE_MANAGEMENT ||
      frameSubType != SUBTYPE_PROBE_REQUEST)
        return;
  getMAC(addr, sniffedPacket->data, 10);
  Serial.println(addr);
  
}

/**
 * Callback for promiscuous mode
 */
static void ICACHE_FLASH_ATTR callback(uint8_t *buffer, uint16_t length) {
  struct Packet *sniffedPacket = (struct Packet*) buffer;
  printData(sniffedPacket);
}
  


static void getMAC(char *addr, uint8_t* data, uint16_t offset) {
  sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset+0], data[offset+1], data[offset+2], data[offset+3], data[offset+4], data[offset+5]);
}

#define CHANNEL_HOP_INTERVAL_MS   100
static os_timer_t channelHop_timer;

/**
 * Function for channel switching
 */
void channelSwitch()
{
  
  uint8 new_channel = wifi_get_channel() + 1;
  if (new_channel > 13) {
    new_channel = 1;
  }
  wifi_set_channel(new_channel);
}



void setup() {
  // set the WiFi chip to "promiscuous" mode aka monitor mode
  Serial.begin(9600);
  LoRa.setPins(SS, RST, DIO0);
  //LoRa.setSpreadingFactor(12);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    delay(100);
    while (1);
  }
  delay(10);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(1);
  wifi_promiscuous_enable(DISABLE);
  delay(10);
  wifi_set_promiscuous_rx_cb(callback);
  delay(10);
  wifi_promiscuous_enable(ENABLE);

  // setup the channel hoping callback timer
  os_timer_disarm(&channelHop_timer);
  os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelSwitch, NULL);
  os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL_MS, 1);
  

  }


void loop() {
  String xyz  = 1233+ addr;
  LoRa.beginPacket();
  LoRa.print(addr);
  LoRa.endPacket();

  delay(2000);
  delay(10);
}

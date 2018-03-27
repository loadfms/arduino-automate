#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

const char website[] PROGMEM = "ec2-34-228-83-104.compute-1.amazonaws.com";
static uint32_t timer;
static int current_port_index = 0;
static int max_relay_ports = 2;
static int relay_ports[2] = {4,5};
static bool relay_status[2] = {false,false};

byte Ethernet::buffer[700];

//Start Up
void setup () {
  Serial.begin(57600);
  Serial.println("started");

  pinMode(relay_ports[0], OUTPUT);
  Serial.print("Relay 1 on port: " + String(relay_ports[0]));

  pinMode(relay_ports[1], OUTPUT);
  Serial.print("Relay 2 on port: " + String(relay_ports[1]));

  digitalWrite(relay_ports[0], HIGH);
  digitalWrite(relay_ports[1], HIGH);

  ether.hisport = 8080;

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println(F("DNS failed"));

  ether.printIp("SRV: ", ether.hisip);
}

//Loop
void loop () {
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer){
    checkStatus();
    ether.persistTcpConnection(true);
  }
}

//Request status
static void checkStatus () {
      timer = millis() + 3000;
      Serial.println("<<< REQ port:" + String(relay_ports[current_port_index]));
      char portInString[7]; 
      itoa(relay_ports[current_port_index], portInString, 10);
      ether.browseUrl(PSTR("/api/devices/"), portInString, website, ethernet_callback);
  }
  
//Callback and execute function
static void ethernet_callback(byte status, word off, word len) {
  char * message;
  char * found;

  Serial.println(">>> RES port:" + String(relay_ports[current_port_index]));
  
  Ethernet::buffer[off+len] = 0;
  message = (char*) Ethernet::buffer + off;
  found = strstr(message, "turnon");
  
  if (found != NULL){
    Serial.println("Relay on - port" + String(relay_ports[current_port_index]));
    if (!relay_status[current_port_index])
    {
      relay_status[current_port_index] = true;
      digitalWrite(relay_ports[current_port_index], LOW);
    }
  }
  else{
    Serial.println("Relay off - port" + String(relay_ports[current_port_index]));
    if (relay_status[current_port_index])
    {
      relay_status[current_port_index] = false;
      digitalWrite(relay_ports[current_port_index], HIGH);
    }
  }

  if(current_port_index < max_relay_ports - 1)
  {
    current_port_index++;
  } else if (current_port_index >= max_relay_ports -1)
  {
    current_port_index = 0;
  }
}

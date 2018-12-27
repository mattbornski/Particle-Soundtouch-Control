#include "ssdp.h"
#include "soundtouch.h"
#include "speaker.h"

const int MAX_SSDP_PROBES = 32;

const uint32_t SSDP_TX_TTL = 60000;

const int SOUNDTOUCH_BROADCAST_TX_PORT = 1900;
const int SOUNDTOUCH_BROADCAST_RX_PORT = 1900;
const IPAddress SOUNDTOUCH_BROADCAST_IP_ADDRESS = IPAddress( 239, 255, 255, 250 );

const char * SOUNDTOUCH_DISCOVERY_PACKET = F("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 5\r\nST: urn:schemas-upnp-org:device:MediaRenderer:1\r\n\r\n");
const char * SOUNDTOUCH_USN_PREFIX = F("USN:uuid:BO5EBO5E-F00D-F00D-FEED-");
const char * SOUNDTOUCH_XML_DETAILS_LOCATION_PREFIX = F("Location: ");
const char * SOUNDTOUCH_HTTP_TOKEN = F("http");
const char * SOUNDTOUCH_DOT_XML_TOKEN = F(".xml");
const char * SOUNDTOUCH_COLON_TOKEN = F(":");
const char * SOUNDTOUCH_COLON_SLASH_SLASH_TOKEN = F("://");
const char * SOUNDTOUCH_SLASH_TOKEN = F("/");

SSDPClient::SSDPClient(SoundtouchClient *soundtouchClient) {
    this->soundtouchClient = soundtouchClient;

    this->broadcastUdpClient = NULL;
}

SSDPClient::~SSDPClient() {
    this->soundtouchClient = NULL;
    this->dealloc();
}

void SSDPClient::dealloc() {
    this->cleanupDiscovery();
}

void SSDPClient::cleanupDiscovery() {
    if (this->broadcastUdpClient != NULL) {
        this->broadcastUdpClient->stop();
    }
    delete this->broadcastUdpClient;
    this->broadcastUdpClient = NULL;
}

void SSDPClient::report(String speakerAddress) {
    if (this->soundtouchClient != NULL) {
        Speaker probedSpeaker = Speaker(speakerAddress);
        if (probedSpeaker.online) {
            this->soundtouchClient->addSpeaker(probedSpeaker);
        }
    }
}

// This should be invoked repeatedly while in discovery mode
void SSDPClient::discover() {
    if (millis() - this->lastSsdpPacketTx >= SSDP_TX_TTL) {
        this->cleanupDiscovery();
    }

    if (this->broadcastUdpClient == NULL) {
        #ifdef SOUNDTOUCH_LOGGING
            Serial.println("Initiating SSDP discovery");
        #endif

        this->broadcastUdpClient = new UDP();
        this->broadcastUdpClient->begin(SOUNDTOUCH_BROADCAST_RX_PORT);
        this->broadcastUdpClient->beginPacket(SOUNDTOUCH_BROADCAST_IP_ADDRESS, SOUNDTOUCH_BROADCAST_TX_PORT);
        this->broadcastUdpClient->write(SOUNDTOUCH_DISCOVERY_PACKET);
        this->broadcastUdpClient->endPacket();
        this->broadcastUdpClient->flush();
        this->lastSsdpPacketTx = millis();

        return;
    }

    if (this->broadcastUdpClient->parsePacket() > 0) {
        String response = "";
        while (this->broadcastUdpClient->available()) {
            char c = this->broadcastUdpClient->read();
            response += c;
        }

        if (response.indexOf(SOUNDTOUCH_USN_PREFIX) != -1) {
            int locationPrefixStartIndex = response.indexOf(SOUNDTOUCH_XML_DETAILS_LOCATION_PREFIX);
            if (locationPrefixStartIndex != -1) {
                int locationUrlStartIndex = response.indexOf(SOUNDTOUCH_HTTP_TOKEN, locationPrefixStartIndex);
                if (locationUrlStartIndex != -1) {
                    int locationUrlSuffixIndex = response.indexOf(SOUNDTOUCH_DOT_XML_TOKEN, locationUrlStartIndex);
                    String location = response.substring(locationUrlStartIndex, locationUrlSuffixIndex + 4);

                    // Extract the IP address from the SSDP location info.
                    int ipAddressStartIndex = location.indexOf(SOUNDTOUCH_COLON_SLASH_SLASH_TOKEN) + 3;
                    int ipAddressStopIndex = location.indexOf(SOUNDTOUCH_COLON_TOKEN, ipAddressStartIndex);
                    if (ipAddressStopIndex == -1) {
                        ipAddressStopIndex = location.indexOf(SOUNDTOUCH_SLASH_TOKEN, ipAddressStartIndex);
                    }
                    String ipAddress = location.substring(ipAddressStartIndex, ipAddressStopIndex);
                    this->report(ipAddress);
                }
            }
        }
    }
}

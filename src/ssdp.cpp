#include "ssdp.h"
#include "soundtouch.h"
#include "speaker.h"

const int MAX_SSDP_PROBES = 32;

const char * soundTouchDiscoveryPacket = "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 5\r\nST: urn:schemas-upnp-org:device:MediaRenderer:1\r\n\r\n";
const char * soundTouchUSNPrefix = "USN:uuid:BO5EBO5E-F00D-F00D-FEED-";
const char * xmlDetailsLocationPrefix = "Location: ";

SSDPClient::SSDPClient(Soundtouch *soundtouch) {
    this->soundtouch = soundtouch;
}

void SSDPClient::begin() {
    broadcastPort = 1900;
    broadcastIP = IPAddress( 239, 255, 255, 250 );
    listenPort = 1900;
}

void SSDPClient::discover() {
    UDP broadcastUdpClient;
    broadcastUdpClient.begin(listenPort);
    broadcastUdpClient.beginPacket(broadcastIP, broadcastPort);
    broadcastUdpClient.write(soundTouchDiscoveryPacket);
    broadcastUdpClient.endPacket();
    broadcastUdpClient.flush();

    String ipAddresses[MAX_SSDP_PROBES];
    int ipAddressesFound = 0;
    uint32_t msDelay = millis();
    Serial.print("  ");
    while ((millis() - msDelay) < 5000) {
        Serial.print(".");
        delay(100);
        if (broadcastUdpClient.parsePacket() > 0) {
            String response = "";
            while (broadcastUdpClient.available()) {
                char c = broadcastUdpClient.read();
                response += c;
            }

            if (response.indexOf(soundTouchUSNPrefix) != -1) {
                int locationPrefixStartIndex = response.indexOf(xmlDetailsLocationPrefix);
                if (locationPrefixStartIndex != -1) {
                    int locationUrlStartIndex = response.indexOf("http", locationPrefixStartIndex);
                    if (locationUrlStartIndex != -1) {
                        int locationUrlSuffixIndex = response.indexOf(".xml", locationUrlStartIndex);
                        String location = response.substring(locationUrlStartIndex, locationUrlSuffixIndex + 4);

                        // Extract the IP address from the SSDP location info.
                        int ipAddressStartIndex = location.indexOf("://") + 3;
                        int ipAddressStopIndex = location.indexOf(":", ipAddressStartIndex);
                        if (ipAddressStopIndex == -1) {
                            ipAddressStopIndex = location.indexOf("/", ipAddressStartIndex);
                        }
                        String ipAddress = location.substring(ipAddressStartIndex, ipAddressStopIndex);
                        ipAddresses[ipAddressesFound++] = ipAddress;
                    }
                }
            }
            // break;
        }
    }
    broadcastUdpClient.stop();
    Serial.println();

    for (int index = 0; index < ipAddressesFound; index++) {
        String ipAddress = ipAddresses[index];
        Serial.print("  - Probe ");
        Serial.println(ipAddress);
        Speaker *probedSpeaker = new Speaker(ipAddress);
        if (probedSpeaker->probe()) {
            soundtouch->addSpeaker(probedSpeaker);
        } else {
            delete probedSpeaker;
        }
        delete ipAddress;
    }
}

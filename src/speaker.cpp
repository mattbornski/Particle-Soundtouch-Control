#include "speaker.h"

const char *keySender = "Gabbo";
const char *keyTemplate = "<key state=\"%s\" sender=\"%s\">%s</key>";

const char *deviceIdStartMarker = "<info deviceID=\"";
const char *deviceIdStopMarker = "\"";

const char *nameStartMarker = "<name>";
const char *nameStopMarker = "<";

const char *sourceStartMarker = "<ContentItem source=\"";
const char *sourceStopMarker = "\"";

const char *playStatusStartMarker = "<playStatus>";
const char *playStatusStopMarker = "</playStatus>";

const char *httpStatusCodeStartMarker = "HTTP/1.1 ";
const char *httpStatusCodeStopMarker = " ";

String locate(String haystack, const char *startMarker, const char *stopMarker) {
    int startIndex = haystack.indexOf(startMarker) + strlen(startMarker);
    int stopIndex = haystack.indexOf(stopMarker, startIndex);
    return haystack.substring(startIndex, stopIndex);
}

bool responseRepresentsSuccess(String response) {
    String responseCodeText = locate(response, httpStatusCodeStartMarker, httpStatusCodeStopMarker);
    int responseCode = atoi(responseCodeText);
    return (responseCode >= 200 && responseCode < 300);
}

Speaker::Speaker(String ipAddressString) {
    uint8_t ipAddressBytes[4];
    int decimalIndex = 0;
    for (int i = 0; i < 4; i++) {
        int nextDecimalIndex = ipAddressString.indexOf('.', decimalIndex);
        if (nextDecimalIndex == -1) {
            nextDecimalIndex = ipAddressString.length();
        }
        ipAddressBytes[i] = (uint32_t)atoi(ipAddressString.substring(decimalIndex, nextDecimalIndex));
        decimalIndex = nextDecimalIndex + 1;
    }

    this->ipAddress = IPAddress(ipAddressBytes);
}

Speaker::Speaker(uint32_t ipAddressNumber) {
    this->ipAddress = IPAddress(ipAddressNumber);
}

bool Speaker::probe() {
    String info = this->get("/info");
    if (info.length() == 0) {
        Serial.println("    - No response");
        return false;
    }

    this->deviceId = locate(info, deviceIdStartMarker, deviceIdStopMarker);
    if (this->deviceId.length() != 12) {
        Serial.println("    - No device id");
        return false;
    }

    this->friendlyName = locate(info, nameStartMarker, nameStopMarker);
    if (this->friendlyName.length() == 0) {
        Serial.println("    - No name");
        return false;
    }

    Serial.println("    - Found " + this->deviceId + " ('" + this->friendlyName + "')");
    return true;
}

bool Speaker::refresh() {
    String nowPlaying = this->get("/now_playing");
    if (nowPlaying.length() == 0) {
        return false;
    }

    String source = locate(nowPlaying, sourceStartMarker, sourceStopMarker);
    this->lastKnownIsActive = (source.compareTo("STANDBY") != 0);

    String playing = locate(nowPlaying, playStatusStartMarker, playStatusStopMarker);
    this->lastKnownIsPlaying = (playing.compareTo("PLAY") == 0);

    return true;
}

String Speaker::request(const char *method, const char *path, String body) {
    TCPClient client;
    String response;
    if (client.connect(this->ipAddress, 8090)) {
        // Serial.println(String::format("%s %d.%d.%d.%d:8090%s", method, this->ipAddress[0], this->ipAddress[1], this->ipAddress[2], this->ipAddress[3], path));
        client.println(String::format("%s %s HTTP/1.1", method, path));
        client.println(String::format("Host: %d.%d.%d.%d:8090", this->ipAddress[0], this->ipAddress[1], this->ipAddress[2], this->ipAddress[3]));
        if (body.length() > 0) {
            // Serial.println(body);
            client.println("Content-Type: application/xml");
            client.println(String::format("Content-Length: %d", body.length()));
            client.println();
            client.print(body);
        }
        client.println();

        uint32_t msWait = millis();
        while (!client.available()) {
            delay(25);

            if ((millis() - msWait) > 10000) {
                break;
            }
        }
        while (client.available()) {
            char c = client.read();
            response += c;
        }
        // Serial.println(response);
    } else {
        Serial.println("    - Connection failed");
    }

    client.stop();
    return response;
}

String Speaker::get(const char *path) {
    return this->request("GET", path, "");
}

String Speaker::post(const char *path, String body) {
    return this->request("POST", path, body);
}

bool Speaker::key(const char *keyName) {
    String press = String::format(keyTemplate, "press", keySender, keyName);
    if (responseRepresentsSuccess(this->post("/key", press))) {
        String release = String::format(keyTemplate, "release", keySender, keyName);
        return responseRepresentsSuccess(this->post("/key", release));
    }
    return false;
}

void Speaker::play() {
    Serial.println("  - Play");
    if (!this->lastKnownIsActive) {
        if (this->key("POWER")) {
            this->lastKnownIsActive = true;
        }
    }
    if (this->key("PLAY")) {
        this->lastKnownIsPlaying = true;
    }
}

void Speaker::pause() {
    Serial.println("  - Pause");
    if (this->key("PAUSE")) {
        this->lastKnownIsPlaying = false;
    }
}

void Speaker::setVolume() {

}

void Speaker::changeVolume(int delta) {

}

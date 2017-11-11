#include "speaker.h"

const char *keySender = "Gabbo";
const char *keyTemplate = "<key state=\"%s\" sender=\"%s\">%s</key>";

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
    probe();
}

bool Speaker::probe() {
    String info = this->get("/info");
    if (info.length() == 0) {
        return false;
    }

    int deviceIdStart = info.indexOf("<info deviceID=\"") + strlen("<info deviceID=\"");
    int deviceIdStop = info.indexOf("\"", deviceIdStart);
    this->deviceId = info.substring(deviceIdStart, deviceIdStop);

    if (this->deviceId.length() != 12) {
        return false;
    }

    int nameStart = info.indexOf("<name>") + strlen("<name>");
    int nameStop = info.indexOf("<", nameStart);
    this->friendlyName = info.substring(nameStart, nameStop);

    if (this->friendlyName.length() == 0) {
        return false;
    }

    return true;
}

bool Speaker::refresh() {
    String nowPlaying = this->get("/now_playing");
    if (nowPlaying.length() == 0) {
        return false;
    }

    int playStatusStart = nowPlaying.indexOf("<playStatus>") + strlen("<playStatus>");
    int playStatusStop = nowPlaying.indexOf("</playStatus>", playStatusStart);
    String playing = nowPlaying.substring(playStatusStart, playStatusStop);

    this->lastKnownIsPlaying = (playing.compareTo("PLAY") == 0);
}

String Speaker::request(const char *method, const char *path, String body) {
    TCPClient client;
    String response;
    if (client.connect(this->ipAddress, 8090)) {
        Serial.println(String::format("%s %d.%d.%d.%d:8090%s", method, this->ipAddress[0], this->ipAddress[1], this->ipAddress[2], this->ipAddress[3], path));
        client.println(String::format("%s %s HTTP/1.1", method, path));
        client.println(String::format("Host: %d.%d.%d.%d:8090", this->ipAddress[0], this->ipAddress[1], this->ipAddress[2], this->ipAddress[3]));
        if (body.length() > 0) {
            Serial.println(body);
            client.println("Content-Type: application/xml");
            client.println(String::format("Content-Length: %d", body.length()));
            client.println();
            client.print(body);
        }
        client.println();

        uint32_t msWait = millis();
        while (!client.available()) {
            delay(25);

            if ((millis() - msWait) < 10000) {
                break;
            }
        }
        while (client.available()) {
            char c = client.read();
            response += c;
        }
        Serial.println(response);
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

void Speaker::key(const char *keyName) {
    String press = String::format(keyTemplate, "press", keySender, keyName);
    this->post("/key", press);
    String release = String::format(keyTemplate, "release", keySender, keyName);
    this->post("/key", release);
}

void Speaker::play() {
    this->key("PLAY");
}

void Speaker::pause() {
    this->key("PAUSE");
}

void Speaker::setVolume() {

}

void Speaker::changeVolume(int delta) {

}

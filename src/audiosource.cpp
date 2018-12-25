#include "audiosource.h"

const char *createZonePath = "/setZone";
const char *addZoneMembersPath = "/addZoneSlave";
const char *removeZoneMembersPath = "/removeZoneSlave";

const char *zoneTemplate = "<zone master=\"%s\" senderIPAddress=\"%s\">%s</zone>";
const char *zoneMemberTemplate = "<member ipaddress=\"%s\">%s</member>";

AudioSource::AudioSource() {
    this->masterSpeaker = NULL;
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        this->memberSpeakers[index] = NULL;
    }
}

bool AudioSource::join(Speaker *newMemberSpeaker) {
    if (newMemberSpeaker == NULL) {
        return false;
    }

    bool alreadyMember = false;
    for (int index = 0; index < MAX_SPEAKERS; index++) {
        if (this->memberSpeakers[index] != NULL) {
            if (this->memberSpeakers[index]->ipAddress == newMemberSpeaker->ipAddress) {
                alreadyMember = true;
                break;
            }
        }
    }
    if (alreadyMember) {
        return false;
    }

    String members = String::format(zoneMemberTemplate, newMemberSpeaker->ipAddress);
    String payload = String::format(zoneTemplate, this->masterSpeaker->deviceId, newMemberSpeaker->ipAddress, members);
}

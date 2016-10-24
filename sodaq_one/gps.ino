#include <Arduino.h>
#include <Sodaq_UBlox_GPS.h>

void gpsSetup() { sodaq_gps.init(GPS_ENABLE); }

/*!
 * Find a GPS fix, but first wait a while
 */
String gpsGetPostion(long waittime)
{
    String res;
    uint32_t timeout = waittime * 1000;

    debugSerial.println(String("waiting for fix ..., timeout=") + timeout + String("ms"));

    uint32_t start = millis();

    if (sodaq_gps.scan(false, timeout)) {
        debugSerial.println(String(" time to find fix: ") + (millis() - start) + String("ms"));
        debugSerial.println(String(" datetime = ") + sodaq_gps.getDateTimeString());

        res = String("lat=") + String(sodaq_gps.getLat(), 7) + " " + String("lon=") + String(sodaq_gps.getLon(), 7) + " " + String("sats=") + String(sodaq_gps.getNumberOfSatellites());

        debugSerial.println(res);
    } else {
        debugSerial.println("GPS: No Fix");
        return "";
    }
    return res;
}

String gpsGetDateTime(long waittime) {
    String res = "";
    uint32_t timeout = waittime * 1000;
    if (sodaq_gps.scan(false, timeout)) {
        res= String("datetime =") + sodaq_gps.getDateTimeString();
        debugSerial.println(res);
    }

    return res;
}

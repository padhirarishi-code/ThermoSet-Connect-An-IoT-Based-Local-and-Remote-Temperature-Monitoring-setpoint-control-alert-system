#ifndef _ESP01_H_
#define _ESP01_H_

/* ---- WiFi network credentials ---- */
/* TODO: replace with your own WiFi SSID and password */
#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

/* ---- ThingSpeak: main data channel (device WRITES here) ---- */
/* TODO: replace with your own ThingSpeak Write API Key            */
/* Field1 - Temperature (periodic upload)                          */
/* Field2 - Alert temperature (uploaded when temp > set point)     */
/* Field3 - Current set point (uploaded whenever it changes)       */
#define TS_WRITE_API_KEY "YOUR_TS_WRITE_API_KEY"

/* ---- ThingSpeak: set-point entry channel (device READS from here) ---- */
/* A SEPARATE channel dedicated to remote set-point entry, as required   */
/* by the project spec. The user updates Field1 of this channel (via the */
/* ThingSpeak app/API); the device polls it periodically and, if the     */
/* value differs from the one stored in EEPROM, adopts and stores it.    */
/* TODO: replace with your own set-point channel ID and Read API Key     */
#define SP_CHANNEL_ID    0
#define SP_READ_API_KEY  "YOUR_TS_READ_API_KEY"
#define SP_FIELD         1

/* Function declarations for ESP01 WiFi module */

/* Check AT command communication */
int esp01_connectAP_AT(void);

/* Disable command echo */
int esp01_connectAP_ATE0(void);

/* Configure TCP single connection mode */
int esp01_connectAP_TCP_MODE(void);

/* Disconnect from access point */
int esp01_connectAP_QUIT_AP(void);

/* Connect ESP01 to WiFi access point */
int esp01_connectAP_JOIN_AP(void);

/* Send data to ThingSpeak cloud (main data channel) */
int esp01_sendToThingspeak(int field, int num);

/* Read the latest value of a field from a ThingSpeak channel
   (used to fetch the remotely-entered set point)               */
int esp01_readThingspeakField(unsigned int channel_id, int field, char *read_key);

/* Upload sensor / status data (LCD status + retry wrapper) */
void update_data(int field, int num);

/* Poll the set-point entry channel and sync EEPROM/set_point if changed.
   Returns 1 if the set point was updated, 0 if unchanged, -1 on failure. */
int sync_cloud_setpoint(unsigned int *set_point);

/* Initialize ESP01 module (AT handshake + join WiFi) */
void init_esp01(void);

#endif

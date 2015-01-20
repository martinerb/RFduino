#ifndef GSM_H
#define GSM_H

#define MEGA
#include "HWSerial.h"
//#include <inttypes.h>
//#include "WideTextFinder.h"


#define ctrlz 26 //Ascii character for ctr+z. End of a SMS.
#define cr    13 //Ascii character for carriage return.
#define lf    10 //Ascii character for line feed.
#define ctrlz 26 //Ascii character for ctr+z. End of a SMS.
#define cr    13 //Ascii character for carriage return.
#define lf    10 //Ascii character for line feed.
#define GSM_LIB_VERSION 308 // library version X.YY (e.g. 1.00)

#include "HWSerial.h"

// pins definition
#define GSM_ON              8 // connect GSM Module turn ON to pin 77
#define GSM_RESET           9 // connect GSM Module RESET to pin 35
//#define DTMF_OUTPUT_ENABLE  71 // connect DTMF Output Enable not used
#define DTMF_DATA_VALID     14 // connect DTMF Data Valid to pin 14
#define DTMF_DATA0          72 // connect DTMF Data0 to pin 72
#define DTMF_DATA1          73 // connect DTMF Data1 to pin 73
#define DTMF_DATA2          74 // connect DTMF Data2 to pin 74
#define DTMF_DATA3          75 // connect DTMF Data3 to pin 75

// length for the internal communication buffer
#define COMM_BUF_LEN        200

// some constants for the IsRxFinished() method
#define RX_NOT_STARTED      0
#define RX_ALREADY_STARTED  1

// some constants for the InitParam() method
#define PARAM_SET_0   0
#define PARAM_SET_1   1

// DTMF signal is NOT valid

// status bits definition
#define STATUS_NONE                 0
#define STATUS_INITIALIZED          1
#define STATUS_REGISTERED           2
#define STATUS_USER_BUTTON_ENABLE   4

// GPRS status
#define CHECK_AND_OPEN    0
#define CLOSE_AND_REOPEN  1

// Common string used
#define str_ok 		"OK"			//string to reduce stack usage
#define str_at		"AT"			//string to reduce stack usage

// SMS type
// use by method IsSMSPresent()
enum sms_type_enum {
     SMS_UNREAD,
     SMS_READ,
     SMS_ALL,

     SMS_LAST_ITEM
};

enum comm_line_status_enum {
     // CLS like CommunicationLineStatus
     CLS_FREE,   // line is free - not used by the communication and can be used
     CLS_ATCMD,  // line is used by AT commands, includes also time for response
     CLS_DATA,   // for the future - line is used in the CSD or GPRS communication
     CLS_LAST_ITEM
};

enum rx_state_enum {
     RX_NOT_FINISHED = 0,      // not finished yet
     RX_FINISHED,              // finished, some character was received
     RX_FINISHED_STR_RECV,     // finished and expected string received
     RX_FINISHED_STR_NOT_RECV, // finished, but expected string not received
     RX_TMOUT_ERR,             // finished, no character received
     // initial communication tmout occurred
     RX_LAST_ITEM
};


enum at_resp_enum {
     AT_RESP_ERR_NO_RESP = -1,   // nothing received
     AT_RESP_ERR_DIF_RESP = 0,   // response_string is different from the response
     AT_RESP_OK = 1,             // response_string was included in the response

     AT_RESP_LAST_ITEM
};

enum registration_ret_val_enum {
     REG_NOT_REGISTERED = 0,
     REG_REGISTERED,
     REG_NO_RESPONSE,
     REG_COMM_LINE_BUSY,

     REG_LAST_ITEM
};

enum call_ret_val_enum {
     CALL_NONE = 0,
     CALL_INCOM_VOICE,
     CALL_ACTIVE_VOICE,
     CALL_INCOM_VOICE_AUTH,
     CALL_INCOM_VOICE_NOT_AUTH,
     CALL_INCOM_DATA_AUTH,
     CALL_INCOM_DATA_NOT_AUTH,
     CALL_ACTIVE_DATA,
     CALL_OTHERS,
     CALL_NO_RESPONSE,
     CALL_COMM_LINE_BUSY,

     CALL_LAST_ITEM
};


enum getsms_ret_val_enum {
     GETSMS_NO_SMS   = 0,
     GETSMS_UNREAD_SMS,
     GETSMS_READ_SMS,
     GETSMS_OTHER_SMS,

     GETSMS_NOT_AUTH_SMS,
     GETSMS_AUTH_SMS,

     GETSMS_LAST_ITEM
};


class GSM {
public:
     enum GSM_st_e { ERROR, IDLE, READY, ATTACHED, TCPSERVERWAIT, TCPCONNECTEDSERVER, TCPCONNECTEDCLIENT };
     uint8_t comm_buf[COMM_BUF_LEN+1];  // communication buffer +1 for 0x00 termination
     void InitParam (uint8_t group);

private:
     int _status;
     uint8_t comm_line_status;

     // global status - bits are used for representation of states
     uint8_t module_status;

     // variables connected with communication buffer

     uint8_t *p_comm_buf;               // pointer to the communication buffer
     uint8_t rx_state;                  // internal state of rx state machine
     uint16_t start_reception_tmout; // max tmout for starting reception
     uint16_t interchar_tmout;       // previous time in msec.
     unsigned long prev_time;        // previous time in msec.

     // last value of speaker volume
     uint8_t last_speaker_volume;
     char InitSMSMemory(void);

protected:

     int isIP(const char* cadena);

public:


          uint8_t comm_buf_len;              // num. of characters in the buffer
     HWSerial _cell;
     inline void setStatus(GSM_st_e status) {
          _status = status;
     }
     GSM();
     inline int getStatus() {
          return _status;
     };
     virtual int begin(int baud_rate);
     inline void SetCommLineStatus(uint8_t new_status) {
          comm_line_status = new_status;
     };
     inline uint8_t GetCommLineStatus(void) {
          return comm_line_status;
     };
     void RxInit(uint16_t start_comm_tmout, uint16_t max_interchar_tmout);
     uint8_t IsRxFinished(void);
     uint8_t IsStringReceived(char const *compare_string);
     uint8_t WaitResp(uint16_t start_comm_tmout, uint16_t max_interchar_tmout);
     uint8_t WaitResp(uint16_t start_comm_tmout, uint16_t max_interchar_tmout,
                   char const *expected_resp_string);
     char SendATCmdWaitResp(char const *AT_cmd_string,
                            uint16_t start_comm_tmout, uint16_t max_interchar_tmout,
                            char const *response_string,
                            uint8_t no_of_attempts);
     char SendATCmdWaitResp(const __FlashStringHelper *AT_cmd_string,
                            uint16_t start_comm_tmout, uint16_t max_interchar_tmout,
                            char const *response_string,
                            uint8_t no_of_attempts);
     char* SendATCmdWaitRespValue(char const *AT_cmd_string,
         uint16_t start_comm_tmout, uint16_t max_interchar_tmout,
         uint8_t no_of_attempts);
     void Echo(uint8_t state);


     //-----------------------
     // turns off/on the speaker
     void SetSpeaker(uint8_t off_on);
     // checks if module is registered in the GSM network
     // must be called regularly
     uint8_t CheckRegistration(void);

     // User button methods
     inline uint8_t IsUserButtonEnable(void) {
          return (module_status & STATUS_USER_BUTTON_ENABLE);
     };
     inline void DisableUserButton(void) {
          module_status &= ~STATUS_USER_BUTTON_ENABLE;
     };
     inline void EnableUserButton(void) {
          module_status |= STATUS_USER_BUTTON_ENABLE;
     };
     uint8_t IsUserButtonPushed(void);

     // Phonebook's methods
     char GetPhoneNumber(uint8_t position, char *phone_number);
     char WritePhoneNumber(uint8_t position, char *phone_number);
     char DelPhoneNumber(uint8_t position);
     char ComparePhoneNumber(uint8_t position, char *phone_number);

     // returns registration state
     uint8_t IsRegistered(void);
     // returns whether complete initialization was made
     uint8_t IsInitialized(void);
     //-----------------------
     char sendSMS(char *number_str, char *message_str);
     char getSMS(byte position, char *phone_number, char *SMS_text, byte max_SMS_len);
     char isSMSPresent(byte required_status);
     int setPIN(char *pin);
};

#endif

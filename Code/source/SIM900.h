#ifndef SIMCOM900_H
#define SIMCOM900_H
#include "GSM.h"
#include "HardwareSerial.h"
class SIMCOM900 : public virtual GSM {

private:
     int configandwait(char* pin);
     int changeNSIPmode(char);

public:
     SIMCOM900();
     ~SIMCOM900();
     int getCCI(char* cci);
     char* getIMEI();
     //int sendSMS(const char* to, const char* msg);

     boolean readSMS(char* msg, int msglength, char* number, int nlength);
     boolean readCall(char* number, int nlength);
     boolean call(char* number, unsigned int milliseconds);
     char forceON();
     virtual int read(char* result, int resultlength);
     virtual uint8_t read();
     virtual int available();
     int readCellData(int &mcc, int &mnc, long &lac, long &cellid);
     void SimpleRead();
     void WhileSimpleRead();

     char sendSMS(char *number_str, char *message_str);
     // Phonebook's methods
     char GetPhoneNumber(uint8_t position, char *phone_number);
     char WritePhoneNumber(uint8_t position, char *phone_number);
     char DelPhoneNumber(uint8_t position);
     char ComparePhoneNumber(uint8_t position, char *phone_number);
     char getSMS(byte position, char *phone_number, char *SMS_text, byte max_SMS_len);
     char isSMSPresent(byte required_status);


};

extern SIMCOM900 gsm;

#endif


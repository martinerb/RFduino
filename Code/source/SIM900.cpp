#include "SIM900.h"
#include "Streaming.h"
#include "HWSerial.h"

//#define _GSM_CONNECTION_TOUT_ 4
//#define _TCP_CONNECTION_TOUT_ 5
//#define _GSM_DATA_TOUT_ 6

SIMCOM900 gsm;
SIMCOM900::SIMCOM900() {
}
;
SIMCOM900::~SIMCOM900() {
}
;

/**********************************************************
 Function: 	This function premits to wake up the module
 (only for SIM908) when it goes in energy saving
 mode.

 Author:		Marco Martines
 Created:	unknown
 Modified:	18/02/2014

 Output:		none

 Comments:	It would be nice to call this function
 automatically when gsm.begin is called (of course
 only if a SIM908 is used).
 **********************************************************/

//char SIMCOM900::forceON()
//{
//	 char ret_val=0;
//     char *p_char;
//     char *p_char1;
//
//     SimpleWriteln(F("AT+CREG?"));
//     WaitResp(5000, 100, str_ok);
//     if(IsStringReceived(str_ok)) {
//          ret_val=1;
//     }
//
//     p_char = strchr((char *)(gsm.comm_buf),',');
//     p_char1 = p_char+1;
//     *(p_char1+2)=0;
//     p_char = strchr((char *)(p_char1), ',');
//     if (p_char != NULL) {
//          *p_char = 0;
//     }
//
//     if((*p_char1)=='4') {
//          digitalWrite(GSM_ON, HIGH);
//          delay(1200);
//          digitalWrite(GSM_ON, LOW);
//          delay(10000);
//          ret_val=2;
//     }
//
//     return ret_val;
//}
int SIMCOM900::configandwait(char* pin) {
	int connCode;
	//_tf.setTimeout(_GSM_CONNECTION_TOUT_);
	_cell.begin(9600);
	if (pin)
		setPIN(pin); //syv

	// Try 10 times to register in the network. Note this can take some time!
	for (int i = 0; i < 10; i++) {
		//Ask for register status to GPRS network.
		//SimpleWriteln(F("AT+CGREG?"));
		_cell.println(F("AT+CGREG?"));
		//Se espera la unsolicited response de registered to network.
		while (gsm.WaitResp(5000, 50, "+CGREG: 0,") != RX_FINISHED_STR_RECV)
		//while (_tf.find("+CGREG: 0,"))  // CHANGE!!!!
		{
			//connCode=_tf.getValue();
			connCode = _cell.read();
			if ((connCode == 1) || (connCode == 5)) {
				setStatus(READY);

				//SimpleWriteln(F("AT+CMGF=1")); //SMS text mode.
				_cell.println(F("AT+CMGF=1"));
				delay(200);
				// Buah, we should take this to readCall()
				//SimpleWriteln(F("AT+CLIP=1")); //SMS text mode.
				_cell.println(F("AT+CLIP=1"));
				delay(200);
				//_cell << "AT+QIDEACT" <<  _DEC(cr) << endl; //To make sure not pending connection.
				//delay(1000);

				return 1;
			}
		}
	}
	return 0;
}

/**
 * SIMCOM900::read(char* buffer, int buffersize)
 *
 * Waits for data to be readable from the gsm module, reads data until
 * no more is available or the buffer has been filled
 *
 * returns number of bytes read
 *
 */
int SIMCOM900::read(char* result, int resultlength) {
	char temp;
	int i = 0;	// Wait until we start receiving data
	while (gsm.available() < 1) {
		delay(100);
	}

	while (gsm.available() > 0 && i < (resultlength - 1)) {
		temp = _cell.read();
		if (temp > 0) {
			result[i] = temp;
			i++;
		}
		delay(1);
	}
	// Terminate the string
	result[resultlength - 1] = '\0';
	return i;
}

int SIMCOM900::readCellData(int &mcc, int &mnc, long &lac, long &cellid) {
	if (getStatus() == IDLE)
		return 0;

	//_tf.setTimeout(_GSM_DATA_TOUT_);
	//_cell.flush();
	_cell.begin(9600);
	_cell.println(F("AT+QENG=1,0"));
	_cell.println(F("AT+QENG?"));
//	SimpleWriteln(F("AT+QENG=1,0"));
//	SimpleWriteln(F("AT+QENG?"));
	if (gsm.WaitResp(5000, 50, "+QENG") != RX_FINISHED_STR_NOT_RECV)
		return 0;

	//mcc=_tf.getValue(); // The first one is 0
	mcc = _cell.read();
	//mcc=_tf.getValue();
	mcc = _cell.read();
	//mnc=_tf.getValue();
	mnc = _cell.read();
	//lac=_tf.getValue();
	lac = _cell.read();
	//cellid=_tf.getValue();
	cellid = _cell.read();

	gsm.WaitResp(5000, 50, "+OK");
	_cell.println(F("AT+QENG=1,0"));
	//SimpleWriteln(F("AT+QENG=1,0"));
	gsm.WaitResp(5000, 50, "+OK");
	return 1;
}

boolean SIMCOM900::readCall(char* number, int nlength) {
	int index;

	if (getStatus() == IDLE)
		return false;

	_cell.begin(9600);
	//_tf.setTimeout(_GSM_DATA_TOUT_);
	if (gsm.WaitResp(5000, 50, "+CLIP: \"") != RX_FINISHED_STR_RECV)
	//if(_tf.find("+CLIP: \""))
			{
		_cell.getString("", "\"", number, nlength);
		_cell.println(F("ATH"));
		//SimpleWriteln(F("ATH"));
		delay(1000);
		//_cell.flush();
		return true;
	};
	return false;
}
;

boolean SIMCOM900::call(char* number, unsigned int milliseconds) {
	if (getStatus() == IDLE)
		return false;

	//_tf.setTimeout(_GSM_DATA_TOUT_);
	_cell.begin(9600);
	_cell.print(F("ATD"));
	_cell.print(number);
	_cell.println(F(";"));
	delay(milliseconds);
	_cell.println(F("ATH"));
//	SimpleWrite(F("ATD"));
//	SimpleWrite(number);
//	SimpleWriteln(F(";"));
//	delay(milliseconds);
//	SimpleWriteln(F("ATH"));
	return true;

}

int SIMCOM900::changeNSIPmode(char mode) {
	//_tf.setTimeout(_TCP_CONNECTION_TOUT_);

	//if (getStatus()!=ATTACHED)
	//    return 0;

	//_cell.flush();
	_cell.begin(9600);
	_cell.print(F("AT+QIDNSIP="));
	_cell.println(mode);
	//SimpleWrite(F("AT+QIDNSIP="));
	//SimpleWriteln(mode);
	if (gsm.WaitResp(5000, 50, str_ok) != RX_FINISHED_STR_NOT_RECV)
		return 0;
	//if(!_tf.find(str_ok)) return 0;

	return 1;
}

int SIMCOM900::getCCI(char *cci) {
	//Status must be READY
	if ((getStatus() != READY))
		return 2;
	//AT command to get CCID.
	_cell.begin(9600);
	_cell.println(F("AT+QCCID"));
	//SimpleWriteln(F("AT+QCCID"));

	//Read response from modem
	_cell.getString("AT+QCCID\r\r\r\n", "\r\n", cci, 21);

	//Expect str_ok.
	if (gsm.WaitResp(5000, 50, str_ok) != RX_FINISHED_STR_NOT_RECV)
		return 0;
	else
		return 1;
}

char* SIMCOM900::getIMEI() {
	_cell.begin(9600);
	char* t;
	if (AT_RESP_OK == gsm.SendATCmdWaitResp("AT+GSN=?", 500, 100, str_ok, 5)) {

	}
	//013777008675397

	t = gsm.SendATCmdWaitRespValue("AT+GSN", 500, 100, 5);
	if (t != "") {
		_cell.printError("habe emei bekommen");
		return t;
	}
	return "";

//
//	if (gsm.WaitResp(5000, 50, str_ok) != RX_FINISHED_STR_NOT_RECV)
//		return 0;
//	else
//		return 1;
}

int SIMCOM900::available() {
	return _cell.available();
}

uint8_t SIMCOM900::read() {
	return _cell.read();
}

/**********************************************************
 Method sets speaker volume

 speaker_volume: volume in range 0..14

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module did not answer in timeout
 -3 - GSM module has answered "ERROR" string

 OK ret val:
 -----------
 0..14 current speaker volume
 **********************************************************/
/*
 char GSM::SetSpeakerVolume(byte speaker_volume)
 {

 char ret_val = -1;

 if (CLS_FREE != GetCommLineStatus()) return (ret_val);
 SetCommLineStatus(CLS_ATCMD);
 // remember set value as last value
 if (speaker_volume > 14) speaker_volume = 14;
 // select speaker volume (0 to 14)
 // AT+CLVL=X<CR>   X<0..14>
 _cell.print("AT+CLVL=");
 _cell.print((int)speaker_volume);
 _cell.print("\r"); // send <CR>
 // 10 sec. for initial comm tmout
 // 50 msec. for inter character timeout
 if (RX_TMOUT_ERR == WaitResp(10000, 50)) {
 ret_val = -2; // ERROR
 }
 else {
 if(IsStringReceived(str_ok)) {
 last_speaker_volume = speaker_volume;
 ret_val = last_speaker_volume; // OK
 }
 else ret_val = -3; // ERROR
 }

 SetCommLineStatus(CLS_FREE);
 return (ret_val);
 }
 */
/**********************************************************
 Method increases speaker volume

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module did not answer in timeout
 -3 - GSM module has answered "ERROR" string

 OK ret val:
 -----------
 0..14 current speaker volume
 **********************************************************/
/*
 char GSM::IncSpeakerVolume(void)
 {
 char ret_val;
 byte current_speaker_value;

 current_speaker_value = last_speaker_volume;
 if (current_speaker_value < 14) {
 current_speaker_value++;
 ret_val = SetSpeakerVolume(current_speaker_value);
 }
 else ret_val = 14;

 return (ret_val);
 }
 */
/**********************************************************
 Method decreases speaker volume

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module did not answer in timeout
 -3 - GSM module has answered "ERROR" string

 OK ret val:
 -----------
 0..14 current speaker volume
 **********************************************************/
/*
 char GSM::DecSpeakerVolume(void)
 {
 char ret_val;
 byte current_speaker_value;

 current_speaker_value = last_speaker_volume;
 if (current_speaker_value > 0) {
 current_speaker_value--;
 ret_val = SetSpeakerVolume(current_speaker_value);
 }
 else ret_val = 0;

 return (ret_val);
 }
 */

/**********************************************************
 Method sends DTMF signal
 This function only works when call is in progress

 dtmf_tone: tone to send 0..15

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module didn't answer in timeout
 -3 - GSM module has answered "ERROR" string

 OK ret val:
 -----------
 0.. tone
 **********************************************************/
/*
 char GSM::SendDTMFSignal(byte dtmf_tone)
 {
 char ret_val = -1;

 if (CLS_FREE != GetCommLineStatus()) return (ret_val);
 SetCommLineStatus(CLS_ATCMD);
 // e.g. AT+VTS=5<CR>
 _cell.print("AT+VTS=");
 _cell.print((int)dtmf_tone);
 _cell.print("\r");
 // 1 sec. for initial comm tmout
 // 50 msec. for inter character timeout
 if (RX_TMOUT_ERR == WaitResp(1000, 50)) {
 ret_val = -2; // ERROR
 }
 else {
 if(IsStringReceived(str_ok)) {
 ret_val = dtmf_tone; // OK
 }
 else ret_val = -3; // ERROR
 }

 SetCommLineStatus(CLS_FREE);
 return (ret_val);
 }
 */
/**********************************************************
 Method reads phone number string from specified SIM position

 position:     SMS position <1..20>

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module didn't answer in timeout
 -3 - position must be > 0
 phone_number is empty string

 OK ret val:
 -----------
 0 - there is no phone number on the position
 1 - phone number was found
 phone_number is filled by the phone number string finished by 0x00
 so it is necessary to define string with at least
 15 bytes(including also 0x00 termination character)

 an example of usage:
 GSM gsm;
 char phone_num[20]; // array for the phone number string

 if (1 == gsm.GetPhoneNumber(1, phone_num)) {
 // valid phone number on SIM pos. #1
 // phone number string is copied to the phone_num array
 #ifdef DEBUG_PRINT
 gsm.DebugPrint("DEBUG phone number: ", 0);
 gsm.DebugPrint(phone_num, 1);
 #endif
 }
 else {
 // there is not valid phone number on the SIM pos.#1
 #ifdef DEBUG_PRINT
 gsm.DebugPrint("DEBUG there is no phone number", 1);
 #endif
 }
 **********************************************************/

char SIMCOM900::GetPhoneNumber(byte position, char *phone_number) {
	char ret_val = -1;
	char *p_char;
	char *p_char1;
	_cell.begin(9600);

	if (position == 0)
		return (-3);
	if (CLS_FREE != GetCommLineStatus())
		return (ret_val);
	SetCommLineStatus(CLS_ATCMD);
	ret_val = 0; // not found yet
	phone_number[0] = 0; // phone number not found yet => empty string

	//send "AT+CPBR=XY" - where XY = position
	_cell.print(F("AT+CPBR="));
	_cell.print((int) position);
	_cell.print("\r");

	// 5000 msec. for initial comm tmout
	// 50 msec. for inter character timeout
	switch (WaitResp(5000, 50, "+CPBR")) {
	case RX_TMOUT_ERR:
		// response was not received in specific time
		ret_val = -2;
		break;

	case RX_FINISHED_STR_RECV:
		// response in case valid phone number stored:
		// <CR><LF>+CPBR: <index>,<number>,<type>,<text><CR><LF>
		// <CR><LF>OK<CR><LF>

		// response in case there is not phone number:
		// <CR><LF>OK<CR><LF>
		p_char = strstr((char *) (comm_buf), ",\"");
		if (p_char != NULL) {
			p_char++;
			p_char++;       // we are on the first phone number character
			// find out '"' as finish character of phone number string
			p_char1 = strchr((char *) (p_char), '"');
			if (p_char1 != NULL) {
				*p_char1 = 0; // end of string
			}
			// extract phone number string
			strcpy(phone_number, (char *) (p_char));
			// output value = we have found out phone number string
			ret_val = 1;
		}
		break;

	case RX_FINISHED_STR_NOT_RECV:
		// only OK or ERROR => no phone number
		ret_val = 0;
		break;
	}

	SetCommLineStatus(CLS_FREE);
	return (ret_val);
}

/**********************************************************
 Method writes phone number string to the specified SIM position

 position:     SMS position <1..20>
 phone_number: phone number string for the writing

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module didn't answer in timeout
 -3 - position must be > 0

 OK ret val:
 -----------
 0 - phone number was not written
 1 - phone number was written
 **********************************************************/
char SIMCOM900::WritePhoneNumber(byte position, char *phone_number) {
	char ret_val = -1;

	if (position == 0)
		return (-3);
	if (CLS_FREE != GetCommLineStatus())
		return (ret_val);
	SetCommLineStatus(CLS_ATCMD);
	ret_val = 0; // phone number was not written yet

	//send: AT+CPBW=XY,"00420123456789"
	// where XY = position,
	//       "00420123456789" = phone number string
	_cell.print(F("AT+CPBW="));
	_cell.print((int) position);
	_cell.print(F(",\""));
	_cell.print(phone_number);
	_cell.print(F("\"\r"));

	// 5000 msec. for initial comm tmout
	// 50 msec. for inter character timeout
	switch (WaitResp(5000, 50, str_ok)) {
	case RX_TMOUT_ERR:
		// response was not received in specific time
		break;

	case RX_FINISHED_STR_RECV:
		// response is OK = has been written
		ret_val = 1;
		break;

	case RX_FINISHED_STR_NOT_RECV:
		// other response: e.g. ERROR
		break;
	}

	SetCommLineStatus(CLS_FREE);
	return (ret_val);
}

/**********************************************************
 Method del phone number from the specified SIM position

 position:     SMS position <1..20>

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module didn't answer in timeout
 -3 - position must be > 0

 OK ret val:
 -----------
 0 - phone number was not deleted
 1 - phone number was deleted
 **********************************************************/
char SIMCOM900::DelPhoneNumber(byte position) {
	char ret_val = -1;

	if (position == 0)
		return (-3);
	if (CLS_FREE != GetCommLineStatus())
		return (ret_val);
	SetCommLineStatus(CLS_ATCMD);
	ret_val = 0; // phone number was not written yet

	//send: AT+CPBW=XY
	// where XY = position
	_cell.print(F("AT+CPBW="));
	_cell.print((int) position);
	_cell.print(F("\r"));

	// 5000 msec. for initial comm tmout
	// 50 msec. for inter character timeout
	switch (WaitResp(5000, 50, str_ok)) {
	case RX_TMOUT_ERR:
		// response was not received in specific time
		break;

	case RX_FINISHED_STR_RECV:
		// response is OK = has been written
		ret_val = 1;
		break;

	case RX_FINISHED_STR_NOT_RECV:
		// other response: e.g. ERROR
		break;
	}

	SetCommLineStatus(CLS_FREE);
	return (ret_val);
}

/**********************************************************
 Function compares specified phone number string
 with phone number stored at the specified SIM position

 position:       SMS position <1..20>
 phone_number:   phone number string which should be compare

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module didn't answer in timeout
 -3 - position must be > 0

 OK ret val:
 -----------
 0 - phone numbers are different
 1 - phone numbers are the same


 an example of usage:
 if (1 == gsm.ComparePhoneNumber(1, "123456789")) {
 // the phone num. "123456789" is stored on the SIM pos. #1
 // phone number string is copied to the phone_num array
 #ifdef DEBUG_PRINT
 gsm.DebugPrint("DEBUG phone numbers are the same", 1);
 #endif
 }
 else {
 #ifdef DEBUG_PRINT
 gsm.DebugPrint("DEBUG phone numbers are different", 1);
 #endif
 }
 **********************************************************/
char SIMCOM900::ComparePhoneNumber(byte position, char *phone_number) {
	char ret_val = -1;
	char sim_phone_number[20];

	ret_val = 0; // numbers are not the same so far
	if (position == 0)
		return (-3);
	if (1 == GetPhoneNumber(position, sim_phone_number)) {
		//Serial.print("CHIAMANTE ");
		//Serial.println(phone_number);
		//Serial.print("SALVATO ");
		//Serial.println(sim_phone_number);

		// there is a valid number at the spec. SIM position
		// -------------------------------------------------
		if (0 == strcmp(phone_number, sim_phone_number)) {
			// phone numbers are the same
			// --------------------------
			ret_val = 1;
		}
	}
	return (ret_val);
}

/**********************************************************
 Method sends SMS

 number_str:   pointer to the phone number string
 message_str:  pointer to the SMS text string


 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module didn't answer in timeout
 -3 - GSM module has answered "ERROR" string

 OK ret val:
 -----------
 0 - SMS was not sent
 1 - SMS was sent


 an example of usage:
 GSM gsm;
 gsm.SendSMS("00XXXYYYYYYYYY", "SMS text");
 **********************************************************/
char SIMCOM900::sendSMS(char *number_str, char *message_str) {
	_cell.begin(9600);
	if (strlen(message_str) > 159)
		Serial.println(F("Don't send message longer than 160 characters"));
	char ret_val = -1;
	byte i;
	char end[2];
	end[0] = 0x1a;
	end[1] = '\0';
	// try to send SMS 3 times in case there is some problem
	for (i = 0; i < 1; i++) {
		// send  AT+CMGS="number_str"

		//gsm.SimpleWrite(F("AT+CMGS=\""));
		//gsm.SimpleWrite(number_str);
		//gsm.SimpleWriteln("\"");
		_cell.print(F("AT+CMGS=\""));
		_cell.print(number_str);
		_cell.println("\"");

		// 1000 msec. for initial comm tmout
		// 50 msec. for inter character timeout
		if (RX_FINISHED_STR_RECV == gsm.WaitResp(1000, 500, ">")) {
			// send SMS text

			//gsm.SimpleWrite(message_str);
			//gsm.SimpleWriteln(end);
			_cell.print(message_str);
			_cell.println(end);
			_cell.flush(); // erase rx circular buffer
			if (RX_FINISHED_STR_RECV == gsm.WaitResp(7000, 5000, "+CMGS")) {
				// SMS was send correctly
				ret_val = 1;
				break;
			} else
				continue;
		} else {
			// try again
			continue;

		}
	}

	gsm.SetCommLineStatus(CLS_FREE);
	return (ret_val);
}

/**********************************************************
 Method reads SMS from specified memory(SIM) position

 position:     SMS position <1..20>
 phone_number: a pointer where the phone number string of received SMS will be placed
 so the space for the phone number string must be reserved - see example
 SMS_text  :   a pointer where SMS text will be placed
 max_SMS_len:  maximum length of SMS text excluding also string terminating 0x00 character

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module didn't answer in timeout
 -3 - specified position must be > 0

 OK ret val:
 -----------
 GETSMS_NO_SMS       - no SMS was not found at the specified position
 GETSMS_UNREAD_SMS   - new SMS was found at the specified position
 GETSMS_READ_SMS     - already read SMS was found at the specified position
 GETSMS_OTHER_SMS    - other type of SMS was found


 an example of usage:
 GSM gsm;
 char position;
 char phone_num[20]; // array for the phone number string
 char sms_text[100]; // array for the SMS text string

 position = gsm.IsSMSPresent(SMS_UNREAD);
 if (position) {
 // there is new SMS => read it
 gsm.GetSMS(position, phone_num, sms_text, 100);
 #ifdef DEBUG_PRINT
 gsm.DebugPrint("DEBUG SMS phone number: ", 0);
 gsm.DebugPrint(phone_num, 0);
 gsm.DebugPrint("\r\n          SMS text: ", 0);
 gsm.DebugPrint(sms_text, 1);
 #endif
 }
 **********************************************************/
char SIMCOM900::getSMS(byte position, char *phone_number, char *SMS_text,
		byte max_SMS_len) {
	char ret_val = -1;
	char *p_char;
	char *p_char1;
	byte len;
	_cell.begin(9600);

	if (position == 0)
		return (-3);
	if (CLS_FREE != gsm.GetCommLineStatus())
		return (ret_val);
	gsm.SetCommLineStatus(CLS_ATCMD);
	phone_number[0] = 0;  // end of string for now
	ret_val = GETSMS_NO_SMS; // still no SMS

	//send "AT+CMGR=X" - where X = position

	//gsm.SimpleWrite(F("AT+CMGR="));
	//gsm.SimpleWriteln((int) position);
	_cell.print(F("AT+CMGR="));
	_cell.println((int) position);

	// 5000 msec. for initial comm tmout
	// 100 msec. for inter character tmout
	switch (gsm.WaitResp(5000, 100, "+CMGR")) {
	case RX_TMOUT_ERR:
		// response was not received in specific time
		ret_val = -2;
		break;

	case RX_FINISHED_STR_NOT_RECV:
		// OK was received => there is NO SMS stored in this position
		if (gsm.IsStringReceived("OK")) {
			// there is only response <CR><LF>OK<CR><LF>
			// => there is NO SMS
			ret_val = GETSMS_NO_SMS;
		} else if (gsm.IsStringReceived("ERROR")) {
			// error should not be here but for sure
			ret_val = GETSMS_NO_SMS;
		}
		break;

	case RX_FINISHED_STR_RECV:
		// find out what was received exactly

		//response for new SMS:
		//<CR><LF>+CMGR: "REC UNREAD","+XXXXXXXXXXXX",,"02/03/18,09:54:28+40"<CR><LF>
		//There is SMS text<CR><LF>OK<CR><LF>
		if (gsm.IsStringReceived("\"REC UNREAD\"")) {
			// get phone number of received SMS: parse phone number string
			// +XXXXXXXXXXXX
			// -------------------------------------------------------
			ret_val = GETSMS_UNREAD_SMS;
		}
		//response for already read SMS = old SMS:
		//<CR><LF>+CMGR: "REC READ","+XXXXXXXXXXXX",,"02/03/18,09:54:28+40"<CR><LF>
		//There is SMS text<CR><LF>
		else if (gsm.IsStringReceived("\"REC READ\"")) {
			// get phone number of received SMS
			// --------------------------------
			ret_val = GETSMS_READ_SMS;
		} else {
			// other type like stored for sending..
			ret_val = GETSMS_OTHER_SMS;
		}

		// extract phone number string
		// ---------------------------
		p_char = strchr((char *) (gsm.comm_buf), ',');
		p_char1 = p_char + 2; // we are on the first phone number character
		p_char = strchr((char *) (p_char1), '"');
		if (p_char != NULL) {
			*p_char = 0; // end of string
			strcpy(phone_number, (char *) (p_char1));
		}

		// get SMS text and copy this text to the SMS_text buffer
		// ------------------------------------------------------
		p_char = strchr(p_char + 1, 0x0a);  // find <LF>
		if (p_char != NULL) {
			// next character after <LF> is the first SMS character
			p_char++; // now we are on the first SMS character

			// find <CR> as the end of SMS string
			p_char1 = strchr((char *) (p_char), 0x0d);
			if (p_char1 != NULL) {
				// finish the SMS text string
				// because string must be finished for right behaviour
				// of next strcpy() function
				*p_char1 = 0;
			}
			// in case there is not finish sequence <CR><LF> because the SMS is
			// too long (more then 130 characters) sms text is finished by the 0x00
			// directly in the gsm.WaitResp() routine

			// find out length of the SMS (excluding 0x00 termination character)
			len = strlen(p_char);

			if (len < max_SMS_len) {
				// buffer SMS_text has enough place for copying all SMS text
				// so copy whole SMS text
				// from the beginning of the text(=p_char position)
				// to the end of the string(= p_char1 position)
				strcpy(SMS_text, (char *) (p_char));
			} else {
				// buffer SMS_text doesn't have enough place for copying all SMS text
				// so cut SMS text to the (max_SMS_len-1)
				// (max_SMS_len-1) because we need 1 position for the 0x00 as finish
				// string character
				memcpy(SMS_text, (char *) (p_char), (max_SMS_len - 1));
				SMS_text[max_SMS_len] = 0; // finish string
			}
		}
		break;
	}

	gsm.SetCommLineStatus(CLS_FREE);
	return (ret_val);
}

/**********************************************************
 Method finds out if there is present at least one SMS with
 specified status

 Note:
 if there is new SMS before IsSMSPresent() is executed
 this SMS has a status UNREAD and then
 after calling IsSMSPresent() method status of SMS
 is automatically changed to READ

 required_status:  SMS_UNREAD  - new SMS - not read yet
 SMS_READ    - already read SMS
 SMS_ALL     - all stored SMS

 return:
 ERROR ret. val:
 ---------------
 -1 - comm. line to the GSM module is not free
 -2 - GSM module didn't answer in timeout

 OK ret val:
 -----------
 0 - there is no SMS with specified status
 1..20 - position where SMS is stored
 (suitable for the function GetSMS())


 an example of use:
 GSM gsm;
 char position;
 char phone_number[20]; // array for the phone number string
 char sms_text[100];

 position = gsm.IsSMSPresent(SMS_UNREAD);
 if (position) {
 // read new SMS
 gsm.GetSMS(position, phone_num, sms_text, 100);
 // now we have phone number string in phone_num
 // and SMS text in sms_text
 }
 **********************************************************/
char SIMCOM900::isSMSPresent(byte required_status) {

	_cell.begin(9600);
	char ret_val = -1;
	char *p_char;
	byte status;

	if (CLS_FREE != gsm.GetCommLineStatus())
		return (ret_val);
	gsm.SetCommLineStatus(CLS_ATCMD);
	ret_val = 0; // still not present

	switch (required_status) {
	case SMS_UNREAD:
		//gsm.SimpleWriteln(F("AT+CMGL=\"REC UNREAD\""));
		_cell.println(F("AT+CMGL=\"REC UNREAD\""));
		break;
	case SMS_READ:
		_cell.println(F("AT+CMGL=\"REC READ\""));
		//gsm.SimpleWriteln(F("AT+CMGL=\"REC READ\""));
		break;
	case SMS_ALL:
		_cell.println(F("AT+CMGL=\"ALL\""));
		//gsm.SimpleWriteln(F("AT+CMGL=\"ALL\""));
		break;
	}

	// 5 sec. for initial comm tmout
	// and max. 1500 msec. for inter character timeout
	gsm.RxInit(5000, 1500);
	// wait response is finished
	do {
		if (gsm.IsStringReceived("OK")) {
			// perfect - we have some response, but what:

			// there is either NO SMS:
			// <CR><LF>OK<CR><LF>

			// or there is at least 1 SMS
			// +CMGL: <index>,<stat>,<oa/da>,,[,<tooa/toda>,<length>]
			// <CR><LF> <data> <CR><LF>OK<CR><LF>
			status = RX_FINISHED;
			break; // so finish receiving immediately and let's go to
			// to check response
		}
		status = gsm.IsRxFinished();
	} while (status == RX_NOT_FINISHED);

	switch (status) {
	case RX_TMOUT_ERR:
		// response was not received in specific time
		ret_val = -2;
		break;

	case RX_FINISHED:
		// something was received but what was received?
		// ---------------------------------------------
		if (gsm.IsStringReceived("+CMGL:")) {
			// there is some SMS with status => get its position
			// response is:
			// +CMGL: <index>,<stat>,<oa/da>,,[,<tooa/toda>,<length>]
			// <CR><LF> <data> <CR><LF>OK<CR><LF>
			p_char = strchr((char *) gsm.comm_buf, ':');
			if (p_char != NULL) {
				ret_val = atoi(p_char + 1);
			}
		} else {
			// other response like OK or ERROR
			ret_val = 0;
		}

		// here we have gsm.WaitResp() just for generation tmout 20msec. in case OK was detected
		// not due to receiving
		gsm.WaitResp(20, 20);
		break;
	}

	gsm.SetCommLineStatus(CLS_FREE);
	return (ret_val);
}
//-----------------------------------------------------

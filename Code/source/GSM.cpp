/*
 This is a Beta version.
 last modified 18/08/2012.

 This library is based on one developed by Arduino Labs
 and it is modified to preserve the compability
 with the Arduino's product.

 The library is modified to use the GSM Shield,
 developed by www.open-electronics.org
 (http://www.open-electronics.org/arduino-gsm-shield/)
 and based on SIM900 chip,
 with the same commands of Arduino Shield,
 based on QuectelM10 chip.
 */

#include "GSM.h"
#include "variant.h"
#include "Arduino.h"
#include "timer.h"

GSM::GSM() {
}
;

int GSM::begin(int baud_rate) {
	_cell.begin(baud_rate);
	timerStart();
	delay(2000);
	int response = -1;
	int cont = 0;
	boolean norep = true;
	boolean turnedON = false;
	SetCommLineStatus(CLS_ATCMD);
	p_comm_buf = &comm_buf[0];
	setStatus(IDLE);

	// if no-reply we turn to turn on the module
	for (cont = 0; cont < 3; cont++) {
		if (AT_RESP_ERR_NO_RESP == SendATCmdWaitResp(str_at, 500, 100, str_ok, 5) && !turnedON) {	//check power
			_cell.printError("set Power on");

		} else {
			WaitResp(1000, 1000);
		}
	}
	if (AT_RESP_OK == SendATCmdWaitResp(str_at, 500, 100, str_ok, 5)) {
		_cell.printError("turnedON=true1;");
		turnedON = true;
		norep = false;
	}
	if (norep == true && !turnedON) {
		_cell.printError("Module does not response ");
		timerStart();
		delay(10000);
		if (AT_RESP_OK == SendATCmdWaitResp(str_at, 500, 100, str_ok, 5)) {
			_cell.printError("turnedON=true;");
			turnedON = true;
			norep = false;
		} else {
			_cell.printError("ERROR: SIM900 doesn't answer. Check power and serial pins in GSM.cpp");
			return 0;
		}
	}
	SetCommLineStatus(CLS_FREE);
	if (turnedON) {
		WaitResp(50, 50);
		InitParam(PARAM_SET_0);
		InitParam(PARAM_SET_1);		//configure the module
		Echo(0);               //enable AT echo
		char *pin = "19911";
		setPIN(pin);
		setStatus(READY);
		_cell.printError("GSM Device initialized");
		return (1);

	} else {
		//just to try to fix some problems with 115200 baudrate
		_cell.begin(115200);
		delay(1000);
		_cell.print("AT+IPR=");
		_cell.print(baud_rate);
		_cell.print("\r"); // send <CR>
		_cell.printError("not on");
		Serial.println("not on");
		return (0);
	}
}

void GSM::InitParam(byte group) {
	switch (group) {
	case PARAM_SET_0:
		// check comm line
		//if (CLS_FREE != GetCommLineStatus()) return;

		SetCommLineStatus(CLS_ATCMD);
		// Reset to the factory settings
		SendATCmdWaitResp(F("AT&F"), 1000, 50, str_ok, 5);
		// switch off echo
		SendATCmdWaitResp(F("ATE0"), 500, 50, str_ok, 5);
		// setup fixed baud rate
		SendATCmdWaitResp("AT+IPR=9600", 500, 50, str_ok, 5);
		// setup mode
		SendATCmdWaitResp("AT#SELINT=1", 500, 50, str_ok, 5);
		// Switch ON User LED - just as signalization we are here
		//SendATCmdWaitResp("AT#GPIO=8,1,1", 500, 50, str_ok, 5);
		// Sets GPIO9 as an input = user button
		//SendATCmdWaitResp("AT#GPIO=9,0,0", 500, 50, str_ok, 5);
		// allow audio amplifier control
		//SendATCmdWaitResp("AT#GPIO=5,0,2", 500, 50, str_ok, 5);
		// Switch OFF User LED- just as signalization we are finished
		//SendATCmdWaitResp("AT#GPIO=8,0,1", 500, 50, str_ok, 5);
		SetCommLineStatus(CLS_FREE);
		break;

	case PARAM_SET_1:
		// check comm line
		//if (CLS_FREE != GetCommLineStatus()) return;
		SetCommLineStatus(CLS_ATCMD);
		// Request calling line identification
		SendATCmdWaitResp(F("AT+CLIP=1"), 500, 50, str_ok, 5);
		// Mobile Equipment Error Code
		SendATCmdWaitResp(F("AT+CMEE=0"), 500, 50, str_ok, 5);
		// Echo canceller enabled
		SendATCmdWaitResp("AT#SHFEC=1", 500, 50, str_ok, 5);
		// Ringer tone select (0 to 32)
		//SendATCmdWaitResp("AT#SRS=26,0", 500, 50, str_ok, 5);
		// Microphone gain (0 to 7) - response here sometimes takes
		// more than 500msec. so 1000msec. is more safety
		//SendATCmdWaitResp("AT#HFMICG=7", 1000, 50, str_ok, 5);

		// set the SMS mode to text
		SendATCmdWaitResp(F("AT+CMGF=1"), 500, 50, str_ok, 5);
		// Auto answer after first ring enabled
		// auto answer is not used
		//SendATCmdWaitResp("ATS0=1", 500, 50, str_ok, 5);
		// select ringer path to handsfree
		//SendATCmdWaitResp("AT#SRP=1", 500, 50, str_ok, 5);
		// select ringer sound level
		//SendATCmdWaitResp("AT+CRSL=2", 500, 50, str_ok, 5);
		// we must release comm line because SetSpeakerVolume()
		// checks comm line if it is free
		SetCommLineStatus(CLS_FREE);
		// select speaker volume (0 to 14)
		//SetSpeakerVolume(9);
		// init SMS storage
		InitSMSMemory();

		// select phonebook memory storage
		SendATCmdWaitResp(F("AT+CPBS=\"SM\""), 1000, 50, str_ok, 5);
		SendATCmdWaitResp(F("AT+CIPSHUT"), 500, 50, "SHUT OK", 5);
		break;
	}
}

byte GSM::WaitResp(uint16_t start_comm_tmout, uint16_t max_interchar_tmout, char const *expected_resp_string) {
	byte status;
	byte ret_val;

	RxInit(start_comm_tmout, max_interchar_tmout);
	// wait until response is not finished
	do {
		status = IsRxFinished();
	} while (status == RX_NOT_FINISHED);

	if (status == RX_FINISHED) {

		if (IsStringReceived(expected_resp_string)) {
			ret_val = RX_FINISHED_STR_RECV;
		} else {
			ret_val = RX_FINISHED_STR_NOT_RECV;
		}
	} else {
		ret_val = RX_TMOUT_ERR;
	}
	return (ret_val);
}

/**********************************************************
 Method sends AT command and waits for response

 return:
 AT_RESP_ERR_NO_RESP = -1,   // no response received
 AT_RESP_ERR_DIF_RESP = 0,   // response_string is different from the response
 AT_RESP_OK = 1,             // response_string was included in the response
 **********************************************************/
char GSM::SendATCmdWaitResp(char const *AT_cmd_string, uint16_t start_comm_tmout, uint16_t max_interchar_tmout, char const *response_string,
		byte no_of_attempts) {
	byte status;
	char ret_val = AT_RESP_ERR_NO_RESP;
	byte i;

	_cell.begin(9600);
	for (i = 0; i < no_of_attempts; i++) {
		// delay 500 msec. before sending next repeated AT command
		// so if we have no_of_attempts=1 tmout will not occurred
		if (i > 0)
			delay(500);

		// Serial.printf("geschickt %s \r\n",AT_cmd_string);

		_cell.println(AT_cmd_string);
		status = WaitResp(start_comm_tmout, max_interchar_tmout);
		if (status == RX_FINISHED) {
			//	Serial.println("recieved something ");
			// something was received but what was received?
			// ---------------------------------------------
			if (IsStringReceived(response_string)) {
				ret_val = AT_RESP_OK;
				break;  // response is OK => finish
			} else
				ret_val = AT_RESP_ERR_DIF_RESP;
		} else {
			// nothing was received
			// --------------------
			Serial.println("recieved nothing ");
			ret_val = AT_RESP_ERR_NO_RESP;
		}
	}
	WaitResp(1000, 5000);
	return (ret_val);
}

/**********************************************************
 Method sends AT command and waits for response

 return:
 AT_RESP_ERR_NO_RESP = -1,   // no response received
 AT_RESP_ERR_DIF_RESP = 0,   // response_string is different from the response
 AT_RESP_OK = 1,             // response_string was included in the response
 **********************************************************/
char GSM::SendATCmdWaitResp(const __FlashStringHelper *AT_cmd_string, uint16_t start_comm_tmout, uint16_t max_interchar_tmout,
		char const *response_string, byte no_of_attempts) {
	byte status;
	char ret_val = AT_RESP_ERR_NO_RESP;
	byte i;

	for (i = 0; i < no_of_attempts; i++) {
		// delay 500 msec. before sending next repeated AT command
		// so if we have no_of_attempts=1 tmout will not occurred
		if (i > 0)
			delay(500);

		_cell.println(AT_cmd_string);
		status = WaitResp(start_comm_tmout, max_interchar_tmout);
		if (status == RX_FINISHED) {
			// something was received but what was received?
			// ---------------------------------------------

			if (IsStringReceived(response_string)) {
				ret_val = AT_RESP_OK;
				break;  // response is OK => finish
			} else
				ret_val = AT_RESP_ERR_DIF_RESP;
		} else {
			// nothing was received
			// --------------------
			ret_val = AT_RESP_ERR_NO_RESP;
		}
	}
	return (ret_val);
}

char* GSM::SendATCmdWaitRespValue(const char *AT_cmd_string, uint16_t start_comm_tmout, uint16_t max_interchar_tmout, byte no_of_attempts) {
	byte status;
	byte i;
	char* ret = "";
	for (i = 0; i < no_of_attempts; i++) {
		// delay 500 msec. before sending next repeated AT command
		// so if we have no_of_attempts=1 tmout will not occurred
		if (i > 0)
			delay(500);

		_cell.println(AT_cmd_string);
		status = WaitResp(start_comm_tmout, max_interchar_tmout);
		if (status == RX_FINISHED) {
			// something was received but what was received?
			// ---------------------------------------------
			ret = (char*) comm_buf;
			//_cell.printError(ret);
			if (ret != "") {
				return ret;
			}
		}
	}
	return "";
}

byte GSM::WaitResp(uint16_t start_comm_tmout, uint16_t max_interchar_tmout) {
	byte status;

	RxInit(start_comm_tmout, max_interchar_tmout);
	// wait until response is not finished
	do {
		status = IsRxFinished();
	} while (status == RX_NOT_FINISHED);

	return (status);
}

byte GSM::IsRxFinished(void) {
	byte num_of_bytes;
	byte ret_val = RX_NOT_FINISHED;  // default not finished
	// Rx state machine
	// ----------------
	if (rx_state == RX_NOT_STARTED) {
		// Reception is not started yet - check tmout
		if (!_cell.available()) {
			// still no character received => check timeout
			if ((unsigned long) (millis() - prev_time) >= start_reception_tmout) {
				// timeout elapsed => GSM module didn't start with response
				// so communication is takes as finished
				comm_buf[comm_buf_len] = 0x00;
				ret_val = RX_TMOUT_ERR;
			}
		} else {
			// at least one character received => so init inter-character
			// counting process again and go to the next state
			prev_time = millis(); // init tmout for inter-character space
			rx_state = RX_ALREADY_STARTED;
		}
	}

	if (rx_state == RX_ALREADY_STARTED) {
		// Reception already started
		// check new received bytes
		// only in case we have place in the buffer
		num_of_bytes = _cell.available();
		// if there are some received bytes postpone the timeout
		if (num_of_bytes)
			prev_time = millis();

		// read all received bytes
		while (num_of_bytes) {
			num_of_bytes--;
			if (comm_buf_len < COMM_BUF_LEN) {
				// we have still place in the GSM internal comm. buffer =>
				// move available bytes from circular buffer
				// to the rx buffer
				*p_comm_buf = _cell.read();
				p_comm_buf++;
				comm_buf_len++;
				comm_buf[comm_buf_len] = 0x00; // and finish currently received characters
				// so after each character we have
				// valid string finished by the 0x00
			} else {
				// comm buffer is full, other incoming characters
				// will be discarded
				// but despite of we have no place for other characters
				// we still must to wait until
				// inter-character tmout is reached

				// so just readout character from circular RS232 buffer
				// to find out when communication id finished(no more characters
				// are received in inter-char timeout)
				_cell.read();
			}
		}

		// finally check the inter-character timeout
		if ((unsigned long) (millis() - prev_time) >= interchar_tmout) {
			// timeout between received character was reached
			// reception is finished
			comm_buf[comm_buf_len] = 0x00;  // for sure finish string again
			// but it is not necessary
			ret_val = RX_FINISHED;
		}
	}
	return (ret_val);
}

/**********************************************************
 Method checks received bytes

 compare_string - pointer to the string which should be find

 return: 0 - string was NOT received
 1 - string was received
 **********************************************************/
byte GSM::IsStringReceived(char const *compare_string) {
	char *ch;
	byte ret_val = 0;

	if (comm_buf_len) {
		ch = strstr((char *) comm_buf, compare_string);
		if (ch != NULL) {
			ret_val = 1;
		} else {
			ret_val = 0;
		}
	} else {
		ret_val = 0;
	}
	return (ret_val);
}

void GSM::RxInit(uint16_t start_comm_tmout, uint16_t max_interchar_tmout) {
	rx_state = RX_NOT_STARTED;
	start_reception_tmout = start_comm_tmout;
	interchar_tmout = max_interchar_tmout;
	prev_time = millis();
	comm_buf[0] = 0x00; // end of string
	p_comm_buf = &comm_buf[0];
	comm_buf_len = 0;
	_cell.flush(); // erase rx circular buffer
}

void GSM::Echo(byte state) {
	if (state == 0 or state == 1) {
		SetCommLineStatus(CLS_ATCMD);

		_cell.print("ATE");
		_cell.print((int) state);
		_cell.print("\r");
		delay(500);
		SetCommLineStatus(CLS_FREE);
	}
}

char GSM::InitSMSMemory(void) {
	char ret_val = -1;

	if (CLS_FREE != GetCommLineStatus())
		return (ret_val);
	SetCommLineStatus(CLS_ATCMD);
	ret_val = 0; // not initialized yet

	// Disable messages about new SMS from the GSM module
	SendATCmdWaitResp(F("AT+CNMI=2,0"), 1000, 50, str_ok, 2);

	// send AT command to init memory for SMS in the SIM card
	// response:
	// +CPMS: <usedr>,<totalr>,<usedw>,<totalw>,<useds>,<totals>
	if (AT_RESP_OK == SendATCmdWaitResp(F("AT+CPMS=\"SM\",\"SM\",\"SM\""), 1000, 1000, "+CPMS:", 10)) {
		ret_val = 1;
	} else
		ret_val = 0;

	SetCommLineStatus(CLS_FREE);
	return (ret_val);
}

int GSM::isIP(const char* cadena) {
	int i;
	for (i = 0; i < strlen(cadena); i++)
		if (!(cadena[i] == '.' || (cadena[i] >= 48 && cadena[i] <= 57)))
			return 0;
	return 1;
}

//---------------------------------------------
/**********************************************************
 Turns on/off the speaker

 off_on: 0 - off
 1 - on
 **********************************************************/
void GSM::SetSpeaker(byte off_on) {
	if (CLS_FREE != GetCommLineStatus())
		return;
	SetCommLineStatus(CLS_ATCMD);
	if (off_on) {
		//SendATCmdWaitResp("AT#GPIO=5,1,2", 500, 50, "#GPIO:", 1);
	} else {
		//SendATCmdWaitResp("AT#GPIO=5,0,2", 500, 50, "#GPIO:", 1);
	}
	SetCommLineStatus(CLS_FREE);
}

byte GSM::IsRegistered(void) {
	return (module_status & STATUS_REGISTERED);
}

byte GSM::IsInitialized(void) {
	return (module_status & STATUS_INITIALIZED);
}

/**********************************************************
 Method checks if the GSM module is registered in the GSM net
 - this method communicates directly with the GSM module
 in contrast to the method IsRegistered() which reads the
 flag from the module_status (this flag is set inside this method)

 - must be called regularly - from 1sec. to cca. 10 sec.

 return values:
 REG_NOT_REGISTERED  - not registered
 REG_REGISTERED      - GSM module is registered
 REG_NO_RESPONSE     - GSM doesn't response
 REG_COMM_LINE_BUSY  - comm line between GSM module and Arduino is not free
 for communication
 **********************************************************/
byte GSM::CheckRegistration(void) {
	byte status;
	//byte ret_val = REG_NOT_REGISTERED;
	byte ret_val = -1;
	_cell.begin(9600);
	if (CLS_FREE != GetCommLineStatus())
		return (REG_COMM_LINE_BUSY);
	SetCommLineStatus(CLS_ATCMD);
	_cell.println(F("AT+CREG?"));
	// 5 sec. for initial comm tmout
	// 50 msec. for inter character timeout
	status = WaitResp(5000, 50);

	if (status == RX_FINISHED) {
		_cell.printError("something recieved");
		_cell.printError((char*) comm_buf);
		// something was received but what was received?
		// ---------------------------------------------
		if (IsStringReceived("+CREG: 0,1") || IsStringReceived("+CREG: 0,5")) {
			// it means module is registered
			// ----------------------------
			module_status |= STATUS_REGISTERED;

			// in case GSM module is registered first time after reset
			// sets flag STATUS_INITIALIZED
			// it is used for sending some init commands which
			// must be sent only after registration
			// --------------------------------------------
			if (!IsInitialized()) {
				module_status |= STATUS_INITIALIZED;
				SetCommLineStatus(CLS_FREE);
				InitParam(PARAM_SET_1);
			}
			_cell.printError("gsm registered");
			ret_val = REG_REGISTERED;
		} else {
			_cell.printError("not registered");
			// NOT registered
			// --------------
			module_status &= ~STATUS_REGISTERED;
			ret_val = REG_NOT_REGISTERED;
		}
	} else {
		// nothing was received
		// --------------------
		ret_val = REG_NO_RESPONSE;
	}
	SetCommLineStatus(CLS_FREE);
	return (ret_val);
}

int GSM::setPIN(char *pin) {
	//Status = READY or ATTACHED.
	if ((getStatus() != IDLE))
		return 2;

	//AT command to set PIN.
	_cell.begin(9600);
	_cell.print(F("AT+CPIN="));
	_cell.println(pin);

	//Expect str_ok.

	if (WaitResp(5000, 50, str_ok) != RX_FINISHED_STR_NOT_RECV)
		return 0;
	else
		return 1;
}


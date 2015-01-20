//#include "sms.h"
///**********************************************************
//Method reads SMS from specified memory(SIM) position and
//makes authorization - it means SMS phone number is compared
//with specified SIM phonebook position(s) and in case numbers
//match GETSMS_AUTH_SMS is returned, otherwise GETSMS_NOT_AUTH_SMS
//is returned
//
//position:     SMS position to be read <1..20>
//phone_number: a pointer where the tel. number string of received SMS will be placed
//              so the space for the phone number string must be reserved - see example
//SMS_text  :   a pointer where SMS text will be placed
//max_SMS_len:  maximum length of SMS text excluding terminating 0x00 character
//
//first_authorized_pos: initial SIM phonebook position where the authorization process
//                      starts
//last_authorized_pos:  last SIM phonebook position where the authorization process
//                      finishes
//
//                      Note(important):
//                      ================
//                      In case first_authorized_pos=0 and also last_authorized_pos=0
//                      the received SMS phone number is NOT authorized at all, so every
//                      SMS is considered as authorized (GETSMS_AUTH_SMS is returned)
//
//return:
//        ERROR ret. val:
//        ---------------
//        -1 - comm. line to the GSM module is not free
//        -2 - GSM module didn't answer in timeout
//        -3 - position must be > 0
//
//        OK ret val:
//        -----------
//        GETSMS_NO_SMS           - no SMS was found at the specified position
//        GETSMS_NOT_AUTH_SMS     - NOT authorized SMS found at the specified position
//        GETSMS_AUTH_SMS         - authorized SMS found at the specified position
//
//
//an example of usage:
//        GSM gsm;
//        char phone_num[20]; // array for the phone number string
//        char sms_text[100]; // array for the SMS text string
//
//        // authorize SMS with SIM phonebook positions 1..3
//        if (GETSMS_AUTH_SMS == gsm.GetAuthorizedSMS(1, phone_num, sms_text, 100, 1, 3)) {
//          // new authorized SMS was detected at the SMS position 1
//          #ifdef DEBUG_PRINT
//            gsm.DebugPrint("DEBUG SMS phone number: ", 0);
//            gsm.DebugPrint(phone_num, 0);
//            gsm.DebugPrint("\r\n          SMS text: ", 0);
//            gsm.DebugPrint(sms_text, 1);
//          #endif
//        }
//
//        // don't authorize SMS with SIM phonebook at all
//        if (GETSMS_AUTH_SMS == gsm.GetAuthorizedSMS(1, phone_num, sms_text, 100, 0, 0)) {
//          // new SMS was detected at the SMS position 1
//          // because authorization was not required
//          // SMS is considered authorized
//          #ifdef DEBUG_PRINT
//            gsm.DebugPrint("DEBUG SMS phone number: ", 0);
//            gsm.DebugPrint(phone_num, 0);
//            gsm.DebugPrint("\r\n          SMS text: ", 0);
//            gsm.DebugPrint(sms_text, 1);
//          #endif
//        }
//**********************************************************/
//char SMSGSM::GetAuthorizedSMS(byte position, char *phone_number, char *SMS_text, byte max_SMS_len,
//                              byte first_authorized_pos, byte last_authorized_pos)
//{
//     char ret_val = -1;
//     byte i;
//
//     ret_val = GetSMS(position, phone_number, SMS_text, max_SMS_len);
//     if (ret_val < 0) {
//          // here is ERROR return code => finish
//          // -----------------------------------
//     } else if (ret_val == GETSMS_NO_SMS) {
//          // no SMS detected => finish
//          // -------------------------
//     } else if (ret_val == GETSMS_READ_SMS) {
//          // now SMS can has only READ attribute because we have already read
//          // this SMS at least once by the previous function GetSMS()
//          //
//          // new READ SMS was detected on the specified SMS position =>
//          // make authorization now
//          // ---------------------------------------------------------
//          if ((first_authorized_pos == 0) && (last_authorized_pos == 0)) {
//               // authorization is not required => it means authorization is OK
//               // -------------------------------------------------------------
//               ret_val = GETSMS_AUTH_SMS;
//          } else {
//               ret_val = GETSMS_NOT_AUTH_SMS;  // authorization not valid yet
//               for (i = first_authorized_pos; i <= last_authorized_pos; i++) {
//                    if (gsm.ComparePhoneNumber(i, phone_number)) {
//                         // phone numbers are identical
//                         // authorization is OK
//                         // ---------------------------
//                         ret_val = GETSMS_AUTH_SMS;
//                         break;  // and finish authorization
//                    }
//               }
//          }
//     }
//     return (ret_val);
//}
//
//
///**********************************************************
//Method deletes SMS from the specified SMS position
//
//position:     SMS position <1..20>
//
//return:
//        ERROR ret. val:
//        ---------------
//        -1 - comm. line to the GSM module is not free
//        -2 - GSM module didn't answer in timeout
//        -3 - position must be > 0
//
//        OK ret val:
//        -----------
//        0 - SMS was not deleted
//        1 - SMS was deleted
//**********************************************************/
//char SMSGSM::DeleteSMS(byte position)
//{
//     char ret_val = -1;
//
//     if (position == 0) return (-3);
//     if (CLS_FREE != gsm.GetCommLineStatus()) return (ret_val);
//     gsm.SetCommLineStatus(CLS_ATCMD);
//     ret_val = 0; // not deleted yet
//
//     //send "AT+CMGD=XY" - where XY = position
//     gsm.SimpleWrite(F("AT+CMGD="));
//     gsm.SimpleWriteln((int)position);
//
//
//     // 5000 msec. for initial comm tmout
//     // 20 msec. for inter character timeout
//     switch (gsm.WaitResp(5000, 50, "OK")) {
//     case RX_TMOUT_ERR:
//          // response was not received in specific time
//          ret_val = -2;
//          break;
//
//     case RX_FINISHED_STR_RECV:
//          // OK was received => SMS deleted
//          ret_val = 1;
//          break;
//
//     case RX_FINISHED_STR_NOT_RECV:
//          // other response: e.g. ERROR => SMS was not deleted
//          ret_val = 0;
//          break;
//     }
//
//     gsm.SetCommLineStatus(CLS_FREE);
//     return (ret_val);
//}
//

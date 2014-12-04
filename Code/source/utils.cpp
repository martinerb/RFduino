//#include "Arduino.h"
//#include "utils.h"
//#include "errors.h"
//#include "config.h"
//
///**
// * Used to call something every 'time_period' milliseconds.
// *
// * This function is similar to TimedAction, but much more primitive.
// * http://playground.arduino.cc/Code/TimedAction
// *
// * See also IF_HAS_TIME_PASSED() macro.
// */
//bool has_time_passed(long time_period, long& last_passed) {
//    long now = millis();
//    if (now - last_passed > time_period) {
//        last_passed = now;
//        return true;
//    }
//    else
//        return false;
//}
//
///**
// * Get free memory (in bytes?).
// * From: http://forum.pololu.com/viewtopic.php?f=10&t=989
// */
//extern char __bss_end;
//extern char *__brkval;
//
//int get_free_memory()
//{
//  int free_memory;
//
//  if((int)__brkval == 0)
//    free_memory = ((int)&free_memory) - ((int)&__bss_end);
//  else
//    free_memory = ((int)&free_memory) - ((int)__brkval);
//
//  return free_memory;
//}
//
///**
// * Waits to get RESUME on the serial.
// * Returns 0 if it gets it,
// * ABORTED if it gets ABORT.
// */
///*
//errv_t wait_for_resume() {
//    while(1) {
//        errv_t ret = check_aborted(true);
//        if (ret == RESUMED) {
//            return 0;
//        }
//        else if (ret) {
//            return ret;
//        }
//    }
//}*/
//
///**
// * Check if we should abort whatever we ware doing right now.
// * Returns 0 if we should not abort, ABORTED if we should abort.
// *
// * If receive_resume is set to true, 'RESUME' is a valid cmd as well.
// */
///*
//errv_t check_aborted(bool receive_resume) {
//    int ret = 0;
//    if (Serial.available() > 0) {
//        char cmd[8+1];
//        // The conversion to String depends on having a trailing NULL!
//        memset(cmd, 0, 8+1);
//
//        // TODO this is not nice, other parsing code is in loop()
//        // move to one function somehow?
//        // XXX why do we use readBytes here and not readBytesUntil as in
//        // sketch.ino? Why 8 bytes and not MAX_COMMAND_LENGTH?
//        //    --> introduced in commit 612367ef
//        if(Serial.readBytes(cmd, 8)) {
//            String cmd_str = String(cmd);
//            if (cmd_str.equals("ABORT\r\n")) {
//                ret = ABORTED;
//            }
//            else if (receive_resume && cmd_str.equals("RESUME\r\n")) {
//                DEBUG_MSG_LN(String("Free mem: ") + String(get_free_memory()));
//                return RESUMED;
//            } else {
//                ERROR(strerror(INVALID_COMMAND));
//                DEBUG_MSG_LN(String("check_aborted: got '") + String(cmd) + String("'"));
//            }
//        }
//        // To save some bytes commenting out:
//        //else {
//        //    // we check above Serial.available() > 0 --> can we get here?
//        //    DEBUG_MSG_LN("check_aborted: something went wrong");
//        //}
//    }
//    else if (digitalRead(ABORT_BTN_PIN) == LOW) { // pull up inverts logic!
//        ret = ABORTED;
//    }
//    else if (receive_resume && digitalRead(RESUME_BTN_PIN) == LOW) {
//        return RESUMED;
//    }
//
//    return ret;
//}
//*/

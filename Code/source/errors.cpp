#include "Arduino.h"
#include "errors.h"

/**
 * Returns the name for an error code. Keep in sync with errors.h (until a better
 * method to implement this comes along.
 *
 * The awkward formatting is on purpose: This way you spot immediately if the
 * case value and the returned string do not match (typos, copy paste errors).
 */
String strerror(errv_t errno)
{
    switch(errno){
        case        DELAY_UNTIL_TIMEOUT:
            return "DELAY_UNTIL_TO";
        case        ABORTED:
            return "ABORTED";

        case        INVALID_COMMAND:
            return "INVAL_CMD";

        case        WEIGHT_NOT_STABLE:
            return "WEIGHT_NOT_STABLE";

        case        POURING_INACCURATE:
            return "INACCURATE";

        case        ADS1231_TIMEOUT_HIGH:
            return "ADS_TO_H";

        case        ADS1231_TIMEOUT_LOW:
            return "ADS_TO_L";

        case        ADS1231_WOULD_BLOCK:
            return "ADS_WB";

        case        ADS1231_STABLE_TIMEOUT:
            return "ADS_STAB_TO";

        default:
            return "UNDEF_" + String(errno);
    }
}

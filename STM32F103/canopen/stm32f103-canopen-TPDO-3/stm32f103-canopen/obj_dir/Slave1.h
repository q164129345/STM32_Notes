
/* File generated by gen_cfile.py. Should not be modified. */

#ifndef SLAVE1_H
#define SLAVE1_H

#include "data.h"

/* Prototypes of function provided by object dictionnary */
UNS32 Slave1_valueRangeTest (UNS8 typeValue, void * value);
const indextable * Slave1_scanIndexOD (CO_Data *d, UNS16 wIndex, UNS32 * errorCode);

/* Master node data struct */
extern CO_Data Slave1_Data;
extern UNS8 test_Value;	    /* Mapped at index 0x2000, subindex 0x00*/

#endif // SLAVE1_H

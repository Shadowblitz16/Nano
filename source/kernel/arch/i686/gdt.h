#pragma once

#define i8259_GDT_CODE_SEGMENT 0x08
#define i8259_GDT_DATA_SEGMENT 0x10

void i8259_GDT_Init();
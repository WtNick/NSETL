/*
 * arch.h
 *
 * Created: 9/2/2016 13:05:49
 *  Author: Nick
 */ 


#ifndef ARCH_H_
#define ARCH_H_
#include "compiler.h"

#define ASSERT(...)

#define CRITICAL_SECTION_ENTER()  __disable_irq()
#define CRITICAL_SECTION_LEAVE()  __enable_irq()

#define LEADINGZEROS(c) clz(c)
#define TRAILINGZEROS(c) ctz(c)

#define RESULT_SUCCESS 0
#define RESULT_FAILURE -1
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

#endif /* ARCH_H_ */
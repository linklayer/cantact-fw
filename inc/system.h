#ifndef _SYSTEM_H
#define _SYSTEM_H


void system_init(void);
void system_hex32(char *out, uint32_t val);
void system_irq_enable(void);
void system_irq_disable(void);


#endif

#ifndef __CLK_GATE_H__
#define __CLK_GATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_gate_t {
	const char * name;
	const char * parent;
	int shift;
	int invert;
	physical_addr_t phys;
	virtual_addr_t virt;
};

bool_t clk_gate_register(struct clk_gate_t * gclk);
bool_t clk_gate_unregister(struct clk_gate_t * gclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_GATE_H__ */

/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (c) Valentine Sinitsyn, 2014
 *
 * Authors:
 *  Valentine Sinitsyn <valentine.sinitsyn@gmail.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#ifndef _JAILHOUSE_ASM_VCPU_H
#define _JAILHOUSE_ASM_VCPU_H

#include <jailhouse/cell.h>
#include <jailhouse/cell-config.h>
#include <jailhouse/entry.h>
#include <jailhouse/paging.h>
#include <asm/percpu.h>
#include <asm/processor.h>

#define X86_CR0_HOST_STATE \
	(X86_CR0_PG | X86_CR0_WP | X86_CR0_NE | X86_CR0_ET | X86_CR0_TS | \
	 X86_CR0_MP | X86_CR0_PE)
#define X86_CR4_HOST_STATE	X86_CR4_PAE

struct vcpu_io_bitmap {
	u8 *data;
	u32 size;
};

struct vcpu_execution_state {
	u64 efer;
	u64 rflags;
	u16 cs;
	u64 rip;
};

struct vcpu_io_intercept {
	u16 port;
	unsigned int size;
	bool in;
	unsigned int inst_len;
	bool rep_or_str;
};

struct vcpu_mmio_intercept {
	u64 phys_addr;
	bool is_write;
};

int vcpu_vendor_init(void);

int vcpu_cell_init(struct cell *cell);
int vcpu_vendor_cell_init(struct cell *cell);

int vcpu_map_memory_region(struct cell *cell,
			   const struct jailhouse_memory *mem);
int vcpu_unmap_memory_region(struct cell *cell,
			     const struct jailhouse_memory *mem);
void vcpu_cell_exit(struct cell *cell);
void vcpu_vendor_cell_exit(struct cell *cell);

int vcpu_init(struct per_cpu *cpu_data);
void vcpu_exit(struct per_cpu *cpu_data);

void __attribute__((noreturn)) vcpu_activate_vmm(struct per_cpu *cpu_data);
void __attribute__((noreturn)) vcpu_deactivate_vmm(void);

void vcpu_handle_exit(struct per_cpu *cpu_data);

void vcpu_park(void);

void vcpu_nmi_handler(void);

void vcpu_tlb_flush(void);

/*
 * vcpu_map_inst() and vcpu_get_inst_bytes() contract:
 *
 * On input, *size gives the number of bytes to get.
 * On output, *size is the number of bytes available.
 *
 * If the function fails (returns NULL), *size is undefined.
 */

const u8 *vcpu_map_inst(const struct guest_paging_structures *pg_structs,
			unsigned long pc, unsigned int *size);

const u8 *vcpu_get_inst_bytes(const struct guest_paging_structures *pg_structs,
			      unsigned long pc, unsigned int *size);

void vcpu_skip_emulated_instruction(unsigned int inst_len);

void vcpu_vendor_get_cell_io_bitmap(struct cell *cell,
		                    struct vcpu_io_bitmap *out);

void vcpu_vendor_get_execution_state(struct vcpu_execution_state *x_state);
void vcpu_vendor_get_io_intercept(struct vcpu_io_intercept *io);
void vcpu_vendor_get_mmio_intercept(struct vcpu_mmio_intercept *mmio);

bool vcpu_get_guest_paging_structs(struct guest_paging_structures *pg_structs);

void vcpu_vendor_set_guest_pat(unsigned long val);

void vcpu_handle_hypercall(void);

bool vcpu_handle_io_access(void);
bool vcpu_handle_mmio_access(void);

bool vcpu_handle_msr_read(void);
bool vcpu_handle_msr_write(void);

void vcpu_handle_cpuid(void);

bool vcpu_handle_xsetbv(void);

void vcpu_reset(bool hard_reset);

#endif

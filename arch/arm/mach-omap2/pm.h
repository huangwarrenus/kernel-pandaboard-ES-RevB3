/*
 * OMAP2/3 Power Management Routines
 *
 * Copyright (C) 2008 Nokia Corporation
 * Jouni Hogander
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ARCH_ARM_MACH_OMAP2_PM_H
#define __ARCH_ARM_MACH_OMAP2_PM_H

#include <linux/err.h>

#include "powerdomain.h"

extern void *omap3_secure_ram_storage;
extern void omap3_pm_off_mode_enable(int);
extern void omap4_pm_oswr_mode_enable(int);
extern void omap4_pm_off_mode_enable(int);
extern void omap_sram_idle(void);
extern int omap_set_pwrdm_state(struct powerdomain *pwrdm, u32 state);
extern int omap3_idle_init(void);
extern int omap4_idle_init(void);
extern int omap_pm_clkdms_setup(struct clockdomain *clkdm, void *unused);
extern int (*omap_pm_suspend)(void);
extern int omap5_idle_init(void);
extern void omap_trigger_io_chain(void);

#ifdef CONFIG_PM
extern void omap4_device_set_state_off(u8 enable);
extern bool omap4_device_prev_state_off(void);
extern bool omap4_device_next_state_off(void);
extern void omap4_device_clear_prev_off_state(void);
#else
static inline void omap4_device_set_state_off(u8 enable)
{
}
static inline bool omap4_device_prev_state_off(void)
{
	return false;
}
static inline bool omap4_device_next_state_off(void)
{
	return false;
}
#endif

#if defined(CONFIG_PM_OPP)
extern int omap3_opp_init(void);
extern int omap4_opp_init(void);
#else
static inline int omap3_opp_init(void)
{
	return -EINVAL;
}
static inline int omap4_opp_init(void)
{
	return -EINVAL;
}
#endif

#ifdef CONFIG_PM
int omap4_pm_cold_reset(char *reason);
#else
int omap4_pm_cold_reset(char *reason)
{
	return -EINVAL;
}
#endif

/*
 * cpuidle mach specific parameters
 *
 * The board code can override the default C-states definition using
 * omap3_pm_init_cpuidle
 */
struct cpuidle_params {
	/*
	 * exit_latency = sleep + wake-up latencies of the MPU,
	 * which include the MPU itself and the peripherals needed
	 * for the MPU to execute instructions (e.g. main memory,
	 * caches, IRQ controller, MMU etc). Some of those peripherals
	 * can belong to other power domains than the MPU subsystem and so
	 * the corresponding latencies must be included in this figure.
	 */
	u32 exit_latency;
	/*
	 * target_residency: required amount of time in the C state
	 * to break even on energy cost
	 */
	u32 target_residency;
	/* validates the C-state on the given board */
	u8 valid;
};

#if defined(CONFIG_PM) && defined(CONFIG_CPU_IDLE)
extern void omap3_pm_init_cpuidle(struct cpuidle_params *cpuidle_board_params);
#else
static
inline void omap3_pm_init_cpuidle(struct cpuidle_params *cpuidle_board_params)
{
}
#endif

extern int omap3_pm_get_suspend_state(struct powerdomain *pwrdm);
extern int omap3_pm_set_suspend_state(struct powerdomain *pwrdm, int state);

#ifdef CONFIG_PM_DEBUG
extern u32 enable_off_mode;
#else
#define enable_off_mode 0
#endif

#if defined(CONFIG_PM_DEBUG) && defined(CONFIG_DEBUG_FS)
extern void pm_dbg_update_time(struct powerdomain *pwrdm, int prev);
#else
#define pm_dbg_update_time(pwrdm, prev) do {} while (0);
#endif /* CONFIG_PM_DEBUG */

/* 24xx */
extern void omap24xx_idle_loop_suspend(void);
extern unsigned int omap24xx_idle_loop_suspend_sz;

extern void omap24xx_cpu_suspend(u32 dll_ctrl, void __iomem *sdrc_dlla_ctrl,
					void __iomem *sdrc_power);
extern unsigned int omap24xx_cpu_suspend_sz;

/* 3xxx */
extern void omap34xx_cpu_suspend(int save_state);

/* omap3_do_wfi function pointer and size, for copy to SRAM */
extern void omap3_do_wfi(void);
extern unsigned int omap3_do_wfi_sz;
/* ... and its pointer from SRAM after copy */
extern void (*omap3_do_wfi_sram)(void);

/* save_secure_ram_context function pointer and size, for copy to SRAM */
extern int save_secure_ram_context(u32 *addr);
extern unsigned int save_secure_ram_context_sz;

extern void omap3_save_scratchpad_contents(void);

#define PM_RTA_ERRATUM_i608		(1 << 0)
#define PM_SDRC_WAKEUP_ERRATUM_i583	(1 << 1)

#if defined(CONFIG_PM) && defined(CONFIG_ARCH_OMAP3)
extern u16 pm34xx_errata;
#define IS_PM34XX_ERRATUM(id)		(pm34xx_errata & (id))
extern void enable_omap3630_toggle_l2_on_restore(void);
#else
#define IS_PM34XX_ERRATUM(id)		0
static inline void enable_omap3630_toggle_l2_on_restore(void) { }
#endif		/* defined(CONFIG_PM) && defined(CONFIG_ARCH_OMAP3) */

#define PM_OMAP4_ROM_IVAHD_TESLA_ERRATUM	(1 << 0)
#define PM_OMAP4_ROM_L3INSTR_ERRATUM		(1 << 1)

#if defined(CONFIG_PM) && defined(CONFIG_ARCH_OMAP4)
extern u16 pm44xx_errata;
#define IS_PM44XX_ERRATUM(id)		(pm44xx_errata & (id))
#else
#define IS_PM44XX_ERRATUM(id)		0
#endif

#ifdef CONFIG_OMAP_SMARTREFLEX
extern int omap_devinit_smartreflex(void);
extern void omap_enable_smartreflex_on_init(void);
#else
static inline int omap_devinit_smartreflex(void)
{
	return -EINVAL;
}

static inline void omap_enable_smartreflex_on_init(void) {}
#endif

/**
 * struct omap_pmic_map - Describe the OMAP PMIC data for OMAP
 * @name:	name of the voltage domain
 * @pmic_data:	pmic data associated with it
 * @omap_chip:	initialize with OMAP_CHIP_INIT the OMAP chips this data maps to
 * @special_action: callback for any specific action to take for that map
 *
 * Since we support multiple PMICs each potentially functioning on multiple
 * OMAP devices, we describe the parameters in a map allowing us to reuse the
 * data as necessary.
 */
struct omap_pmic_map {
	char			*name;
	struct omap_voltdm_pmic	*pmic_data;
	int			(*special_action)(struct voltagedomain *);
};

#ifdef CONFIG_PM
extern int omap_pmic_register_data(struct omap_pmic_map *map);
#else
static inline int omap_pmic_register_data(struct omap_pmic_map *map)
{
	return -EINVAL;
}
#endif
extern void omap_init_all_pmic(void);

#ifdef CONFIG_TWL4030_CORE
extern int omap_twl4030_init(void);
extern int omap3_twl_set_sr_bit(bool enable);
#else
static inline int omap_twl4030_init(void)
{
	return -EINVAL;
}
#endif

#ifdef CONFIG_MFD_PALMAS
extern int omap_palmas_init(void);
#else
static inline int omap_palmas_init(void)
{
	return -EINVAL;
}
#endif

#ifdef CONFIG_PM
extern int omap_sar_save(void);
extern void omap_sar_overwrite(void);
#else
static inline void omap_sar_save(void)
{
}
static inline void omap_sar_overwrite(void)
{
}
#endif

#ifdef CONFIG_OMAP_TPS6236X
extern int omap_tps6236x_board_setup(bool use_62361, int gpio_vsel0,
					int gpio_vsel1, int pull0, int pull1);
extern int omap_tps6236x_init(void);

extern int omap_tps6236x_update(char *name, u16 old_chip_id, u16 new_chip_id);
#else
static inline int omap_tps6236x_board_setup(bool use_62361, int gpio_vsel0,
					int gpio_vsel1, int pull0, int pull1)
{
	return -EINVAL;
}
static inline int omap_tps6236x_init(void)
{
	return -EINVAL;
}
static inline int omap_tps6236x_update(char *name, u16 old_chip_id,
		u16 new_chip_id)
{
	return -EINVAL;
}
#endif

extern int omap4_ldo_trim_configure(void);

#ifdef CONFIG_PM
extern bool omap_pm_is_ready_status;
/**
 * omap_pm_is_ready() - tells if OMAP pm framework is done it's initialization
 *
 * In few cases, to sequence operations properly, we'd like to know if OMAP's PM
 * framework has completed all it's expected initializations.
 */
static inline bool omap_pm_is_ready(void)
{
	return omap_pm_is_ready_status;
}
extern void omap_pm_setup_oscillator(u32 tstart, u32 tshut);
extern void omap_pm_get_oscillator(u32 *tstart, u32 *tshut);
#else
static inline bool omap_pm_is_ready(void)
{
	return false;
}
static inline void omap_pm_setup_oscillator(u32 tstart, u32 tshut) { }
static inline void omap_pm_get_oscillator(u32 *tstart, u32 *tshut) { }
#endif
#endif

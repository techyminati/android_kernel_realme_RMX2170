/***************************************************************
** Copyright (C),  2018,  OPPO Mobile Comm Corp.,  Ltd
** VENDOR_EDIT
** File : dsi_oppo_support.c
** Description : display driver private management
** Version : 1.0
** Date : 2018/03/17
** Author : Jie.Hu@PSW.MM.Display.Stability
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**   Hu.Jie          2018/03/17        1.0           Build this moudle
******************************************************************/
#include <linux/dsi_oppo_support.h>
#include <soc/oppo/boot_mode.h>
#include <soc/oppo/oppo_project.h>
#include <soc/oppo/device_info.h>
#include <soc/oppo/boot_mode.h>
#include <linux/notifier.h>
#include <linux/module.h>

static enum oppo_display_support_list  oppo_display_vendor = OPPO_DISPLAY_UNKNOW;
static enum oppo_display_power_status oppo_display_status = OPPO_DISPLAY_POWER_OFF;
static enum oppo_display_scene oppo_siaplay_save_scene = OPPO_DISPLAY_NORMAL_SCENE;
#ifdef VENDOR_EDIT
/*LiPing-M@PSW.MM.Display.LCD.Stable,2019-1-10 add for panel id*/
int oppo_panel_id;
#endif /* VENDOR_EDIT */

static BLOCKING_NOTIFIER_HEAD(oppo_display_notifier_list);

int oppo_display_register_client(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&oppo_display_notifier_list,
						nb);
}
EXPORT_SYMBOL(oppo_display_register_client);


int oppo_display_unregister_client(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&oppo_display_notifier_list,
						  nb);
}
EXPORT_SYMBOL(oppo_display_unregister_client);

static int oppo_display_notifier_call_chain(unsigned long val, void *v)
{
	return blocking_notifier_call_chain(&oppo_display_notifier_list, val,
					    v);
}

bool is_oppo_correct_display(enum oppo_display_support_list lcd_name) {
	return (oppo_display_vendor == lcd_name ? true:false);
}

bool is_silence_reboot(void) {
	if((MSM_BOOT_MODE__SILENCE == get_boot_mode()) || (MSM_BOOT_MODE__SAU == get_boot_mode())) {
		return true;
	} else {
		return false;
	}
}

int set_oppo_display_vendor(const char * display_name) {
	if (display_name == NULL)
		return -1;

	oppo_display_vendor = OPPO_18081_SAMSUNG_DISPLAY_FHD_CMD_PANEL;
	if (!strcmp(display_name,"dsi_oppo18097samsung_ams641rw01_1080_2340_cmd_display")) {
		if (oppo_panel_id == 0) {
			register_device_proc("lcd", "AMS641RW01", "samsung1024");
		}
	} else if (!strcmp(display_name,"dsi_oppo19031samsung_ams641rw01_1080_2400_cmd_display")) {
		register_device_proc("lcd", "AMS655LSI60", "samsung1024");
	} else if (!strcmp(display_name,"dsi_oppo19031samsung_ams641rw01_1080_2400_61fps_cmd_display")) {
		register_device_proc("lcd", "AMS655UE01", "samsung1024");
	} else if (!strcmp(display_name,"dsi_oppo19031samsung_ams655ue01_1080_2400_61fps_cmd_display")) {
		register_device_proc("lcd", "AMS655MGN61", "samsung1024");
	} else if (!strcmp(display_name,"dsi_oppo19111samsung_ams641rw16_1080_2340_cmd_display")) {
		register_device_proc("lcd", "AMS641RW16", "samsung1024");
	#ifdef ODM_LQ_EDIT
	/*zhangjialong@ODM_LQ@Multimedia.Dispaly,2019/11/05,add for  lcd name node*/
	} else if (!strcmp(display_name,"dsi_hx83112a_dsbj_vid_display")) {
		register_device_proc("lcd", "hx83112a", "dsjm");
	} else if (!strcmp(display_name,"dsi_nt36672c_boe_video_display")) {
		register_device_proc("lcd", "nt36672c", "boe vdo mode");
	} else if (!strcmp(display_name,"dsi_nt36672c_jdi_video_display")) {
		register_device_proc("lcd", "nt36672c", "jdi vdo mode");
	#ifdef ODM_TARGET_DEVICE_206B1
	} else if (!strcmp(display_name,"dsi_ams643xf01_samsung_amoled_cmd_display")) {
		register_device_proc("lcd", "ams643xf01", "samsung1024");
	} else if (!strcmp(display_name,"dsi_s6e8fc1x01_samsung_amoled_video_display")) {
		register_device_proc("lcd", "s6e8fc1x01", "samsung1024");
	#endif /*ODM_TARGET_DEVICE_206B1*/
	#endif /*ODM_LQ_EDIT*/
	}  else {
		oppo_display_vendor = OPPO_DISPLAY_UNKNOW;
		pr_err("%s panel vendor info set failed!", __func__);
	}

	return 0;
}

void notifier_oppo_display_early_status(enum oppo_display_power_status power_status) {
	int blank;
	OPPO_DISPLAY_NOTIFIER_EVENT oppo_notifier_data;
	switch (power_status) {
		case OPPO_DISPLAY_POWER_ON:
			blank = OPPO_DISPLAY_POWER_ON;
			oppo_notifier_data.data = &blank;
			oppo_notifier_data.status = OPPO_DISPLAY_POWER_ON;
			oppo_display_notifier_call_chain(OPPO_DISPLAY_EARLY_EVENT_BLANK,
						     &oppo_notifier_data);
			break;
		case OPPO_DISPLAY_POWER_DOZE:
			blank = OPPO_DISPLAY_POWER_DOZE;
			oppo_notifier_data.data = &blank;
			oppo_notifier_data.status = OPPO_DISPLAY_POWER_DOZE;
			oppo_display_notifier_call_chain(OPPO_DISPLAY_EARLY_EVENT_BLANK,
						     &oppo_notifier_data);
			break;
		case OPPO_DISPLAY_POWER_DOZE_SUSPEND:
			blank = OPPO_DISPLAY_POWER_DOZE_SUSPEND;
			oppo_notifier_data.data = &blank;
			oppo_notifier_data.status = OPPO_DISPLAY_POWER_DOZE_SUSPEND;
			oppo_display_notifier_call_chain(OPPO_DISPLAY_EARLY_EVENT_BLANK,
						     &oppo_notifier_data);
			break;
		case OPPO_DISPLAY_POWER_OFF:
			blank = OPPO_DISPLAY_POWER_OFF;
			oppo_notifier_data.data = &blank;
			oppo_notifier_data.status = OPPO_DISPLAY_POWER_OFF;
			oppo_display_notifier_call_chain(OPPO_DISPLAY_EARLY_EVENT_BLANK,
						     &oppo_notifier_data);
			break;
		default:
			break;
		}
}

void notifier_oppo_display_status(enum oppo_display_power_status power_status) {
	int blank;
	OPPO_DISPLAY_NOTIFIER_EVENT oppo_notifier_data;
	switch (power_status) {
		case OPPO_DISPLAY_POWER_ON:
			blank = OPPO_DISPLAY_POWER_ON;
			oppo_notifier_data.data = &blank;
			oppo_notifier_data.status = OPPO_DISPLAY_POWER_ON;
			oppo_display_notifier_call_chain(OPPO_DISPLAY_EVENT_BLANK,
						     &oppo_notifier_data);
			break;
		case OPPO_DISPLAY_POWER_DOZE:
			blank = OPPO_DISPLAY_POWER_DOZE;
			oppo_notifier_data.data = &blank;
			oppo_notifier_data.status = OPPO_DISPLAY_POWER_DOZE;
			oppo_display_notifier_call_chain(OPPO_DISPLAY_EVENT_BLANK,
						     &oppo_notifier_data);
			break;
		case OPPO_DISPLAY_POWER_DOZE_SUSPEND:
			blank = OPPO_DISPLAY_POWER_DOZE_SUSPEND;
			oppo_notifier_data.data = &blank;
			oppo_notifier_data.status = OPPO_DISPLAY_POWER_DOZE_SUSPEND;
			oppo_display_notifier_call_chain(OPPO_DISPLAY_EVENT_BLANK,
						     &oppo_notifier_data);
			break;
		case OPPO_DISPLAY_POWER_OFF:
			blank = OPPO_DISPLAY_POWER_OFF;
			oppo_notifier_data.data = &blank;
			oppo_notifier_data.status = OPPO_DISPLAY_POWER_OFF;
			oppo_display_notifier_call_chain(OPPO_DISPLAY_EVENT_BLANK,
						     &oppo_notifier_data);
			break;
		default:
			break;
		}
}

void set_oppo_display_power_status(enum oppo_display_power_status power_status) {
	oppo_display_status = power_status;
}

enum oppo_display_power_status get_oppo_display_power_status(void) {
	return oppo_display_status;
}
EXPORT_SYMBOL(get_oppo_display_power_status);

void set_oppo_display_scene(enum oppo_display_scene display_scene) {
	oppo_siaplay_save_scene = display_scene;
}

enum oppo_display_scene get_oppo_display_scene(void) {
	return oppo_siaplay_save_scene;
}

EXPORT_SYMBOL(get_oppo_display_scene);

bool is_oppo_display_support_feature(enum oppo_display_feature feature_name) {
	bool ret = false;
	switch (feature_name) {
		case OPPO_DISPLAY_HDR:
			ret = false;
			break;
		case OPPO_DISPLAY_SEED:
			ret = true;
			break;
		case OPPO_DISPLAY_HBM:
			ret = true;
			break;
		case OPPO_DISPLAY_LBR:
			ret = true;
			break;
		case OPPO_DISPLAY_AOD:
			ret = true;
			break;
		case OPPO_DISPLAY_ULPS:
			ret = false;
			break;
		case OPPO_DISPLAY_ESD_CHECK:
			ret = true;
			break;
		case OPPO_DISPLAY_DYNAMIC_MIPI:
			ret = true;
			break;
		case OPPO_DISPLAY_PARTIAL_UPDATE:
			ret = false;
			break;
		default:
			break;
	}
	return ret;
}



/**
 * @file
 *
 * @date May 7, 2020
 * @author Anton Bondarev
 */

#ifndef THIRD_PARTY_BSP_DIALOG_DA14681_DA14681_ROM_H_
#define THIRD_PARTY_BSP_DIALOG_DA14681_DA14681_ROM_H_


extern void platform_initialization(void);

extern void flash_identify(void);
extern void flash_erase(void);
extern void flash_write(void);
extern void flash_read(void);

extern void hci_init(void);
extern void hci_reset(void);
extern void hci_send_2_host(void);
extern void hci_send_2_controller(void);

extern void rwble_hl_reset(void);
extern void rwble_hl_send_message(void);
extern void rwip_check_wakeup_boundary(void);
extern void rwip_init(void);
extern void rwip_reset(void);
extern void rwip_version(void);
extern void rwip_schedule(void);
extern void rwip_sleep(void);
extern void rwip_prevent_sleep_set(void);
extern void rwip_wakeup(void);
extern void rwip_prevent_sleep_clear(void);
extern void rwip_wakeup_end(void);
extern void rwip_wakeup_delay_set(void);
extern void rwip_sleep_enable(void);
extern void rwip_ext_wakeup_enable(void);
extern void rwble_init(void);
extern void rwble_reset(void);
extern void rwble_version(void);
extern void rwble_send_message(void);

extern void llc_state_handler(void);
extern void llc_default_handler(void);
extern void llm_local_cmds(void);
extern void llm_local_le_feats(void);
extern void llm_local_le_states(void);
extern void llm_state_handler(void);
extern void llm_default_handler(void);

extern void gapm_init(void);
extern void gapm_init_attr(void);
extern void gapm_get_operation(void);
extern void gapm_get_requester(void);
extern void gapm_reschedule_operation(void);
extern void gapm_send_complete_evt(void);
extern void gapm_send_error_evt(void);
extern void gapm_con_create(void);
extern void gapm_con_enable(void);
extern void gapm_con_cleanup(void);
extern void gapm_get_id_from_task(void);
extern void gapm_get_task_from_id(void);
extern void gapm_is_disc_connection(void);
extern void gapm_adv_sanity(void);
extern void gapm_adv_op_sanity(void);
extern void gapm_set_adv_mode(void);
extern void gapm_set_adv_data(void);
extern void gapm_execute_adv_op(void);
extern void gapm_scan_op_sanity(void);
extern void gapm_set_scan_mode(void);
extern void gapm_execute_scan_op(void);
extern void gapm_connect_op_sanity(void);
extern void gapm_basic_hci_cmd_send(void);
extern void gapm_execute_connect_op(void);
extern void gapm_get_role(void);
extern void gapm_get_ad_type_flag(void);
extern void gapm_add_to_filter(void);
extern void gapm_is_filtered(void);
extern void gapm_update_air_op_state(void);
extern void gapm_get_irk(void);
extern void gapm_get_bdaddr(void);

extern void llc_pdu_send_func(void);
extern void lld_data_tx_check_func(void);
extern void lld_data_tx_prog_func(void);
extern void lld_evt_deferred_elt_pop(void);
extern void llcp_unknown_rsp_handler(void);

extern void gapc_param_update_cmd_handler(void);
extern void gapc_encrypt_cmd_handler(void);
extern void llm_local_le_feats_addr(void);

#endif /* THIRD_PARTY_BSP_DIALOG_DA14681_DA14681_ROM_H_ */

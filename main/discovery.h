#ifndef DISCOVERY_H_
#define DISCOVERY_H_

void bt_app_gap_init(void);
void bt_app_gap_start_discovery(void);
void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);


#endif /* DISCOVERY_H_ */

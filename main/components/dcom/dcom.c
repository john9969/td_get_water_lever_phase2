#include "dcom.h"
#include "esp_log.h"
static const char* TAG = "DCOM";
static void dcom_task(void * arx);
static bool is_state_changed(STATE old_state, STATE new_state);
Dcom dcom = {.dcom_gpio = &wifi_poweron_gpio, .state = STATE_ON};
void dcom_init(Dcom *p_dcom){	
	p_dcom->dcom_task = NULL;
	xTaskCreate(dcom_task,"Dcom task",1024,(void*)p_dcom,3,&p_dcom->dcom_task);
}

void dcom_deinit(Dcom *p_dcom){
	vTaskDelete(p_dcom->dcom_task);
}

void dcom_set_state(Dcom* dcom,STATE state){
	dcom->state = state;
}

void dcom_task(void* arx)
{
	Dcom * p_dcom = (Dcom*) arx;
	STATE old_state = STATE_OFF;
	while (1)
	{
		STATE current_state = p_dcom->state;
		if(is_state_changed (old_state, current_state)){
			switch (current_state)
			{
			case STATE_OFF:
				DCOM_OFF
				ESP_LOGI(TAG,"DCOM OFF");
				break;
			case STATE_ON:
				DCOM_ON
				ESP_LOGI(TAG,"DCOM ON");
				break;
			default:
				break;
			}
			old_state = current_state;
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

bool is_state_changed(STATE old_state, STATE new_state){
	return old_state != new_state;
}
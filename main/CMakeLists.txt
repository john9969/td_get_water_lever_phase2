# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.
file(GLOB board_source "board/*.c"
			"board/gpio/*.c"
			"board/nvc_flash/*.c"
			"board/timer/*.c"
			"board/pcf8563/*.c"
			"board/uart_hw/*.c"
			"board/adc/*.c"
			)
file(GLOB app_source "app/*.c"
			"app/wifi_app/*.c"
			"app/storage/*c"
			"app/measuring_app/*c"
			"app/config_app/*c"
			)
file (GLOB app_components "components/*c"
			"components/mywifi/*c"
			"components/button/*c"
			"components/dcom/*c"
			"components/rtc/*c"
			"components/file_storage/*c"
			"components/led/*c"
			"components/lazer_sensor/*c"
)
file (GLOB app_service "service/*c"
			"service/i2c/*c"
			"service/http/*c"
			"service/tiny-json/*c"
	)

idf_component_register(
    SRCS 	${board_source}
			${app_source}
			${app_components}
			${app_service}
			main.c
    	 		 # list the source files of this component
    INCLUDE_DIRS   "board" # optional, add here public include directories
						"board/gpio"
						"board/timer"
						"board/nvc_flash"
						"board/uart_hw"
						"board/pcf8563"
						"board/adc"
						"app"
						"app/wifi_app"
						"app/storage"
						"app/measuring_app"
						"app/config_app"
						"components"
						"components/mywifi"
						"components/button"
						"components/dcom"
						"components/rtc"
						"components/led"
						"components/file_storage"
						"components/lazer_sensor"
						"service"
						"service/i2c"
						"service/http"
						"service/tiny-json"
    EMBED_FILES   # "html_source/index.html"
    			  # "html_source/img_avatar2.png"
    			  # "html_source/chart.html"
    			  # "html_source/highcharts.js"
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
	REQUIRES            nvs_flash
						esp_timer
						esp_wifi
						wpa_supplicant
						mqtt
						spiffs
						esp_http_client
						# optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
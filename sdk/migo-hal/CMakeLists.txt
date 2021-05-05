idf_component_register (
    SRC_DIRS 
        "src"
                    
    INCLUDE_DIRS 
        "include"
    
    PRIV_REQUIRES nvs_flash app_update esp_adc_cal fatfs spiffs
)
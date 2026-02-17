/**
 * @file tx335_main.c
 * @brief Headless driver for TX335 treadmill using ESP32-WROVER-KIT_v4.1
 * 
 * Pin Configuration:
 * - Pin 3: GND (Ground)
 * - Pin 4: PWR 12-15V (Power - external)
 * - Pin 5: PWM output (speed control)
 * - Pin 7: ENABLE output (motor enable)
 * - Pin 8: MTR_PULSE input (motor pulse feedback)
 * 
 * External components: LDO and level shifter
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "tx335_treadmill.h"

static const char *TAG = "TX335";

/* Pin definitions for treadmill interface */
#define PWM_GPIO        GPIO_NUM_5      // PWM output for speed control
#define ENABLE_GPIO     GPIO_NUM_7      // Enable/disable motor
#define MTR_PULSE_GPIO  GPIO_NUM_8      // Motor pulse feedback (input)

/* PWM Configuration */
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT   // 13-bit resolution
#define LEDC_FREQUENCY          (5000)              // 5 kHz PWM frequency

/* Motor pulse counter */
static volatile uint32_t motor_pulse_count = 0;

/**
 * @brief GPIO interrupt handler for motor pulse counting
 */
static void IRAM_ATTR motor_pulse_isr_handler(void* arg)
{
    motor_pulse_count++;
}

/**
 * @brief Initialize GPIO pins for treadmill control
 */
static esp_err_t init_gpio(void)
{
    esp_err_t ret;
    
    // Configure ENABLE pin as output
    gpio_config_t enable_conf = {
        .pin_bit_mask = (1ULL << ENABLE_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ret = gpio_config(&enable_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ENABLE GPIO: %d", ret);
        return ret;
    }
    
    // Initially disable motor
    gpio_set_level(ENABLE_GPIO, 0);
    ESP_LOGI(TAG, "ENABLE GPIO configured on pin %d (initially disabled)", ENABLE_GPIO);
    
    // Configure MTR_PULSE pin as input with interrupt on rising edge
    gpio_config_t pulse_conf = {
        .pin_bit_mask = (1ULL << MTR_PULSE_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    ret = gpio_config(&pulse_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure MTR_PULSE GPIO: %d", ret);
        return ret;
    }
    
    // Install GPIO ISR service
    ret = gpio_install_isr_service(0);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to install GPIO ISR service: %d", ret);
        return ret;
    }
    
    // Add ISR handler for motor pulse
    ret = gpio_isr_handler_add(MTR_PULSE_GPIO, motor_pulse_isr_handler, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add ISR handler for MTR_PULSE: %d", ret);
        return ret;
    }
    
    ESP_LOGI(TAG, "MTR_PULSE GPIO configured on pin %d (interrupt enabled)", MTR_PULSE_GPIO);
    
    return ESP_OK;
}

/**
 * @brief Initialize PWM (LEDC) for speed control
 */
static esp_err_t init_pwm(void)
{
    esp_err_t ret;
    
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer: %d", ret);
        return ret;
    }
    
    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PWM_GPIO,
        .duty           = 0,  // Start with 0% duty cycle
        .hpoint         = 0
    };
    ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC channel: %d", ret);
        return ret;
    }
    
    ESP_LOGI(TAG, "PWM configured on pin %d at %d Hz", PWM_GPIO, LEDC_FREQUENCY);
    
    return ESP_OK;
}

/**
 * @brief Set treadmill speed (0-100%)
 * @param speed_percent Speed percentage (0-100)
 */
void set_treadmill_speed(uint8_t speed_percent)
{
    if (speed_percent > 100) {
        speed_percent = 100;
    }
    
    // Calculate duty cycle (13-bit resolution: 0-8191)
    uint32_t duty = (speed_percent * 8191) / 100;
    
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    
    ESP_LOGI(TAG, "Speed set to %d%% (duty: %lu)", speed_percent, duty);
}

/**
 * @brief Enable or disable the treadmill motor
 * @param enable true to enable, false to disable
 */
void set_treadmill_enable(bool enable)
{
    gpio_set_level(ENABLE_GPIO, enable ? 1 : 0);
    ESP_LOGI(TAG, "Motor %s", enable ? "ENABLED" : "DISABLED");
}

/**
 * @brief Get the current motor pulse count
 * @return Number of pulses counted
 */
uint32_t get_motor_pulse_count(void)
{
    return motor_pulse_count;
}

/**
 * @brief Reset the motor pulse counter
 */
void reset_motor_pulse_count(void)
{
    motor_pulse_count = 0;
    ESP_LOGI(TAG, "Motor pulse count reset");
}

/**
 * @brief Demo task to show treadmill control
 */
static void treadmill_demo_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting treadmill demo...");
    
    // Wait a bit before starting
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // Enable motor
    set_treadmill_enable(true);
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // Ramp up speed slowly
    ESP_LOGI(TAG, "Ramping up speed...");
    for (int speed = 0; speed <= 30; speed += 5) {
        set_treadmill_speed(speed);
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI(TAG, "Motor pulses: %lu", get_motor_pulse_count());
    }
    
    // Hold at 30% for a while
    ESP_LOGI(TAG, "Holding at 30%% speed...");
    vTaskDelay(pdMS_TO_TICKS(10000));
    ESP_LOGI(TAG, "Motor pulses: %lu", get_motor_pulse_count());
    
    // Ramp down
    ESP_LOGI(TAG, "Ramping down speed...");
    for (int speed = 30; speed >= 0; speed -= 5) {
        set_treadmill_speed(speed);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
    // Disable motor
    set_treadmill_enable(false);
    
    ESP_LOGI(TAG, "Demo complete. Total motor pulses: %lu", get_motor_pulse_count());
    
    // Keep task alive
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 TX335 Treadmill Driver starting...");
    ESP_LOGI(TAG, "Pin Configuration:");
    ESP_LOGI(TAG, "  PWM (speed):   GPIO %d", PWM_GPIO);
    ESP_LOGI(TAG, "  ENABLE:        GPIO %d", ENABLE_GPIO);
    ESP_LOGI(TAG, "  MTR_PULSE:     GPIO %d", MTR_PULSE_GPIO);
    ESP_LOGI(TAG, "  GND:           Pin 3");
    ESP_LOGI(TAG, "  PWR (12-15V):  Pin 4 (external)");
    ESP_LOGI(TAG, "Note: Using external LDO and level shifter");
    
    // Initialize GPIO
    esp_err_t ret = init_gpio();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO initialization failed!");
        return;
    }
    
    // Initialize PWM
    ret = init_pwm();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PWM initialization failed!");
        return;
    }
    
    ESP_LOGI(TAG, "Initialization complete");
    
    // Create demo task
    xTaskCreate(treadmill_demo_task, "treadmill_demo", 4096, NULL, 5, NULL);
}

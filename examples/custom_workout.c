/**
 * @file custom_workout.c
 * @brief Example of a custom workout program for the TX335 treadmill
 * 
 * This example demonstrates how to create a custom workout program
 * with different speed intervals.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "tx335_treadmill.h"

static const char *TAG = "WORKOUT";

/**
 * @brief Workout interval structure
 */
typedef struct {
    uint8_t speed;      // Speed percentage (0-100)
    uint32_t duration;  // Duration in seconds
    const char *name;   // Name of interval
} workout_interval_t;

/**
 * @brief Example interval workout program
 * 
 * A simple interval training program:
 * - 5 min warm-up at 20%
 * - 2 min at 40%
 * - 1 min at 60%
 * - 2 min at 40%
 * - 5 min cool-down at 20%
 */
static const workout_interval_t workout_program[] = {
    {20, 300, "Warm-up"},
    {40, 120, "Moderate"},
    {60, 60,  "High intensity"},
    {40, 120, "Moderate"},
    {20, 300, "Cool-down"},
    {0,  0,   NULL}  // End marker
};

/**
 * @brief Execute a workout program
 */
void execute_workout(const workout_interval_t *program)
{
    ESP_LOGI(TAG, "Starting workout program...");
    
    // Enable motor
    set_treadmill_enable(true);
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // Reset pulse counter
    reset_motor_pulse_count();
    
    // Execute each interval
    int interval_num = 1;
    const workout_interval_t *interval = program;
    
    while (interval->name != NULL) {
        ESP_LOGI(TAG, "Interval %d: %s - %d%% for %lu seconds",
                 interval_num, interval->name, 
                 interval->speed, interval->duration);
        
        // Set speed
        set_treadmill_speed(interval->speed);
        
        // Run for duration, logging every 10 seconds
        for (uint32_t elapsed = 0; elapsed < interval->duration; elapsed += 10) {
            vTaskDelay(pdMS_TO_TICKS(10000));
            uint32_t pulses = get_motor_pulse_count();
            ESP_LOGI(TAG, "  Time: %lu/%lu sec, Pulses: %lu",
                     elapsed + 10, interval->duration, pulses);
        }
        
        interval_num++;
        interval++;
    }
    
    // Stop motor
    set_treadmill_speed(0);
    vTaskDelay(pdMS_TO_TICKS(2000));
    set_treadmill_enable(false);
    
    uint32_t total_pulses = get_motor_pulse_count();
    ESP_LOGI(TAG, "Workout complete! Total pulses: %lu", total_pulses);
}

/**
 * @brief Simple speed control example
 */
void simple_speed_control_example(void)
{
    ESP_LOGI(TAG, "Simple speed control example");
    
    // Enable motor
    set_treadmill_enable(true);
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // Start at 25% speed
    set_treadmill_speed(25);
    ESP_LOGI(TAG, "Running at 25%% for 30 seconds...");
    vTaskDelay(pdMS_TO_TICKS(30000));
    
    // Increase to 50%
    set_treadmill_speed(50);
    ESP_LOGI(TAG, "Running at 50%% for 30 seconds...");
    vTaskDelay(pdMS_TO_TICKS(30000));
    
    // Stop
    set_treadmill_speed(0);
    vTaskDelay(pdMS_TO_TICKS(2000));
    set_treadmill_enable(false);
    
    ESP_LOGI(TAG, "Done! Total pulses: %lu", get_motor_pulse_count());
}

/**
 * @brief Pulse monitoring example
 * 
 * This example shows how to monitor motor pulses to calculate actual speed
 */
void pulse_monitoring_example(void)
{
    ESP_LOGI(TAG, "Pulse monitoring example");
    
    set_treadmill_enable(true);
    reset_motor_pulse_count();
    
    // Run at constant speed and measure pulses
    set_treadmill_speed(40);
    
    for (int i = 0; i < 6; i++) {
        uint32_t start_pulses = get_motor_pulse_count();
        vTaskDelay(pdMS_TO_TICKS(10000));  // Wait 10 seconds
        uint32_t end_pulses = get_motor_pulse_count();
        
        uint32_t pulses_per_10s = end_pulses - start_pulses;
        ESP_LOGI(TAG, "Pulses in last 10s: %lu (avg: %.1f/sec)",
                 pulses_per_10s, pulses_per_10s / 10.0f);
    }
    
    set_treadmill_speed(0);
    set_treadmill_enable(false);
}

/**
 * @brief Custom workout task
 * 
 * Replace the demo task in tx335_main.c with this to use custom workouts
 */
void custom_workout_task(void *pvParameters)
{
    // Wait for system to stabilize
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // Choose which example to run:
    
    // Option 1: Run the full workout program
    execute_workout(workout_program);
    
    // Option 2: Simple speed control
    // simple_speed_control_example();
    
    // Option 3: Pulse monitoring
    // pulse_monitoring_example();
    
    // Keep task alive
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}

/* To use this in your application:
 * 
 * 1. Copy this file to your main/ directory
 * 2. Add it to main/CMakeLists.txt:
 *    idf_component_register(SRCS "tx335_main.c" "custom_workout.c"
 *                           INCLUDE_DIRS ".")
 * 3. In tx335_main.c, replace the demo task with:
 *    xTaskCreate(custom_workout_task, "custom_workout", 4096, NULL, 5, NULL);
 */

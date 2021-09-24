// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Globals
static int shared_var = 0;

static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;
//*****************************************************************************
// Tasks

// Increment shared variable (the wrong way)
void incTask(void *parameters)
{

    int local_var;
    char taskname = *(char *)parameters;

    // Loop forever
    while (1)
    {

        // Roundabout way to do "shared_var++" randomly and poorly
        local_var = shared_var;
        local_var++;
        vTaskDelay(500 / portTICK_PERIOD_MS);
        // vTaskDelay(random(100, 5000) / portTICK_PERIOD_MS);
        shared_var = local_var;

        Serial.print(taskname);
        Serial.print(":");
        // Serial.println(shared_var);

        // Print out new shared variable
        Serial.println(shared_var);

        TaskHandle_t h = xTaskGetCurrentTaskHandle();

        if(shared_var == 50 && h == task_1)
        {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup()
{

    // Hack to kinda get randomness
    randomSeed(analogRead(0));

    // Configure Serial
    Serial.begin(115200);

    // Wait a moment to start (so we don't miss Serial output)
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println();
    Serial.println("---FreeRTOS Race Condition Demo---");

    char taskname1[] = "1";
    char taskname2[] = "2";
    // Start task 1
    xTaskCreatePinnedToCore(incTask,
                            "Increment Task 1",
                            1024,
                            (void *)&taskname1,
                            1,
                            &task_1,
                            app_cpu);

    // Start task 2
    xTaskCreatePinnedToCore(incTask,
                            "Increment Task 2",
                            1024,
                            (void *)&taskname2,
                            1,
                            &task_2,
                            app_cpu);

    // Delete "setup and loop" task
    vTaskDelete(NULL);
}

void loop()
{
    // Execution should never get here
}
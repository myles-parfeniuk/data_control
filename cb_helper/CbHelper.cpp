#include "CbHelper.hpp"

bool CbHelper::initialized = false;
TaskHandle_t CbHelper::main_cb_task_hdl = NULL;
SemaphoreHandle_t CbHelper::pending_cb_list_mutex = xSemaphoreCreateRecursiveMutex();
std::vector<std::function<void(void)>> CbHelper::pending_cb_list;

void CbHelper::initialize()
{
    if (!initialized)
    {
        xTaskCreate(&main_cb_task, "main_cb_task", default_stack_size, nullptr, 5, &main_cb_task_hdl);
    }

    initialized = true;
}

void CbHelper::execute_callbacks(std::function<void(void)> cb)
{
    lock_pending_cb_list();
    pending_cb_list.push_back(cb);
    unlock_pending_cb_list();

    if (eTaskGetState(main_cb_task_hdl) != eRunning)
        xTaskNotifyGive(main_cb_task_hdl);
}

void CbHelper::main_cb_task(void* arg)
{
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block until notified on index 0 by call to set()

        while (!pending_cb_list.empty())
        {
            pending_cb_list.back()();
            lock_pending_cb_list();
            pending_cb_list.pop_back();
            unlock_pending_cb_list();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
}

void CbHelper::lock_pending_cb_list()
{
    xSemaphoreTake(pending_cb_list_mutex, portMAX_DELAY);
}

void CbHelper::unlock_pending_cb_list()
{
    xSemaphoreGive(pending_cb_list_mutex);
}

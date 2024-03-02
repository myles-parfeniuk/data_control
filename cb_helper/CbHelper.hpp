#pragma once
// standard library includes
#include <functional>
#include <vector>
// esp-idf includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

class CbHelper
{

    public:
        /**
         * @brief Starts main cb task.
         *
         * Only called once when the first data wrapper object is created.
         *
         * @return void, nothing to return
         */
        static void initialize();

        /**
         * @brief Passes call-backs to be executed in main_cb_task.
         *
         * This method is called from the set() method in the DataWrapper child classes such as CallAlways, CallSame, & CallDifferent.
         * It pushes their respective callback execution function to the pending_cb list and notifies the main callback execution task if it is not
         * already running.
         *
         * @param cb the cb executor to be added to the pending_cb_list
         *
         * @return void, nothing to return
         */
        static void execute_callbacks(std::function<void()> cb);

    private:
        /**
         * @brief Task responsible for executing non-immediate call-backs of all DataWrapper objects.
         *
         * This task is notified whenever set() is called on a DataWrapper object, it runs until all
         * functions within the pending_cb_list have been executed, it then waits to be notified by the next call to set().
         *
         * @return void, nothing to return
         */
        static void main_cb_task(void* arg);

        /**
         * @brief Lock pending callback  list.
         *
         * Take the pending callback list semaphore such that any processing respective to it is not interrupted by other tasks.
         *
         * @return void, nothing to return
         */
        static void lock_pending_cb_list();

        /**
         * @brief Unlock pending callback list.
         *
         * Give the pending callback list semaphore to indicate any processing is complete.
         *
         * @return void, nothing to return
         */
        static void unlock_pending_cb_list();

        static std::vector<std::function<void(void)>>
                pending_cb_list; ///<pending cb_executor functions from DataWrapper subclasses. Pushed to list when execute_callbacks() is called.
        static TaskHandle_t main_cb_task_hdl;                      ///<task handle of main call-back task
        static bool initialized;                                   ///< whether or not the main task has been started
        static const constexpr char* TAG = "CbHelper";             ///<class tag, used in logging statements
        static const constexpr uint32_t default_stack_size = 4096; ///< default stack size of main task
        static SemaphoreHandle_t pending_cb_list_mutex;            ///< pending callback list semaphore used by lock and unlock functions
};

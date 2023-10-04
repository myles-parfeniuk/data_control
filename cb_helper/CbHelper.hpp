#pragma once
//standard library includes
#include <functional>
#include <vector>
//esp-idf includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

class CbHelper {     

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
    * It pushes their cb_executor function to the pending_cb list and resume's main_cb_task such that any 
    * non immediate call-backs registered to the respective DataWrapper object will be executed. 
    * 
    * @param cb the cb executor to be added to the pending_cb_list
    * 
    * @return void, nothing to return
    */
    static void execute_callbacks(std::function<void ()> cb);

    private:
    /**
    * @brief Task responsible for executing non-immediate call-backs of all DataWrapper objects.
    * 
    * This task is resumed whenever set() is called on a DataWrapper object, it runs until all
    * functions within the pending_cb_list have been executed and self suspends upon completion. 
    * 
    * @return void, nothing to return
    */
    static void main_cb_task(void *arg);

    static void lock_pending_cb_list();
    static void unlock_pending_cb_list();

    static std::vector<std::function<void(void)>> pending_cb_list; ///<pending cb_executor functions from DataWrapper subclasses. Pushed to list when execute_callbacks() is called.
    static TaskHandle_t main_cb_task_hdl; ///<task handle of main call-back task
    static bool initialized; ///< whether or not the main task has been started
    static const constexpr char* TAG = "CbHelper"; ///<class tag, used in logging statements
    static const constexpr uint32_t default_stack_size = 4096; ///< default stack size of main task
    static SemaphoreHandle_t pending_cb_list_mutex; 


};


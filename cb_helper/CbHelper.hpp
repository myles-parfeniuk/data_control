#pragma once
#include <functional>
#include <vector>
#include "../task_wrapper/TaskWrapper.hpp"

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
    * @brief Task responsible for executing non-immediate call-backs of all DataWrapper objects.
    * 
    * This task is resumed whenever set() is called on a DataWrapper object, it runs until all
    * functions within the pending_cb_list have been executed and self suspends upon completion. 
    * 
    * @return void, nothing to return
    */
    static void main_task();

    /**
    * @brief Sets stack depth of main call-back task. 
    * 
    * Sets the stack depth of the task responsible for executing all non-immediate call-backs.
    * 
    * @param cb the desired stack depth for main call-back task, default is 4096bytes
    * 
    * @return void, nothing to return
    */
    static void set_main_cb_task_depth(uint32_t stack_depth);

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
    static std::vector<std::function<void(void)>> pending_cb_list; ///<pending cb_executor functions from DataWrapper subclasses. Pushed to list when execute_callbacks() is called.
    static TaskWrapper<CbHelper> main_cb_task; ///<task responsible for executing all non-immediate call-backs
    static bool initialized; ///< whether or not the main task has been started
    static const constexpr char* TAG = "CbHelper"; ///<class tag, used in logging statements
    static const constexpr uint32_t default_stack_size = 4096; ///< default stack size of main task


};


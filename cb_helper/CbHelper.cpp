#include "CbHelper.hpp"

bool CbHelper::initialized = false;
TaskWrapper<CbHelper> CbHelper::main_cb_task(&CbHelper::main_task);
std::vector<std::function<void(void)>> CbHelper::pending_cb_list;

void CbHelper::initialize()
{
    if(!initialized){
        main_cb_task.set_stack_size(4096);
        main_cb_task.start_task();
        
    }

    initialized = true; 
    
}

void CbHelper::execute_callbacks(std::function<void(void)> cb)
{
   pending_cb_list.push_back(cb);
   main_cb_task.resume_task();
}

void CbHelper::main_task()
{
    while(1){

    while(!pending_cb_list.empty())
    {
        pending_cb_list.back()(); 
        pending_cb_list.pop_back();
        vTaskDelay(15/portTICK_PERIOD_MS); 
    }

    main_cb_task.suspend_task(); 
    }
}

void CbHelper:: set_main_cb_task_depth(uint32_t stack_depth)
{
    main_cb_task.set_stack_size(stack_depth);
}
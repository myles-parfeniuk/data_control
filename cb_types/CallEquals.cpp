#include "CallEquals.hpp"

using namespace DataControl;

template <typename T>
CallEquals<T>::CallEquals(T data, T compare_data)
{
    this->next_id = 0;
    this->stack_depth = this->default_stack_depth;
    this->cb_task_complete = true;
    this->name = "unamed";
    this->logging_en = false;
    this->compare_data = compare_data;
}

template <typename T>
CallEquals<T>::CallEquals(T data, T compare_data, bool logging_en)
{
    this->next_id = 0;
    this->stack_depth = this->default_stack_depth;
    this->cb_task_complete = true;
    this->name = "unamed";
    this->logging_en = logging_en;
    this->compare_data = compare_data;
}

template <typename T>
CallEquals<T>::CallEquals(T data, T compare_data ,bool logging_en, const char *name)
{
    this->next_id = 0;
    this->stack_depth = this->default_stack_depth;
    this->cb_task_complete = true;
    this->name = name;
    this->logging_en = logging_en;
    this->compare_data = compare_data;
}


template <typename T>
void CallEquals<T>::set(T new_data)
{
    this->new_data = new_data; 

    xTaskCreate(&cb_task, "cb_task", this->stack_depth, this, 4, &this->cb_task_hdl);
}


 template <typename T>
 void CallEquals<T>::cb_routine(){
    static uint16_t i = 0;
    this->cb_task_complete = false;
    
    if(this->data == this->compare_data)
    {
        if(!this->follower_list.empty())
        {
            for(i = 0; i < this->follower_list.size(); i++)
            {
                this->follower_list.at(i).cb(this->new_data); //call all follower call-back functions
            }
        }
    }

    this->data = this->new_data;

    this->cb_task_complete = true;  

 }

 template <typename T>
 void CallEquals<T>::set_compare_data(T new_compare_data)
 {
    while(!this->cb_task_complete);
    this->compare_data = new_compare_data; 

 }


 template <typename T>
 void CallEquals<T>::cb_task(void *data)
 {
    CallEquals *local_data = (CallEquals *)data; 

    local_data->cb_routine();
    vTaskDelete(NULL);  //self-delete task
 }
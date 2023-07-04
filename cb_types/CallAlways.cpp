#include "CallAlways.hpp"

using namespace DataControl;

template <typename T>
void CallAlways<T>::set(T new_data)
{
    this->new_data = new_data; 

    xTaskCreate(&cb_task, "cb_task", this->stack_depth, this, 4, &this->cb_task_hdl);
}


 template <typename T>
 void CallAlways<T>::cb_routine(){
    static uint16_t i = 0;
    this->cb_task_complete = false;

    if(!this->follower_list.empty())
    {
        //loop through all followers
        for(i = 0; i < this->follower_list.size(); i++)
        {
            if(!this->follower_list.at(i).paused) //if follower is not paused
                this->follower_list.at(i).cb(this->new_data); //call follower call-back function
        }
    }

    this->data = this->new_data;

    this->cb_task_complete = true;  

 }


 template <typename T>
 void CallAlways<T>::cb_task(void *data){
    CallAlways *local_data = (CallAlways *)data; 

    local_data->cb_routine();
    vTaskDelete(NULL);  //self-delete task
 }
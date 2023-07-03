#include "DataWrapper.hpp"

//esp-idf component includes
#include "freertos/projdefs.h"
#include "esp_err.h"
#include "esp_log.h"

using namespace DataControl;

template <typename T>
/** 
*   @brief  contains information about a follower
*/
struct DataWrapper<T>::Follower {
    std::function<void(T)> cb; ///< call-back function of follower
    uint16_t f_id; ///< unique follower ID
};


template <typename T>
DataWrapper<T>:: DataWrapper():
next_id(0), stack_depth(default_stack_depth), cb_task_complete(true), name("unamed"), logging_en(false)
{
}

template <typename T>
DataWrapper<T>:: DataWrapper(T data):
data(data), next_id(0), stack_depth(default_stack_depth), cb_task_complete(true), name("unamed"), logging_en(false) 
{
}

template <typename T>
DataWrapper<T>:: DataWrapper(T data, bool logging_en):
data(data), next_id(0), stack_depth(default_stack_depth), cb_task_complete(true), name("unamed"), logging_en(logging_en)
{
}

template <typename T>
DataWrapper<T>:: DataWrapper(T data, bool logging_en, const char * name):
data(data), next_id(0), stack_depth(default_stack_depth), cb_task_complete(true), name(name), logging_en(logging_en)
{
}


template <typename T>
T DataWrapper<T>::get()
{
    return data; 
}

template <typename T>
 uint16_t DataWrapper<T>::follow(std::function<void(T)> cb)
 {
    Follower follower = {
        .cb = cb,
        .f_id = next_id,
    };

    next_id++; 

    if(logging_en)
        ESP_LOGI(TAG, "%s -> Follow: %d", name, follower.f_id);

    follower_list.push_back(follower);

    return follower.f_id;
 }

 template <typename T>
 void DataWrapper<T>::un_follow(uint16_t follower_id){

    static uint16_t i = 0;

    while(!cb_task_complete);

    if(!follower_list.empty()){
        
        for(i = 0; i < follower_list.size(); i++)
        {
            if(follower_list.at(i).f_id == follower_id){
                if(logging_en)
                    ESP_LOGI(TAG, "%s -> Unfollow: %d", name, follower_list.at(i).f_id);

                follower_list.erase(follower_list.begin() + i); 
            }
        }

    }
 }

template <typename T>
void DataWrapper<T>:: set_cb_task_depth(uint16_t stack_depth){
    while(!cb_task_complete);
    this->stack_depth = stack_depth;
}

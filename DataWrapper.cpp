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
    bool paused; ///< pauses calls to follower callback function, if true this follower's call-back will not be executed
};


template <typename T>
DataWrapper<T>:: DataWrapper():
next_id(0), stack_depth(default_stack_depth), cb_task_complete(true), name("unamed"), logging_en(false)
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
    //initialize new follower
    Follower follower = {
        .cb = cb, //assign callback
        .f_id = next_id, //assign id
        .paused = false, //follower unpaused by default
    };

    next_id++; //generate id for next follower

    if(logging_en)
        ESP_LOGI(TAG, "%s -> Follow: %d", name, follower.f_id);

    follower_list.push_back(follower); //push new follower to list

    return follower.f_id; //return the id of the new follower
 }

 template <typename T>
 bool DataWrapper<T>::un_follow(uint16_t follower_id){

    static uint16_t i = 0;
    bool follower_found = false; 

    while(!cb_task_complete);

    if(!follower_list.empty()){
        
        //loop through all followers
        for(i = 0; i < follower_list.size(); i++)
        {
            //if matching ID is found
            if(follower_list.at(i).f_id == follower_id){
                if(logging_en)
                    ESP_LOGI(TAG, "%s -> Unfollow: %d", name, follower_list.at(i).f_id);

                follower_list.erase(follower_list.begin() + i); //erase follower from list
                follower_found = true;
            }
        }

    }

    return follower_found; //whether or not the follower ID matched one in the list
 }

template <typename T>
void DataWrapper<T>:: set_cb_task_depth(uint16_t stack_depth){
    while(!cb_task_complete); //wait until cb routine has completed if it's currently running
    this->stack_depth = stack_depth; //set the new stack depth
}

template <typename T>
bool DataWrapper<T>::pause(uint16_t follower_id)
{
    bool follower_found = false;
    uint16_t i = 0;

    //loop through all followers
    for(i = 0; i < follower_list.size(); i++)
    {
        //if matching id is found
        if(follower_list.at(i).f_id == follower_id){
            if(logging_en)
               ESP_LOGI(TAG, "%s -> Paused callback for follower: f_id_%d", name, follower_list.at(i).f_id);

                follower_list.at(i).paused = true; //pause follower such that its callback will not be executed
                follower_found = true; 
            }
    }


    return follower_found; //whether or not the follower ID matched one in the list

}

template <typename T>
bool DataWrapper<T>::un_pause(uint16_t follower_id)
{
    bool follower_found = false;
    uint16_t i = 0; 
    
    //loop through all followers
    for(i = 0; i < follower_list.size(); i++)
    {
        //if matching id is found
        if(follower_list.at(i).f_id == follower_id){
            if(logging_en)
               ESP_LOGI(TAG, "%s -> Pause ballback for follower: f_id_%d", name, follower_list.at(i).f_id);

                follower_list.at(i).paused = false; //un-pause follower such that its callback will resume normal execution 
                follower_found = true; 
            }
    }


    return follower_found; //whether or not the follower ID matched one in the list

}

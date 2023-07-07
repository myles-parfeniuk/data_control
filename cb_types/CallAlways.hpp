#pragma once
#include "../DataWrapper.hpp"

namespace DataControl {

/**
*
* @brief DataWrapper that executes callbacks whenever set() method is called.
*
* For further information see base class DataWrapper documentation. 
*
* @author Myles Parfeniuk
*
*/
template <typename T>
class CallAlways : public DataWrapper<T>
{
     public:
        using DataWrapper<T>::DataWrapper;
    /**
    * @brief Set value of data. 
    * 
    * Overwrite current data with a new value. When this function is called, any followers registered to this DataWrapper object will have their call-backs executed. 
    * Upon completion of call-back executions, the current data value will be over-written with the data passed into this function. 
    * @param new_data the new value to over-write current data with. 
    * @return void, nothing to return
    */
        void set(T new_data);

    private:
    /*
    * @brief Call-back task.
    * 
    * Task responsible for executing the call-back functions of followers registered to this DataWrapper object.
    * Executes follower callback always. 
    * This task is started when set() is called, and self-deletes upon completion. 
    * 
    * @return void, nothing to return
    */
        void cb_task();

   /*
    * @brief Launches Call-back task.
    * 
    * This function is used to get around the fact xTaskCreate() from the freertos api requires a static task function.
    * To prevent having to write the callback task from the context of a static function, this serves as a trampoline to launch the cb_task()
    * from the DataWrapper object passed into xTaskCreate().
    * @return void, nothing to return
    */
        static void cb_task_trampoline(void *data);
};

};
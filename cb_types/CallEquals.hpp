#pragma once
#include "../DataWrapper.hpp"

namespace DataControl {

/**
*
* @brief DataWrapper that executes callbacks when set() method is called and new data matches comparison data.
*
* For further information see base class DataWrapper documentation. 
*
* @author Myles Parfeniuk
*
*/
template <typename T>
class CallEquals : public DataWrapper<T>
{
    public: 
    
    /**
    * @brief Construct a CallEquals DataWrapper object. 
    * 
    * Construct a CallEquals DataWrapper object with initial value, comparison data, logging status, and name. Enabling logs may be desirable for debugging purposes. 
    * 
    * @param data initial value of data
    * @param compare_data initial value of comparison data
    * @param logging_en logging status, true to enable logging, false to disable (optional, default false)
    * @param name name of data wrapper object, printed out with logs to aid debugging (optional, default "unamed")
    * @return void, nothing to return
    */
        CallEquals(T data, T compare_data , bool logging_en = false, const char *name = "unamed");
        
    /**
    * @brief Set value of data. 
    * 
    * Overwrite current data with a new value. When this function is called, any followers registered to this DataWrapper object will have their call-backs executed. 
    * Upon completion of call-back executions, the current data value will be over-written with the data passed into this function. 
    * @param new_data the new value to over-write current data with. 
    * @return void, nothing to return
    */
        void set(T new_data);

    /**
    * @brief Set value of comparison data. 
    * 
    * Overwrite current comparison data with a new value. If the callback routine is executing, the compare data will not be over-written
    * until it has completed. The set_compare_data_from_cb() method can be called to over-write the compare data immediately. 
    * 
    * @param new_compare_data the new comparison value to over-write current comparison data with. 
    * @return void, nothing to return
    */
        void set_compare_data(T new_compare_data);

    /**
    * @brief Set value of comparison data from context of call-back function. 
    * 
    * Overwrite current comparison data with a new value immediately, does not wait for call-back routine to complete.
    * This method should be called instead of set_compare_data() if within the context of a follower call-back function.  
    * 
    * @param new_compare_data the new comparison value to over-write current comparison data with. 
    * @return void, nothing to return
    */
        void set_compare_data_from_cb(T new_compare_data);

    private:
  /**
    * @brief Call-back task.
    * 
    * Task responsible for executing the call-back functions of followers registered to this DataWrapper object.
    * Executes follower callback if and only if the new data is equal to the comparison data. 
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
    
    T compare_data; ///< data to check against new_data when set() method is called
};

};


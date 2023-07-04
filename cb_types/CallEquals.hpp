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
    * Overwrite current data with a new value. When this function is called, if the new data matches the comparison data all followers within this DataWrapper 
    * object's follower list will have their call-backs executed. Upon completion of call-back executions, the current data value will be over-written with
    * the data passed into this function. If the get() method is called within a callback it will return the current data, the input parameter of a call-back 
    * is the new data that is to overwrite the current data. 
    * 
    * @param new_data the new value to over-write current data with. 
    * @return void, nothing to return
    */
        void set(T new_data);

    /**
    * @brief Set value of comparison data. 
    * 
    * Overwrite current comparison data with a new value. The comparison data is checked against the new data whenever the set() method is called. 
    * If the new data matches the comparison data all follower callbacks will be executed. 
    * 
    * @param new_compare_data the new comparison value to over-write current comparison data with. 
    * @return void, nothing to return
    */
        void set_compare_data(T new_compare_data);

    private:
    /**
    * @brief Call-back function routine.
    * 
    * This function should not be called directly. It is called from the call-back task and executes the call-back
    * functions of all followers on this DataWrapper object's follower list. 
    * 
    * @return void, nothing to return
    */
        void cb_routine();
    
    /**
    * @brief Call-back task.
    * 
    * This task is created to run the call-back routine whenever this DataWrapper object is set and the new data matches the comparison data. Upon completion of the call-back routine
    * this task is deleted. 
    * 
    * @return void, nothing to return
    */
        static void cb_task(void *data);
        T compare_data; ///< data to check against new_data when set() method is called
};

};
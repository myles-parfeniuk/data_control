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
    * Overwrite current data with a new value. When this function is called, all followers within this DataWrapper object's follower list will
    * have their call-backs executed. Upon completion of call-back executions, the current data value will be over-written with
    * the data passed into this function. If the get() method is called within a callback it will return the current data, the input parameter of a call-back 
    * is the new data that is to overwrite the current data. 
    * 
    * @param new_data the new value to over-write current data with. 
    * @return void, nothing to return
    */
        void set(T new_data);

    /**
    * @brief Call-back function routine.
    * 
    * This function should not be called directly. It is called from the call-back task and executes the call-back
    * functions of all followers on this DataWrapper object's follower list. 
    * 
    * @return void, nothing to return
    */
        void cb_routine();
    private:
    /**
    * @brief Call-back task.
    * 
    * This task is created to run the call-back routine whenever this DataWrapper object is set. Upon completion of the call-back routine this task is deleted. 
    * 
    * @return void, nothing to return
    */
        static void cb_task(void *data);
};

};
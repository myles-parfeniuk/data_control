#pragma once
#include "../DataWrapper.hpp"

namespace DataControl {

/**
* @brief DataWrapper that executes callbacks when set() method is called and new data is the same as the current data.
*
* @author Myles Parfeniuk
*/
template <typename T>
class CallSame : public DataWrapper<T>
{
    public:

    /**
     * @brief Construct a CallSame DataWrapper object. 
     * 
     * Construct a CallSame DataWrapper with initial value, logging status, and name. Enabling logs may be desirable for debugging purposes. 
     * CallSame objects execute callbacks whenever set() method is called and new data is the same as the current data.
     * 
     * @param data initial value of data
     * @param logging_en logging status, true to enable debug logs, false to disable (optional, default false)
     * @param name name of data wrapper object, printed out with debug logs to aid debugging (optional, default "unamed")
     * @return void, nothing to return
     */
        CallSame(T data, bool logging_en = false, const char *name = "unamed"):
        DataWrapper<T>(data, logging_en, name) 
        {

        }

    /**
    * @brief Set value of data. 
    * 
    * Overwrite current data with a new value. When this function is called, any followers registered to this DataWrapper object will have their call-backs executed. 
    * Upon completion of call-back executions, the current data value will be over-written with the data passed into this function. 
    * 
    * @param new_data the new value to over-write current data with. 
    * 
    * @return void, nothing to return
    */
        void set(T new_data);

    private:

    /**
    * @brief Executes non-immediate call-backs 
    * 
    * Method responsible for executing non-immediate call-back functions of followers registered to this DataWrapper object.
    * Executes follower callback if and only if the new data is the same as the previous data. 
    * This function is called from CbHelper's main task. 
    * 
    * @return void, nothing to return
    */
        void cb_executor();
};

};
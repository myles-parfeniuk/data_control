#pragma once
#include "../DataWrapper.hpp"

namespace DataControl {

/**
*
* @brief DataWrapper that executes callbacks when set() method is called and new data is different from current data.
*
* For further information see base class DataWrapper documentation. 
*
* @author Myles Parfeniuk
*
*/
template <typename T>
class CallDifferent : public DataWrapper<T>
{
    public:
        using DataWrapper<T>::DataWrapper;
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
    * Executes follower callback if and only if the new data is different from the previous data. 
    * This function is called from CbHelper's main task. 
    * 
    * @return void, nothing to return
    */
        void cb_executor();
};
};


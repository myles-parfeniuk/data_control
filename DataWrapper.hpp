#pragma once
//esp-idf includes
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//standard library includes
#include <cstdint>
#include <functional>
#include <vector>
#include <string>

namespace DataControl{
/**
*
* @brief Base DataWrapper class
*
* Stores a variable/data and allows for execution of callback functions when it is changed. When changing the data
* is desired, the set() method should be called, upon which any callbacks registered with the follow()
* method will be executed, and the data updated. 
* DataWrapper objects should not be instantiated directly, instead insatiate objects from derived classes
* of which definitions can be found in cb_types/ directory.  
*
* @author Myles Parfeniuk
*
*/
template <typename T>
class DataWrapper {       
  
  public:

  /**
  * @brief Construct a new DataWrapper object.
  *
  * Default DataWrapper constructor. Data will be NULL until set is called if this constructor is used. 
  *
  * @return void, nothing to return
  */             
    DataWrapper();

  /**
  * @brief Construct a DataWrapper object. 
  * 
  * Construct a DataWrapper with initial value, logging status, and name. Enabling logs may be desirable for debugging purposes. 
  * 
  * @param data initial value of data
  * @param logging_en logging status, true to enable logging, false to disable (optional, default false)
  * @param name name of data wrapper object, printed out with logs to aid debugging (optional, default "unamed")
  * @return void, nothing to return
  */
    DataWrapper(T data, bool logging_en = false, const char *name = "unamed");

  /**
  * @brief Follow DataWrapper object.
  *        
  * Trigger callback when this DataWrapper object is set.
  * The call-back function parameter should have a single input parameter of type T. This input will be the new value of the data being set. 
  * Calling the get() method from within the callback will return the current value. When all callbacks have been executed the current
  * value will over written with the new value.           
  * 
  * @param cb the call-back function to be called when DataWrapper object is set, must take the form "void f(T new_data)"
  * @return follower ID, if un-following DataWrapper object is desired this return value must be saved
  */
    uint16_t follow(std::function<void(T)> cb); 

  /**
  * @brief Un-follow DataWrapper object
  * 
  * Stop triggering callback for respective follower permanently when this DataWrapper object is set.
  * 
  * @param follower_id The ID of the follower to remove from the follower list. Returned when follow() is called. 
  * @return true if follower successfully un-followed, false if otherwise
  */
    bool un_follow(uint16_t follower_id);

  /**
  * @brief Pause follower callback 
  * 
  * Temporarily stop triggering callback for respective follower when this DataWrapper object is set.
  * Re-enable callback by calling the un_pause() method. 
  * 
  * @param follower_id The ID of the follower owning the callback to be paused. 
  * @return true if follower successfully paused, false if otherwise
  */
    bool pause(uint16_t follower_id);

  /**
  * @brief un-pause follower callback 
  * 
  * Re-enable callback for respective follower when this DataWrapper object is set.
  * 
  * @param follower_id The ID of the follower owning the callback to be un-paused. 
  * @return true if follower successfully un-paused, false if otherwise
  */
    bool un_pause(uint16_t follower_id);

  /**
  * @brief Get current data. 
  * 
  * @return the current data
  */
    T get();

  /**
  * @brief Set stack-depth of call-back task. 
  * 
  * @param stack_depth Size of stack-depth to set. 
  * @return void, nothing to return
  */
    void set_cb_task_depth(uint16_t stack_depth);

  protected:
    T data; ///<the current data
    T new_data; ///<new data, overwrites current data after execution of all call-back functions
    uint16_t next_id; ///<next follower ID to assign
    uint16_t stack_depth; ///<depth of task executing call-back functions
    struct Follower; 
    std::vector<Follower> follower_list; ///<list of all followers, each element contains the call-back function and ID of a follower
    xTaskHandle cb_task_hdl; ///<task handle of call-back task
    bool cb_task_complete; ///<whether or not the call-back task has completed, false if running, true if completed
    const char *name; ///<name of DataWrapper object, used in log statements
    bool logging_en;  ///<whether or not logging is enabled, false if disabled, true if enabled
    
    static const constexpr char* TAG = "DataControl"; ///<class tag, used in logging statements
    static const constexpr uint16_t default_stack_depth = 4096; ///<default stack depth

};
};
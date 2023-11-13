#pragma once
//esp-idf includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
//standard library includes
#include <cstdint>
#include <functional>
#include <vector>
#include <string>
#include "cb_helper/CbHelper.hpp"

namespace DataControl{
/**
*
* @brief Base DataWrapper class
*
* Stores a variable/data and allows for execution of callback functions when it is modified.
* 
* When modifying the data is desired, the set() method should be called, upon which:
* - any callbacks registered with the follow() method will be executed
* - the data currently stored will be updated with the new value 
*
* DataWrapper objects are not possible to instantiate directly, instead insatiate objects from child classes
* of which definitions can be found in cb_types/ directory.  
*
* @author Myles Parfeniuk
*
*/
template <typename T>
class DataWrapper {       
  
  public:

  /**
  * @brief Follow DataWrapper object.
  *        
  * Registers a call-back with this data wrapper object. 
  * Registered call-back functions are executed when the set() method is called on the same data wrapper object. 
  * 
  * The call-back function parameter should have a single input parameter of type T. 
  * This parameter will be the new data passed to the callback when set() has been called. 
  * 
  * Calling the get() method from within the callback will return the previous value. 
  * When all callbacks have been executed the current value will over written with the new value.    
  *        
  * @param cb the call-back function to be called when DataWrapper object is set, must take the form "void my_callback(T new_data)"
  * @param execute_immediately if true the call-back is executed in the same thread in which set() was called, only recommended for short call-back functions
  * @return follower ID, if un-following DataWrapper object is desired this return value must be saved
  */
    uint16_t follow(std::function<void(T)> cb, bool execute_immediately = false); 

  /**
  * @brief Un-follow DataWrapper object
  * 
  * Stop triggering callback for respective follower permanently.
  * 
  * @param follower_id The ID of the follower to remove from the follower list. Returned when follow() is called. 
  * @return true if follower successfully un-followed, false if otherwise
  */
    bool un_follow(uint16_t follower_id);

  /**
  * @brief Un-follow DataWrapper object from its own callback
  * 
  * Stop triggering callback for respective follower permanently from within the context of callback. 
  * This function assumes ownership of the follower lists, and does not take or give their respective semaphores. 
  * 
  * @param follower_id The ID of the follower to remove from the follower list. Returned when follow() is called. 
  * @return true if follower successfully un-followed, false if otherwise
  */
    bool un_follow_from_cb(uint16_t follower_id);

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
  * @brief Pause follower callback from its own callback
  * 
  * Temporarily stop triggering callback for respective follower when this DataWrapper object is set, from within the context of callback.
  * This function assumes ownership of the follower lists, and does not take or give their respective semaphores. 
  * Re-enable callback by calling the un_pause() method. 
  * 
  * @param follower_id The ID of the follower owning the callback to be paused. 
  * @return true if follower successfully paused, false if otherwise
  */
    bool pause_from_cb(uint16_t follower_id);

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
  * @brief un-pause follower callback from its own callback
  * 
  * Re-enable callback for respective follower when this DataWrapper object is set, from within the context of callback.
  * This function assumes ownership of the follower lists, and does not take or give their respective semaphores. 
  * 
  * @param follower_id The ID of the follower owning the callback to be un-paused. 
  * @return true if follower successfully un-paused, false if otherwise
  */
    bool un_pause_from_cb(uint16_t follower_id);

  /**
  * @brief Get current data. 
  * 
  * @return the current data
  */
    T get();

  protected:

  /**
  * @brief Construct a DataWrapper object. 
  * 
  * Construct a DataWrapper with initial value, logging status, and name. Enabling logs may be desirable for debugging purposes. 
  * This constructor is called by child classes, DataWrapper objects are not instantiated directly. 
  * 
  * @param data initial value of data
  * @param logging_en logging status, true to enable debug logs, false to disable (optional, default false)
  * @param name name of data wrapper object, printed out with debug logs to aid debugging (optional, default "unamed")
  * @return void, nothing to return
  */
    DataWrapper(T data, bool logging_en = false, const char *name = "unamed");

  /**
  * @brief Lock follower list.
  * 
  * Take the follower list semaphore such that any processing respective to it is not interrupted by other tasks.  
  * 
  * @return void, nothing to return
  */
    void lock_follower_list();

  /**
  * @brief Unlock follower list.
  * 
  * Give the follower list semaphore to indicate any processing is complete. 
  * 
  * @return void, nothing to return
  */
    void unlock_follower_list();

  /**
  * @brief Lock immediate follower list.
  * 
  * Take the immediate follower list semaphore such that any processing respective to it is not interrupted by other tasks.  
  * 
  * @return void, nothing to return
  */
    void lock_immediate_follower_list();

  /**
  * @brief Unlock immediate follower list.
  * 
  * Give the immediate follower list semaphore to indicate any processing is complete. 
  * 
  * @return void, nothing to return
  */
    void unlock_immediate_follower_list();
  
    T data; ///<the current data
    T new_data; ///<new data, overwrites current data after execution of all call-back functions
    uint16_t next_follower_id; ///<next follower ID to assign
    struct Follower; 
    std::vector<Follower> follower_list; ///<list of followers
    std::vector<Follower> immediate_follower_list; ///<list of followers for which callbacks are executed immediately from the thread which set() was called
    bool cb_task_complete; ///<whether or not the call-back task has completed, false if running, true if completed
    const char *name; ///<name of DataWrapper object, used in log statements
    bool logging_en;  ///<whether or not logging is enabled, false if disabled, true if enabled
    static const constexpr char* TAG = "DataControl"; ///<class tag, used in logging statements
    SemaphoreHandle_t follower_list_mutex; ///< follower list semaphore used by lock and unlock functions
    SemaphoreHandle_t immediate_follower_list_mutex; ///< immediate follower list semaphore used by lock and unlock functions
};

};


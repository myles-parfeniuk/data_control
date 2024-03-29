#include "DataWrapper.hpp"

// esp-idf component includes
#include "freertos/projdefs.h"
#include "esp_err.h"

using namespace DataControl;

template <typename T>
/**
 *   @brief  contains information about a follower
 */
struct DataWrapper<T>::Follower
{
        std::function<void(T)> cb; ///< call-back function of follower
        uint16_t f_id;             ///< unique follower ID
        bool paused;               ///< pauses calls to follower callback function, if true this follower's call-back will not be executed
};

template <typename T>
DataWrapper<T>::DataWrapper(T data, bool logging_en, const char* name)
    : data(data)
    , next_follower_id(0)
    , cb_task_complete(true)
    , name(name)
    , logging_en(logging_en)
    , follower_list_mutex(xSemaphoreCreateRecursiveMutex())
    , immediate_follower_list_mutex(xSemaphoreCreateRecursiveMutex())
{
    CbHelper::initialize();
}

template <typename T>
T DataWrapper<T>::get()
{
    return data;
}

template <typename T>
uint16_t DataWrapper<T>::follow(std::function<void(T)> cb, bool execute_immediately)
{
    // initialize new follower
    Follower follower = {
            .cb = cb,                 // assign callback
            .f_id = next_follower_id, // assign id
            .paused = false,          // follower unpaused by default
    };

    next_follower_id++; // generate id for next follower

    if (logging_en)
        ESP_LOGI(TAG, "%s -> Follow: %d", name, follower.f_id);

    if (!execute_immediately)
    {
        lock_follower_list();
        follower_list.push_back(follower); // push new follower to list
        unlock_follower_list();
    }
    else
    {
        lock_immediate_follower_list();
        immediate_follower_list.push_back(follower); // push new immediate follower to list
        unlock_immediate_follower_list();
    }

    return follower.f_id; // return the id of the new follower
}

template <typename T>
bool DataWrapper<T>::un_follow(uint16_t follower_id)
{

    static uint16_t i = 0;
    if (!immediate_follower_list.empty())
    {
        lock_immediate_follower_list();
        // loop through all immediate followers
        for (i = 0; i < immediate_follower_list.size(); i++)
        {
            // if matching ID is found
            if (immediate_follower_list.at(i).f_id == follower_id)
            {
                if (logging_en)
                    ESP_LOGI(TAG, "%s -> Unfollow: %d", name, immediate_follower_list.at(i).f_id);

                immediate_follower_list.erase(immediate_follower_list.begin() + i); // erase follower from list
                unlock_immediate_follower_list();
                return true;
            }
        }
    }
    unlock_immediate_follower_list();

    if (!follower_list.empty())
    {
        lock_follower_list();
        // loop through all followers
        for (i = 0; i < follower_list.size(); i++)
        {
            // if matching ID is found
            if (follower_list.at(i).f_id == follower_id)
            {
                if (logging_en)
                    ESP_LOGI(TAG, "%s -> Unfollow: %d", name, follower_list.at(i).f_id);

                follower_list.erase(follower_list.begin() + i); // erase follower from list
                unlock_follower_list();
                return true;
            }
        }
        unlock_follower_list();
    }

    return false; // no matching follower id found
}

template <typename T>
bool DataWrapper<T>::un_follow_from_cb(uint16_t follower_id)
{
    static uint16_t i = 0;

    if (!immediate_follower_list.empty())
    {
        // loop through all immediate followers
        for (i = 0; i < immediate_follower_list.size(); i++)
        {
            // if matching ID is found
            if (immediate_follower_list.at(i).f_id == immediate_follower_list)
            {
                if (logging_en)
                    ESP_LOGI(TAG, "%s -> Unfollow: %d", name, immediate_follower_list.at(i).f_id);

                immediate_follower_list.erase(immediate_follower_list.begin() + i); // erase follower from list
                return true;
            }
        }
    }

    if (!follower_list.empty())
    {
        // loop through all followers
        for (i = 0; i < follower_list.size(); i++)
        {
            // if matching ID is found
            if (follower_list.at(i).f_id == follower_id)
            {
                if (logging_en)
                    ESP_LOGI(TAG, "%s -> Unfollow: %d", name, follower_list.at(i).f_id);

                follower_list.erase(follower_list.begin() + i); // erase follower from list
                return true;
            }
        }
    }

    return false; // no matching follower id found
}

template <typename T>
bool DataWrapper<T>::pause(uint16_t follower_id)
{
    uint16_t i = 0;

    if (!immediate_follower_list.empty())
    {
        lock_immediate_follower_list();
        // loop through all immediate followers
        for (i = 0; i < immediate_follower_list.size(); i++)
        {
            // if matching id is found
            if (immediate_follower_list.at(i).f_id == follower_id)
            {
                if (logging_en)
                    ESP_LOGI(TAG, "%s -> Paused callback for follower: f_id_%d", name, immediate_follower_list.at(i).f_id);

                immediate_follower_list.at(i).paused = true; // pause follower such that its callback will not be executed
                unlock_immediate_follower_list();
                return true;
            }
        }
        unlock_immediate_follower_list();
    }

    if (!follower_list.empty())
    {
        lock_follower_list();
        // loop through all followers
        for (i = 0; i < follower_list.size(); i++)
        {
            // if matching id is found
            if (follower_list.at(i).f_id == follower_id)
            {
                if (logging_en)
                    ESP_LOGI(TAG, "%s -> Paused callback for follower: f_id_%d", name, follower_list.at(i).f_id);

                follower_list.at(i).paused = true; // pause follower such that its callback will not be executed
                unlock_follower_list();
                return true;
            }
        }
        unlock_follower_list();
    }

    return false; // no matching follower id found
}

template <typename T>
bool DataWrapper<T>::pause_from_cb(uint16_t follower_id)
{
    uint16_t i = 0;
    // loop through all immediate followers
    for (i = 0; i < immediate_follower_list.size(); i++)
    {
        // if matching id is found
        if (immediate_follower_list.at(i).f_id == follower_id)
        {
            if (logging_en)
                ESP_LOGI(TAG, "%s -> Paused callback for follower: f_id_%d", name, immediate_follower_list.at(i).f_id);

            immediate_follower_list.at(i).paused = true; // pause follower such that its callback will not be executed
            return true;
        }
    }

    // loop through all followers
    for (i = 0; i < follower_list.size(); i++)
    {
        // if matching id is found
        if (follower_list.at(i).f_id == follower_id)
        {
            if (logging_en)
                ESP_LOGI(TAG, "%s -> Paused callback for follower: f_id_%d", name, follower_list.at(i).f_id);

            follower_list.at(i).paused = true; // pause follower such that its callback will not be executed
            return true;
        }
    }

    return false; // no matching follower id found
}

template <typename T>
bool DataWrapper<T>::un_pause(uint16_t follower_id)
{
    uint16_t i = 0;

    if (!immediate_follower_list.empty())
    {
        lock_immediate_follower_list();
        // loop through all immediate followers
        for (i = 0; i < immediate_follower_list.size(); i++)
        {
            // if matching id is found
            if (immediate_follower_list.at(i).f_id == follower_id)
            {
                if (logging_en)
                    ESP_LOGI(TAG, "%s -> Unpaused callback for follower: f_id_%d", name, immediate_follower_list.at(i).f_id);

                immediate_follower_list.at(i).paused = false; // un-pause follower such that its callback will resume normal execution
                unlock_immediate_follower_list();
                return true;
            }
        }
        unlock_immediate_follower_list();
    }

    if (!follower_list.empty())
    {
        lock_follower_list();
        // loop through all followers
        for (i = 0; i < follower_list.size(); i++)
        {
            // if matching id is found
            if (follower_list.at(i).f_id == follower_id)
            {
                if (logging_en)
                    ESP_LOGI(TAG, "%s -> Unpaused callback for follower: f_id_%d", name, follower_list.at(i).f_id);

                follower_list.at(i).paused = false; // un-pause follower such that its callback will resume normal execution
                unlock_follower_list();
                return true;
            }
        }
        unlock_follower_list();
    }

    return false; // no matching follower id found
}

template <typename T>
bool DataWrapper<T>::un_pause_from_cb(uint16_t follower_id)
{
    uint16_t i = 0;

    // loop through all immediate followers
    for (i = 0; i < immediate_follower_list.size(); i++)
    {
        // if matching id is found
        if (immediate_follower_list.at(i).f_id == follower_id)
        {
            if (logging_en)
                ESP_LOGI(TAG, "%s -> Unpaused callback for follower: f_id_%d", name, immediate_follower_list.at(i).f_id);

            immediate_follower_list.at(i).paused = false; // un-pause follower such that its callback will resume normal execution
            return true;
        }
    }

    // loop through all followers
    for (i = 0; i < follower_list.size(); i++)
    {
        // if matching id is found
        if (follower_list.at(i).f_id == follower_id)
        {
            if (logging_en)
                ESP_LOGI(TAG, "%s -> Unpaused callback for follower: f_id_%d", name, follower_list.at(i).f_id);

            follower_list.at(i).paused = false; // un-pause follower such that its callback will resume normal execution
            return true;
        }
    }

    return false; // no matching follower id found
}

template <typename T>
void DataWrapper<T>::lock_follower_list()
{
    xSemaphoreTake(follower_list_mutex, portMAX_DELAY);
}

template <typename T>
void DataWrapper<T>::lock_immediate_follower_list()
{
    xSemaphoreTake(immediate_follower_list_mutex, portMAX_DELAY);
}

template <typename T>
void DataWrapper<T>::unlock_follower_list()
{
    xSemaphoreGive(follower_list_mutex);
}

template <typename T>
void DataWrapper<T>::unlock_immediate_follower_list()
{
    xSemaphoreGive(immediate_follower_list_mutex);
}

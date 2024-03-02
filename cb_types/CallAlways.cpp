#include "CallAlways.hpp"

using namespace DataControl;

template <typename T>
void CallAlways<T>::set(T new_data)
{
    uint16_t i = 0;

    this->new_data = new_data;

    if (!this->immediate_follower_list.empty())
    {
        for (i = 0; i < this->immediate_follower_list.size(); i++)
        {
            if (!this->immediate_follower_list.at(i).paused) // if follower is not paused
            {
                this->immediate_follower_list.at(i).cb(new_data); // call follower call-back function
            }
        }
    }

    CbHelper::execute_callbacks(std::bind(&CallAlways::cb_executor, this));
}

template <typename T>
void CallAlways<T>::cb_executor()
{
    static uint16_t i = 0;

    this->cb_task_complete = false;

    if (!this->follower_list.empty())
    {
        // loop through all followers
        for (i = 0; i < this->follower_list.size(); i++)
        {
            if (!this->follower_list.at(i).paused)            // if follower is not paused
                this->follower_list.at(i).cb(this->new_data); // call follower call-back function
        }
    }

    this->data = this->new_data;
    this->cb_task_complete = true;
}
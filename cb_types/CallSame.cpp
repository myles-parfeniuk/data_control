#include "CallSame.hpp"

using namespace DataControl;

template <typename T>
void CallSame<T>::set(T new_data)
{
    uint16_t i = 0;
    
    this->new_data = new_data; 

    this->lock_immediate_follower_list(); 
    if(!this->immediate_follower_list.empty())
    {
        if(this->data == new_data)
        {
            for(i = 0; i < this->immediate_follower_list.size(); i++)
            {
                if(!this->immediate_follower_list.at(i).paused) //if follower is not paused  
                    this->immediate_follower_list.at(i).cb(new_data); //call follower call-back function
            }
        }
    }
    this->unlock_immediate_follower_list(); 

    CbHelper::execute_callbacks(std::bind(&CallSame::cb_executor, this)); //for non immediate call-backs
}


 template <typename T>
 void CallSame<T>::cb_executor(){
    static uint16_t i = 0;
    this->cb_task_complete = false;
    
    this->lock_follower_list();
    if(this->data == this->new_data)
    {
        if(!this->follower_list.empty())
        {
            //loop through all followers
            for(i = 0; i < this->follower_list.size(); i++)
            {
                if(!this->follower_list.at(i).paused) //if follower is not paused  
                    this->follower_list.at(i).cb(this->new_data); //call follower call-back function
            }
        }
    }
    this->unlock_follower_list();

    this->data = this->new_data;

    this->cb_task_complete = true;

 }
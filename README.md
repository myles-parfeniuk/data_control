![image](DataControl_banner.png)
<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about">about</a>
    </li>
    <li>
      <a href="#getting-started">getting started</a>
      <ul>
        <li><a href="#adding to project">adding to project</a></li>
      </ul>
    </li>
   <li><a href="#usage">usage</a></li>
       <ul>
        <li><a href="#quick start">quick start</a></li>
        <li><a href="#quick example">quick example</a></li>
      </ul>
  <li><a href="#license">license</a></li>
  <li><a href="#contact">contact</a></li>
  </ol>
</details>


<!-- ABOUT -->
## About

DataControl is a C++ component written for esp-idf version 4.0, intended to simplify the management of data and actions associated with that data.
For example, one might desire to read the output of a temperature sensor and execute several functions to perform tasks like changing fan speed,
turning on an led, etc... This component simplifies that process by allowing call-back functions to easily be associated with a piece of data and
executed whenever it is set. 

It was originally inspired by an in-house component I used at a workplace; however, this component has been re-written without using
that component as a reference. Its functionality shares some similarities, but the implementation is completely different.
<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Getting Started
### Adding to Project

1. Create a "components" directory in the root workspace directory of your esp-idf project if it does not exist already
2. Cd into the newly created components directory and clone the repository 
   ```sh
   cd components
   git clone https://github.com/myles-parfeniuk/data_control.git
   ```
3. You may have to edit project CMakeLists.txt file to include the component
  ```sh
  idf_component_register(SRC_DIRS "." 
                    INCLUDE_DIRS "" 
                    REQUIRES data_control)
  ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->
## Usage
### Quick Start
This is intended to be a quick guide, full documentation generated with Doxygen can be found in the documentation directory.

To use:

1. Instantiate a data wrapper object of any type. The data wrapper object can be declared as CallAlways, CallDifferent, CallSame, or CallEquals. 

   Example syntax:
```cpp
    //integer data with initial value of 0, callbacks executed every time set() method is called (call always)
    DataControl::CallAlways<int16_t> press_count(0); 
    //bool data with initial value of false, callbacks executed every time set() method is called and new data doesn't match current data (call different)
    DataControl::CallDifferent<bool> button_pressed(false);
```
2. Register any desired call back functions by calling the follow() method. These can be instantiated as lambda functions as well. 

   Example syntax:
```cpp

   /*in led_control.cpp*/
   
   button_pressed.follow(
    //lambda call-back function
    [](bool new_data)
    {
       //turn on an led when a button is held
       if(new_data){
            turn_on_led_one(); //turn on led
       } else {
            turn_off_led_one(); //turn off led
       }
   });

   press_count.follow(
    //lambda call-back function
    [](int16_t new_data){
       //turn on an led after 100 presses
       if(new_data >= 100){
            turn_on_led_two(); 
       }
   });

   /*in log_maker.cpp*/

    press_count.follow(
    //lambda call-back function
    [&press_count](int16_t new_data){
       //print the new and current press count
       ESP_LOGI(TAG, "Current Press Count: %d | New Press Count: %d", press_count.get(), new_data);
   });

```

3. Update the data within data wrapper object using set() method as desired, and all call-backs previously registered with the follow() method will be executed automatically.

   Example syntax: 
```cpp
   /*in button_driver.cpp*/

   //when a button press is detected:
   button_pressed.set(true); //set button_pressed as true
   press_count.set(press_count.get() + 1); //increment press_count by 1

   //when the button is no longer being pressed:
   button_pressed.set(false); //set button_pressed as false
```

### Quick Example
A quick example that generates some terminal output which can be seen in the gif below.

Code:
```cpp

/*main.cpp*/

//standard library includes
#include <iostream> 

//in-house component includes
#include "DataControl.hpp"

//esp-idf component includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

extern "C" void app_main()
{
    uint16_t counter_id = 0;

    DataControl::CallAlways<int16_t> counter(0); //initialize data with value of 0
    DataControl::CallDifferent<bool> over_five(false); //initialize data with value of false
    DataControl::CallDifferent<bool> over_ten(false); //initialize data with value of false

    //register callbacks
    counter_id = counter.follow(
    [&counter](int16_t new_data){
        ESP_LOGI("Follower0", "| Current Count: %d | New Count: %d |", counter.get(), new_data); //print current and new counts
    });

    over_five.follow(
    [](bool new_data){
        ESP_LOGE("Follower1", "Count is now over five.");
    });

    over_ten.follow(
    [&counter, counter_id](bool new_data){
        counter.un_follow(counter_id); //un-follow counter when count exceeds ten
        ESP_LOGE("Follower2", "Count is now over ten, counter un-followed.");
    });

    while(1){
        
        counter.set(counter.get() + 1);
        vTaskDelay(500/portTICK_PERIOD_MS); //delay for 0.5 seconds
        if(counter.get() > 5){
           over_five.set(true); //set over_five true after 5 iterations of loop
        }

        if(counter.get() > 10){
            over_ten.set(true); //set over_ten true after 10 iterations of loop
        }

    }
}
```

Terminal Output:

![image](DataControl_terminal_output.gif)


<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.md` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Myles Parfeniuk - myles.parfenyuk@gmail.com

Project Link: [https://github.com/myles-parfeniuk/data_control](https://github.com/myles-parfeniuk/data_control)

<p align="right">(<a href="#readme-top">back to top</a>)</p>
# Events

A minimalist event Dispatcher. MIT License.

This is written mainly for my own use in hobby projects, but feel free to use/modify it (MIT).
I'm open to suggestions / issues / fixes etc.

## How to use

### Create a Dispatcher instance

You need to specify the arguments passed to event handlers.

Example for event handler for a click event, passing one `int` and two `floats`.
```cpp
#include "Events/Dispatcher.hpp"

int main ()
{
    lk::Dispatcher<int, float, float> click_event {};
    return 0;
}
```

### Subscribe to the Dispatcher

You can subscribe functions and member-functions to the Dispatcher. They will be called whenever `Dispatcher::dispatch` is called.

Example for our `click_event` dispatcher.
```cpp
#include <iostream>
#include "Events/Dispatcher.hpp"

void my_click_handler (int mb, float x, float y)
{
    // [...] handle the click here
    std::cout << "Clicked!" << std::endl;
}

int main ()
{
    lk::Dispatcher<int, float, float> dispatcher {};
    dispatcher += my_click_handler;
    return 0;
}

```

**Yes, it really is _this_ simple.**

The requirement, of course, is that the handler has the same argument layout as specified in the Dispatcher (`int, float, float`, int this case).

Now with a **member function**, this becomes a little more interesting, as we pass a pointer to the instance of our object as well as a pointer to the function within the object as a `std::pair`. This approach was chosen because `std::pair` can be used as an argument to `+=`.

```cpp
#include <iostream>
#include "Events/Dispatcher.hpp"

class MyObject
{
public:
    void my_click_handler (int mb, float x, float y)
    {
        // [...] handle the click here
        std::cout << "Clicked!" << std::endl;
    }
};

int main ()
{
    lk::Dispatcher<int, float, float> dispatcher {};
    
    MyObject object;
    dispatcher += std::pair (&object, &MyObject::my_click_handler);
    
    return 0;
}
```

**NOTE**: Keep in mind to [unsubscribe](#unsubscribe-from-the-dispatcher) from the dispatcher before the object is destroyed, or that the dispatcher is destroyed before the object is, because otherwise the dispatcher will hold a pointer to an instance that is no longer alive.

### Dispatch an event

Now that some functions are subscribed to the dispatcher, we can dispatch an event. Let's say, in our example, somehow the user has clicked and we want to "fire" the event.

This is quite intuitive:

With some meaningless values (need to be `int, float, float` still):
```cpp
dispatcher.dispatch (0, 45.5f, 156.3f);
```

Entire file:
```cpp
#include <iostream>
#include "Events/Dispatcher.hpp"

class MyObject
{
public:
    void my_click_handler (int mb, float x, float y)
    {
        // [...] handle the click here
        std::cout << "Clicked!" << std::endl;
    }
};

int main ()
{
    lk::Dispatcher<int, float, float> dispatcher {};
    
    MyObject object;
    dispatcher += std::pair (&object, &MyObject::my_click_handler);
    
    dispatcher.dispatch (0, 45.5f, 156.3f);
    
    return 0;
}
```

### Unsubscribe from the dispatcher

This is as simple as using the same syntax as the subscription, only with `-=` instead of `+=`.

For normal functions:
```cpp
dispatcher -= my_click_handler;
```

For member functions:
```cpp
dispatcher -= std::pair (&object, &MyObject::my_click_handler);
```

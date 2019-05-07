// File "Event.hpp" created May 2019
// Copyright (c) Lion Kortlepel 2019
#ifndef EVENTS_DISPATCHER_HPP
#define EVENTS_DISPATCHER_HPP

#include <map>
#include <memory>
#include <vector>

namespace lk
{
    template<typename... _ArgTypes>
    class MemberHandlerParent
    {
        /*
         * We need the pointers one and two so that we can compare handlers.
         * This is a really ugly way to do it, but it's the only way.
         */
        void* one;
        void* two;
    public:
        MemberHandlerParent (void* one, void* two)
            : one (one), two (two)
        {}
        
        virtual void call (_ArgTypes...) = 0;
    
        bool operator== (const MemberHandlerParent& rhs) const
        {
            return one == rhs.one && two == rhs.two;
        }
    
        bool operator!= (const MemberHandlerParent& rhs) const
        {
            return !(rhs == *this);
        }
    };
    
    template<typename _T, typename... _ArgTypes>
    class MemberHandler 
        : public MemberHandlerParent<_ArgTypes...>
    {
    private:
        _T* obj;
        void (_T::*func) (_ArgTypes...);
    public:
        MemberHandler (_T* obj, void (_T::*func) (_ArgTypes...))
            : obj (obj), func (func), MemberHandlerParent<_ArgTypes...> ((void*) obj, (void*) func)
        {
        }
        
        void call (_ArgTypes... args) override
        {
            (obj->*func) (args...);
        }
    
        bool operator== (const MemberHandler& rhs)
        {        
            return static_cast<const lk::MemberHandlerParent<_ArgTypes...>&>(*this) ==
                   static_cast<const lk::MemberHandlerParent<_ArgTypes...>&>(rhs) && obj == rhs.obj && func == rhs.func;
        }
    
        bool operator!= (const MemberHandler& rhs) const
        {
            return !(rhs == *this);
        }
    };
    
    /// Dispatcher is a minimalistic event dispatcher.
    /// Functions and Methods (member-functions) can be subscribed to a 
    /// dispatcher by use of the `+=` operator, and unsubscribed by use of the
    /// `-=` operator. 
    /// 
    /// Subscribed functions will be called whenever the Dispatcher::dispatch
    /// method is called, and all subscribed functions will get the arguments
    /// passed to the Dispatcher::dispatch method.
    /// 
    /// \tparam _ArgTypes Arguments to be expected and forwarded to the 
    /// subscribed handlers from Dispatcher::dispatch.
    template<typename... _ArgTypes>
    class Dispatcher
    {
    public:
        using Signature = void (_ArgTypes...);
        using SignaturePtr = void (*) (_ArgTypes...);
    private:
        std::map<
            void*, 
            SignaturePtr
        > handlers;
        
        std::vector<
            std::unique_ptr<MemberHandlerParent<_ArgTypes...>> 
        > member_handlers;
    public:
        Dispatcher ()
        {
        }
        
        /// Subscribes a function to the Dispatcher as an Event-Handler.
        /// \param func Handler function.
        void operator+= (SignaturePtr func)
        {
            handlers.insert (std::pair ((void*) func, func));
        }
        
        /// Subscribes a member function to the Dispatcher as an Event-Handler.
        /// \tparam _ClassType Type of the object that owns the member function.
        /// \param pair Pair of _ClassType* and member function address.
        template<typename _ClassType>
        void operator+= (const std::pair<_ClassType*, 
            void (_ClassType::*) (_ArgTypes...)>& pair)
        {
            member_handlers.push_back (
                std::make_unique<MemberHandler<_ClassType, _ArgTypes...>> 
                    (pair.first, pair.second));
        }
    
        /// Unsubscribes a member function from this Dispatcher.
        /// \tparam _ClassType Type of the object that owns the member function.
        /// \param pair Pair of _ClassType* and member function address.
        template<typename _ClassType>
        void operator-= (const std::pair<_ClassType*, 
            void (_ClassType::*) (_ArgTypes...)>& pair)
        {
            std::unique_ptr<MemberHandlerParent<_ArgTypes...>> handler = 
                std::make_unique<MemberHandler<_ClassType, _ArgTypes...>> 
                (pair.first, pair.second);
            for (auto i = member_handlers.begin (); i != member_handlers.end (); ++i)
            {
                if (**i == *handler)
                {
                    member_handlers.erase (i);
                    break;
                }
            }
        }
    
        /// Unsubscribes a function from the Dispatcher.
        /// \param func Handler function.
        void operator-= (SignaturePtr func)
        {
            auto key = (void*) func;
            if (handlers.find (key) != handlers.end ())
            {
                handlers.erase (key);
            }
        }
        
        /// Dispatches the event to all handlers.
        /// \param args Arguments to be passed to all handlers. 
        void dispatch (_ArgTypes... args) const
        {
            for (auto& pair : handlers)
            {
                pair.second (std::forward (args...));
            }
            
            for (auto& member_handler : member_handlers)
            {
                member_handler->call (std::forward (args...));
            }
        }
    };
}


#endif //EVENTS_DISPATCHER_HPP
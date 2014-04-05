#pragma once

#include <chrono>

namespace wonder_rabbit_project
{
  namespace time
  {
    // repeat for future time-point
    template
    < class T_clock    = std::chrono::high_resolution_clock
    , class T_function = std::function<void()>
    >
    auto for_time_until
    ( const T_function& f
    , const typename T_clock::time_point& target_time
    )
      -> void
    {
      while(T_clock::now() < target_time)
        f();
    }
    
    // measuring time
    template
    < class T_duration = std::chrono::nanoseconds
    , class T_clock    = std::chrono::high_resolution_clock
    , class T_function = std::function<void()>
    >
    auto time(const T_function& f)
      -> T_duration
    {
      using std::chrono::duration_cast;
      
      auto start_time = T_clock::now();
      f();
      return duration_cast<T_duration>(T_clock::now() - start_time);
    }
    
    // process speed adjuster
    template
    < class T_duration    = std::chrono::nanoseconds
    , class T_clock       = std::chrono::high_resolution_clock
    , class T_function    = std::function<void()>
    , class T_duration_in = std::chrono::nanoseconds
    >
    auto adjust(const T_function& f, const T_duration_in target_time)
      -> T_duration
    {
      auto measured_time = time<T_duration, T_clock, T_function>(f);
      auto delta_time = target_time - measured_time;
      std::this_thread::sleep_for(delta_time);
      return delta_time;
    }
    
    // adjust loop with skipping
    template
    < class T_duration    = std::chrono::nanoseconds
    , class T_clock       = std::chrono::high_resolution_clock
    , class T_function    = std::function<void()>
    , class T_duration_in = std::chrono::nanoseconds
    , class T_hook_skipping = std::function<void(const T_duration&)>
    >
    auto adjust_loop_with_skipping
    ( const T_function& f
    , const T_duration_in target_time
    , const bool& condition_to_continue
    , const T_hook_skipping& hook_skipping
        = [](const T_duration&){ }
    )
      -> void
    {
      T_duration dt_bank(0);
      
      while(condition_to_continue)
      {
        if(dt_bank.count() >= 0)
          dt_bank += adjust<T_duration, T_clock>(f, target_time);
        else
        {
          hook_skipping(dt_bank);
          dt_bank += target_time;
        }
      }
    }
    
  }
}
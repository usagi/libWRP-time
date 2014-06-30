#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <string>
#include <ctime>

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace time
    {
      // std::chrono::time_point to ISO8601 std::string
      template < class T_time_point = std::chrono::steady_clock::time_point >
      auto to_string_iso8601( const T_time_point& t )
        -> std::string
      {
        using namespace std::chrono;
        const time_t ct
        ( ( duration_cast< seconds >( system_clock::now( ).time_since_epoch( ) )
          + duration_cast< seconds >( t - T_time_point::clock::now( ) )
          ).count( )
        );
        std::string r = "0000-00-00T00:00:00Z.";
        std::strftime( const_cast< char* >( r.data( ) ), r.size( ), "%FT%TZ", std::gmtime( &ct ) );
        return r;
      }
      
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
      
      template
      < class T_duration    = std::chrono::nanoseconds
      , class T_clock       = std::chrono::high_resolution_clock
      , class T_function    = std::function<void()>
      , class T_duration_in = std::chrono::nanoseconds
      >
      auto adjust_loop_with_skipping
      ( const T_function& f
      , const T_duration_in target_time
      , const bool& condition_to_continue
      )
        -> void
      { adjust_loop_with_skipping<T_duration, T_clock>(f, target_time, condition_to_continue, [](const std::chrono::nanoseconds&){}); }
      
    }
  }
}

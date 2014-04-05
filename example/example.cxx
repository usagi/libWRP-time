#include <chrono>
#include <thread>
#include <iostream>

#include <wonder_rabbit_project/time.hxx>

auto main()
  -> int
{
  using wonder_rabbit_project::time::adjust_loop_with_skipping;
  using wonder_rabbit_project::time::for_time_until;
  using wonder_rabbit_project::time::time;
  using wonder_rabbit_project::time::adjust;
  using clock_t = std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;
  using std::chrono::nanoseconds;
  using std::cerr;
  
  auto example_function = []()
  {
    using std::this_thread::sleep_for;
    sleep_for(milliseconds(100));
  };
  
  // 1. `for_time_until` and `time` example:
  // using libWRP-time `for_time_until` helper function
  
  cerr << "\n1. `for_time_until` and `time` example\n";
  
  for_time_until
  ( [&]()
    {
      // using libWRP-time `time` helper function
      auto dt = time(example_function);
      cerr << "tick! measured time: " << dt.count() << " [ns]\n";
    }
  , clock_t::now() + milliseconds(1000)
  );
  
  // 2. `adjust` example:
  
  cerr << "\n2. `adjust` example\n";
  
  for_time_until
  ( [&]()
    {
      // using libWRP-time `adjust` helper function
      auto dt = adjust(example_function, milliseconds(200));
      cerr << "tack! adjust wait: " << dt.count() << "[ns]\n";
    }
  , clock_t::now() + milliseconds(2000)
  );
  
  // 3. `adjust_loop_with_skipping` example:
  {
    cerr << "\n3. `adjust_loop_with_skipping` example\n";
    
    unsigned skip_counter          = 0;
    bool     condition_to_continue = true;

    // using libWRP-time 'adjust_loop_with_skipping'
    adjust_loop_with_skipping
    ( [&]()
      {
        example_function();
        cerr << "tick!\n";
      }
    , milliseconds(50)
    , condition_to_continue
    // (optional) hoook function if skip occurred.
    , [&](const nanoseconds& dt_bank)
      {
        cerr << "skipping! dt_bank: " << dt_bank.count() << "[ns]\n";
        if(++skip_counter > 10)
        {
          cerr << "  --> skipping over 10 count. to end the loop.\n";
          condition_to_continue = false;
        }
      }
    );
  } 
}
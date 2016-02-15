///////////////////////////////////////////////////////////////////////////////
// eathread_thread.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EATHREAD_EATHREAD_THREAD_H
#define EATHREAD_EATHREAD_THREAD_H

#include <EABase/config/eaplatform.h>

#if defined(EA_PLATFORM_LINUX)
#include <algorithm>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

namespace EA {
namespace Thread {

    static EA_CONSTEXPR int kThreadPriorityDefault = 0;

    inline bool SetThreadPriority(int priority) 
    {
    #if defined(EA_PLATFORM_LINUX)
        pid_t pid = getpid();
        int scheduler = sched_getscheduler(pid);

        // In Linux the default priority is defined as 0. This may not be correct on exotic systems.
        constexpr int defaultPriority = 0;

        // Get the min/max priority for the scheduler. This can vary per system and per scheduler.
        int minPriority = sched_get_priority_min(scheduler);
        int maxPriority = sched_get_priority_max(scheduler);
        
        // Subtract the priority from the default priority (Under Linux lower is higher priority)
        // Then clamp to the range [minPriority, maxPriortiy]
        int newPriority = std::max(std::min(defaultPriority - priority, maxPriority), minPriority);
        
        // Probably necessary. References claim that negative priorities require root access.
        // Currently this means that it's impossible to set a higher priority than default.
        newPriority = std::max(newPriority, 0);
        
        // Currently Linux deviates from POSIX standards and this function call should set the thread priority rather than the priority of the whole process.
        // It is noted that programs should not relying on this behaviour as it could be rectified eventually.
        // It should also be noted that this is setting the "niceness" and not the actual priority.
        // Changing priority would require changing the scheduler, if that's desired, but that presents other issues.
        // Niceness on linux is generally a fairly hard hint for the scheduler and should be sufficient.
        int result = setpriority(PRIO_PROCESS, pid, newPriority);
        return result >= 0;
    #else
        return false;
    #endif
    }
}} // namespace EA::Thread


#endif  // EATHREAD_EATHREAD_THREAD_H

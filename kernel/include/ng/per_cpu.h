#pragma once

#define cpu_local __attribute__((section("percpu")))
#define cpu_ref(var) (*(typeof(var) __seg_gs *)&var)

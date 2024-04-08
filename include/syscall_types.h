#pragma once

enum procstate {
	PS_COPYFDS = 0x0001,
	PS_SETRUN = 0x0002,
};

enum fault_type {
	NULL_DEREF,
	ASSERT,
};


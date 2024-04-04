#pragma once
#ifndef NX_MEMORY_H
#define NX_MEMORY_H

#include "concepts.h"
#include "utility.h"
#include <stddef.h>
#include <stdlib.h>

namespace nx {

template <class T> class unique_ptr {
	T *m_ptr;

public:
	unique_ptr()
		: m_ptr(nullptr) { }

	explicit unique_ptr(T *ptr)
		: m_ptr(ptr) { }

	template <class Derived>
		requires derived_from<T, Derived>
	explicit unique_ptr(Derived *ptr)
		: m_ptr(ptr) { }

	unique_ptr(const unique_ptr &) = delete;

	unique_ptr(unique_ptr &&other) noexcept
		: m_ptr(other.m_ptr) {
		other.m_ptr = nullptr;
	}

	template <class Derived>
		requires derived_from<Derived, T>
	explicit unique_ptr(unique_ptr<Derived> &&other) noexcept
		: m_ptr(other.release()) { }

	~unique_ptr() { delete m_ptr; }

	unique_ptr &operator=(const unique_ptr &) = delete;

	unique_ptr &operator=(unique_ptr &&other) noexcept {
		if (this != &other) {
			delete m_ptr;
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}
		return *this;
	}

	T *get() const { return m_ptr; }

	T *release() {
		T *ptr = m_ptr;
		m_ptr = nullptr;
		return ptr;
	}

	void reset(T *ptr = nullptr) {
		delete m_ptr;
		m_ptr = ptr;
	}

	void swap(unique_ptr &other) noexcept {
		void *ptr = m_ptr;
		m_ptr = other.m_ptr;
		other.m_ptr = ptr;
	}

	explicit operator bool() const { return m_ptr != nullptr; }
	T *operator->() const { return m_ptr; }
	T &operator*() const { return *m_ptr; }
};

template <class T, class... Args> unique_ptr<T> make_unique(Args &&...args) {
	return unique_ptr<T>(new T(forward<Args>(args)...));
}

template <class T> class shared_ptr {
	struct control_block {
		size_t strong_count;
		size_t weak_count;
		T value;
	};

	control_block *m_control_block;

public:
	shared_ptr()
		: m_control_block(nullptr) { }

	explicit shared_ptr(T *ptr)
		: m_control_block(new control_block { 1, 0, *ptr }) { }

	shared_ptr(const shared_ptr &other)
		: m_control_block(other.m_control_block) {
		if (m_control_block) {
			m_control_block->strong_count++;
		}
	}

	shared_ptr(shared_ptr &&other) noexcept
		: m_control_block(other.m_control_block) {
		other.m_control_block = nullptr;
	}

	~shared_ptr() {
		if (m_control_block) {
			m_control_block->strong_count--;
			if (m_control_block->strong_count == 0) {
				delete m_control_block;
			}
		}
	}

	shared_ptr &operator=(const shared_ptr &other) {
		if (this != &other) {
			if (m_control_block) {
				m_control_block->strong_count--;
				if (m_control_block->strong_count == 0) {
					delete m_control_block;
				}
			}
			m_control_block = other.m_control_block;
			if (m_control_block) {
				m_control_block->strong_count++;
			}
		}
		return *this;
	}

	shared_ptr &operator=(shared_ptr &&other) noexcept {
		if (this != &other) {
			if (m_control_block) {
				m_control_block->strong_count--;
				if (m_control_block->strong_count == 0) {
					delete m_control_block;
				}
			}
			m_control_block = other.m_control_block;
			other.m_control_block = nullptr;
		}
		return *this;
	}

	T *get() const {
		return m_control_block ? &m_control_block->value : nullptr;
	}
	T *operator->() const { return get(); }
	T &operator*() const { return *get(); }
};

template <class T, class... Args> shared_ptr<T> make_shared(Args &&...args) {
	return shared_ptr<T>(new T(forward<Args>(args)...));
}

}

#endif // NX_MEMORY_H

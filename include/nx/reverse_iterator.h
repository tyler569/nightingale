#pragma once
#ifndef NX_REVERSE_ITERATOR_H
#define NX_REVERSE_ITERATOR_H

namespace nx {

template <class T> class reverse_iterator {
	T m_it;

public:
	explicit constexpr reverse_iterator(T it)
		: m_it(it) { }

	T &operator*() { return *m_it; }
	T *operator->() { return m_it; }

	reverse_iterator &operator++() {
		m_it--;
		return *this;
	}

	reverse_iterator &operator--() {
		m_it++;
		return *this;
	}

	reverse_iterator operator++(int) {
		reverse_iterator tmp = *this;
		m_it--;
		return tmp;
	}

	reverse_iterator operator--(int) {
		reverse_iterator tmp = *this;
		m_it++;
		return tmp;
	}

	reverse_iterator operator+(int n) {
		reverse_iterator tmp = *this;
		tmp.m_it -= n;
		return tmp;
	}

	reverse_iterator operator-(int n) {
		reverse_iterator tmp = *this;
		tmp.m_it += n;
		return tmp;
	}

	reverse_iterator &operator+=(int n) {
		m_it -= n;
		return *this;
	}

	reverse_iterator &operator-=(int n) {
		m_it += n;
		return *this;
	}

	bool operator==(const reverse_iterator &other) const {
		return m_it == other.m_it;
	}

	bool operator!=(const reverse_iterator &other) const {
		return m_it != other.m_it;
	}

	int operator<=>(const reverse_iterator &other) const {
		return m_it <=> other.m_it;
	}
};

}

#endif // NX_REVERSE_ITERATOR_H

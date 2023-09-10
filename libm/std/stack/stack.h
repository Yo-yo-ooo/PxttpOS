#pragma once

#include "../vector/vector.h"

namespace std
{
    template<class T>
	class stack
	{
	public:
		stack() {} // 自定义类型自动调用构造与析构
		void push(const T& x) {_c.push_back(x);}
		void pop() {_c.pop_back();}
		T& top() {return _c.back();}
		const T& top()const {return _c.back();}
		size_t size()const {return _c.size();}
		bool empty()const {return _c.empty();}
	private:
			std::vector<T> _c;
    }
} // namespace std

#pragma once
#include "stdafx.h"

class ThreadDelegateFunctor
{
public:
	virtual void complete() = 0;
	virtual ~ThreadDelegateFunctor() {};
};

template<typename R>
class DelegateFunctor:public ThreadDelegateFunctor
{
public:
	R complete() = 0;
};

template<typename ... Args> class DelegateFunctorImpl;

template<typename R, typename ... Args>
class DelegateFunctorImpl<R(Args ...)>
{
public:
	typedef R(*FT)(Args ...);

	DelegateFunctorImpl(FT methodFunction,Args&&... args) : methodFunction_(methodFunction),m_args(std::forward_as_tuple<Args>(args)...) { ; }

	R complete()
	{
		return methodFunction_(m_args);
	}
private:
	FT methodFunction_;
	std::tuple<Args...> m_args;
};


//TODO: change
template<typename FT>
class Task : public DelegateFunctorImpl<FT>
{
public:
	Task() : DelegateFunctorImpl<FT>(NULL) { ; }
	Task(FT function) : DelegateFunctorImpl<FT>(function) { ; }
};

template<typename T, typename R, typename ... Args>
class DelegateFunctorImpl<R(T::*)(Args ...)>:public DelegateFunctor<R>
{
public:
	typedef R(T::*FT)(Args ...);
	typedef T HostType;

	DelegateFunctorImpl(FT fn, T* obj, Args&&... args) : m_fn(fn), m_obj(obj), m_args(std::forward<Args>(args)...) { ; }

	R complete()
	{
		return (m_obj->*m_fn)(std::get<Args>(m_args)...);
	}

	void setArgs(Args&&... args)
	{
		m_args = std::forward<Args>(args)...;
	}
private:
	FT m_fn;
	T* m_obj;
	std::tuple<Args...> m_args;
};


//TODO: change
template<typename FT>
class Closure : public DelegateFunctorImpl<FT>
{
public:
	typedef typename DelegateFunctorImpl<FT>::HostType HostType;
	Closure(HostType* obj, FT fn) : DelegateFunctorImpl<FT>(fn, obj) { ; }
};
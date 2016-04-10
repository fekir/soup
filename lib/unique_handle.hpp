/*
	Copyright (C) 2016 Federico Kircheis

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <utility>
#include <stdexcept>

template< class T, typename Deleter, T invalidvalue>
class unique_handle1{
private:
	T handle = invalidvalue;
	const Deleter D = {}; // temporary fix, I'm apparently unable to pass the structure as template parameter and use it directly
public:
	// constructible
	constexpr unique_handle1() = default;
	constexpr explicit unique_handle1( const T& h ) : handle(h){};

	// destructible
	~unique_handle1(){
		this->reset();
	}

	// movable
	//unique_handle1( unique_handle1<T, Deleter, invalidvalue>&& u )  noexcept = default;
	//unique_handle1& operator=( unique_handle1<T, Deleter, invalidvalue>&& other)  noexcept = default;

	// movable
	unique_handle1(unique_handle1<T, Deleter, invalidvalue>&& other) noexcept : handle(std::move(other.validhandle)), handle(std::move(other.handle)) {
		other.validhandle = false;
	}
	unique_handle1& operator=(unique_handle1<T, Deleter, invalidvalue>&& other)  noexcept {
		this->validhandle = std::move(other.validhandle);
		other.validhandle = false;
		this->handle = std::move(other.handle);
		return *this;
	}

	// non copyable
	unique_handle1( const unique_handle1<T, Deleter, invalidvalue>& other )	  = delete; // non construction-copyable
	unique_handle1& operator=( const unique_handle1<T, Deleter, invalidvalue>& ) = delete; // non copyable

	// Modifiers
	T release(){
		const T tmp = handle;
		handle = invalidvalue;
		return tmp;
	}
	void reset(const T& h) {
		reset();
		handle = h;
	}
	void reset() {
		if((*this)){
			D(this->handle);
			handle = invalidvalue;
		}
	}

	void swap(unique_handle1& other) noexcept(swap(std::declval<T&>(), std::declval<T&>())){
		using std::swap;
		swap(handle, other.handle);
	}

	// Observers
	const T& get() const {
		return handle;
	}

	explicit operator bool() const{
		return handle != invalidvalue;
	}
};

template< class T, class Deleter>
class unique_handle2{
private:
	bool validhandle = false;
	T handle;
	const Deleter D = {}; // temporary fix, I'm apparently unable to pass the structure as template parameter and use it directly
public:
	// constructible
	unique_handle2() = default; //unable to make constexpr since it does not initialize all members
	constexpr explicit unique_handle2( const T& h ) noexcept : validhandle(true), handle(h) {};

	// destructible
	~unique_handle2(){
		this->reset();
	}

	// movable
	unique_handle2( unique_handle2<T, Deleter>&& other ) noexcept : validhandle(std::move(other.validhandle)), handle(std::move(other.handle)){
		other.validhandle = false;
	}
	unique_handle2& operator=( unique_handle2<T, Deleter>&& other)  noexcept {
		this->validhandle = std::move(other.validhandle);
		other.validhandle = false;
		this->handle = std::move(other.handle);
		return *this;
	}

	// non copyable
	unique_handle2( const unique_handle2<T, Deleter>& other )	   = delete; // non construction-copyable
	unique_handle2& operator=( const unique_handle2<T, Deleter>& ) = delete; // non copyable

	// Modifiers
	T release(){
		if(!validhandle){
			throw std::runtime_error("no valid handle to release!");
		}
		validhandle = false;
		return handle;
	}
	void reset(const T& h) {
		reset();
		handle = h;
		validhandle = true;
	}
	void reset() {
		if(validhandle){
			//Deleter.operator()(this->handle);
			D(handle);
			validhandle = false;
		}
	}

	void swap(unique_handle2& other) noexcept(swap(std::declval<T&>(), std::declval<T&>())){
		using std::swap;
		swap(handle, other.handle);
		swap(validhandle, other.validhandle);
	}

	// Observers
	const T& get() const {
		if(!validhandle) {
			throw std::runtime_error("no valid handle to get!");
		}
		return handle;
	}

	explicit operator bool() const noexcept{
		return validhandle;
	}
};


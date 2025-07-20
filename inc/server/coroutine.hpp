#pragma once

#include <bit>
#include <coroutine>
#include <exception>
#include <nlohmann/json.hpp>
#include <sys/socket.h>

#include "common/logger.hpp"

// i can:
// refuse from oop. no return type, take pointer to potential json, and msgtype.
// dynamic polymorhism
// ???

using Size = std::string::size_type;

struct userThread {

    struct promise_type {
    using coro_handle = std::coroutine_handle<promise_type>;

    userThread get_return_object() {
        return {
        ._handle = std::coroutine_handle<promise_type>::from_promise(*this)
        };
    }

    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }

    void return_void() {}

    // since i don;t throw any exception it can be empty
    // overwise check exception_ptr cppref
    void unhandled_exception() {}
  };

  std::coroutine_handle<promise_type> _handle;

  operator std::coroutine_handle<promise_type>() const { return _handle; }
  operator std::coroutine_handle<>() const { return _handle; }
};

// MB I NEED TO USE POLL
void sendStat( int serverSocket, std::string&& json ) {
    send(serverSocket, json.c_str(), json.size(), 0);
}

/*
   // it should 
    auto len = json.size();
    char* binLen = reinterpret_cast< char* >( &len );

    json = binLen + json;
   */
// collect statistics from users
userThread collectStat( int clientSock, std::string& json ) {
}

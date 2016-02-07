/*
 * stdin.hpp
 *
 *  Created on: 07.02.2016
 *      Author: Klemens
 */

#ifndef INCLUDE_BOOST_PROCESS_WINDOWS_INITIALIZERS_STDIN_HPP_
#define INCLUDE_BOOST_PROCESS_WINDOWS_INITIALIZERS_STDIN_HPP_

#include <cstddef>
#include <boost/process/detail/initializers/base.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/detail/winapi/handle_info.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/process/windows/pipe.hpp>

namespace boost { namespace process { namespace windows { namespace initializers {

struct sync_stdin : ::boost::process::detail::initializers::base
{
    explicit sync_stdin(const boost::iostreams::file_descriptor_source &source) : source_(source) {}

    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        ::boost::detail::winapi::SetHandleInformation(source_.handle(),
                ::boost::detail::winapi::HANDLE_FLAG_INHERIT_,
                ::boost::detail::winapi::HANDLE_FLAG_INHERIT_);
        e.startup_info.hStdInput = source_.handle();
        e.startup_info.dwFlags |= ::boost::detail::winapi::STARTF_USESTDHANDLES_;
        e.inherit_handles = true;
    }

private:
    boost::iostreams::file_descriptor_source source_;
};

struct async_stdin : ::boost::process::detail::initializers::base
{

};

struct close_stdin : public ::boost::process::detail::initializers::base
{
    template <class WindowsExecutor>
    void on_setup(WindowsExecutor &e) const
    {
        e.startup_info.hStdInput = boost::detail::winapi::INVALID_HANDLE_VALUE_;
        e.startup_info.dwFlags  |= boost::detail::winapi::STARTF_USESTDHANDLES_;
    }
};


struct std_in_
{
    close_stdin operator()(std::nullptr_t) const { return close_stdin();}
    close_stdin operator= (std::nullptr_t) const { return close_stdin();}
    sync_stdin operator()(const boost::iostreams::file_descriptor_source & source) const {return sync_stdin(source);}
    sync_stdin operator= (const boost::iostreams::file_descriptor_source & source) const {return sync_stdin(source);}
    sync_stdin operator()(const boost::iostreams::file_descriptor & descr) const {return sync_stdin(descr.handle());}
    sync_stdin operator= (const boost::iostreams::file_descriptor & descr) const {return sync_stdin(descr.handle());}
    sync_stdin operator()(boost::iostreams::stream<boost::iostreams::file_descriptor> & strm) const {return sync_stdin(strm->handle());}
    sync_stdin operator= (boost::iostreams::stream<boost::iostreams::file_descriptor> & strm) const {return sync_stdin(strm->handle());}
    sync_stdin operator()(boost::process::windows::pipe& pipe_) const {return sync_stdin(boost::iostreams::file_descriptor_source(pipe_.source));}
    sync_stdin operator= (boost::process::windows::pipe& pipe_) const {return sync_stdin(boost::iostreams::file_descriptor_source(pipe_.source));}

    async_stdin operator()(std::istream & ostr) const {return async_stdin(ostr);}
    async_stdin operator= (std::istream & ostr) const {return async_stdin(ostr);}

    template<template<class> class Container> async_stdin operator()(Container<char> & buffer) const {return async_stdin(buffer); };
    template<template<class> class Container> async_stdin operator= (Container<char> & buffer) const {return async_stdin(buffer); };
};

constexpr std_in_ std_in;


}}}}
#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_INITIALIZERS_STDIN_HPP_ */
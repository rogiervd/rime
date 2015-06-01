/*
Copyright 2014 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef RIME_NOTHING_HPP_INCLUDED
#define RIME_NOTHING_HPP_INCLUDED

namespace rime {

namespace callable {

    /**
    Functor that does nothing when called.
    Its <c>operator()</c> takes any arguments, always returns void, and never
    performs any action.
    This can be useful, for example, as argument to rime::call_if.
    */
    struct nothing {
        template <class ... Arguments>
            void operator() (Arguments const & ...) const {}
    };

} // namespace callable

/**
Do nothing.

This is a function object that can be called with anything, and it will never
perform any action, and will always return void.
This can be useful, for example, as argument to rime::call_if.
*/
static const auto nothing = callable::nothing();

} // namespace rime

#endif // RIME_NOTHING_HPP_INCLUDED

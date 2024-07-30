#ifndef DIADEL_EVENT_HPP
#define DIADEL_EVENT_HPP

#include "MemberDelegate.hpp"

#include <memory>

namespace dd
{
    /**
     * A wrapper for a single delegate. Allows any subclass of Delegate to be bound.
     * @tparam Args Delegate arguments
     */
    template<typename... Args>
    class Event final
    {
    public:
        Event() = default;
        explicit Event(std::unique_ptr<Delegate<Args...>> del)
            : m_delegate(std::move(del)) {}
        Event(const Event& other)
            : m_delegate(other.m_delegate->clone()) {}
        Event(Event&& other) noexcept
            : m_delegate(std::move(other.m_delegate)) {}

        Event& operator=(const Event& other)
        {
            m_delegate.reset(other.m_delegate->clone());
            return *this;
        }

        Event& operator=(Event&& other) noexcept
        {
            m_delegate = std::move(other.m_delegate);
            return *this;
        }

        Event& operator=(std::unique_ptr<Delegate<Args...>> del)
        {
            m_delegate = std::move(del);
            return *this;
        }

        ~Event() = default;

        /**
         * Binds a function to this event
         * @tparam C Object class
         * @param object Object context
         * @param function Class method
         */
        template<typename C>
        void bind(C* object, void(C::*function)(Args...))
        {
            m_delegate = std::make_unique<MemberDelegate<C, Args...>>(object, function);
        }

        /**
         * Removes any bound function
         */
        void unbind() {m_delegate.reset();}

        /**
         * Executes the bound function
         * @param args Function arguments
         */
        void execute(Args... args)
        {
            if (!m_delegate) return;

            (*m_delegate)(execute(args...));
        }

    private:
        std::unique_ptr<Delegate<Args...>> m_delegate;
    };
}

#endif // DIADEL_EVENT_HPP

#ifndef DIADEL_EVENT_HPP
#define DIADEL_EVENT_HPP

#include "MemberDelegate.hpp"
#include "FunctorDelegate.hpp"

#include <memory>

namespace dd
{
    /**
     * A wrapper for a single delegate. Allows any subclass of Delegate to be bound.
     * @tparam ArgsT Delegate arguments
     */
    template<typename... ArgsT>
    class Event final
    {
    public:
        using DelegatePtr = std::unique_ptr<Delegate<ArgsT...>>;
        
        Event() = default;
        explicit Event(DelegatePtr del)
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

        Event& operator=(DelegatePtr del)
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
        void bind(C* object, void(C::*function)(ArgsT...))
        {
            m_delegate = std::make_unique<MemberDelegate<C, ArgsT...>>(object, function);
        }

        /**
         * Binds a function pointer to this event
         * @param function Functor
         */
        void bind(void(*function)(ArgsT...))
        {
            m_delegate = std::make_unique<FunctorDelegate<ArgsT...>>(function);
        }

        /**
         * Removes any bound function
         */
        void unbind() {m_delegate.reset();}

        /**
         * Executes the bound function
         * @param args Function arguments
         */
        void execute(ArgsT&&... args)
        {
            if (!m_delegate) return;

            (*m_delegate)(execute(std::forward<ArgsT>(args)...));
        }

    private:
        DelegatePtr m_delegate;
    };
}

#endif // DIADEL_EVENT_HPP

#ifndef DIADEL_FUNCTORDELEGATE_HPP
#define DIADEL_FUNCTORDELEGATE_HPP

#include "Delegate.hpp"

namespace dd
{
    template<typename... Args>
    struct FunctorDelegate : Delegate<Args...>
    {
        FunctorDelegate() = default;
        FunctorDelegate(const FunctorDelegate& other)
            : m_function(other.m_function) {}
        FunctorDelegate(FunctorDelegate&& other) noexcept
            : m_function(other.m_function)
        {
            other.reset();
        }
        explicit FunctorDelegate(void(*function)(Args...))
            : m_function(function) {}

        FunctorDelegate& operator=(const FunctorDelegate& other)
        {
            if (other != *this)
                m_function = other.m_function;

            return *this;
        }

        FunctorDelegate& operator=(FunctorDelegate&& other) noexcept
        {
            if (other != *this)
            {
                m_function = other.m_function;
                other.reset();
            }

            return *this;
        }

        FunctorDelegate& operator=(void(*function)(Args...))
        {
            m_function = function;
            return *this;
        }

        bool operator==(const FunctorDelegate& other) {return m_function == other.m_function;}
        bool operator!=(const FunctorDelegate& other) {return !(*this == other);}
        bool operator==(void(*function)(Args...)) {return m_function == function;}
        bool operator!=(void(*function)(Args...)) {return !(*this == function);}

        ~FunctorDelegate() override = default;
        
        std::unique_ptr<Delegate<Args...>> clone() const override
        {
            return std::make_unique<FunctorDelegate>(m_function);
        }

        void reset() override
        {
            m_function = nullptr;
        }
        
    private:
        void execute(Args... args) override
        {
            if (m_function)
                (*m_function)(args...);
        }
        
        void(*m_function)(Args...) {nullptr};
    };
}

#endif // DIADEL_FUNCTORDELEGATE_HPP
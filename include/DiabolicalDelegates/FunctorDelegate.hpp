#ifndef DIADEL_FUNCTORDELEGATE_HPP
#define DIADEL_FUNCTORDELEGATE_HPP

#include "Delegate.hpp"

namespace dd
{
    /**
     * Specialised wrapper for free-floating/global functions with no context
     * @tparam ArgsT Function arguments
     */
    template<typename... ArgsT>
    struct FunctorDelegate : Delegate<ArgsT...>
    {
        FunctorDelegate() = default;
        FunctorDelegate(const FunctorDelegate& other)
            : m_function(other.m_function) {}
        FunctorDelegate(FunctorDelegate&& other) noexcept
            : m_function(other.m_function)
        {
            other.reset();
        }
        explicit FunctorDelegate(void(*function)(ArgsT...))
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

        FunctorDelegate& operator=(void(*function)(ArgsT...))
        {
            m_function = function;
            return *this;
        }

        bool operator==(const FunctorDelegate& other) {return m_function == other.m_function;}
        bool operator!=(const FunctorDelegate& other) {return !(*this == other);}
        bool operator==(void(*function)(ArgsT...)) {return m_function == function;}
        bool operator!=(void(*function)(ArgsT...)) {return !(*this == function);}

        ~FunctorDelegate() override = default;
        
        [[nodiscard]] std::unique_ptr<Delegate<ArgsT...>> clone() const override
        {
            return std::make_unique<FunctorDelegate>(m_function);
        }

        void reset() noexcept override
        {
            m_function = nullptr;
        }
        
    private:
        void execute(ArgsT&&... args) override
        {
            if (m_function)
                (*m_function)(std::forward<ArgsT>(args)...);
        }
        
        void(*m_function)(ArgsT...) {nullptr};
    };
}

#endif // DIADEL_FUNCTORDELEGATE_HPP

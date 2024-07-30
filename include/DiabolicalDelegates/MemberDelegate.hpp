#ifndef DIADEL_MEMBERDELEGATE_HPP
#define DIADEL_MEMBERDELEGATE_HPP

#include "Delegate.hpp"

namespace dd
{
    /**
     * Specialised delegate that holds class member functions along with their class context as an object
     */
    template<typename C, typename... Args>
    struct MemberDelegate : Delegate<Args...>
    {
        MemberDelegate() = default;
        MemberDelegate(const MemberDelegate& other)
            : m_object(other.m_object), m_function(other.m_function) {}
        MemberDelegate(MemberDelegate&& other) noexcept
            : m_object(other.m_object), m_function(other.m_function)
        {
            other.reset();
        }
        MemberDelegate(C* object, void(C::*function)(Args...))
            : m_object(object), m_function(function) {}

        MemberDelegate& operator=(const MemberDelegate& other)
        {
            if (other != *this)
            {
                m_object = other.m_object;
                m_function = other.m_function;
            }
            return *this;
        }

        MemberDelegate& operator=(MemberDelegate&& other) noexcept
        {
            if (other != *this)
            {
                m_object = other.m_object;
                m_function = other.m_function;
                other.reset();
            }
            return *this;
        }

        MemberDelegate& operator=(void(C::*function)(Args...))
        {
            m_function = function;
            return *this;
        }

        bool operator==(const MemberDelegate& other)
        {
            return m_object == other.m_object && m_function == other.m_function;
        }

        bool operator!=(const MemberDelegate& other)
        {
            // CLion says this can be simplified, but then simplifies it to *this != other, which would make an
            // infinite loop, no? funny things
            return !(*this == other);
        }

        ~MemberDelegate() override = default;

        std::unique_ptr<Delegate<Args...>> clone() const override
        {
            return std::make_unique<MemberDelegate>(m_object, m_function);
        }

        void reset() override
        {
            m_object = nullptr;
            m_function = nullptr;
        }

        C* getObject() const {return m_object;}

    private:
        void execute(Args... args) override
        {
            if (m_object && m_function)
                (m_object->*m_function)(args...);
        }

        C* m_object {nullptr};
        void(C::*m_function)(Args...) {nullptr};
    };
}

#endif // DIADEL_MEMBERDELEGATE_HPP

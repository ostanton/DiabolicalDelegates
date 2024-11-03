#ifndef DIADEL_MEMBERDELEGATE_HPP
#define DIADEL_MEMBERDELEGATE_HPP

#include "Delegate.hpp"

namespace dd
{
    /**
     * Specialised delegate that holds class member functions along with their class context as an object
     * @tparam C Class context
     * @tparam ArgsT Function arguments
     */
    template<typename C, typename... ArgsT>
    struct MemberDelegate : Delegate<ArgsT...>
    {
        MemberDelegate() = default;
        MemberDelegate(const MemberDelegate& other)
            : m_object(other.m_object), m_function(other.m_function) {}
        MemberDelegate(MemberDelegate&& other) noexcept
            : m_object(other.m_object), m_function(other.m_function)
        {
            other.reset();
        }
        MemberDelegate(C* object, void(C::*function)(ArgsT...))
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

        MemberDelegate& operator=(void(C::*function)(ArgsT...))
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

        [[nodiscard]] std::unique_ptr<Delegate<ArgsT...>> clone() const override
        {
            return std::make_unique<MemberDelegate>(m_object, m_function);
        }

        void reset() noexcept override
        {
            m_object = nullptr;
            m_function = nullptr;
        }

        [[nodiscard]] C* getObject() const noexcept {return m_object;}

    private:
        void execute(ArgsT&&... args) override
        {
            if (m_object && m_function)
                (m_object->*m_function)(std::forward<ArgsT>(args)...);
        }

        C* m_object {nullptr};
        void(C::*m_function)(ArgsT...) {nullptr};
    };
}

#endif // DIADEL_MEMBERDELEGATE_HPP

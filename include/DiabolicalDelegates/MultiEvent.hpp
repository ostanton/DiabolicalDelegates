#ifndef DIALDEL_MULTIEVENT_HPP
#define DIALDEL_MULTIEVENT_HPP

#include "MemberDelegate.hpp"
#include "FunctorDelegate.hpp"

#include <vector>
#include <memory>
#include <algorithm>

namespace dd
{
    /**
     * A wrapper for multiple delegates, of varying types, all callable via the broadcast() member function.
     * @tparam ArgsT Delegate arguments
     */
    template<typename... ArgsT>
    class MultiEvent final
    {
    public:
        using DelegatePtr = std::unique_ptr<Delegate<ArgsT...>>;
        using Container = std::vector<DelegatePtr>;
        using Iterator = typename Container::iterator;
        using ConstIterator = typename Container::const_iterator;
        
        MultiEvent() = default;

        // Move conversion from single unique delegate
        explicit MultiEvent(DelegatePtr del)
        {
            m_delegates.emplace_back(std::move(del));
        }

        // Move conversion from vector of delegates
        explicit MultiEvent(Container&& delegates)
        {
            m_delegates = std::move(std::forward<Container>(delegates));
        }

        // Copy constructor
        MultiEvent(const MultiEvent& other)
        {
            copy(other);
        }

        // Move constructor
        MultiEvent(MultiEvent&& other) noexcept
        {
            move(std::move(other));
        }

        // Copy assignment operator
        MultiEvent& operator=(const MultiEvent& other)
        {
            copy(other);
            return *this;
        }

        // Move assignment operator
        MultiEvent& operator=(MultiEvent&& other) noexcept
        {
            move(std::move(other));
            return *this;
        }

        MultiEvent& operator+(DelegatePtr del)
        {
            m_delegates.emplace_back(std::move(del));
            return *this;
        }

        MultiEvent& operator+(const MultiEvent& other)
        {
            for (auto& del : other.m_delegates)
            {
                m_delegates.emplace_back(del->clone());
            }

            return *this;
        }

        MultiEvent& operator+(MultiEvent&& other)
        {
            for (auto& del : other.m_delegates)
            {
                m_delegates.emplace_back(std::move(del));
            }

            return *this;
        }

        MultiEvent& operator-(Delegate<ArgsT...>* del)
        {
            if (!del) return *this;
            
            if (auto it = std::find_if(m_delegates.begin(), m_delegates.end(),
                [&](auto& uniqueDel)
                {
                    return *uniqueDel == *del;
                }); it != m_delegates.end())
            {
                m_delegates.erase(it);
            }

            return *this;
        }

        ~MultiEvent() = default;

        /**
         * Adds the specified member function
         * @tparam C Object class
         * @param object Object context
         * @param function Class method
         */
        template<typename C>
        void add(C* object, void(C::*function)(ArgsT...))
        {
            m_delegates.emplace_back(std::make_unique<MemberDelegate<C, ArgsT...>>(object, function));
        }

        /**
         * Adds the specified function
         * @param function Functor
         */
        void add(void(*function)(ArgsT...))
        {
            m_delegates.emplace_back(std::make_unique<FunctorDelegate<ArgsT...>>(function));
        }

        /**
         * Adds the specified member function if it does not already exist
         * @tparam C Object class
         * @param object Object context
         * @param function Class method
         * @return Successful
         */
        template<typename C>
        bool addUnique(C* object, void(C::*function)(ArgsT...))
        {
            // fail to add if it already exists
            if (auto it = getDelegateIterator(object, function); it != m_delegates.end())
                return false;

            add(object, function);
            return true;
        }

        /**
         * Adds the specified function if it does not already exist
         * @param function Functor
         * @return Successful
         */
        bool addUnique(void(*function)(ArgsT...))
        {
            // fail to add if it already exists
            if (auto it = getDelegateIterator(function); it != m_delegates.end())
                return false;

            add(function);
            return true;
        }

        /**
         * Removes the specified member function
         * @tparam C Object class
         * @param object Object context
         * @param function Class method
         * @return Successful
         */
        template<typename C>
        bool remove(C* object, void(C::*function)(ArgsT...))
        {
            if (auto it = getDelegateIterator(object, function); it != m_delegates.end())
            {
                m_delegates.erase(it);
                return true;
            }

            return false;
        }

        /**
         * Removes the specified function
         * @param function Functor
         * @return Successful
         */
        bool remove(void(*function)(ArgsT...))
        {
            if (auto it = getDelegateIterator(function); it != m_delegates.end())
            {
                m_delegates.erase(it);
                return true;
            }

            return false;
        }

        /**
         * Executes every delegate that this MultiEvent owns
         * @param args Function arguments
         */
        void broadcast(ArgsT&&... args)
        {
            if (m_delegates.empty()) return;

            for (auto& del : m_delegates)
            {
                (*del)(std::forward<ArgsT>(args)...);
            }
        }

    private:
        /**
         * Copies another MultiEvent into this one
         * @param other The MultiEvent to copy
         */
        void copy(const MultiEvent& other)
        {
            m_delegates.clear();
            m_delegates.reserve(other.m_delegates.size());

            for (auto& del : other.m_delegates)
            {
                m_delegates.emplace_back(del->clone());
            }
        }

        /**
         * Moves another MultiEvent into this one
         * @param other The MultiEvent to move
         */
        void move(MultiEvent&& other) noexcept
        {
            m_delegates = std::move(other.m_delegates);
        }

        template<typename C>
        ConstIterator getDelegateIterator(C* object, void(C::*function)(ArgsT...)) const
        {
            MemberDelegate<C, ArgsT...> del {object, function};
            return std::find_if(m_delegates.begin(), m_delegates.end(),
                [&](auto& uniqueDelegate)
                {
                    return *uniqueDelegate == del;
                });
        }

        ConstIterator getDelegateIterator(void(*function)(ArgsT...)) const
        {
            FunctorDelegate<ArgsT...> del {function};
            return std::find_if(m_delegates.begin(), m_delegates.end(),
                [&](auto& uniqueDelegate)
                {
                    return *uniqueDelegate == del;
                });
        }

        Container m_delegates;
    };
}

#endif // DIALDEL_MULTIEVENT_HPP

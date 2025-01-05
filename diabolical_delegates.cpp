export module ostanton.dd;

import std;

export namespace ostanton
{
    namespace dd
    {
        /**
         * Interface for implementation-defined delegates
         * @tparam RetT Return type
         * @tparam ArgsT Argument types
         */
        template<typename RetT = void, typename... ArgsT>
        class IDelegate
        {
        public:
            IDelegate() = default;
            IDelegate(const IDelegate&) = default;
            IDelegate(IDelegate&&) noexcept = default;
            IDelegate& operator=(const IDelegate&) = default;
            IDelegate& operator=(IDelegate&&) noexcept = default;
            virtual ~IDelegate() = default;

            [[nodiscard]] bool operator!=(const IDelegate& other) const {return !(*this == other);}

            RetT operator()(ArgsT&&... args) const
            {
                return execute(std::forward<ArgsT>(args)...);
            }

            [[nodiscard]] virtual IDelegate* clone() const = 0;
            [[nodiscard]] virtual bool equal(IDelegate*) const = 0;

        protected:
            virtual RetT execute(ArgsT&&... args) const = 0;
        };

        /**
         * Delegate for class member functions
         * @tparam C Class type
         * @tparam RetT Return type
         * @tparam ArgsT Argument types
         */
        template<typename C, typename RetT = void, typename... ArgsT>
        requires std::is_class_v<C>
        class CMemberDelegate final : public IDelegate<RetT, ArgsT...>
        {
        public:
            using FunctionType = RetT(C::*)(ArgsT...);
            using IDelegate<RetT, ArgsT...>::IDelegate;
            explicit CMemberDelegate(C* object, const FunctionType function)
                : m_object(object), m_function(function)
            {}

            [[nodiscard]] bool operator==(const CMemberDelegate& other) const
            {
                return m_object == other.m_object
                    && m_function == other.m_function;
            }

            [[nodiscard]] C* getObject() const noexcept {return m_object;}

        private:
            [[nodiscard]] CMemberDelegate* clone() const override
            {
                return new CMemberDelegate(m_object, m_function);
            }

            [[nodiscard]] bool equal(IDelegate<RetT, ArgsT...>* other) const override
            {
                // feels very wrong, but seems to work so hey-ho
                if (auto const otherMember = dynamic_cast<CMemberDelegate*>(other))
                    return *this == *otherMember;

                return false;
            }
            
            RetT execute(ArgsT&&... args) const override
            {
                return (m_object->*m_function)(std::forward<ArgsT>(args)...);
            }

            C* m_object {nullptr};
            FunctionType m_function {nullptr};
        };

        /**
         * Delegate for C-style functions/functors
         * @tparam RetT Return type
         * @tparam ArgsT Argument types
         */
        template<typename RetT = void, typename... ArgsT>
        class CFunctorDelegate final : public IDelegate<RetT, ArgsT...>
        {
        public:
            using FunctionType = RetT(*)(ArgsT...);
            using IDelegate<RetT, ArgsT...>::IDelegate;
            explicit CFunctorDelegate(const FunctionType function)
                : m_function(function)
            {}

            [[nodiscard]] bool operator==(const CFunctorDelegate& other) const
            {
                return m_function == other.m_function;
            }

        private:
            [[nodiscard]] CFunctorDelegate* clone() const override
            {
                return new CFunctorDelegate(m_function);
            }

            [[nodiscard]] bool equal(IDelegate<RetT, ArgsT...>* other) const override
            {
                // also feels wrong, same reason, etc., etc. whatever
                if (auto const otherFunctor = dynamic_cast<CFunctorDelegate*>(other))
                    return *this == *otherFunctor;

                return false;
            }
            
            RetT execute(ArgsT&&... args) const override
            {
                return (*m_function)(std::forward<ArgsT>(args)...);
            }
            
            FunctionType m_function;
        };

        /**
         * Event for any kind of delegate
         * @tparam RetT Return type
         * @tparam ArgsT Argument types
         */
        template<typename RetT = void, typename... ArgsT>
        class CEventWithReturn final
        {
        public:
            template<typename C>
            requires std::is_class_v<C>
            using MemberFunctionType = typename CMemberDelegate<C, RetT, ArgsT...>::FunctionType;
            using FunctorFunctionType = typename CFunctorDelegate<RetT, ArgsT...>::FunctionType;
            using DelegateType = IDelegate<RetT, ArgsT...>;
            using DelegatePtr = std::unique_ptr<DelegateType>;

            CEventWithReturn() = default;
            CEventWithReturn(const CEventWithReturn& other)
                : m_delegate(other.m_delegate->clone())
            {}
            CEventWithReturn(CEventWithReturn&&) noexcept = default;
            CEventWithReturn& operator=(const CEventWithReturn& other)
            {
                m_delegate.reset(other.m_delegate->clone());
                return *this;
            }
            CEventWithReturn& operator=(CEventWithReturn&&) noexcept = default;
            ~CEventWithReturn() = default;

            template<typename C>
            requires std::is_class_v<C>
            void bind(C* object, MemberFunctionType<C> function)
            {
                m_delegate = std::make_unique<CMemberDelegate<C, RetT, ArgsT...>>(object, function);
            }

            void bind(FunctorFunctionType function)
            {
                m_delegate = std::make_unique<CFunctorDelegate<RetT, ArgsT...>>(function);
            }

            void unbind()
            {
                m_delegate.reset();
            }

            RetT execute(ArgsT&&... args) const
            {
                return (*m_delegate)(std::forward<ArgsT>(args)...);
            }

        private:
            DelegatePtr m_delegate;
        };

        template<typename... ArgsT>
        using CEvent = CEventWithReturn<void, ArgsT...>;

        /**
         * Event for any number of delegates
         * @tparam RetT Return type
         * @tparam ArgsT Argument types
         */
        template<typename RetT = void, typename... ArgsT>
        class CMultiEventWithReturn final
        {
        public:
            template<typename C>
            requires std::is_class_v<C>
            using MemberFunctionType = typename CMemberDelegate<C, RetT, ArgsT...>::FunctionType;
            using FunctorFunctionType = typename CFunctorDelegate<RetT, ArgsT...>::FunctionType;
            using DelegateType = IDelegate<RetT, ArgsT...>;
            using DelegatePtr = std::unique_ptr<DelegateType>;
            using DelegateContainer = std::vector<DelegatePtr>;
            using ConstIterator = typename DelegateContainer::const_iterator;

            CMultiEventWithReturn() = default;
            CMultiEventWithReturn(const CMultiEventWithReturn& other)
            {
                copy(other);
            }
            CMultiEventWithReturn(CMultiEventWithReturn&&) noexcept = default;
            CMultiEventWithReturn& operator=(const CMultiEventWithReturn& other)
            {
                copy(other);
                return *this;
            }
            CMultiEventWithReturn& operator=(CMultiEventWithReturn&&) noexcept = default;
            ~CMultiEventWithReturn() = default;

            /**
             * Adds a member function delegate, and returns if it was successful
             * @tparam C Object class
             * @param object Object context
             * @param function Function to add
             * @param unique Whether to only add if this event does not already have it
             * @return Successfully added
             */
            template<typename C>
            requires std::is_class_v<C>
            bool add(C* object, MemberFunctionType<C> function, const bool unique = true)
            {
                if (unique)
                {
                    if (auto it = getDelegateIterator(object, function); it != m_delegates.end())
                        return false;
                }
                
                m_delegates.emplace_back(std::make_unique<CMemberDelegate<C, RetT, ArgsT...>>(object, function));
                return true;
            }

            /**
             * Adds a C-style functor delegate, and returns if it was successful
             * @param function Function to add
             * @param unique Whether to only add if this event does not already have it
             * @return Successfully added
             */
            bool add(FunctorFunctionType function, const bool unique = true)
            {
                if (unique)
                {
                    if (auto it = getDelegateIterator(function); it != m_delegates.end())
                        return false;
                }

                m_delegates.emplace_back(std::make_unique<CFunctorDelegate<RetT, ArgsT...>>(function));
                return true;
            }

            template<typename C>
            requires std::is_class_v<C>
            bool remove(C* object, MemberFunctionType<C> function, const bool single = true)
            {
                if (single)
                {
                    if (auto it = getDelegateIterator(object, function); it != m_delegates.end())
                    {
                        m_delegates.erase(it);
                        return true;
                    }

                    return false;
                }

                for (auto it = m_delegates.rbegin(); it != m_delegates.rend();)
                {
                    if (*it->get() == CMemberDelegate<C, RetT, ArgsT...>(object, function))
                        it = decltype(it)(m_delegates.erase(std::next(it).base()));
                    else
                        ++it;
                }

                // might want to check if we did actually remove anything before just returning true
                return true;
            }

            bool remove(FunctorFunctionType function, const bool single = true)
            {
                if (single)
                {
                    if (auto it = getDelegateIterator(function); it != m_delegates.end())
                    {
                        m_delegates.erase(it);
                        return true;
                    }

                    return false;
                }

                for (auto it = m_delegates.rbegin(); it != m_delegates.rend();)
                {
                    if (*it->get() == CFunctorDelegate<RetT, ArgsT...>(function))
                        it = decltype(it)(m_delegates.erase(std::next(it).base()));
                    else
                        ++it;
                }

                // ditto above function
                return true;
            }

            void broadcast(ArgsT&&... args) const
            {
                for (const auto& del : m_delegates)
                {
                    (*del)(std::forward<ArgsT>(args)...);
                }
            }

            /**
             * Executes each delegate, and passes their return values into the given function
             * @tparam FuncT Function type
             * @param function Called with each executed delegate
             * @param args Arguments
             */
            template<std::invocable<RetT> FuncT>
            void broadcastReturn(FuncT function, ArgsT&&... args) const
            {
                for (const auto& del : m_delegates)
                {
                    function((*del)(std::forward<ArgsT>(args)...));
                }
            }
            
        private:
            void copy(const CMultiEventWithReturn& other)
            {
                m_delegates.clear();
                m_delegates.reserve(other.m_delegates.size());

                for (const auto& del : other.m_delegates)
                {
                    m_delegates.emplace_back(del->clone());
                }
            }
            
            template<typename C>
            ConstIterator getDelegateIterator(C* object, MemberFunctionType<C> function) const
            {
                CMemberDelegate<C, RetT, ArgsT...> compare {object, function};
                return std::ranges::find_if(m_delegates.begin(), m_delegates.end(),
                    [&compare](auto& uniqueDelegate)
                    {
                        return uniqueDelegate->equal(&compare);
                    });
            }

            ConstIterator getDelegateIterator(FunctorFunctionType function) const
            {
                CFunctorDelegate<RetT, ArgsT...> compare {function};
                return std::ranges::find_if(m_delegates.begin(), m_delegates.end(),
                    [&compare](auto& uniqueDelegate)
                    {
                        return uniqueDelegate->equal(&compare);
                    });
            }
            
            DelegateContainer m_delegates;
        };

        template<typename... ArgsT>
        using CMultiEvent = CMultiEventWithReturn<void, ArgsT...>;
    } // dd
} // ostanton

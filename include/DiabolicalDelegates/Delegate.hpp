#ifndef DIADEL_DELEGATE_HPP
#define DIADEL_DELEGATE_HPP

#include <memory>

namespace dd
{
    /**
     * A wrapper for any kind of function, implemented via its derivatives.
     * @tparam ArgsT Function arguments
     */
    template<typename... ArgsT>
    struct Delegate
    {
        Delegate() = default;
        Delegate(const Delegate&) = default;
        Delegate(Delegate&&) = default;
        Delegate& operator=(const Delegate&) = default;
        Delegate& operator=(Delegate&&) = default;
        virtual ~Delegate() = default;

        void operator()(ArgsT&&... args) {execute(std::forward<ArgsT>(args)...);}

        /**
         * Creates a clone of this delegate
         * @return Unique pointer to new delegate memory
         */
        [[nodiscard]] virtual std::unique_ptr<Delegate> clone() const = 0;

        /**
         * Resets the state of this delegate
         */
        virtual void reset() noexcept = 0;

    protected:
        virtual void execute(ArgsT&&...) = 0;
    };
}

#endif // DIADEL_DELEGATE_HPP

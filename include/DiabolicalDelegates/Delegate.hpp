#ifndef DIADEL_DELEGATE_HPP
#define DIADEL_DELEGATE_HPP

#include <memory>

namespace dd
{
    /**
     * A wrapper for any kind of function, implemented via its derivatives.
     * @tparam Args Function arguments
     */
    template<typename... Args>
    struct Delegate
    {
        Delegate() = default;
        Delegate(const Delegate&) = default;
        Delegate(Delegate&&) = default;
        Delegate& operator=(const Delegate&) = default;
        Delegate& operator=(Delegate&&) = default;
        virtual ~Delegate() = default;

        void operator()(Args... args) {execute(args...);}

        /**
         * Creates a clone of this delegate
         * @return Unique pointer to new delegate memory
         */
        virtual std::unique_ptr<Delegate> clone() const = 0;

        /**
         * Resets the state of this delegate
         */
        virtual void reset() = 0;

    protected:
        virtual void execute(Args...) = 0;
    };
}

#endif // DIADEL_DELEGATE_HPP

# Diabolical Delegates

A diabolical approach to delegates and events in C++. Uses C++20's modules, and C++23's modularised standard library.

## Dependencies

None. Except C++23.

## How to include

Copy & paste the `diabolical_delegates.cpp` file wherever, then `import` the module.

## How to use

There are currently 5 classes:
- Delegate
- MemberDelegate
- FunctorDelegate
- Event
- MultiEvent

`Delegate`s and `Event`s have return values and variadic template parameters.
These are what define the signatures of the functions that can be bound to them.

### Delegate

A delegate is simply a wrapper for any kind of function, implemented in its derived classes.

### MemberDelegate

A subclass of `Delegate`, it implements member functions with their object context.

### FunctorDelegate

A subclass of `Delegate`, it implements free-floating/global functions via a simple function pointer.

### Event

A wrapper for a delegate of any kind. It can be bound and unbound, supports whatever subclasses of `Delegate` there are.

### MultiEvent

Another version of `Event` which supports any number of `Delegate`s, not just one. These are `add`ed instead of `bind`ed.
Since `MultiEvent`s can have any number of `Delegate`s, their `broadcast` function is split into two. One executes all
the delegates without returning anything, and the other takes in a function to put the return value of each delegate execution
into.

<p align="center">
  <img src="billy-butcher-diabolical.gif">
</p>

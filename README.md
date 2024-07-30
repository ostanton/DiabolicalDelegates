# Diabolical Delegates

A diabolical approach to delegates and events in C++.

## Dependencies

None. Except C++17.

## How to build

It's an interface library. You don't!

## How to include

Either download/copy & paste the header code into your project's include directory, or fetch it with CMake or CPM.

## How to use

There are currently four classes:
- Delegate
- MemberDelegate
- Event
- MultiEvent

`Delegate`s and `Event`s have variadic template parameters, but no return values (may be supported in the future).
These are what define the signatures of the functions that can be bound to them.

### Delegate

A delegate is simply a wrapper for any kind of function, implemented in its derived classes.

### MemberDelegate

A subclass of `Delegate`, it implements member functions with their object context.

### Event

A wrapper for a delegate of any kind. It can be bound and unbound, currently only supports `MemberDelegate`s.

### MultiEvent

Another version of `Event` which supports any number of `Delegate`s, not just one. These are `add`ed instead of `bind`ed.
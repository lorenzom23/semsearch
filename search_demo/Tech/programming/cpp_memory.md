# Memory management in modern C++

In modern C++ you should almost never call new and delete directly. Resources are tied to the lifetime of objects through a technique called RAII: the constructor acquires the resource and the destructor releases it, so cleanup happens automatically even when an exception is thrown.

For dynamic memory the standard library provides smart pointers. A unique_ptr owns its object exclusively and cannot be copied, only moved; when it goes out of scope the object is destroyed. A shared_ptr keeps a reference count and destroys the object when the last owner disappears, at the cost of an extra allocation and atomic operations.

Containers such as vector and string manage their own memory too. Calling reserve on a vector before inserting many elements avoids repeated reallocations, and it is one of the cheapest optimizations available.

Common bugs include dangling references to elements of a vector that has grown, use after free and memory leaks caused by cycles of shared_ptr. Tools like AddressSanitizer and Valgrind detect most of these problems at run time.

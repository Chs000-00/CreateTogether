#pragma once

template <class T>
class Singleton {

public:
    static T* get() {
        if(!instance)
            instance = new T;
        return instance;
    }

protected:
     Singleton() = default;

     Singleton(Singleton const &) = delete;
     Singleton &operator=(const Singleton &) = delete;
     static inline T* instance = nullptr;
};

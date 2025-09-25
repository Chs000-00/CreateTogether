template <class T>
class Singleton {

public:
    static T* get() {
        static T inst;
        return inst;
    }

protected:
     Singleton() = default;

     Singleton(Singleton const &) = delete;
     Singleton &operator=(const Singleton &) = delete;
}

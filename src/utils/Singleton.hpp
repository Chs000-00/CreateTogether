template <class T>
class Singleton {

public:
    static T* get() {
        static NetManager inst;
        return p;
    }

protected:
     Singleton() = default;

     Singleton(Singleton const &) = delete;
     Singleton &operator=(const Singleton &) = delete;
}

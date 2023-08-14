
#ifndef  singleton_INC
#define  singleton_INC

#include <QMutexLocker>
#include <QMutex>
#include <QAtomicPointer>

typedef enum {
	SceneArea_first = 0,
	SceneArea_second,
	SceneArea_third,
	SceneArea_four
}SceneType;

template <typename T>
class Singleton
{
public:
    static T *instance()
    {
        if ( s_instance.testAndSetOrdered(0,0) ) {  //若单例还未创建则进入分支，testAndSetOrdered可实现获取单例指针原子操作。
            QMutexLocker locker(s_mutex);
            if (s_instance.testAndSetOrdered(0,0)) {//再次检测，当有一个以上线程同时进入分支时可以确保唯一实例。
                s_instance.store(new T);
            }
        }
        return s_instance.load();
    }

    static void destroy()
    {
        T *tins = s_instance.fetchAndStoreOrdered(0);// Atomic fetch-and-store
        if (tins) {
            delete tins;
        }
    }
    static bool isInstanceExist()
    {
        return !(s_instance.testAndSetOrdered(0,0));
    }

protected:
    Singleton(){}
    ~Singleton(){}

private:
    Singleton(const Singleton &);
    Singleton &operator =(const Singleton &);

private:
    static QMutex *s_mutex;
    static QAtomicPointer<T> s_instance;
};

template <typename T>
QMutex *Singleton<T>::s_mutex=new QMutex;

template <typename T>
QAtomicPointer<T> Singleton<T>::s_instance;

#endif   /* ----- #ifndef singleton_INC  ----- */

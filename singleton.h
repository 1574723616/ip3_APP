
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
        if ( s_instance.testAndSetOrdered(0,0) ) {  //��������δ����������֧��testAndSetOrdered��ʵ�ֻ�ȡ����ָ��ԭ�Ӳ�����
            QMutexLocker locker(s_mutex);
            if (s_instance.testAndSetOrdered(0,0)) {//�ٴμ�⣬����һ�������߳�ͬʱ�����֧ʱ����ȷ��Ψһʵ����
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

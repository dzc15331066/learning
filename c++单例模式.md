```
template<class T>
class Singleton
{
public:
    static T *getInstance(){
        if(m_instance == NULL){
            m_instance = new T;
        }
        return m_instance;
    }
    static void destroy(){
        delete m_instance;
        m_instance = NULL;
    }

protected:
    Singleton(){}
    ~Singleton(){}
   
private:
    Singleton(const Singleton&){}
    Singleton & operator=(const Singleton&){}

private:
    static T*m_instance;
    
};

template<class T>T* Singleton<T>::m_instance = NULL;

```

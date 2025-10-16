#ifndef LIRSCACHE_H
#define LIRSCACHE_H


namespace Cache
{

class Cache { 

public:
    Cache() = default;

    int add(int a, int b) {
        return  a + b + ((a + b) % 17 == 0);
    }
};

}


#endif // LIRSCACHE_H
#pragma once

template<int BITS> class EMA {
private:

    int sum =  0;
    // half-way (-1 at end means 0.5 rounds down)
    const int half = (1 << (BITS-1)) - 1;

public:

    EMA(int initial=0) : sum(initial << BITS) {  }

    inline void update(const int value) {
        sum = (sum + value) - (sum >> BITS);
    }

    inline EMA& operator=(const int value) {
        update(value);
        return *this;
    }
    
    void set(const int value) {
        sum = (value << BITS);
    }

    inline operator int() {
        return (sum + half) >> BITS;
    }

    float asFloat() {
        return (float)((sum + half) >> BITS);
    }
};
/*
https://stackoverflow.com/a/2787123
*/

#ifndef MIN_HEAP
#define MIN_HEAP

#include <queue>
//#include <functional>
#include <vector>

template<class _Ty = void>
struct greaterThanPtr
{	// functor for operator>

    constexpr bool operator()(const _Ty& _Left, const _Ty& _Right) const
    {	// apply operator> to operands
        return (*_Left > *_Right);
    }
};

using std::priority_queue;
using std::vector;
//using std::greater;
template <typename T>
using min_heap = priority_queue<T, vector<T>, greaterThanPtr<T>>;

#endif

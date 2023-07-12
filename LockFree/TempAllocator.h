#ifndef __TEMP_ALLOCATOR_HEADER__
#define __TEMP_ALLOCATOR_HEADER__


namespace _temp
{


template<typename Ty>
struct Allocator
{
	Ty* operator() ()
	{
		return new Ty();
	}
};

template<typename Ty>
struct Deallocator
{
	void operator() (Ty*& ptr)
	{
		if (nullptr != ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}
};


}


#endif // !__TEMP_ALLOCATOR_HEADER__
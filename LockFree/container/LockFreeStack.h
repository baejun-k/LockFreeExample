#ifndef __LOCK_FREE_STACK_HEADER__
#define __LOCK_FREE_STACK_HEADER__


#include <atomic>
#include <assert.h>


namespace container
{

template<
	typename DataType,
	template<typename> typename Alloc,
	template<typename> typename Dealloc
>
class LockFreeStack
{
private:
	struct Node
	{
		DataType data;
		std::atomic<LockFreeStack::Node*> next;

		Node()
			: data()
			, next(nullptr)
		{}
	};

	using Node_t        = LockFreeStack::Node;
	using Allocator_t   = Alloc<Node_t>;
	using Deallocator_t = Dealloc<Node_t>;

	std::atomic<Node_t*> m_top;

	Allocator_t   m_allocator;
	Deallocator_t m_deallocator;

public:
	LockFreeStack()
		: m_top(nullptr)
		, m_allocator()
		, m_deallocator()
	{
		assert(m_top.is_lock_free());
	}
	~LockFreeStack()
	{
		this->Clear();
	}

	bool Push(const DataType& in_data)
	{
		Node_t* pNewNode = m_allocator();
		if (nullptr == pNewNode)
		{
			return false;
		}
		pNewNode->data = in_data;
		this->_Push(pNewNode);
		return true;
	}

	bool Push(DataType&& in_data)
	{
		Node_t* pNewNode = m_allocator();
		if (nullptr == pNewNode)
		{
			return false;
		}
		pNewNode->data = std::move(in_data);
		this->_Push(pNewNode);
		return true;
	}

	bool Pop(DataType& out_data)
	{
		Node_t* pResult = this->_Pop();
		if (nullptr == pResult)
		{
			return false;
		}
		out_data = pResult->data;
		m_deallocator(pResult);
		return true;
	}

	bool Pop()
	{
		Node_t* pResult = this->_Pop();
		if (nullptr == pResult)
		{
			return false;
		}
		m_deallocator(pResult);
		return true;
	}

	void Clear()
	{
		Node_t* pNode = this->_Pop();
		while (nullptr != pNode)
		{
			m_deallocator(pNode);
			pNode = this->_Pop();
		}
	}

private:
	void _Push(Node_t* pNewNode)
	{
		while (true)
		{
			Node_t* pPrevTop = m_top.load();
			pNewNode->next.store(pPrevTop);
			if (m_top.compare_exchange_weak(pPrevTop, pNewNode))
			{
				break;
			}
		}
	}

	Node_t* _Pop()
	{
		Node_t* pPrevTop = m_top.load();
		while (true)
		{
			if (nullptr == pPrevTop)
			{
				break;
			}
			else if (m_top.compare_exchange_weak(pPrevTop, pPrevTop->next))
			{
				break;
			}
			pPrevTop = m_top.load();
		}

		return pPrevTop;
	}
};

}


#endif // !__LOCK_FREE_STACK_HEADER__
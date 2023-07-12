#ifndef __LOCK_FREE_QUEUE_HEADER__
#define __LOCK_FREE_QUEUE_HEADER__


#include <atomic>
#include <assert.h>


namespace container
{

template<
	typename DataType,
	template<typename> typename Alloc,
	template<typename> typename Dealloc
>
class LockFreeQueue
{
private:
	struct Node
	{
		DataType data;
		std::atomic<LockFreeQueue::Node*> next;

		Node()
			: data()
			, next(nullptr)
		{}
	};

	using Node_t        = LockFreeQueue::Node;
	using Allocator_t   = Alloc<Node_t>;
	using Deallocator_t = Dealloc<Node_t>;

	Node_t m_head;
	std::atomic<Node_t*> m_trailer;

	Allocator_t   m_allocator;
	Deallocator_t m_deallocator;

public:
	LockFreeQueue()
		: m_head()
		, m_trailer(&m_head)
		, m_allocator()
		, m_deallocator()
	{
		assert(m_trailer.is_lock_free());
	}
	~LockFreeQueue()
	{
		this->Clear();
	}

	bool Enqueue(const DataType& in_data)
	{
		Node_t* pNewNode = m_allocator();
		if (nullptr == pNewNode)
		{
			return false;
		}
		pNewNode->data = in_data;
		this->_Enqueue(pNewNode);
		return true;
	}

	bool Enqueue(DataType&& in_data)
	{
		Node_t* pNewNode = m_allocator();
		if (nullptr == pNewNode)
		{
			return false;
		}
		pNewNode->data = std::move(in_data);
		this->_Enqueue(pNewNode);
		return true;
	}

	bool Dequeue(DataType& out_data)
	{
		Node_t* pResult = this->_Dequeue();
		if (nullptr == pResult)
		{
			return false;
		}
		out_data = pResult->data;
		m_deallocator(pResult);
		return true;
	}

	bool Dequeue()
	{
		Node_t* pResult = this->_Dequeue();
		if (nullptr == pResult)
		{
			return false;
		}
		m_deallocator(pResult);
		return true;
	}

	void Clear()
	{
		Node_t* pNode = this->_Dequeue();
		while (nullptr != pNode)
		{
			m_deallocator(pNode);
			pNode = this->_Dequeue();
		}
	}

private:
	void _Enqueue(Node_t* pNewNode)
	{
		Node_t* pTail = m_trailer.load();
		Node_t* pTailNext = pTail->next;

		while (true)
		{
			if (nullptr == pTailNext)
			{
				if (pTail->next.compare_exchange_weak(pTailNext, pNewNode))
				{
					break;
				}
			}
			else
			{
				m_trailer.compare_exchange_weak(pTail, pTailNext);
				pTail = m_trailer.load();
				pTailNext = pTail->next;
			}
		}
		m_trailer.compare_exchange_weak(pTail, pNewNode);
	}

	Node_t* _Dequeue()
	{
		Node_t* pHead = &m_head;
		Node_t* pPrevHead = pHead->next.load();

		while (true)
		{
			if (nullptr == pPrevHead)
			{
				break;
			}
			else if (pHead->next.compare_exchange_weak(pPrevHead, pPrevHead->next))
			{
				break;
			}

			pPrevHead = pHead->next.load();
		}

		Node_t* pResult = pPrevHead;
		m_trailer.compare_exchange_weak(pPrevHead, pHead);
		return pResult;
	}
};

}


#endif // !__LOCK_FREE_QUEUE_HEADER__
#pragma once

#include "Entity.h"
#include "Sequence.h"
namespace sdb {
template<typename KeyType, typename ValueType>
class Array : public Sequence<KeyType>
{
public:
    Array(Entity * parent = NULL) : Sequence<KeyType>(parent) 
	{
		m_lastSearchResult = NULL;
		m_lastSearchNode = NULL;
	}
    
    void insert(const KeyType & x, ValueType * v) {
		Pair<KeyType, Entity> * p = Sequence<KeyType>::insert(x);
		if(!p->index) p->index = new Single<ValueType>;
		Single<ValueType> * d = static_cast<Single<ValueType> *>(p->index);
		d->setData(v);
	}
	
	ValueType * value() {
		Single<ValueType> * s = static_cast<Single<ValueType> *>(Sequence<KeyType>::currentIndex());
		return s->data();
	}
	
	const KeyType key() const {
		return Sequence<KeyType>::currentKey();
	}
	
	void remove(const KeyType & k) {
		Sequence<KeyType>::remove(k);
	}
	
	ValueType * find(const KeyType & k, MatchFunction::Condition mf = MatchFunction::mExact, KeyType * extraKey = NULL) 
	{
// reuse search result
		if(m_lastSearchResult && m_lastSearchKey == k)
			return m_lastSearchResult;
			
// probably it is the same node
		Pair<Entity *, Entity> g;
		if(m_lastSearchNode) g = m_lastSearchNode->findInNode(k);
		
// restart search
		if(!g.index) g = Sequence<KeyType>::findEntity(k, mf, extraKey);
		
// keep the node reached
		m_lastSearchNode = static_cast<BNode<KeyType> *>(g.key);
		
		if(!g.index) return NULL;
		
		Single<ValueType> * s = static_cast<Single<ValueType> *>(g.index);
		
// keep search result
		m_lastSearchKey = k;
		m_lastSearchResult = s->data();
		
		return m_lastSearchResult;
	}

private:
	ValueType * m_lastSearchResult;
	BNode<KeyType> * m_lastSearchNode;
	KeyType m_lastSearchKey;
};
} //end namespace sdb

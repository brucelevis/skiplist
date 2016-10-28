/* skiplist 
 * c++ implimentation
 * copy from https://gist.github.com/unsuthee/4116663
 * add an index method: get value by rank
 */
#ifndef __SKIPLIST_HPP__
#define __SKIPLIST_HPP__

#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

template<typename T>
struct skipNode {
	T t;
	int *length;
	skipNode<T> **next;
	skipNode(int level):length(NULL),next(NULL)
	{
		next = new skipNode<T> *[level];
		for (int i = 0; i < level; i++)
		{
			next[i] = NULL;
		}
		length = new int[level];
		for (int i = 0; i < level; i++)
		{
			length[i] = 0;
		}
	}
	~skipNode()
	{
		if (next != NULL)
			delete [] next;
		if (length != NULL)
			delete [] length;
	}
};

template<typename T>
class _Compare
{
public:
	bool operator()(const T &a, const T &b)
	{
		return a < b;
	}
};

template<typename T, int MAX_LEVEL = 16>
class skiplist
{
public:
	typedef T value_type;
	static const int max_level = MAX_LEVEL;
	bool insert(value_type t)
	{
		skipNode<value_type> *update[MAX_LEVEL], *curNode = Head;
		int length[MAX_LEVEL];
		memset(&update, 0, sizeof(skipNode<value_type> *)*MAX_LEVEL);
		memset(&length, 0, sizeof(int)*MAX_LEVEL);
		for (int level = cur_level-1; level >= 0; level--)
		{
			while (curNode->next[level] != NULL && curNode->next[level]->t < t)
			{
				length[level] += curNode->length[level];
				curNode = curNode->next[level];
			}
			update[level] = curNode;
		}
		int nlevel = randomLevel(0.5);
		skipNode<value_type> *node = new skipNode<value_type>(nlevel);
		if (node == NULL)
		{
			return false;
		}
		if (nlevel > cur_level)
		{
			for (int i = cur_level; i < nlevel; i++)
			{
				update[i] = Head;
				update[i]->length[i] = cur_length;
				length[i] = 0;
			}
			cur_level = nlevel;
		}
		int steps = 0;
		for (int i = 0; i < nlevel; ++i)
		{
			node->next[i] = update[i]->next[i];
			node->length[i] = update[i]->length[i]-steps;
			update[i]->next[i] = node;
			update[i]->length[i] = steps+1;
			steps += length[i];
		}
		for (int i = nlevel; i < cur_level; i++)
		{
			update[i]->length[i] += 1;
		}
		node->t = t;
		cur_length++;
		return true;
	}
	bool remove(T &t)
	{
		skipNode<T> *update[MAX_LEVEL];
		skipNode<T> *curNode = Head;
		for (int level = cur_level-1; level >= 0; level--)
		{
			while (curNode->next[level] != NULL && curNode->next[level]->t < t)
			{
				curNode = curNode->next[level];
			}
			update[level] = curNode;
		}
		if (curNode->next[0] == NULL || curNode->next[0]->t != t)
		{
			return false;
		}
		curNode = curNode->next[0]; // target, remove
		for (int level = cur_level-1; level >= 0; level--)
		{
			if (update[level]->next[level] == curNode)
			{
				update[level]->next[level] = curNode->next[level];
				update[level]->length[level] += curNode->length[level]-1;
			}
			else
				update[level]->length[level]--;
		}
		while (cur_level > 0 && Head->next[cur_level-1] == NULL)
		{
			Head->length[cur_level-1] = 0;
			cur_level--;
		}
		delete curNode;
		cur_length--;
		return true;
	}
	int get_rank(T &t)
	{
		int i = 0;
		skipNode<T> *curNode = Head;
		for (int level = cur_level-1; level >= 0; level--)
		{
			while (curNode->next[level] != NULL && curNode->next[level]->t < t)
			{
				i += curNode->length[level];
				curNode = curNode->next[level];
			}
		}
		if (curNode->t == t)
		{
			return i;
		}
		return -1;
	}
	int find_first(T &t);
	int find_last(T &t);
	int size() {return cur_length;}
	const T& operator[](int index) const;
	void dprint() const
	{
		std::cerr << "skiplist(" << cur_length << "): [";
		for (skipNode<value_type> *cur = Head->next[0]; cur != NULL;)
		{
			std::cerr << cur->t << ", ";
			cur = cur->next[0];
		}
		std::cerr << "]" << std::endl;
		for (int level = cur_level-1; level >= 0; level--)
		{
			skipNode<value_type> *curNode = Head->next[level];
			std::cerr << "*" << std::string(std::max(Head->length[level]-1, 0), '-');
			while (curNode != NULL)
			{
				std::cerr << "O";
				std::string padding = "";
				if (curNode->next[level] == NULL)
					padding = std::string(curNode->length[level],'-');
				else
					padding = std::string(std::max(curNode->length[level]-1,0), '-');
				std::cerr << padding;
				curNode = curNode->next[level];
			}
			std::cerr << "*" << std::endl;
		}
	}
	skiplist():cur_level(0),cur_length(0)
	{
		Head = new skipNode<value_type>(MAX_LEVEL);
	}
	~skiplist()
	{
		for (skipNode<value_type> *cur = Head; cur != NULL;)
		{
			skipNode<value_type> *tmp = cur;
			cur = cur->next[0];
			delete tmp;
		}
	}
protected:
	int randomLevel()
	{
		int nlevel = 1;
		while ((random()&0x1 == 1) && nlevel < MAX_LEVEL)
			++nlevel;
		return nlevel;
	}
	int randomLevel(float p)
	{
		int nlevel = 1;
		while (((random()&0xffff) < int(p*0xffff)) && nlevel < MAX_LEVEL)
			++nlevel;
		return nlevel;
	}
private:
	skipNode<value_type> *Head;
	int cur_level;
	int cur_length;

};

#endif // __SKIPLIST_HPP__

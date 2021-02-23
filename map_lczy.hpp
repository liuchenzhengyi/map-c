/**
* implement a container like std::map
*/
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

	template<
		class Key,
		class T,
		class Compare = std::less<Key>
	> class map {
	public:
		typedef pair<const Key, T> value_type;
	private:
		Compare Cmp;
		struct AvlNode
		{
			value_type *data;
			AvlNode *prev, *next;
			AvlNode *left, *right;
			int height;
			AvlNode(const value_type &d, AvlNode *l = NULL, AvlNode *r = NULL, AvlNode *p = NULL, AvlNode *n = NULL, int h = 1)
				:left(l), right(r), prev(p), next(n), height(h) {
				data = new value_type(d);
			}
			AvlNode() :data(NULL), left(NULL), right(NULL), prev(NULL), next(NULL), height(1) {}
			~AvlNode() { if (data) delete data; }
		};
		AvlNode *root;
		AvlNode *head, *tail;
		int current_size;
	public:
		class const_iterator;
		class iterator {
			friend class const_iterator;
			friend class map;
		private:
			map * master;
			AvlNode * p;
		public:
			iterator(map *m = NULL, AvlNode *pp = NULL) : master(m), p(pp) {}
			iterator(const iterator &other) :master(other.master), p(other.p) {}
			iterator(const const_iterator &other) :master(other.master), p(other.p) {}
			iterator operator++(int) {
				if (p == master->tail)
					throw index_out_of_bound();
				iterator tmp = *this;
				p = p->next;
				return tmp;
			}
			iterator & operator++() {
				if (p == master->tail)
					throw index_out_of_bound();
				p = p->next;
				return *this;
			}
			iterator operator--(int) {
				if (p == master->head->next)
					throw index_out_of_bound();
				iterator tmp = *this;
				p = p->prev;
				return tmp;
			}
			iterator & operator--() {
				if (p == master->head->next)
					throw index_out_of_bound();
				p = p->prev;
				return *this;
			}
			value_type & operator*() const { return *(p->data); }
			bool operator==(const iterator &rhs) const { return (master == rhs.master && p == rhs.p); }
			bool operator==(const const_iterator &rhs) const { return (master == rhs.master && p == rhs.p); }
			bool operator!=(const iterator &rhs) const { return (master != rhs.master || p != rhs.p); }
			bool operator!=(const const_iterator &rhs) const { return (master != rhs.master || p != rhs.p); }
			value_type* operator->() const noexcept { return p->data; }
		};
		class const_iterator {
			friend class iterator;
			friend class map;
		private:
			const map * master;
			const AvlNode * p;
		public:
			const_iterator(const map *m = NULL, const AvlNode *pp = NULL) :master(m), p(pp) {}
			const_iterator(const const_iterator &other) :master(other.master), p(other.p) {}
			const_iterator(const iterator &other) :master(other.master), p(other.p) {}
			value_type & operator*() const { return *(p->data); }
			const_iterator operator++(int) {
				if (p == master->tail)
					throw index_out_of_bound();
				const_iterator tmp = *this;
				p = p->next;
				return tmp;
			}
			const_iterator & operator++() {
				if (p == master->tail)
					throw index_out_of_bound();
				p = p->next;
				return *this;
			}
			const_iterator operator--(int) {
				if (p == master->head->next)
					throw index_out_of_bound();
				const_iterator tmp = *this;
				p = p->prev;
				return tmp;
			}
			const_iterator & operator--() {
				if (p == master->head->next)
					throw index_out_of_bound();
				p = p->prev;
				return *this;
			}
			bool operator==(const iterator &rhs) const { return (master == rhs.master && p == rhs.p); }
			bool operator==(const const_iterator &rhs) const { return (master == rhs.master && p == rhs.p); }
			bool operator!=(const iterator &rhs) const { return (master != rhs.master || p != rhs.p); }
			bool operator!=(const const_iterator &rhs) const { return (master != rhs.master || p != rhs.p); }
			value_type* operator->() const noexcept { return p->data; }
		};
		map() :root(NULL), current_size(0) { head = new AvlNode(); tail = new AvlNode(); head->next = tail; tail->prev = head; }
		map(const map &other) :current_size(other.size()) {
			head = new AvlNode();
			tail = new AvlNode();
			head->next = tail;
			tail->prev = head;
			root = makeTree(other.root);
			makeLink(root, head);
		}
		map & operator=(const map &other) {
			if (&other == this)
				return *this;
			clear();
			current_size = other.size();
			root = makeTree(other.root);
			makeLink(root, head);
		}
		~map() { clear(); delete head; delete tail; }
		T & at(const Key &key) {
			AvlNode *t = root;
			while (t != NULL && (Cmp(t->data->first, key) || Cmp(key, t->data->first)))
				t = (Cmp(key, t->data->first) ? t->left : t->right);

			if (t == NULL)
				throw index_out_of_bound();
			else
				return t->data->second;
		}
		const T & at(const Key &key) const {
			AvlNode *t = root;
			while (t != NULL && (Cmp(t->data->first, key) || Cmp(key, t->data->first)))
				t = (Cmp(key, t->data->first) ? t->left : t->right);

			if (t == NULL)
				throw index_out_of_bound();
			else
				return t->data->second;
		}
		T & operator[](const Key &key) {
			AvlNode *t = root;
			while (t != NULL && (Cmp(t->data->first, key) || Cmp(key, t->data->first)))
				t = (Cmp(key, t->data->first) ? t->left : t->right);

			if (t == NULL)
			{
				iterator p = insert(value_type(key, T())).first;
				return p->second;
			}
			else
				return t->data->second;
		}
		const T & operator[](const Key &key) const { return at(key); }
		iterator begin() { return iterator(this, head->next); }
		const_iterator cbegin() const { return const_iterator(this, head->next); }
		iterator end() { return iterator(this, tail); }
		const_iterator cend() const { return const_iterator(this, tail); }
		bool empty() const { return current_size == 0; }
		size_t size() const { return current_size; }
		void clear() {
			current_size = 0;
			AvlNode *p = head->next;
			AvlNode *q;
			while (p != tail)
			{
				q = p->next;
				delete p;
				p = q;
			}

			root = NULL;
			head->next = tail;
			tail->prev = head;
		}
		pair<iterator, bool> insert(const value_type &value) {
			bool flag = insert(value, root);
			AvlNode *tmp = root;
			while ((Cmp(tmp->data->first, value.first) || Cmp(value.first, tmp->data->first)))
				tmp = (Cmp(value.first, tmp->data->first) ? tmp->left : tmp->right);

			return pair<iterator, bool>(iterator(this, tmp), flag);
		}
		void erase(iterator pos) {
			if (pos.p == tail || root == NULL || pos.master != this)
				throw invalid_iterator();
			erase(pos->first, root);
		}
		size_t count(const Key &key) const {
			AvlNode *t = root;
			while (t != NULL && (Cmp(t->data->first, key) || Cmp(key, t->data->first)))
				t = (Cmp(key, t->data->first) ? t->left : t->right);

			if (t == NULL)
				return 0;
			else
				return 1;
		}
		iterator find(const Key &key) {
			AvlNode *t = root;
			while (t != NULL && (Cmp(t->data->first, key) || Cmp(key, t->data->first)))
				t = (Cmp(key, t->data->first) ? t->left : t->right);

			if (t == NULL)
				return end();
			else
				return iterator(this, t);
		}
		const_iterator find(const Key &key) const {
			AvlNode *t = root;
			while (t != NULL && (Cmp(t->data->first, key) || Cmp(key, t->data->first)))
				t = (Cmp(key, t->data->first) ? t->left : t->right);

			if (t == NULL)
				return cend();
			else
				return const_iterator(this, t);
		}
	private:
		bool insert(const value_type &value, AvlNode *&t)
		{
			bool flag = true;
			if (t == NULL)
			{
				current_size++;
				AvlNode *Pre = NULL, *tmp = root;
				while (tmp) {
					if (Cmp(tmp->data->first, value.first) && (!Pre || Cmp(Pre->data->first, tmp->data->first)))
						Pre = tmp;
					tmp = (Cmp(value.first, tmp->data->first) ? tmp->left : tmp->right);
				}
				if (Pre)
					t = new AvlNode(value, NULL, NULL, Pre, Pre->next);
				else
					t = new AvlNode(value, NULL, NULL, head, head->next);
				t->prev->next = t;
				t->next->prev = t;
			}
			else if (Cmp(value.first, t->data->first)) {
				if (!insert(value, t->left))
					flag = false;
				if (height(t->left) - height(t->right) == 2) {
					if (Cmp(value.first, t->left->data->first)) LL(t);
					else LR(t);
				}
			}
			else if (Cmp(t->data->first, value.first)) {
				if (!insert(value, t->right))
					flag = false;
				if (height(t->right) - height(t->left) == 2) {
					if (Cmp(t->right->data->first, value.first)) RR(t);
					else RL(t);
				}
			}
			else
				flag = false;

			t->height = Max(height(t->left), height(t->right)) + 1;
			return flag;
		}
		bool erase(const Key &del, AvlNode *&t)
		{
			if (t == NULL) return true;
			if (!(Cmp(del, t->data->first) || Cmp(t->data->first, del)))
			{
				if (t->left == NULL || t->right == NULL)
				{
					current_size--;
					AvlNode *oldNode = t;
					oldNode->prev->next = oldNode->next;
					oldNode->next->prev = oldNode->prev;
					t = (t->left != NULL ? t->left : t->right);
					delete oldNode;
					return false;
				}
				else
				{
					AvlNode *tmp = t->right, *p = t->right;
					while (tmp->left != NULL) {
						p = tmp;
						tmp = tmp->left;
					}
					AvlNode *f = root, *tt = t;
					while (tt != root && f->left != tt && f->right != tt)
						f = (Cmp(del, f->data->first) ? f->left : f->right);
					if (tmp == tt->right)
					{
						AvlNode *lc = tt->left;
						int h = tt->height;
						tt->left = tmp->left;
						tt->right = tmp->right;
						tt->height = tmp->height;
						tmp->left = lc;
						tmp->right = tt;
						tmp->height = h;
						if (tt != root) {
							if (f->left == tt) f->left = tmp;
							else f->right = tmp;
						}
						else
							root = tmp;
					}
					else
					{
						AvlNode *lc = tt->left, *rc = tt->right;
						int h = tt->height;
						tt->left = tmp->left;
						tt->right = tmp->right;
						tt->height = tmp->height;
						tmp->left = lc;
						tmp->right = rc;
						p->left = tt;
						if (tt != root) {
							if (f->left == tt) f->left = tmp;
							else f->right = tmp;
						}
						else
							root = tmp;
						tmp->height = h;
					}
					delete tt->data;
					tt->data = new value_type(*(tmp->data));
					if (erase(tt->data->first, t->right)) return true;
					return adjust(t, 1);
				}
			}
			if (Cmp(del, t->data->first)) {
				if (erase(del, t->left)) return true;
				return adjust(t, 0);
			}
			else {
				if (erase(del, t->right)) return true;
				return adjust(t, 1);
			}
		}
		int height(AvlNode *&t) const { return t == NULL ? 0 : t->height; }
		bool adjust(AvlNode *&t, int subTree)
		{
			if (subTree) {
				if (height(t->left) - height(t->right) == 1) return true;
				if (height(t->right) == height(t->left)) { --t->height; return false; }
				if (height(t->left->right) > height(t->left->left)) { LR(t); return false; }
				LL(t);
				if (height(t->right) == height(t->left)) return false;
				else return true;
			}
			else {
				if (height(t->right) - height(t->left) == 1) return true;
				if (height(t->right) == height(t->left)) { --t->height; return false; }
				if (height(t->right->left) > height(t->right->right)) { RL(t); return false; }
				RR(t);
				if (height(t->right) == height(t->left)) return false;
				return true;
			}
		}
		void LL(AvlNode *&t)
		{
			AvlNode *t1 = t->left;
			t->left = t1->right;
			t1->right = t;
			t->height = Max(height(t->left), height(t->right)) + 1;
			t1->height = Max(height(t1->left), height(t1->right)) + 1;
			t = t1;
		}
		void RR(AvlNode *&t)
		{
			AvlNode *t1 = t->right;
			t->right = t1->left;
			t1->left = t;
			t->height = Max(height(t->left), height(t->right)) + 1;
			t1->height = Max(height(t1->right), height(t1->left)) + 1;
			t = t1;
		}
		void LR(AvlNode *&t) { RR(t->left); LL(t); }
		void RL(AvlNode *&t) { LL(t->right); RR(t); }
		int Max(int a, int b) { return a > b ? a : b; }
		AvlNode * makeTree(AvlNode *t) {
			if (t == NULL) return NULL;
			return new AvlNode(*(t->data), makeTree(t->left), makeTree(t->right), NULL, NULL, t->height);
		}
		AvlNode * makeLink(AvlNode *t, AvlNode *p) {
			if (!t) return NULL;
			AvlNode *pre = makeLink(t->left, p);
			if (pre) {
				pre->next->prev = t;
				t->next = pre->next;
				pre->next = t;
				t->prev = pre;
			}
			else {
				p->next->prev = t;
				t->next = p->next;
				p->next = t;
				t->prev = p;
			}
			if (t->right == NULL)
				return t;
			return makeLink(t->right, t);
		}


	};
}

#endif

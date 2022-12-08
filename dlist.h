//dlist.h
#pragma once

#include <iostream>
#include <exception>
#include <initializer_list>

#include "container.h"

//定义链表类型
template <typename value_t>
class dlist : public container<value_t> {
//----类型------------------------------------------------------------------------
public:
    using value_type = typename container<value_t>::value_type;
    using pointer = typename container<value_t>::pointer;
    using reference = typename container<value_t>::reference;
    using difference_type = typename container<value_t>::difference_type;

private:
    //定义回调函数类型
    // using callback = void (reference);

    //定义节点类型
    //为了能“在位”创建data对象，显式提供了两个构造函数
    struct node {
        value_type data; //数据域
        node *next = nullptr, *prior = nullptr;  //前后向指针域

        //指定默认构造函数
        node() = default;
        //这是一个构造函数模板，带有不定长模板参数
        //将不定长构造参数直接传递给data的构造函数
        template <typename ...types>
        node(node *n, node *p, types &&...args) : data(args...), next(n), prior(p) {}
    };

    using nodeptr_t = node*;    //定义节点指针类型
    using range = nodeptr_t;
//----类型------------------------------------------------------------------------


//----属性------------------------------------------------------------------------
    node head, tail;    //头尾节点
    size_t length;      //节点数
//----属性------------------------------------------------------------------------


//----内部方法---------------------------------------------------------------------
    //获取指针p指向节点的数据
    reference _get(const nodeptr_t p) try {
        if (empty()) //链表空时抛出异常
            throw std::out_of_range("empty list");

        return p->data;
    } catch (std::out_of_range &e) { //捕获异常
        std::cout << e.what() << std::endl;
        exit(1);
    }

    //将节点p插入到节点pre之后
    void _push(const nodeptr_t pre, const nodeptr_t p) noexcept {
        pre->next->prior = p;
        pre->next = p;

        ++length;
    }

    //在节点pre之后删除一个节点
    void _pop(const nodeptr_t pre) try {
        if (empty())
            throw std::out_of_range("empty list");

        auto p = pre->next;
        pre->next = p->next;
        p->next->prior = pre;
        delete p;

        --length;
    } catch (std::out_of_range &e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }

    //初始化链表l
    constexpr void _init() noexcept {
        length = 0;
        head.next = &tail;
        tail.prior = &head;
    }

    //销毁链表l
    constexpr void _destroy() noexcept {
        for (auto p = head.next; p != &tail; p = head.next) {
            head.next = p->next;
            delete p;
        }
    }
//----内部方法---------------------------------------------------------------------

public:
//----公共接口---------------------------------------------------------------------
    //默认的、平凡的构造函数
    dlist() noexcept {
        _init();
    }

    //重载的、平凡的构造函数。使用了构造函数委托进行初始化
    dlist(const std::initializer_list<value_type> &l) noexcept : dlist() {
        for (auto &v : l)
            push_back(v);
    }

    //复制控制1：复制构造函数
    //采用深复制策略
    //参数必须是本类型的常量左值引用
    dlist(const dlist &l) noexcept : dlist() {
        for (auto p = l.head.next; p != &l.tail; p = p->next)
            push_back(p->data);
    }

    //复制控制2：转移复制构造函数
    //this链表准备接管l链表的资源
    //参数必须是本类型的非常量右值引用
    dlist(dlist &&l) noexcept : head(l.head), tail(l.tail), length(l.length) {
        l.head.next->prior = &head; //l链表的第一个节点的前向指针现在指向this链表的head
        l.tail.prior->next = &tail; //l链表的最后一个节点的后向指针现在指向this链表的tail
        l._init();  //l链表置为空链表。现在l的资源已经全部转移给了this，l可以安全的销毁了。
    }

    //复制控制3：重载的赋值运算符函数
    constexpr dlist &operator=(const dlist &l) {
        _destroy(); //释放this对象原有的资源
        _init();    //初始化this对象

        for (auto p = l.head.next; p != &l.tail; p = p->next)
            push_back(p->data);

        return *this;  //函数返回后，l立即失效，但它的资源已经转移给了this
    }

    //复制控制4：重载的转移赋值运算符函数
    //思路：交换this和l链表的资源，使this保持l的资源，而用l去释放this原来的资源。
    constexpr dlist &operator=(dlist &&l) {
        //交换两个链表的头节点的后向指针，使它们各自指向正确的后继（第一个）节点
        std::swap(head.next, l.head.next);
        //交换两个链表的第一个节点的前向指针，使它们各自指向正确的头节点
        std::swap(head.next->prior, l.head.next->prior);

        //交换两个链表的尾节点的前向指针，使它们各自指向正确的前驱（最后一个）节点
        std::swap(tail.prior, l.tail.prior);
        //交换两个链表的最后一个节点的后向指针，使它们各自指向正确的尾节点
        std::swap(tail.prior->next, l.tail.prior->next);

        std::swap(length, l.length);

        return *this; //函数返回后，l立即失效，它会释放this链表原来的资源
    }

    //复制控制5：禁止复制。放开下行注释，须注释掉复制控制1~4的代码
    // dlist(const dlist&) = delete;

    //destructor
    ~dlist() noexcept {
        _destroy();
    }

    //在链表l头部插入数据v。参数v是常量左值引用可以避免实参形参结合时的不必要复制。
    void push_front(const value_type &v) {
        _push(&head, new node(head.next, &head, v));
    }

    //在链表l尾部插入数据v
    void push_back(const value_type &v) {
        _push(tail.prior, new node(&tail, tail.prior, v));
    }

    //在位尾插方法
    //不定长参数包最好是value_type类型对象的构造参数列表
    //它们被直接传递给node的构造函数，再被传递给数据域data的构造函数，从而避免了不必要的临时对象创建和复制
    //
    template <typename ...types>
    void emplace_back(types &&...args) {
        _push(tail.prior, new node(&tail, tail.prior, args...));
    }

    //在链表头部删除节点
    void pop_front() {
        _pop(&head);
    }

    //在链表尾部删除节点
    void pop_back() {
        _pop(tail.prior->prior);
    }

    void pop(nodeptr_t t) {
        _pop(t->prior);
    }

    //在链表l头部获取数据
    value_type front() {
        return _get(head.next);
    }

    //在链表l尾部获取数据
    value_type back() {
        return _get(tail.prior);
    }

    //判断链表l是否为空。常成员。
    constexpr bool empty() const override {
        return length == 0;
    }

    //获取链表的长度（节点数）。常成员。
    constexpr size_t size() const {
        return length;
    }

    //从头到尾遍历链表。参数f是一个回调函数
    template <typename callback, typename ...types>
    void traverse(callback &&f, types ...args) {
        for (auto p = head.next; p != &tail; p = p->next)
            f(p->data, args...); //调用回调函数
    }

/*
    //在链表l指定位置插入数据v。
    //原来在pos位置的节点现在是新插入节点的下一个。
    //pos从0开始，最大有效值可以到length。
    // void insert(size_t pos, const value_type &v) try {
    //     if (pos > length)
    //         throw std::out_of_range("insert position out of range");

    //     auto p = &head;
    //     //工作指针p从头节点向后移动，一直到插入位置
    //     for (size_t i = 0; i < pos; ++i, p = p->next);
    //     //在p节点后面插入新节点。新节点的后继是原来的p->next，前驱是p
    //     _push(p, new node(p->next, p, v));

    // } catch (std::out_of_range &e) {
    //     std::cout << e.what() << std::endl;
    //     exit(1);
    // }

    //在位插入方法
    //不定长参数包最好是value_type类型对象的构造参数列表
    //它们被直接传递给node的构造函数，再被传递给数据域data的构造函数，从而避免了不必要的临时对象创建和复制
    // template <typename ...types>
    // void emplace(size_t pos, types &&...args) try {
    //     if (pos > length)
    //         throw std::out_of_range("insert position out of range");

    //     auto p = &head;
    //     //工作指针p从头节点向后移动，一直到插入位置
    //     for (size_t i = 0; i < pos; ++i, p = p->next);
    //     //在p节点后面插入新节点。新节点的后继是原来的p->next，前驱是p
    //     _push(p, new node(p->next, p, args...));

    // } catch (std::out_of_range &e) {
    //     std::cout << e.what() << std::endl;
    //     exit(1);
    // }
*/

    //定义迭代器类
    friend class iterator; //友元声明
    class iterator {
    public:
        using container_t = dlist;
        using value_type = typename container_t::value_type;
        using reference = typename container_t::reference;
        using difference_type = typename container_t::difference_type;

    private:
        using range = typename container_t::range;

        //这个迭代器实际上一个原生指针的包装类
        //所以，迭代器内部有一个指向容器存储的指针
        range p;
        container_t *cp;

    public:
        iterator(range r = nullptr, container_t *pc = nullptr) noexcept : p(r), cp(pc) {}
        ~iterator() noexcept {}

        //重载的!=运算符
        bool operator!=(const iterator &itr) const {
            return p != itr.p;
        }

        //重载的<运算符
        bool operator<(const iterator &itr) const {
            if (p == itr.p) return false;

            range t = p;
            while (t != nullptr && t != itr.p) t = t->next;
            return t != nullptr;
        }

        //重载的>=运算符
        bool operator>=(const iterator& itr) const {
            return !operator<(itr);
        }

        //重载的++运算符
        constexpr iterator &operator++() {
            p = p->next; //内部指针后移
            return *this;
        }

        //重载的--运算符
        constexpr iterator &operator--() {
            p = p->prior; //内部指针前移
            return *this;
        }

        //重载的*运算符
        reference operator*() {
            return p->data;
        }

        //重载的->运算符
        auto operator->() {
            return p;
        }

        operator range() {
            return p;
        }

        //重载的+运算符
        iterator operator+(difference_type span) const {
            if (span < 0)
                return operator-(-span);

            iterator itr(p);
            for (difference_type i = 0; i < span && itr.p != &cp->tail; ++i)
                itr.p = itr.p->next;
            return itr;
        }

        //重载的-运算符
        iterator operator-(difference_type span) const {
            if (span < 0)
                return operator+(-span);

            iterator itr(p);
            for (difference_type i = 0; i < span && itr.p != &cp->head; ++i)
                itr.p = itr.p->prior;
            return itr;
        }
    };

/*
    //逆向迭代器
    friend class reverse_iterator;
    class reverse_iterator {
    public:
        using container_t = dlist;
        using value_type = typename container_t::value_type;
        using reference = typename container_t::reference;
        using difference_type = typename container_t::difference_type;

    private:
        using range = typename container_t::range;

        //这个迭代器实际上一个原生指针的包装类
        //所以，迭代器内部有一个指向容器存储的指针
        range p;
        container_t *cp;

    public:
        reverse_iterator(range r = nullptr, container_t *pc = nullptr) noexcept : p(r), cp(pc) {}
        ~reverse_iterator() noexcept {}

        //重载的!=运算符
        bool operator!=(const reverse_iterator &itr) const {
            return p != itr.p;
        }

        //重载的++运算符
        constexpr reverse_iterator &operator++() {
            p = p->prior; //内部指针前移
            return *this;
        }

        //重载的*运算符
        reference operator*() {
            return p->data;
        }

        //重载的->运算符
        auto operator->() {
            return p;
        }

        operator range() {
            return p;
        }

        reverse_iterator operator+(difference_type span) const {
            reverse_iterator itr(p);
            for (difference_type i = 0; i < span && itr.p != &cp->head; ++i)
                itr.p = itr.p->prior;
            return itr;
        }
    };
*/

    constexpr iterator begin() {
        return iterator(head.next, this);
    }

    constexpr iterator end() {
        return iterator(&tail, this);
    }

/*
    constexpr reverse_iterator rbegin() {
        return reverse_iterator(tail.prior, this);
    }

    constexpr reverse_iterator rend() {
        return reverse_iterator(&head, this);
    }
*/

    template <typename ...types>
    void emplace(iterator pos, types &&...args) try {
        if (pos != iterator()) { // 原来：pos != end()
            auto p = range(pos)->prior;
            //在p节点后面插入新节点。新节点的后继是原来的p->next，前驱是p
            _push(p, new node(p->next, p, args...));
        } else
            throw std::out_of_range("insert position out of range");

    } catch (std::out_of_range &e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }

    void insert(iterator pos, const value_type &v) {
        emplace(pos, v);
    }
};

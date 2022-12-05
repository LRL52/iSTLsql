//container.h
#pragma once

template <typename value_t>
struct container {
    using value_type = value_t;    //内部的类型名value_type是全局value_type的别名
    using pointer = value_type*;        //指针类型
    using reference = value_type&;      //左值引用类型
    using difference_type = ptrdiff_t;  //指针差值类型

    virtual bool empty() const = 0;
};

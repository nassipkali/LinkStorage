#ifndef LINKINDEX_HPP
#define LINKINDEX_HPP


template <typename T>
class LinkIndex
{
    public:
        T RootAsSource; // Корень дерева связей использующих эту связь как источник
        T LeftAsSource; // Левый узел дерева связи которую эта связь использует как источник
        T RightAsSource; // Правый узел дерева связи которую эта связь использует как источник
        T SizeAsSource; // Размер, колечество узлов у этой связи
        T RootAsTarget;  // Корень дерева связей использующих эту связь как цель
        T LeftAsTarget; // Левый узел дерева связи которую эта связь использует как цель
        T RightAsTarget; // Правый узел дерева связи которую эта связь использует как цель
        T SizeAsTarget; // Размер, колечество узлов у этой связи
};
#endif

#ifndef LINK_HPP
#define LINK_HPP
#include <stddef.h>

typedef size_t link_t;


class Link
{
    public:
        link_t Source; // Индекс источника связи
        link_t Target; // Индекс цели связи
        link_t RootAsSource; // Корень дерева связей использующих эту связь как источник
        link_t LeftAsSource; // Левый узел дерева связи которую эта связь использует как источник
        link_t RightAsSource; // Правый узел дерева связи которую эта связь использует как источник
        link_t SizeAsSource; // Размер, колечество узлов у этой связи
        link_t RootAsTarget;  // Корень дерева связей использующих эту связь как цель
        link_t LeftAsTarget; // Левый узел дерева связи которую эта связь использует как цель
        link_t RightAsTarget; // Правый узел дерева связи которую эта связь использует как цель
        link_t SizeAsTarget; // Размер, колечество узлов у этой связи
};
#endif

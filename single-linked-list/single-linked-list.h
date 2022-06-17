#pragma once
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <algorithm>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;

        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }

        Type value;
        Node* next_node = nullptr;
    };

    // Шаблон класса «Базовый Итератор».
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) : node_(node) { }

    public:

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            if (node_ == nullptr) assert(false);
            node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
            if (node_ == nullptr) assert(false);
            auto old_node(*this);
            ++(*this);
            return old_node;

        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
            if (node_ == nullptr) assert(false);
            return this->node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            if (node_ == nullptr) assert(false);
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };


public:

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        Iterator it{ head_.next_node };
        return it;
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        Iterator it{ nullptr };
        return it;
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        Iterator it{ head_.next_node };
        return it;
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        Iterator it{ nullptr };
        return it;
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        Iterator it{ head_.next_node };
        return it;
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        Iterator it{ nullptr };
        return it;
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        Iterator it{ &head_ };
        return it;
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        Iterator it{ const_cast<Node*> (&head_) };
        return it;
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        Iterator it{ const_cast<Node*> (&head_) };
        return it;
    }


    ~SingleLinkedList() {
        Clear();
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return !size_;
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        try {
            head_.next_node = new Node(value, head_.next_node);
            ++size_;
        }
        catch (...) {
            throw;
        }

    }

    /*
  * Вставляет элемент value после элемента, на который указывает pos.
  * Возвращает итератор на вставленный элемент
  * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
  */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        try {
            auto new_pos = new Node(value, pos.node_->next_node);
            pos.node_->next_node = new_pos;
            ++size_;
        }
        catch (...) {
            throw;
        }
        return Iterator{ pos.node_->next_node };
    }

    void PopFront() noexcept {
        if (size_) {
            auto pointer = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = pointer;
            --size_;
        }
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        auto pointer = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = pointer;
        --size_;
        return Iterator{ pointer };
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* next_ptr = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = next_ptr;
        }
        size_ = 0;
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        for (auto i = rbegin(values); i != rend(values); ++i) {
            this->PushFront(*i);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList tmp;
        for (auto i = other.begin(); i != other.end(); ++i) {
            tmp.PushFront(*i);
        }
        for (auto i = tmp.begin(); i != tmp.end(); ++i) {
            this->PushFront(*i);
        }
    }

    SingleLinkedList() = default;

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        std::swap(this->head_.next_node, other.head_.next_node);
        std::swap(this->size_, other.size_);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        auto tmp(rhs);
        this->swap(tmp);
        return *this;
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_ = Node();
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs < rhs || lhs == rhs;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs == rhs || lhs > rhs;
}


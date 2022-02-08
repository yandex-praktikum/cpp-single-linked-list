#pragma once

// добавьте неоходимые include-директивы сюда
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iterator>

template <typename Type>
class SingleLinkedList {
	// напишите код класса тут
	//=======================================================================
    // Узел списка
    struct Node
    {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }

        Type value{};
        Node* next_node = nullptr;
    };

    //=======================================================================
    // Шаблон класса Базовый Итератор
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType - совпадает с Type (для Iterator) либо с const Type (для ConstIterator)    
    template <typename ValueType>
    class BasicIterator
    {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Явный Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node)
            : node_(node) // копируем в него аргумент конструктора 
        {
        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора
        // Категория итератора - forward iterator
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
        BasicIterator(const BasicIterator<Type>& other) noexcept
        {
            node_ = other.node_;
        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас.
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return  node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            //return node_ != rhs.node_;
            return !operator==(rhs);
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return  node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            //return node_ != rhs.node_;
            return !operator==(rhs);
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка.
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
            auto old_value(*this); // Сохраняем прежнее значение объекта для последующего возврата
            ++(*this); // используем логику префиксной формы инкремента

            return old_value;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент 
        // т.е предоставляет текущее ЗНАЧЕНИЕ, которое прячется за итератором. А значение хранится в члене value у объекта Node.
       // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
       // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка.
        // чтобы получить указатель на что-то, нужно применить оператор взятия адреса: &
       // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
       // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

 //=======================================================================
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
        if (size_ != 0)
        {
            return Iterator (head_.next_node);
        }
        else
        {
            return end();
        }
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept 
    {
        return Iterator(nullptr);
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        if (size_ != 0)
        {
            //return  ConstIterator(head_.next_node);
            return  ConstIterator(const_cast<Node*>(head_.next_node));
        }
        else
        {
            return end();
        }
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        return  ConstIterator(nullptr);
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        if (size_ != 0)
        {
           return  ConstIterator(const_cast<Node*>(head_.next_node));
        }
        else
        {
            return cend();
        };
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        return  ConstIterator(nullptr);
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        // Реализуйте самостоятельно
        return Iterator(&head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        // Реализуйте самостоятельно
        return ConstIterator(const_cast<Node*>(&head_));
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        // Реализуйте самостоятельно
        return ConstIterator(const_cast<Node*>(&head_));
    }

    

    //=========================================================================
    // Конструкторы и деструкторы
    SingleLinkedList() = default;
    SingleLinkedList(std::initializer_list<Type> values) {
        // Реализуйте конструктор самостоятельно
        Assign(values.begin(), values.end());
    }

    SingleLinkedList(const SingleLinkedList& other) {
        // Реализуйте конструктор самостоятельно
        Assign(other.begin(), other.end());
    }

    ~SingleLinkedList()
    {
        Clear();
    };

    //========================================================================
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        // Реализуйте присваивание самостоятельно
        if (this != &rhs)  //  сравниваем адреса, т.к быстрее
        {
            // делаем временную копию rhs
            SingleLinkedList<Type> temp(rhs);

            // обмен копиями с текущим объектом
            swap(temp);

            // удаление временного объекта temp при выходе 

        }

        return *this;
    }

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        // Реализуйте обмен содержимого списков самостоятельно
        std::swap(other.head_.next_node, head_.next_node);
        std::swap(other.size_, size_);
    }


    //=======================================================================
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }


    // Очищает список за время O(N)
    void Clear() noexcept {
        // Реализуйте метод самостоятельно
        while (head_.next_node != nullptr)
        {
            Node* temp = head_.next_node;
            head_.next_node = temp->next_node;
            delete temp;
        }
        size_ = 0;
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        if (size_ == 0)
        {
            return true;
        }
        return false;
    }

    //Вставляет элемент value после элемента, на который указывает pos.
      //Возвращает итератор на вставленный элемент
      //Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        // Реализуйте метод самостоятельно
        assert(pos.node_ != nullptr);

        Iterator it{ pos.node_ };
        Node* new_node = new Node(value, it.node_->next_node);
        it.node_->next_node = new_node;
        ++size_;

        return ++it;
    }

    void PopFront() noexcept
    {
        assert(head_.next_node != nullptr);
        Node* temp = head_.next_node;
        head_.next_node = temp->next_node;
        delete temp;
        --size_;
    }


    //Удаляет элемент, следующий за pos.
    //Возвращает итератор на элемент, следующий за удалённым
    Iterator EraseAfter(ConstIterator pos) noexcept {
        // Реализуйте метод самостоятельно
        assert(pos.node_ != nullptr);
   
        Iterator it{ pos.node_ };
        if (it == cbefore_begin())
        {
            if (size_ == 0) 
            {
                return end();
            }
            
            PopFront();
            return ++it;
        }

        Node* temp = it.node_->next_node;
        it.node_->next_node = temp-> next_node;
        delete temp;
        --size_;

        return ++it;
    }


private:
    // !Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0;

    //======================================================================= 
    template <typename InputIterator>
    void Assign(InputIterator it_from, InputIterator it_to)
    {
        // Временный список по default()
        SingleLinkedList<Type> temp;

        // указатель на указатель первого узла 
        Node** node_ptr_ptr = &temp.head_.next_node;

        while (it_from != it_to)
        {
            // текущий указатель = next_node =  null
            assert(*node_ptr_ptr == nullptr);

            // создаем новый узел,
            *node_ptr_ptr = new Node(*it_from, nullptr);
            ++temp.size_;

            // указатель = next_node
            node_ptr_ptr = &((*node_ptr_ptr)->next_node);

            ++it_from;
        }
        swap(temp);
    }
	
};

// внешние функции разместите здесь
template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    // Реализуйте обмен самостоятельно
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // Реализуйте сравнение самостоятельно
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());

}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // Реализуйте сравнение самостоятельно
    return  !operator==(lhs, rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // Заглушка. Реализуйте сравнение самостоятельно
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    //  Реализуйте сравнение самостоятельно
    return  !operator>(lhs, rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    //  Реализуйте сравнение самостоятельно
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // Реализуйте сравнение самостоятельно
    return  !operator<(lhs, rhs);
}

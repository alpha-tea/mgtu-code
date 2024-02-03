#include "12_binary_search_tree.h"

// Бинарное дерево с динамической памятью и произвольными объектами, компиляция в 32-бита.
// Все ограничения по памяти 16 бит и/или 32 Кбайта, по объектам 8 бит или 255 байт.
// Неопределённые данные сравниваются побайтно. Символы по таблице аски, слово по значению.
// Число с плавающей точкой сравнивается с учётом окрестности точки. 'epsilon' - задан статично.
// Сравнение текста идёт по всей длинне побуквенно, короткая строка всегда меньше длинны.

short is_node_correct(struct node* n)
{   // Проверяем узел на корректность.
    return !(n == NULL || n->obj == NULL);
}

short node_create(struct node** dst, void* data, unsigned char size)
{   // создание узла с выделением динамическо памяти под него самого и под данные. dst - null обязательно.
    if (dst == NULL || *dst != NULL || data == NULL || size == 0) {
        printf("error in nodes create: *dst(%p), data(%p), size = %d incorrect;\n", dst, data, size);
        return err_incorrect;
    }
    if ((short)(memory_used + size) < 0) {
        printf("not enough free memory, %hu for object size %hhu;\n", memory_free, size);
        return err_memory;
    }
    (*dst) = calloc(1, sizeof(struct node));
    void* new_data = calloc(1, size);
    if (new_data == NULL || (*dst) == NULL) {
        printf("error create nodes: can't allocate dynamic memory;\n");
        exit(-1);
    }
    (*dst)->obj = new_data;
    (*dst)->previous = (*dst)->left = (*dst)->right = NULL;
    for (short i = 0; i < size; ++i)
        ((char*)(*dst)->obj)[i] = ((char*)data)[i];
    printf("create node (%p) from data (%p) to object (%p) and size = %hhu bytes;\n",
           *dst, data, (*dst)->obj, size);
    memory_used += size; memory_free -= size;
    return err_ok;
}

short node_copy(struct node** dst, struct node* src, unsigned char size)  // ptr на ptr
{   // Копирование узла с выделением новой памяти. dst obj == NULL вызвать node_create
    if (!is_node_correct(src) || dst == NULL || (*dst) != NULL) {
        printf("error in nodes copy: dst(%p), src(%p), size = %hhu incorrect or not free memory;\n",
               (*dst), src, size);
        return err_incorrect;
    }
    short result = node_create(dst, src->obj, size);
    if (result)
        return result;
    printf("Copy data node from (%p) to node (%p), data (%p) and size %hhu;\n",
           src->obj, dst, (*dst)->obj, size);
    return err_ok;
}

short node_move(struct node** dst, struct node** src)
{   // перемещение объекта без создания копии, приёмник должен быть пустым
    if (dst == NULL || (*dst) != NULL || !is_node_correct(*src)) {
        printf("nodes move error: dst or src is NULL;\n");
        return err_incorrect;
    }
    *dst = *src;    // копирование структуры поэлементно
    printf("move node from (%p) to (%p), object (%p), reset source;\n", src, dst, (*dst)->obj);
    *src = NULL;    //check
    return err_ok;
}

short node_destroy(struct node** dst, unsigned char size)
{   // освобождение памяти и зануление всех параметров
    if (dst == NULL || !is_node_correct(*dst) || size == 0) {
        printf("error in node destroy: dst(%p) or node incorrect (%p);\n", dst, *dst);
        return err_incorrect;
    }
    printf("destroy node (%p), object (%p), size %hhu bytes;\n", *dst, (*dst)->obj, size);
    free((*dst)->obj);
    free((*dst));
    (*dst) = NULL;
    memory_used -= size;
    memory_free += size;
    return err_ok;
}

short node_to_text(struct binary_tree* tree, struct node* leaf, char* text, short is_clear)
{   // Дополнительная функция преобразования узла в текст.
    if (tree == NULL || !is_node_correct(leaf) || text == NULL) {
        printf("node to text error: tree(%p), text(%p) are NULL or node is incorrect;\n",
               tree, text);
        return err_incorrect;
    }
    if (is_clear)
        for (short i = 0; i < OBJ_MAX; ++i)
            text[i] = 0;
    unsigned char high_nibb_mask = 0xF0, low_nibb_mask = 0x0F, nibb_size = CHAR_BIT >> 0x01, len;
    switch (tree->obj_type) {
    case obj_data: {
        for (unsigned char i = 0; i < tree->obj_size; ++i) {
            text[i << 0x01] = hex_tab[(((unsigned char*)(leaf->obj))[i] & high_nibb_mask) >> nibb_size];
            text[i << 0x01 | 0x01] = hex_tab[(((unsigned char*)(leaf->obj))[i] & low_nibb_mask)];
        }
        break;
    }
    case obj_text: {
        for (unsigned char i = 0; i < tree->obj_size; ++i)
            text[i] = ((char*)leaf->obj)[i];
        break;
    }
    case obj_word: {
        len = sprintf_s(text, OBJ_MAX, "%hd", *((short*)leaf->obj));
        if (!is_clear)
            text[len] = print_default_char;
        break;
    }
    case obj_float: {
        len = sprintf_s(text, OBJ_MAX, "%.2f", *((float*)leaf->obj));
        if (!is_clear)
            text[len] = print_default_char;
        break;
    }
    default:
        printf("warning node to text: no type '%s' in tree;\n", obj_type_name[tree->obj_type]);
        return err_incorrect;
    }
    return err_ok;
}

short nodes_compare(struct binary_tree* tree, struct node* left, struct node* right)
{   // Сравнение двух узлов дерева, если операнд слева меньше, то возвращаем -1, 0 - если равен и +1 - если больше.
    if (!is_node_correct(left) || !is_node_correct(right) || tree->obj_type >= obj_not) {
        printf("nodes compare error: left or right node is NULL;\n");
        return err_incorrect;
    }
    switch (tree->obj_type) {
    case obj_data:
    case obj_text: {
        for (unsigned short i = 0; i < tree->obj_size; ++i)
            if (((char*)left->obj)[i] > ((char*)right->obj)[i])
                return 1;
            else if (((char*)left->obj)[i] < ((char*)right->obj)[i])
                return -1;
        break;
    }
    case obj_word: {
        if ((*(short*)left->obj) > (*(short*)right->obj))
            return 1;
        else if ((*(short*)left->obj) < (*(short*)right->obj))
            return -1;
        break;
    }
    case obj_float: {
        if (fabs((*(float*)left->obj) - (*(float*)right->obj)) < float_epsilon)
            return 0;
        else if ((*(float*)left->obj) > (*(float*)right->obj))
            return 1;
        else
            return -1;
        break;
    }
    default:
        printf("error compare: object type incorrect;\n");
        return 2;
    }
    return err_ok;
}

struct node* tree_search_node(struct binary_tree* tree, struct node* obj)
{   // Поиск узла по равному заданному, используя стандартные пути обхода.
    // Если узел не найден, то возвращается ближайший узел по значению.
    if (tree == NULL || tree->data_size == 0 || !is_node_correct(obj)) {
        printf("error tree search node: tree(%p), tree is empty or object incorrect;\n", tree);
        return NULL;
    }
    short cmp, is_exist = 1;
    struct node* leaf = tree->root;
    while ((cmp = nodes_compare(tree, leaf, obj)) != 0 && is_exist) {
        if (cmp > 0) {
            if (leaf->left != NULL)
                leaf = leaf->left;
            else
                is_exist = 0;
        }
        if (cmp < 0) {
            if (leaf->right != NULL)
                leaf = leaf->right;
            else
                is_exist = 0;
        }
    }
    return leaf;
}

struct node* traverse_node(struct binary_tree* tree, struct node* stack[],
                           short* sp, struct node* left, struct node* right)
{   //Вспомогательная функция добавления левого или правого узла в стек.
    char text[OBJ_MAX];
    if (left != NULL) {
        stack[(*sp)++] = left;
        node_to_text(tree, left, text, 1);
        printf("Push left leaf at (%p), object '%s', stack size %hd;\n", left, text, *sp);
        return left;
    }
    if (right != NULL) {
        stack[(*sp)++] = right;
        node_to_text(tree, right, text, 1);
        printf("Push right leaf at (%p), object '%s', stack size %hd;\n", right, text, *sp);
        return right;
    }
    return NULL;
}

struct node* traverse_pop(struct binary_tree* tree, struct node* stack[], short* sp)
{   // Вспомогательая функция
    struct node* leaf;
    char text[OBJ_MAX];
    if ((*sp) <= IDX_MAX && (*sp) > 0) {
        leaf = stack[--(*sp)];
        node_to_text(tree, leaf, text, 1);
        printf("Pop leaf at (%p), object '%s'\n", leaf, text);
    } else {
        printf("error pop: stack %hd incorrect;\n", *sp);
        return NULL;
    }
    return leaf;
}

short tree_traverse(struct binary_tree* tree, enum search_type type) {
    // Полный обход дерева, сохранение элементов в глобальный массив.
    if (tree == NULL || tree->data_size == 0 || type > sr_not) {
        printf("error tree traverse: tree(%p), tree is empty or type(%d) is incorrect;\n", tree, type);
        return err_incorrect;
    }
    printf("Traversing tree (%p) using '%s' type, size %hu, type '%s';\n",
           tree, sreach_name[type], tree->data_size, obj_type_name[tree->obj_type]);
    struct node* stack[MEM_MAX];
    short sp = 0;
    struct node* leaf = tree->root;
    stack[sp++] = leaf;
    tree_nodes_size = 0;
    while (sp > 0) {
        switch (type) {
        case sr_node_left_right:    // pre-order
            leaf = traverse_pop(tree, stack, &sp);
            tree_nodes[tree_nodes_size++] = leaf;
            traverse_node(tree, stack, &sp, NULL, leaf->right);
            traverse_node(tree, stack, &sp, leaf->left, NULL);
            break;
        case sr_left_node_right:        // in-order
            while (leaf->left != NULL)
                leaf = traverse_node(tree, stack, &sp, leaf->left, NULL);
            leaf = traverse_pop(tree, stack, &sp);
            tree_nodes[tree_nodes_size++] = leaf;
            if (leaf->right != NULL)
                leaf = traverse_node(tree, stack, &sp, NULL, leaf->right);
            break;
        case sr_left_right_node:        // post-order
            while (leaf->left != NULL) {
                stack[sp++] = leaf;
                leaf = leaf->left;
            }
            stack[sp++] = leaf;
            leaf = leaf->previous;
            if (leaf->right != NULL)
                leaf = leaf->right;
            break;
        default:
            printf("no");
        }
    }
    return err_ok;
}

short tree_print(struct binary_tree* tree, unsigned char endl)
{   // Вывод дерева по центру в позицию в глобальной настройке.
    // Используется метод обхода в ширину, с учётом предыдущей строки.
    if (tree == NULL) {
        printf("error print tree: tree is NULL;\n");
        return err_incorrect;
    }
    printf("\nPrint tree at (%p), size %hu, memory %hu bytes, type '%s',"
           " size %hu bytes, console width %u;\n", tree, tree->data_size, tree->memory_size,
           obj_type_name[tree->obj_type], tree->obj_size, scr_width);
    if (tree->data_size == 0) {
        printf("tree is empty, nothing to print;\n");
        return err_ok;
    }
    struct node* line[OBJ_MAX], *next_line[OBJ_MAX];
    short line_size = 1, next_line_size = 0, is_empty = 1, i = 0;
    char txt[OBJ_MAX];
    unsigned char pos, tab = scr_width, lev = 1;
    line[0] = tree->root;
    while (line_size > 0 && is_empty == 1) {
        for (i = 0; i < scr_width; ++i)
            txt[i] = print_default_char;
        txt[scr_width] = '\0';
        pos = scr_width >> lev;
        for (i = 0, next_line_size = 0; i < line_size; i++) {
            if (line[i] != NULL) {
                next_line[next_line_size++] = line[i]->left;
                next_line[next_line_size++] = line[i]->right;
            } else  {
                next_line[next_line_size++] = NULL;
                next_line[next_line_size++] = NULL;
            }
            if (line[i] != NULL)
                node_to_text(tree, line[i], &txt[pos], 0);
            pos += tab;
        }
        printf("%s\n", txt);
        for (i = 0, line_size = 0, is_empty = 0; i < next_line_size; i++, ++line_size) {
            if (next_line[i] != NULL)
                is_empty = 1;
            line[line_size] = next_line[i];
        }
        tab >>= 0x01;
        ++lev;
    }
    while (endl--)
        printf("\n");
    return err_ok;
}

short tree_add_node(struct binary_tree* tree, void* data)
{   // Добавление узла в дерево, объект должен существовать, правила сравнения выше. Использовать search.
    if (tree == NULL || data == NULL) {
        printf("error tree add node: tree(%p) or data(%p) is NULL;\n", tree, data);
        return err_incorrect;
    }
    struct node* new_leaf = NULL, *near_leaf = NULL;
    short cmp = 0, r = 0;
    char txt[OBJ_MAX];
    r = node_create(&new_leaf, data, tree->obj_size);
    if (r > 0) {
        printf("error add node: '%s';\n", errors[r]);
        return r;
    }
    node_to_text(tree, new_leaf, txt, 1);
    printf("Add object of type '%s', size %hu to tree and data '%s' at (%p);\n",
           obj_type_name[tree->obj_type], tree->obj_size, txt, new_leaf);
    if (tree->data_size != 0) {
        near_leaf = tree_search_node(tree, new_leaf);
        if ((cmp = nodes_compare(tree, new_leaf, near_leaf)) != 0) {
            node_to_text(tree, near_leaf, txt, 1);
            if (cmp == +1) {
                near_leaf->right = new_leaf;
                printf("nearest node at (%p), object data '%s', compare flag %hd, add to right;\n",
                       near_leaf, txt, cmp);
            }
            if (cmp == -1) {
                near_leaf->left = new_leaf;
                printf("nearest node at (%p), object data '%s', compare flag %hd, add to left;\n",
                       near_leaf, txt, cmp);
            }
            new_leaf->previous = near_leaf;
        } else {
            printf("node at (%p), object data '%s' is already in tree, destroy new leaf;\n",
                   near_leaf, txt);
            node_destroy(&new_leaf, tree->data_size);
            return err_already;
        }
    } else {
        printf("tree is empty, nearest node is root, moving new leaf to root;\n");
        r = node_move(&tree->root, &new_leaf);
    }
    tree->data_size++;
    tree->memory_size += tree->obj_size;
    return r;
}

void tree_replace_node(struct binary_tree* tree, struct node* dst, struct node* src)
{   // Замена одного элемента дерева другим, вспомогательная.
    if (tree == NULL || !is_node_correct(dst)) {
        printf("Node replace error, tree or dst node are NULL or incorrect;\n");
        return;
    }
    struct node* prev_dst = dst->previous;
    if (prev_dst != NULL) {
        if (prev_dst->left == dst)
            prev_dst->left = src;
        if (prev_dst->right == dst)
            prev_dst->right = src;
    } else {
        tree->root = src;
        printf("dst is root, new object %p set as root;\n", tree->root);
    }
    if (dst->left != NULL)
        dst->left->previous = src;
    if (dst->right != NULL)
        dst->right->previous = src;
    if (src != NULL)
        src->previous = prev_dst;
    node_destroy(&dst, tree->obj_size);
}

short tree_sub_node(struct binary_tree* tree, void* data)
{   // Удаление узла из дерева, объект должен существовать, правила сравнения выше.
    if (tree == NULL || data == NULL || tree->data_size == 0) {
        printf("error sub tree: tree is NULL or parameters incorrect;\n");
        return err_incorrect;
    }
    char text[OBJ_MAX];
    struct node object = {.obj = data, .previous = NULL, .left = NULL, .right = NULL};
    node_to_text(tree, &object, text, 1);
    printf("Sub node '%s' from tree size %hu;\n", text, tree->data_size);
    struct node* leaf = tree_search_node(tree, &object);
    if (nodes_compare(tree, leaf, &object) != 0) {
        printf("Node to sub not founded in tree, nothing to delete.");
        return err_incorrect;
    }
    printf("Element founded in tree = %p, define case of tree subtraction;\n", leaf->obj);
    if (leaf->left == NULL && leaf->right == NULL) {
        printf("Both derived nodes are NULL, so just delete node and free memory;\n");
        tree_replace_node(tree, leaf, NULL);
    } else if (leaf->left == NULL && leaf->right != NULL) {
        printf("Only right derived node is exist, moving it upper to deleted node;\n");
        tree_replace_node(tree, leaf, leaf->right);
    } else if (leaf->left != NULL && leaf->right == NULL) {
        printf("Only left derived node is exist, moving it upper to deleted node;\n");
        tree_replace_node(tree, leaf, leaf->left);
    } else if (leaf->left != NULL && leaf->right != NULL) {
        printf("Both derived nodes are exists, ");
        if (leaf->right->left == NULL)  {
            printf("from right derived -> left-derived not exist, just replace deleted node by right;\n");
            struct node* prev_r = leaf->right, *prev_l = leaf->left;
            tree_replace_node(tree, leaf, prev_r);
            prev_r->left = prev_l;
        } else if (leaf->right->left != NULL) {
            printf("from right derived -> left-derived is exist, replace deleted by this node;\n");
            struct node *prev_r = leaf->right, *prev_rl = leaf->right->left, *prev_l = leaf->left;
            struct node* prev_rlr = leaf->right->left->right;
            tree_replace_node(tree, leaf, leaf->right->left);
            prev_rl->left = prev_l;
            prev_rl->right = prev_r;
            prev_r->left = prev_rlr;
            prev_rlr->previous = prev_r;
        }
    }
    return err_ok;
}

short tree_create(struct binary_tree* tree, void* data, short objects, unsigned char obj_size,
                  enum obj_type type, short is_rnd)
{   // Создание дерева, на вход данные инициализации в виде массива или NULL если просто создать.
    // Если записи уже существуют - выход.
    if (tree == NULL || tree->data_size != 0 || obj_size == 0 || type >= obj_not ||
            (!is_rnd && data) == 0) {
        printf("error tree create: tree(%p), data size = %hu, obj size = %hu, type = %u;\n",
               tree, tree->data_size, tree->obj_size, type);
        return 1;
    }
    tree->obj_type = type;
    tree->obj_size = obj_size;
    tree->root = NULL;
    if (is_rnd)
        printf("Create tree and add %hd random objects of '%s';\n", objects, obj_type_name[type]);
    else
        printf("Create tree and add %hd source object of '%s';\n", objects, obj_type_name[type]);
    unsigned char rnd_text_data[OBJ_MAX];
    short rnd_word, rnd_sign;
    float rnd_float;
    for (short i = 0; i < objects; ++i) {
        if (is_rnd) {
            switch(type) {
            case obj_data: {
                for (unsigned char j = 0; j < obj_size; ++j)
                    rnd_text_data[j] = rand() % OBJ_MAX;
                data = rnd_text_data;
                break;
            }
            case obj_text: {
                for (unsigned char j = 0; j < obj_size; ++j)
                    rnd_text_data[j] = 'A' + rand() % rnd_char_max;
                data = rnd_text_data;
                break;
            }
            case obj_word: {
                rnd_word = (rand() % USHRT_MAX) % rnd_word_max;
                data = &rnd_word;
                break;
            }
            case obj_float: {
                rnd_sign = (rand() % 2) ? 1 : -1;
                rnd_float = rnd_sign * ((rand() % rnd_word_max) / (rand() % rnd_word_max + 1));
                data = &rnd_float;
                break;
            }
            default:
                printf("error in type object, type insupported;\n");
                return 1;
            }
            tree_add_node(tree, data);
        } else
            tree_add_node(tree, (void*)((unsigned char*)data + i * obj_size));
        printf("\n");
    }
    return 0;
}

short tree_copy(struct binary_tree* dst, struct binary_tree* src)
{   // Копирование дерева и всех его данных. Не забыть про память.
    if (src == NULL || dst == NULL || src->data_size == 0 || dst->data_size != 0) {
        printf("error in tree copy: dst(%p), src(%p), tree sizes %d and %d - incorrect;\n",
               dst, src, src->data_size, dst->data_size);
        return err_incorrect;
    }
    if ((src->memory_size + memory_used) > MEM_MAX) {
        printf("not enough free memory, %hu for object size %hu;\n", memory_free, src->memory_size);
        return err_memory;
    }
    printf("Copy tree from (%p) to (%p), size %hu and allocate %hu bytes new memory from free %hu bytes;\n",
           src, dst, src->data_size, src->memory_size, memory_free);
    dst->data_size = dst->memory_size = 0;
    dst->obj_type = src->obj_type;
    dst->obj_size = src->obj_size;
    short r = tree_traverse(src, default_search);
    printf("\nAdd all nodes to new tree, check size %hu;\n", tree_nodes_size);
    for (short i = 0; i < tree_nodes_size && !r; ++i)
        r = tree_add_node(dst, tree_nodes[i]->obj);
    printf("\nMemory check after copy, memory used %hu and free %hu bytes;\n\n", memory_used, memory_free);
    return r;
}

short tree_move(struct binary_tree** dst, struct binary_tree** src)
{   // Перемещение дерева по адресам, копирование не происходит, параметры источника обнуляются.
    if (dst == NULL || src == NULL || *dst == NULL || *src == NULL || (*dst)->data_size > 0) {
        printf("tree move error: source addresses (%p, %p) or destination (%p, %p) is incorrect or"
            "destination is not empty %hu, possible memory leak;\n", src, *src, dst, *dst, (*dst)->data_size);
        return err_incorrect;
    }
    (*dst) = (*src);
    *src = NULL;
    printf("\nMemory check after move, memory used %hu and free %hu bytes;\n\n", memory_used, memory_free);
    return err_ok;
}

short tree_destroy(struct binary_tree* tree)
{   // Освобождение памяти, занимаемой объектами и зануление параметров структуры дерева.
    if (tree == NULL) {
        printf("tree destroy error, tree(%p) is NULL;\n", tree);
        return err_incorrect;
    }
    if (tree->data_size > 0) {
        printf("tree has %hu object nodes, free memory %hu for all nodes;\n", tree->data_size, tree->memory_size);
        tree_traverse(tree, default_search);
        for (short i = 0; i < tree_nodes_size; ++i)
            node_destroy(&tree_nodes[i], tree->obj_size);
    } else {
        printf("Tree has no nodes, nothing to free, just set all parameters at 0 or NULL;\n");
    }
    tree->data_size = tree->memory_size = tree->obj_size = 0;
    tree->obj_type = obj_not;
    tree->root = NULL;
    printf("\nMemory free after destroy, memory used %hu and free %hu bytes;\n\n", memory_used, memory_free);
    return err_ok;
}

short tree_compare(struct binary_tree* left, struct binary_tree* right, short is_order)
{   // Деревья считаются равными если все узлы равны, размеры равны, порядок элементов неважен.
    if (left == NULL || right == NULL || left->data_size != right->data_size ||
            left->obj_type != right->obj_type || left->obj_size != right->obj_size) {
        printf("error compare: left (%p) or right (%p) tree addresses is NULL or has different"
               " sizes %hu and %hu or types '%s' and '%s' of object %hu and %hu bytes;\n" ,
               left, right, left->data_size, right->data_size,
               obj_type_name[left->obj_type], obj_type_name[right->obj_type],
                left->obj_size, right->obj_size);
        return err_incorrect;
    }
    printf("Compare tree (%p) and (%p), sizes %hu, object type '%s' and size %hu bytes is order %hd;\n",
           left, right, left->data_size, obj_type_name[left->obj_type], left->obj_size, is_order);
    short r_left = tree_traverse(left, default_search);
    struct node* tree_nodes_alt[MEM_MAX];
    short tree_alt_size = tree_nodes_size;
    char text[OBJ_MAX];
    for (short i = 0; i < tree_alt_size; ++i)
        tree_nodes_alt[i] = tree_nodes[i];
    short r_right = tree_traverse(right, default_search);
    if (r_left || r_right || tree_nodes_size != left->data_size) {
        printf("error in tree traverse, left %hd, right %hd codes or sizes;\n", r_left, r_right);
        return err_incorrect;
    }
    printf("\nPair of nodes: ");
    if (is_order) {
        for (short i = 0; i < tree_alt_size; ++i) {
            node_to_text(left, tree_nodes_alt[i], text, 1);
            printf("'%s':", text);
            node_to_text(right, tree_nodes[i], text, 1);
            printf("'%s' ", text);
            if (nodes_compare(left, tree_nodes_alt[i], tree_nodes[i]) != 0) {
                printf("\n\n");
                return -1;
            }
        }
    } else {
        for (short i = 0, j = 0, is_found; i < tree_alt_size; ++i) {
            for (j = 0, is_found = 0; j < tree_alt_size && !is_found; ++j)
                if (i != j && nodes_compare(left, tree_nodes_alt[i], tree_nodes[j]) == 0)
                    is_found = 1;
            if (!is_found) {
                printf("\n");
                return -1;
            } else {
                node_to_text(left, tree_nodes[i], text, 1);
                printf("'%s':'%s' ", text, text);
            }
        }
    }
    printf("\n\n");
    return 0;
}

short tree_balance(struct binary_tree* dst, struct binary_tree* src)
{   // Функция балансировки дерева, дополнительно.
    if (dst == NULL || src == NULL || src->data_size == 0) {
        printf("error in balance tree;\n");
        return err_incorrect;
    }
    printf("\nBalance tree using simple traverse and linear search;\n");
    tree_traverse(src, sr_node_left_right);
    short i = 0, j = 0, k = 0, more, less, medium = 0,  diff = src->data_size;
    for (i = 0; i < tree_nodes_size; ++i) {
        more = less = 0;
        for (j = 0; j < tree_nodes_size; ++j) {
            if (i != j) {
                if (nodes_compare(src, tree_nodes[i], tree_nodes[j]) == -1)
                    less++;
                else
                    more++;
            }
        }
        if (diff > abs(less - more)) {
            medium = i;
            diff = abs(less - more);
        }
    }
    char text[OBJ_MAX];
    node_to_text(src, tree_nodes[medium], text, 1);
    printf("medium node in tree '%s', index %hd;\n", text, medium);
    return err_ok;
}

int binary_search_tree()
{   // Вывести параметры нашей структуры и тест функций.
    printf("Laboratory. Binary tree with dynamic memory and random objects.\n\n");
    unsigned char byte_values[3] = {0x12, 0xA6, 0xFF};
    char txt[OBJ_MAX];
    struct binary_tree tree_a = {.data_size = 0, .memory_size = 0, .obj_size = 0, .obj_type = obj_not, .root = NULL},
                                tree_b = {.data_size = 0, .memory_size = 0, .obj_size = 0, .obj_type = obj_not, .root = NULL};
    struct binary_tree* ptr_a = &tree_a, *ptr_b = &tree_b;
    struct node* leaf_a = NULL, *leaf_b = NULL, *leaf_c = NULL, *leaf_d = NULL, *leaf_e =  NULL;
    printf("Struct size %d bytes;\n", sizeof(tree_a));
    printf("Name:\t\tOffset:\tSize:\n");
    printf("data_size\t%d\t%d\n", (void*)&tree_a.data_size - (void*)&tree_a, sizeof(tree_a.data_size));
    printf("memory_size\t%d\t%d\n", (void*)&tree_a.memory_size - (void*)&tree_a, sizeof(tree_a.memory_size));
    printf("obj_typer\t%d\t%d\n", (void*)&tree_a.obj_type - (void*)&tree_a, sizeof(tree_a.obj_type));
    //printf("obj_size\t%d\t%d\n", (void*)&tree_a.root.obj_size - (void*)&tree_a, sizeof(tree_a.root.obj_size));

    printf("struct node size %u bytes and data;\n", sizeof(struct node));
    printf("Name:\t\tOffset:\tSize:\n");
    printf("obj\t\t%d\t%d\n", (void*)&tree_a.root->obj - (void*)&tree_a, sizeof(tree_a.root->obj));
    printf("previous\t%d\t%d\n", (void*)&tree_a.root->previous - (void*)&tree_a, sizeof(tree_a.root->previous));
    printf("left\t\t%d\t%d\n", (void*)&tree_a.root->left - (void*)&tree_a, sizeof(tree_a.root->left));
    printf("right\t\t%d\t%d\n", (void*)&tree_a.root->right - (void*)&tree_a, sizeof(tree_a.root->right));

    printf("test functions of structre node or leafs in tree;\n\n");
    printf("conversion from structre node to string, all object types and correctnes;\n");
    tree_a.data_size = 1;
    node_create(&leaf_a, byte_values, sizeof(byte_values));
    tree_a.obj_type = obj_data; tree_a.obj_size = sizeof(byte_values);
    node_to_text(&tree_a, leaf_a, txt, 1);
    printf("source node type '%s' size %u bytes and value in hex = '%s'\n",
           obj_type_name[obj_data], tree_a.obj_size, txt);


    char* data_text = {"Hello 8BTP!"};
    node_create(&leaf_b, data_text, strlen(data_text));
    tree_a.obj_type = obj_text; tree_a.obj_size = strlen(data_text);
    node_to_text(&tree_a, leaf_b, txt, 1);
    printf("source node type '%s' size %u bytes and value in text = '%s'\n",
           obj_type_name[obj_text], tree_a.obj_size, txt);

    short val_a = 1;
    node_create(&leaf_c, &val_a, obj_type_size[obj_word]);
    tree_a.obj_type = obj_word; tree_a.obj_size = obj_type_size[obj_word];
    node_to_text(&tree_a, leaf_c, txt, 1);
    printf("source node type '%s' size %u bytes and value in word = '%s'\n",
           obj_type_name[obj_word], tree_a.obj_size, txt);

    float val_f = 3.14;
    node_create(&leaf_d, &val_f, obj_type_size[obj_float]);
    tree_a.obj_type = obj_float; tree_a.obj_size = obj_type_size[obj_float];
    node_to_text(&tree_a, leaf_d, txt, 1);
    printf("source node type '%s' size %u bytes and value in float = '%s'\n",
           obj_type_name[obj_float], tree_a.obj_size, txt);

    printf("\n\nCopy and move nodes and compare various types. 0 - equal, 1 - more, -1 - less;\n");
    tree_a.obj_type = obj_text;
    tree_a.obj_size = strlen(data_text);
    node_copy(&leaf_e, leaf_b, tree_a.obj_size);
    node_to_text(&tree_a, leaf_e, txt, 1);
    printf("after copy value '%s', try to copy node with text;\n", txt);
    short res = nodes_compare(&tree_a, leaf_b, leaf_e);
    printf("result of compare of text = %hd;\n", res);
    printf("trying to copy object already exist node, prevent memory leak;\n");
    node_copy(&leaf_e, leaf_b, strlen(data_text));
    node_destroy(&leaf_e, strlen(data_text));
    tree_a.obj_type = obj_float; tree_a.obj_size = obj_type_size[obj_float];
    node_copy(&leaf_e, leaf_d, tree_a.obj_size);
    *((float*)leaf_d->obj) += float_epsilon;
    res = nodes_compare(&tree_a, leaf_e, leaf_d);
    printf("Compare floating point %.2f and %.2f, epsilon = %.2f, result %hd;\n",
           *((float*)leaf_d->obj),  *((float*)leaf_e->obj), float_epsilon, res);

    printf("\nTesting moving nodes and check;\n");
    node_destroy(&leaf_e, obj_type_size[obj_float]);
    node_move(&leaf_e, &leaf_c);
    tree_a.obj_type = obj_word; tree_a.obj_size = obj_type_size[obj_word];
    node_to_text(&tree_a, leaf_e, txt, 1);
    printf("Check moving '%s', source address (%p);\n", txt, leaf_c);

    // Тестирование добавления элемента и вывода дерева поиска.
    printf("\nCreate and add nodes to tree;\n");
    tree_a.data_size = 0; tree_a.memory_size = 0;
    tree_a.obj_type = obj_word; tree_a.root = NULL; tree_a.obj_size = obj_type_size[obj_word];
    char data_chars[] = "FBADCEIGHJ";
    tree_create(&tree_a, data_chars, strlen(data_chars), obj_type_size[obj_text], obj_text, 0);
    tree_print(&tree_a, 1);
    printf("\nTreverse tree using different types of search;\n\n");
    for (short j = 0; j < sr_left_right_node; ++j) {
        tree_traverse(&tree_a, j);
        printf("\nAll elements after traverse: ");
        for (short i = 0; i < tree_nodes_size; ++i) {
            node_to_text(&tree_a, tree_nodes[i], txt, 1);
            printf("%s ", txt);
        }
        printf("\n");
    }
    // Копирование дерева с созданием нового объекта и попытка перемещение.
    printf("\nCopy all elements in tree, creating new object and compare, free memory %hu;\n\n",
           memory_free);
    tree_copy(&tree_b, &tree_a);
    tree_print(&tree_b, 1);
    short r = tree_compare(&tree_a, &tree_b, 1);
    if (!r)
        printf("Yes, trees (%p) and (%p) is equal and also with order flag;\n", &tree_a, &tree_b);
    else
        printf("No, trees (%p) and (%p) is not equal and also with order flag;\n", &tree_a, &tree_b);
    printf("Trying to move to existing tree, preventing memory tree;\n");
    tree_move(&ptr_a, &ptr_b);
    tree_print(ptr_a, 1);
    //char data_chars[] = "FBADCEIGHJ";
    tree_sub_node(&tree_a, &data_chars[0]);
    tree_print(&tree_a, 1);
    tree_sub_node(&tree_a, &data_chars[4]);
    tree_print(&tree_a, 1);
    tree_sub_node(&tree_a, &data_chars[1]);
    tree_print(&tree_a, 1);
    tree_sub_node(&tree_a, &data_chars[8]);
    tree_print(&tree_a, 1);
    tree_sub_node(&tree_a, &data_chars[2]);
    tree_print(&tree_a, 1);
    tree_sub_node(&tree_a, &data_chars[3]);
    tree_print(&tree_a, 1);
    tree_sub_node(&tree_a, &data_chars[6]);
    tree_print(&tree_a, 1);
    tree_destroy(&tree_a);
    // Балансировка дерева
    char data_unb[] = "ABCDEFG";
    tree_create(&tree_a, data_unb, strlen(data_unb), sizeof(char), obj_text, 0);
    tree_balance(&tree_b, &tree_a);
    tree_print(&tree_a, 1);
    // Удаление дерева для последующего перемещения
    printf("\nDestroy copy of tree and free nodes, memory used %hu;\n\n", memory_used);
    tree_destroy(&tree_a);
    printf("\nMoving tree from previous copy to source to print all nodes\n\n");
    tree_move(&ptr_a, &ptr_b);
    // Удаление динамических объектов и проверка памяти
    printf("\nClear all tree, nodes and free dynamic memory;\n\n");
    tree_destroy(ptr_a);
    node_destroy(&leaf_a, 3);
    node_destroy(&leaf_b, strlen(data_text));
    //node_destroy(&leaf_c, obj_type_size[obj_word]);
    node_destroy(&leaf_d, obj_type_size[obj_float]);
    node_destroy(&leaf_e, strlen(data_text));
    printf("free memory %hu and memory used %hu bytes;\n", memory_free, memory_used);
    /*
    printf("\nCompare nodes with various types, 0 - equal, 1 - more, -1 - less;\n");
    short res = nodes_compare(&tree_a, tree_a.root, &leaf_a);
    printf("Compare text leaf_a and root, result %hd;\n", res);
    tree_a.root->obj = &val_f; leaf_b.obj = &val_g;
    tree_a.obj_type = obj_float; tree_a.data_size = obj_type_size[obj_float];
    res = nodes_compare(&tree_a, tree_a.root, &leaf_b);
    printf("Compare floating point %.2f and %.2f, epsilon = %.2f, result %hd;\n",
           val_f, val_g, float_epsilon, res);
    free(leaf_a.obj);   // warning
    tree_a.data_size = 3; tree_a.root->obj = data_text[2];
    tree_a.obj_type = obj_text; tree_a.obj_size = strlen(data_text[2]);
    leaf_a.obj = data_text[1]; leaf_b.obj = data_text[3]; leaf_c.obj = data_text[1];
    tree_a.root->left = &leaf_a; tree_a.root->right = &leaf_b;
    leaf_ptr = tree_search_node(&tree_a, &leaf_c);
    printf("\nSearch existed '%s' data node, nearest from %p, value '%s'\n",
           data_text[1], leaf_ptr, (char*)leaf_ptr->obj);
    leaf_c.obj = data_text[4];
    leaf_ptr = tree_search_node(&tree_a, &leaf_c);
    printf("\nSearch not existed '%s' data node, nearest from %p, value '%s'\n",
           data_text[4], leaf_ptr, (char*)leaf_ptr->obj);
           */
    return 0;
}


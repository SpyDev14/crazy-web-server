# Соглашения в проекте
## Имена функций непосредственно связанных со структурами
Функции делятся на эти группы:
1. **Специальные**
    - **Конструкторы**:
        - Возвращают новый экземпляр типа на основе переданных аргументов **не
          содержащих** другой экземпляр этого типа.
        - **Формат**: `T new_<T>[_<subname>](...)`
        - **Примеры**:
            - `struct Vec new_Vec(size_t item_size)`
            - `struct Vec new_Vec_with_buffer(size_t item_size, void *buffer,
              size_t buffer_capacity)`
            - `struct Config new_Config_from_command_line(int argc, char *argv[])`
    - **Парсеры**:
        - Похожи на конструкторы, но возвращают `bool` (`true` - успешно,
          `false` - не валидная строка) и новый экземпляр структуры в out
          параметре (указатель). Допустимо 2 формата для строк *с* `\0` на
          конце и *без*. (реализовывать оба не обязательно).
        - **Форматы**:
            - `bool parse_<T>[_<subname>](char str[], T *out)`
            - `bool parse_n_<T>[_<subname>](char str[], size_t len, T *out)`
        - **Примеры**:
            - `bool parse_HTTPVersion(char str[], struct HTTPVersion *out)`
            - `bool parse_n_HTTPVersion(char str[], size_t len, struct HTTPVersion *out)`
            - `long parse_long(char str[], long *out)`
            - `long parse_n_long(char str[], size_t len, long *out)`
    - **Деструкторы**:
- Геттеры / сеттеры именуются как `get|set_<T>_<name>`
    - `get_Vec_count`
    - `set_Something_field`

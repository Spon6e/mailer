#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <regex>

namespace mailer::domain {

// TODO(учебное задание): добавить обработку ошибок для неизвестных переменных.
// Сейчас неизвестная переменная заменяется пустой строкой.
template <typename Context>
class TemplateRenderer final
{
public:
    //Сокращение имени чтоб не прописывать постоянно
    using Resolver = std::function<std::string(const Context&, std::string_view)>; //Лямбда выражение, которое принимает Шаблон и string_view и преобразует в string
private:
    Resolver resolver_;
public:
    //Запрещает неявное преобразование
    explicit TemplateRenderer(Resolver resolver)
        : resolver_(std::move(resolver))
    {
    }

    std::string render(const std::string& source, const Context& context) const
    {
        static const std::regex pattern(R"(\{\{\s*([A-Za-z0-9_]+)\s*\}\})");//Внутри [] имя

        std::string result;
        std::size_t preciousPosition = 0;

        const auto words_begin = std::sregex_iterator(source.begin(), source.end(), pattern); //Ищем первое слово
        const std::sregex_iterator end;

        for(auto i = words_begin; i!=end; ++i){
            const std::smatch& match = *i; //Тут наш name

            const auto position = static_cast<std::size_t>(match.position(0));

            const auto length = static_cast<std::size_t>(match.length(0));

            result.append(source, preciousPosition, position - preciousPosition);

            const std::string variableName = match.str(1);

            result += resolver_(context, std::string_view(variableName));

            preciousPosition = position + length;

        }

        result.append(source, preciousPosition, std::string::npos);
        return result;

        // TODO(задание 1): реализуй замену переменных вида {{name}}.
        //
        // Требования:
        // 1. Найди все вхождения шаблона регулярным выражением.
        // 2. Сохрани обычный текст между вхождениями без изменений.
        // 3. Для имени переменной вызови resolver_(context, variableName).
        // 4. Верни новую std::string, не изменяя source напрямую.
        // 5. Реши, что делать с неизвестной переменной: заменить на пустую
        //    строку, оставить исходный текст или вернуть ошибку. Обоснуй выбор.
        //
        // Подсказка: можно использовать std::regex/std::smatch или написать
        // небольшой ручной разбор. Не теряй символы между двумя переменными.
    }
};

} // namespace mailer::domain

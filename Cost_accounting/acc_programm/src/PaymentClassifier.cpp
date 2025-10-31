// ...existing code...
#include "../include/PaymentClassifier.h"
#include "../include/CategoryClassifier.h"
#include <memory>
#include <vector>
#include <string>

// Реализация методов, объявленных в include/PaymentClassifier.h
PaymentClassifier::PaymentClassifier() {
    buildClassifiers();
}

int PaymentClassifier::classify(const std::string& purpose) const {
    for (const auto& classifier : classifiers_) {
        if (classifier->matches(purpose)) {
            return classifier->getId();
        }
    }
    return 0; // не распознано
}

void PaymentClassifier::buildClassifiers() {
    // Здесь создаются все 115 классификаторов
    // Примеры ниже — остальные по аналогии
    classifiers_.push_back(std::make_unique<CategoryClassifier>(1, std::vector<std::string>{"труб"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(2, std::vector<std::string>{"лист"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(3, std::vector<std::string>{
        "тернополь", "аренда", "субаренда", "оренда", "гп-", "мс-", "рах", "помещен", "офис", "підприємств"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(4, std::vector<std::string>{
        "жд", "залізниця", "станція", "вагон", "укрзалізниця"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(5, std::vector<std::string>{
        "доставка", "импорт", "митниця", "таможня", "вэд", "транспортування", "експед", "організація доставки"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(6, std::vector<std::string>{
        "затрати по продажу", "витрати на продаж"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(7, std::vector<std::string>{
        "зарплата", "відпускні", "офіс", "податок на доходи", "військовий збір", "пдфо", "єсв", "іпн", "швачкін"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(8, std::vector<std::string>{
        "зарплата", "новомосковськ", "база"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(9, std::vector<std::string>{
        "зарплата", "цех"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(10, std::vector<std::string>{
        "зарплата", "форпост", "уч.5"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(11, std::vector<std::string>{
        "зарплата", "анд", "база"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(12, std::vector<std::string>{
        "матеріальна допомога", "нецільова матеріальна допомога", "разова матеріальна допомога"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(13, std::vector<std::string>{
        "податок на прибуток", "прибутковий податок"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(14, std::vector<std::string>{
        "ндс імпорт", "ввезення", "митний ндс"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(15, std::vector<std::string>{
        "рахунок ндс", "спецрахунок ндс", "поповнення ндс"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(16, std::vector<std::string>{
        "екологічний податок", "податок на нерухоме майно", "ліцензії", "фонд зп", "єсв"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(17, std::vector<std::string>{
        "комісія", "вед", "міжнародний переказ", "валютно-обмінна операція"
    }));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(18, std::vector<std::string>{
        "комісія", "обслуговування рахунку", "касове обслуговування", "поповнення карток", "банк", "оферта"
    }));

    // Коммунальные платежи (22–39)
    classifiers_.push_back(std::make_unique<CategoryClassifier>(22, std::vector<std::string>{"електроенергія", "офіс"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(23, std::vector<std::string>{"електроенергія", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(24, std::vector<std::string>{"електроенергія", "цех"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(25, std::vector<std::string>{"електроенергія", "форпост"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(26, std::vector<std::string>{"електроенергія", "анд"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(27, std::vector<std::string>{"електроенергія", "нерухомість"}));

    classifiers_.push_back(std::make_unique<CategoryClassifier>(28, std::vector<std::string>{"вода", "водопостачання", "офіс"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(29, std::vector<std::string>{"вода", "новомосковськ"}));
    // ... и так далее для всех 115

    // ГСМ и Ремонт авто (40–49)
    classifiers_.push_back(std::make_unique<CategoryClassifier>(40, std::vector<std::string>{"гсм", "авто", "дніпро"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(41, std::vector<std::string>{"гсм", "авто", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(42, std::vector<std::string>{"гсм", "маз"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(43, std::vector<std::string>{"гсм", "дрезина"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(44, std::vector<std::string>{"гсм", "погрузчик", "форпост"}));

    classifiers_.push_back(std::make_unique<CategoryClassifier>(45, std::vector<std::string>{"ремонт авто", "авто", "дніпро"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(46, std::vector<std::string>{"ремонт авто", "авто", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(47, std::vector<std::string>{"ремонт авто", "маз"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(48, std::vector<std::string>{"ремонт авто", "дрезина"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(49, std::vector<std::string>{"ремонт авто", "погрузчик", "форпост"}));

    // Текущие расходы — Новомосковск база (59–71)
    classifiers_.push_back(std::make_unique<CategoryClassifier>(59, std::vector<std::string>{"ремонт", "підтримка", "новомосковськ", "база"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(60, std::vector<std::string>{"кранове обладнання", "запчастини", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(61, std::vector<std::string>{"кран-1", "кк 12.5"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(62, std::vector<std::string>{"кран-2", "кк 20", "кк 30"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(63, std::vector<std::string>{"кран-3", "кк 12.5 фм"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(64, std::vector<std::string>{"ремонт", "запчастини", "інше обладнання", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(65, std::vector<std::string>{"інструменти", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(66, std::vector<std::string>{"виробничі матеріали", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(67, std::vector<std::string>{"участок порізки", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(68, std::vector<std::string>{"спецодяг", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(69, std::vector<std::string>{"благоустрій", "територія", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(70, std::vector<std::string>{"покупка обладнання", "новомосковськ"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(71, std::vector<std::string>{"інше", "новомосковськ"}));

    // Аналогично для ЦПТ (72–82), Сварочного участка (83–90), Форпост (91–99), АНД (100–108)

    // Инвестиции (109–115)
    classifiers_.push_back(std::make_unique<CategoryClassifier>(109, std::vector<std::string>{"стройка", "офіс", "будівництво"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(110, std::vector<std::string>{"нерухомість", "недвижимость"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(111, std::vector<std::string>{"сонячна станція", "фотоелектрика"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(112, std::vector<std::string>{"аренда землі", "рпб"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(113, std::vector<std::string>{"інвестиції", "абк"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(114, std::vector<std::string>{"інвестиції", "цех"}));
    classifiers_.push_back(std::make_unique<CategoryClassifier>(115, std::vector<std::string>{"тернопіль", "інвестиції тернопіль"}));
}
// ...existing code...
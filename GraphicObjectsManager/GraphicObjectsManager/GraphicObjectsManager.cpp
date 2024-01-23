#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

class GraphicObject { // Базовый класс для всех графических объектов
protected:
    int x, y; // Координаты объекта
    std::string color;// Цвет объекта
    bool isVisible;// Видимость объекта
    int size;// Размер объекта

public:
    GraphicObject(int x, int y, std::string color, int size = 10)// Конструктор
        : x(x), y(y), color(color), isVisible(true), size(size) {}// Инициализация полей

    virtual void move(int deltaX, int deltaY) {// Перемещение объекта
        x += deltaX;// Изменение координат
        y += deltaY;// Изменение координат
    }

    virtual void changeColor(const std::string& newColor) {// Изменение цвета
        color = newColor;// Изменение цвета
    }

    virtual void resize(int newSize) {// Изменение размера
        size = newSize;// Изменение размера
    }

    virtual void toggleVisibility() {// Изменение видимости
        isVisible = !isVisible;
    }

    virtual void draw() const = 0;
    virtual void save(std::ostream& os) const = 0;
    virtual void load(std::istream& is) = 0;
};

class Circle : public GraphicObject {// Это класс круга
private:
    int radius;

public:
    Circle(int x, int y, std::string color, int radius)// Конструктор
        : GraphicObject(x, y, color), radius(radius) {}

    void draw() const override {
        if (isVisible) {
            std::cout << "Drawing Circle at (" << x << "," << y << ") with radius " << radius << " and color " << color << "\n";
        }
    }

    void save(std::ostream& os) const override {
        os << "C " << x << " " << y << " " << color << " " << isVisible << " " << size << " " << radius << "\n";
    }

    void load(std::istream& is) override {
        is >> x >> y >> color >> isVisible >> size >> radius;
    }
};

class Rectangle : public GraphicObject {// Прямоугольник
private:
    int width, height;

public:
    Rectangle(int x, int y, std::string color, int width, int height)// Конструктор
        : GraphicObject(x, y, color), width(width), height(height) {}// Инициализация полей

    void draw() const override {
        if (isVisible) {
            std::cout << "Drawing Rectangle at (" << x << "," << y << ") with width " << width << " and height " << height << " and color " << color << "\n";
        }
    }

    void save(std::ostream& os) const override {
        os << "R " << x << " " << y << " " << color << " " << isVisible << " " << size << " " << width << " " << height << "\n";
    }

    void load(std::istream& is) override {
        is >> x >> y >> color >> isVisible >> size >> width >> height;
    }
};

class CompositeGraphicObject : public GraphicObject {// Композитный графический объект
private:
    std::vector<GraphicObject*> components;

public:
    CompositeGraphicObject() : GraphicObject(0, 0, "none") {}// Конструктор

    void addComponent(GraphicObject* component) {// Добавление компонента
        components.push_back(component);// Добавление компонента в вектор
    }

    void draw() const override {// Визуализация объекта
        for (auto* component : components) {// Визуализация каждого компонента
            component->draw();// Вызов метода draw() для каждого компонента
        }
    }

    void move(int deltaX, int deltaY) override {// Перемещение объекта
        for (auto* component : components) {
            component->move(deltaX, deltaY);
        }
    }

    void toggleVisibility() override {// Изменение видимости
        isVisible = !isVisible;
        for (auto* component : components) {
            component->toggleVisibility();
        }
    }

    void resize(int newSize) override {// Изменение размера
        size = newSize;
        for (auto* component : components) {
            component->resize(newSize);
        }
    }

    void save(std::ostream& os) const override {// Сохранение конфигурации в файл
        os << "Composite " << components.size() << "\n";
        for (const auto* component : components) {
            component->save(os);
        }
    }

    void load(std::istream& is) override {// Загрузка конфигурации из файла
        size_t numComponents;// Количество компонентов
        is >> numComponents;// Чтение количества компонентов
        components.clear();
        for (size_t i = 0; i < numComponents; ++i) {
            std::string type;
            is >> type; // Читаем тип компонента
            GraphicObject* component = nullptr;
            if (type == "C") {
                component = new Circle(0, 0, "", 0);
            }
            else if (type == "R") {
                component = new Rectangle(0, 0, "", 0, 0);
            }
            else if (type == "Composite") {
                component = new CompositeGraphicObject();
            }
            if (component) {
                component->load(is);
                components.push_back(component);
            }
        }
    }

};

void saveConfiguration(const std::vector<GraphicObject*>& objects, const std::string& filename) {// Сохранение конфигурации в файл
    std::ofstream file(filename);// Открытие файла для записи
    if (!file) {
        std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
        return;
    }

    for (const auto* obj : objects) {// Сохранение каждого объекта
        obj->save(file);
    }
}

void loadConfiguration(std::vector<GraphicObject*>& objects, const std::string& filename) {// Загрузка конфигурации из файла
    std::ifstream file(filename);// Открытие файла для чтения
    if (!file) {
        std::cerr << "Не удалось открыть файл для чтения: " << filename << std::endl;
        return;
    }

    objects.clear();
    std::string type;
    while (file >> type) {
        if (type == "C") {
            Circle* circle = new Circle(0, 0, "", 0);
            circle->load(file);
            objects.push_back(circle);
        }
        else if (type == "R") {
            Rectangle* rect = new Rectangle(0, 0, "", 0, 0);
            rect->load(file);
            objects.push_back(rect);
        }
        // Для CompositeGraphicObject требуется отдельная логика
    }
}

void processUserCommands(std::vector<GraphicObject*>& objects) {// Обработка команд пользователя
    while (true) {
        char command;
        std::cout << "Введите команду:\n"
            << "c - создать круг\n"
            << "r - создать прямоугольник\n"
            << "a - создать агрегат\n"
            << "m - двигать\n"
            << "s - сохранить конфигурацию\n"
            << "l - загрузить конфигурацию\n"
            << "q - выход\n"
            << "Выбор: ";
        std::cin >> command;

        switch (command) {// Обработка команды
        case 'c': {// Создание круга
            int x, y, radius;
            std::string color;
            std::cout << "Enter x, y, radius, color: ";
            std::cin >> x >> y >> radius >> color;
            objects.push_back(new Circle(x, y, color, radius));
            break;
        }
        case 'r': {// Создание прямоугольника
            int x, y, width, height;
            std::string color;
            std::cout << "Enter x, y, width, height, color: ";
            std::cin >> x >> y >> width >> height >> color;
            objects.push_back(new Rectangle(x, y, color, width, height));
            break;
        }
        case 'a': {// Создание агрегата
            CompositeGraphicObject* composite = new CompositeGraphicObject();
            int numComponents;
            std::cout << "Введите количество объектов для агрегации: ";
            std::cin >> numComponents;
            while (numComponents-- > 0 && !objects.empty()) {
                composite->addComponent(objects.back());
                objects.pop_back();
            }
            objects.push_back(composite);
            break;
        }
        case 'm': {// Движение объекта
            if (!objects.empty()) {
                int deltaX, deltaY;
                std::cout << "Enter the movement vector (deltaX deltaY): ";
                std::cin >> deltaX >> deltaY;
                objects.back()->move(deltaX, deltaY);
            }
            break;
        }
        case 's': { // Сохранение конфигурации
            std::string filename;
            std::cout << "Введите имя файла для сохранения: ";
            std::cin >> filename;
            saveConfiguration(objects, filename);
            break;
        }
        case 'l': { // Загрузка конфигурации
            std::string filename;
            std::cout << "Введите имя файла для загрузки: ";
            std::cin >> filename;
            loadConfiguration(objects, filename);
            break;
        }
        case 'q': // Выход из программы
            std::cout << "Выход из программы...\n";
            return; // Используем return для выхода из цикла и функции
        default:
            std::cout << "Неизвестная команда!\n";
            break;
        }

        // Визуализация объектов
        for (const auto* obj : objects) {
            obj->draw();
        }
    }
}

void changeCurrentObjectProperties(std::vector<GraphicObject*>& objects) {// Изменение свойств текущего объекта
    if (objects.empty()) {// Проверка наличия объектов
        std::cout << "Нет объектов для изменения.\n";
        return;// Выход из функции
    }

    int objectIndex;// Индекс объекта для изменения
    std::cout << "Введите индекс объекта для изменения (0-" << objects.size() - 1 << "): ";
    std::cin >> objectIndex;
    if (objectIndex < 0 || objectIndex >= objects.size()) {// Проверка корректности индекса
        std::cout << "Неверный индекс объекта.\n";
        return;
    }

    GraphicObject* currentObject = objects[objectIndex];

    int property;
    std::cout << "Выберите свойство для изменения (1-цвет, 2-размер): ";
    std::cin >> property;

    if (property == 1) {
        std::string newColor;
        std::cout << "Введите новый цвет: ";
        std::cin >> newColor;
        currentObject->changeColor(newColor);
    }
    else if (property == 2) {
        int newSize;
        std::cout << "Введите новый размер: ";
        std::cin >> newSize;
        currentObject->resize(newSize);
    }
    else {
        std::cout << "Неверное свойство.\n";
    }
}




int main() {
    setlocale(LC_ALL, "Russian");
    std::vector<GraphicObject*> objects; // Вектор указателей на графические объекты

    while (true) {// Бесконечный цикл
        processUserCommands(objects);// Обработка команд пользователя
        for (const auto* obj : objects) {// Визуализация объектов
            obj->draw();// Вызов метода draw() для каждого объекта
        }
    }

    for (auto* obj : objects) {// Освобождение памяти
        delete obj;
    }

    return 0;
}


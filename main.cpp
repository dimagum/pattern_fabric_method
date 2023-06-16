#include <iostream>
#include <random>
#include <fstream>


int random(int min, int max) {
    static std::mt19937 prng(std::random_device{}());
    return std::uniform_int_distribution<>(min, max)(prng);
}




// будет 2 продукта: клетка границы и клетка внутренней области
class IProduction {
public:
    virtual ~IProduction() = default;
    virtual char release() = 0;
};

// клетка границы
class BorderCell : public IProduction {
public:
    char release() override {
        return random(0, 100) >= 95 ? '0' : '#';
    }
};

// клетка внутренней области
class InnerAreaCell : public IProduction {
public:
    char release() override {
        return random(0, 20) >= 19 ? '*' : ' ';
    }
};


// будет 2 фабрики: одна для клеток границы и одна для клеток внутренней области
class IWorkShop {
public:
    virtual ~IWorkShop() = default;
    virtual IProduction* create() = 0;
};

// фабрика граничных клеток
class BorderCellWorkShop : public IWorkShop {
public:
    IProduction* create() override {
        return new BorderCell();
    }
};

// фабрика клеток внутренней области
class InnerAreaCellWorkShop : public IWorkShop {
public:
    IProduction* create() override {
        return new InnerAreaCell();
    }
};


// класс для генерации нового поля
class FieldGenerator {
    IProduction* border;
    IProduction* inner;
    int n, m;

    bool isBorder(int i, int j) {
        return i == 0 || i == n - 1 || j == 0 || j == m - 1;
    }

    bool isCorner(int i, int j) {
        if (i == 0 && j == 0 ||
            i == n - 1 && j == 0 ||
            i == n - 1 && j == m - 1 ||
            i == 0 && j == m - 1)
        {
            return true;
        }
        return false;
    }

    std::pair<int, int> chooseKeyPosition() {
        return std::make_pair(random(1, n - 1 - 1), random(1, m - 1 - 1));
    }

public:
    FieldGenerator(int n, int m) {
        IWorkShop* creator = new BorderCellWorkShop();
        border = creator->create();
        delete creator;

        creator = new InnerAreaCellWorkShop();
        inner = creator->create();
        delete creator;

        this->n = n;
        this->m = m;
    }
    ~FieldGenerator() {
        delete border;
        delete inner;
    }

    void set_size(int n, int m) {
        this->n = n;
        this->m = m;
    }

    std::string generate() {
        std::string field;
        int door_counter = 0;

        for (int i = 0; i < n; ++i) {
            std::string line;
            for (int j = 0; j < m; ++j) {
                if (isCorner(i, j)) {
                    line += '#';
                    continue;
                }

                if (isBorder(i, j)) {
                    if (door_counter < 4) {
                        line += border->release();
                        if (line.back() == '0') {
                            door_counter++;
                        }
                    }
                    else {
                        line += '#';
                    }
                    continue;
                }

                line += inner->release();

            }
            field += line + '\n';
        }

        auto [x, y] = chooseKeyPosition();

        field[x * n + y] = 'K';

        return field;
    }

};




int main() {
    FieldGenerator generator(20, 40);

    std::ofstream file("field.txt");

    file << generator.generate();

    file.close();

    std::cout << generator.generate();

    return 0;
}

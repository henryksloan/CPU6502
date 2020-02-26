#include <thread>
#include <atomic>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "cpu_6502.h"

int main(int argc, char **argv) {
    std::shared_ptr<Memory> mem = std::make_shared<Memory>(Memory());
    CPU6502 cpu(mem);

    std::ifstream file(argv[1], std::ifstream::binary);
    if (!file) {
        std::cerr << "Could not open file " << argv[1] << '\n';
        std::cout << "Usage: " << argv[0] << " <binary file>\n";
        return 1;
    }
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    mem->load_file(file, 0, length-1, 0x600);
    // mem->print();
    file.clear();
    file.seekg(0, file.beg);

    std::unordered_map<int, sf::Color> col {
         {0x0, sf::Color::Black},
         {0x1, sf::Color::White},
         {0x2, sf::Color::Red},
         {0x3, sf::Color::Cyan},
         {0x4, sf::Color::Magenta},
         {0x5, sf::Color::Green},
         {0x6, sf::Color::Blue},
         {0x7, sf::Color::Yellow},
         {0x8, sf::Color(0xFA, 0xA5, 0x00)},
         {0x9, sf::Color(0xFF, 0xF8, 0xDC)},
         {0xa, sf::Color(0xFF, 0x4D, 0x4D)},
         {0xb, sf::Color(0x69, 0x69, 0x69)},
         {0xc, sf::Color(0xA9, 0xA9, 0xA9)},
         {0xd, sf::Color(0xAE, 0xF3, 0x59)},
         {0xe, sf::Color(0x63, 0xC5, 0xDA)},
         {0xf, sf::Color(0xD3, 0xD3, 0xD3)}
    };

    sf::RenderWindow window(sf::VideoMode(800, 800), "CPU6502");
    window.setFramerateLimit(0);
    sf::Event event;

    std::atomic<bool> done = false;
    std::thread thr([&done, &mem, &cpu] {
        sf::Clock clock;
        while(!done){
            clock.restart();
            mem->write_byte(0xfe, std::rand()%0x100);
            cpu.step();
            long x = clock.restart().asMicroseconds();
            const int fps = 1600;
            // std::cout << x << " " << 1000.0/fps << std::endl;
            // if (x < 1000.0/fps) std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0/fps-x)));
            std::this_thread::sleep_for(std::chrono::microseconds((int)ceil((1000.0*1000.0)/fps)));
        }
    });
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) mem->write_byte(0xff, 0x77);
                if (event.key.code == sf::Keyboard::A) mem->write_byte(0xff, 0x61);
                if (event.key.code == sf::Keyboard::S) mem->write_byte(0xff, 0x73);
                if (event.key.code == sf::Keyboard::D) mem->write_byte(0xff, 0x64);
            }
        }
        window.clear(sf::Color::Black);
        for (int i = 0x200; i < 0x600; i += 0x20) {
            for (int j = i; j < i+0x20; j++) {
                sf::RectangleShape pixel(sf::Vector2f(10, 10));
                // pixel.setPosition(((i-0x200)/0x20)*5, 5*(j-0x200));
                pixel.setPosition(10*(j%0x20), 10*((i-0x200)/0x20));
                pixel.setFillColor(col[mem->read_byte(j)&0xF]);
                window.draw(pixel);
            }
            // std::cout << std::endl;
        }

        window.display();
    }
    done = true;
    thr.join();

    return 0;
}

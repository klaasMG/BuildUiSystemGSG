#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv) {
    std::ifstream file("the_ui_tree_build/assets/shaders/WidgetDataType.txt");
    std::cout << std::filesystem::current_path() << "\n";
    if (!file) return 1;

    std::cout << "opened\n";
    std::cout << file.rdbuf();
}
#include "process_args.hpp"
#include "colors.hpp"
#include "puzzle.hpp"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <vector>

const std::string puzzle_option = "-p";
const std::string file_option = "-f";
const std::string usage_string = "usage: sudoku_solver [-p puzzle1 puzzle2 ... puzzleN] [-f puzzle_file_path]";
std::vector<std::string> args;

void print_success_statistic(int count_solved, int total)
{
    std::cout
        << Color::green << "Successfully solved "
        << Color::yellow << count_solved
        << Color::green << " out of "
        << Color::yellow << total
        << Color::green << " puzzles."
        << Color::endl;
}

void print_usage()
{
    std::cout << usage_string << std::endl;
}

void parse_args(int argc, char *argv[])
{
    if (argc <= 1)
    {
        print_usage();
        exit(0);
    }

    args.reserve(argc - 1);

    for (int i = 1; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }
}

void process_puzzles()
{
    std::cout
        << Color::green << "Processing "
        << Color::yellow << args.size() - 1
        << Color::green << " puzzles..." << Color::endl;

    int total = 0;
    int count_solved = 0;
    for (auto it = args.begin() + 1; it != args.end(); it++)
    {
        count_solved += process_puzzle(*it, ++total);
    }
    print_success_statistic(count_solved, total);
}

void process_args()
{
    const std::string option = args.front();
    if (option == puzzle_option)
    {
        process_puzzles();
    }
    else if (option == file_option)
    {
        if (args.size() < 2)
        {
            std::cout << Color::red << "File option requires a filename." << Color::endl;
            print_usage();
        }
        process_file(args.at(1));
    }
}

void process_file(std::string filepath)
{
    int total = 0;
    int count_solved = 0;
    std::ifstream infile(filepath);
    if (!infile.is_open())
    {
        std::cout
            << Color::red << "Could not open file: " << Color::purple << filepath
            << Color::endl;
        return;
    }
    for (std::string line; getline(infile, line);)
    {
        if (line.size() < 1)
        {
            continue;
        }
        count_solved += process_puzzle(line, ++total);
    }
    print_success_statistic(count_solved, total);
}

void illegal_option(std::string arg)
{
    std::cout << Color::red << "sudoku_solver: illegal option: " << Color::end << arg << std::endl;
    print_usage();
}

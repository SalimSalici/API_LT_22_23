## Highway Route Planner

This program was developed as a project for the Algoritmi e Principi dell'Informatica (API) course @ Milan Polytechnic.

The project was graded based on running time and memory usage, with lower values in both categories resulting in higher scores.

**Professor:** Davide Martinenghi

**Final Grade:** 30/30 with honors

### Evaluation table
| Score        |   Memory  | Time    |       Status       |
|--------------|----------:|--------:|:------------------:|
| 18           | < 128 MiB | < 19 s  | :white_check_mark: |
| 21           | < 118 MiB | < 15 s  | :white_check_mark: |
| 24           | < 108 MiB | < 10 s  | :white_check_mark: |
| 27           |  < 98 MiB | < 6 s   | :white_check_mark: |
| 30           |  < 88 MiB | < 4 s   | :white_check_mark: |
| 30 cum laude |  < 78 MiB | < 1 s   | :white_check_mark: |

### Problem Statement
Given a highway with charging stations at various distances, each with a fleet of electric vehicles of different ranges, the program must:

- Add/remove charging stations along the highway
- Manage electric vehicles at each station (add/scrap)
- Find the route with the fewest stops between any two stations
- When multiple routes have the same number of stops, prefer the one with earlier stops

The full specification of the project (in Italian) can be found [here](https://github.com/SalimSalici/API_LT_22_23/blob/main/Specifica_2022-2023.pdf).

### Tools

| Type                | Name                                                   |
|---------------------|--------------------------------------------------------|
| IDE                 | [Visual Studio Code](https://code.visualstudio.com/)   |
| Compiler            | [gcc](https://gcc.gnu.org/)                            |
| Profiling           | [KCachegrind](https://apps.kde.org/kcachegrind/)       |

### Usage

Compile the code using the following command:
```sh
make main
```

Run a simulation with one of the provided tests:
```sh
main.exe < tests/open_1.txt > open_1_result.txt
```

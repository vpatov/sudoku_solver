# Sudoku Solver

To create the makefile, run `cmake .`, and then `make` to build.

usage: `./sudoku_solver [-p puzzle1 puzzle2 ... puzzleN] [-f puzzle_file_path]`

Example:
```
./sudoku_solver -p 300200000000107000706030500070009080900020004010800050009040301000702000000008006
```

```
Sudoku Solver 1.0
By Vasia Patov

Processing 1 puzzles...
Puzzle 1:
Out of 56 initially empty cells, 16 were assigned using logic, and 40 were assigned using backtracking.
Backtracking required 1342 guesses (total amount of permutations of candidates for unassigned cells was 2.1 x 10^27).
351286497492157638786934512275469183938521764614873259829645371163792845547318926
-------------------------------------
| 3   5   1 | 2   8   6 | 4   9   7 |

| 4   9   2 | 1   5   7 | 6   3   8 |

| 7   8   6 | 9   3   4 | 5   1   2 |
-------------------------------------
| 2   7   5 | 4   6   9 | 1   8   3 |

| 9   3   8 | 5   2   1 | 7   6   4 |

| 6   1   4 | 8   7   3 | 2   5   9 |
-------------------------------------
| 8   2   9 | 6   4   5 | 3   7   1 |

| 1   6   3 | 7   9   2 | 8   4   5 |

| 5   4   7 | 3   1   8 | 9   2   6 |
-------------------------------------

Successfully solved 1 out of 1 puzzles.
```

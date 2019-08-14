#include <iostream>
#include <vector>
#include <fstream>

class Space {
 public:
  Space(int value, int row, int col);
  const std::vector<int> &getInvalidValues() const;
  const int &getValue();
  void addInvalid(int num);
  void simpleSolution();
  void setValue(int v);
  int getRow() const;
  int getCol() const;
 private:
  std::vector<int> invalidValues;
  int row;
  int col;
  int value;
};

Space::Space(int value, int row, int col) : value(value), row(row), col(col) {}

const int &Space::getValue() {
  return value;
}

const std::vector<int> &Space::getInvalidValues() const {
  return invalidValues;
}

void Space::addInvalid(int num) {
  for (int i : invalidValues) {
    if (num == i) return;
  }
  invalidValues.push_back(num);
}

int Space::getRow() const {
  return row;
}

int Space::getCol() const {
  return col;
}

void Space::simpleSolution() {
  int solution = 45;
  for (int i = 0; i < 8; ++i) {
    solution -= invalidValues[i];
  }
  value = solution;
}

void Space::setValue(int v) {
  Space::value = v;
}

class Board {
 public:
  Board();
  void addRow(const std::vector<Space> &row);
  void printBoard();
  void solveBoard();
  void initializeInvalidsAllSpaces();
  void findInvalids(Space &s);
  void commonInvalidsInSquare(Space &s);
  void commonInvalidsInLine(Space &s);
  void setSolved(bool solved);
  bool isSolved() const;
  std::vector<std::vector<Space>> &getBoard();
 private:
  std::vector<std::vector<Space>> board;
  bool solved;
};

Board backtrack(Board board, int row, int col) {
  board.initializeInvalidsAllSpaces();
  if (board.getBoard()[row][col].getValue() == -1) {
    Space &space = board.getBoard()[row][col];
    if ((int) space.getInvalidValues().size() == 9) {
      return board;
    }
    std::vector<int> validValues = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (const int &invalid : space.getInvalidValues()) {
      for (int k = 0; k < (int) validValues.size(); ++k) {
        if (invalid == validValues[k]) {
          validValues.erase(validValues.begin() + k);
          break;
        }
      }
    }
    for (int validValue : validValues) {
      space.setValue(validValue);
      int c, r;
      c = col;
      r = row;
      if (++c > 8) {
        c = 0;
        r++;
      }
      if (r > 8) {
        board.setSolved(true);
        return board;
      }
      auto next = backtrack(board, r, c);
      if (next.isSolved()) return next;
    }
  } else {
    int c, r;
    c = col;
    r = row;
    if (++c > 8) {
      c = 0;
      r++;
    }
    if (r > 8) {
      board.setSolved(true);
      return board;
    }
    return backtrack(board, r, c);
  }
  return board;
}

void Board::addRow(const std::vector<Space> &row) {
  board.push_back(row);
}

void Board::printBoard() {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (board[i][j].getValue() == -1) std::cout << "x ";
      else std::cout << board[i][j].getValue() << ' ';
    }
    std::cout << std::endl;
  }
}

void Board::solveBoard() {
  int itLimit = 0;
  initializeInvalidsAllSpaces();

  while (!solved) {
    int unsolvedSpaces = 0;
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 9; ++j) {
        if (board[i][j].getValue() == -1) {
          unsolvedSpaces++;
          if (board[i][j].getInvalidValues().size() == 8) {
            board[i][j].simpleSolution();
          } else {
            commonInvalidsInSquare(board[i][j]);
            if (board[i][j].getValue() == -1) commonInvalidsInLine(board[i][j]);
          }
          if (board[i][j].getValue() != -1) initializeInvalidsAllSpaces();
        }
      }
    }
    if (unsolvedSpaces == 0) {
      solved = true;
      return;
    }
    if (itLimit >= 100) {
      return;
    }
    itLimit++;
  }
}

void Board::findInvalids(Space &s) {
  //  Check row
  for (int i = 0; i < 9; ++i) {
    if (board[s.getRow()][i].getValue() != -1) {
      s.addInvalid(board[s.getRow()][i].getValue());
    }
  }
  //  Check column
  for (int i = 0; i < 9; ++i) {
    if (board[i][s.getCol()].getValue() != -1) {
      s.addInvalid(board[i][s.getCol()].getValue());
    }
  }
  //  Check square (sector), converts coordinates to apply to the larger squares
  //  i.e. (2,5) becomes (0,3) because that's where the square that the value is
  //  in starts.
  int sRow = s.getRow() / 3;
  int sCol = s.getCol() / 3;

  if (sRow == 1) sRow = 3;
  else if (sRow == 2) sRow = 6;

  if (sCol == 1) sCol = 3;
  else if (sCol == 2) sCol = 6;

  for (int i = sRow; i < sRow + 3; ++i) {
    for (int j = sCol; j < sCol + 3; ++j) {
      if (board[i][j].getValue() != -1)
        s.addInvalid(board[i][j].getValue());
    }
  }
}
Board::Board() : solved(false) {}

void Board::commonInvalidsInSquare(Space &s) {
  //  Checks every square in the space's sector,
  //  if they all have a common invalid, and Space s doesn't, that solves s.
  int sRow = s.getRow() / 3;
  int sCol = s.getCol() / 3;

  if (sRow == 1) sRow = 3;
  else if (sRow == 2) sRow = 6;

  if (sCol == 1) sCol = 3;
  else if (sCol == 2) sCol = 6;
  std::vector<int> sharedInvalids;
  bool firstValidRow = true;
  for (int i = sRow; i < sRow + 3; ++i) {
    for (int j = sCol; j < sCol + 3; ++j) {
      if (i == s.getRow() && j == s.getCol()) continue;
      if (board[i][j].getValue() == -1) {
        if (firstValidRow) {
          firstValidRow = false;
          sharedInvalids = board[i][j].getInvalidValues();
          continue;
        }
        for (int k = 0; k < (int) sharedInvalids.size(); ++k) {
          bool seen = false;
          for (int l : board[i][j].getInvalidValues()) {
            if (l == sharedInvalids[k]) {
              seen = true;
              break;
            }
          }
          if (!seen) {
            sharedInvalids.erase(sharedInvalids.begin() + k);
            k--;
          }
        }
      }
    }
  }
  for (int i : s.getInvalidValues()) {
    for (int j = 0; j < (int) sharedInvalids.size(); ++j) {
      if (i == sharedInvalids[j]) {
        sharedInvalids.erase(sharedInvalids.begin() + j);
        break;
      }
    }
  }
  if ((int) sharedInvalids.size() == 1) {
    s.setValue(sharedInvalids[0]);
  }
}

void Board::initializeInvalidsAllSpaces() {
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (board[i][j].getValue() == -1) {
        findInvalids(board[i][j]);
      }
    }
  }
}

void Board::commonInvalidsInLine(Space &s) {
  //  Check row
  std::vector<int> sharedInvalids;
  bool firstValid = true;
  for (int i = 0; i < 9; ++i) {
    if (i == s.getCol()) continue;
    if (board[s.getRow()][i].getValue() == -1) {
      if (firstValid) {
        sharedInvalids = board[s.getRow()][i].getInvalidValues();
        firstValid = false;
        continue;
      }
      auto invalids = board[s.getRow()][i].getInvalidValues();
      for (int j = 0; j < (int) sharedInvalids.size(); ++j) {
        bool seen = false;
        for (int invalid : invalids) {
          if (sharedInvalids[j] == invalid) {
            seen = true;
            break;
          }
        }
        if (!seen) {
          sharedInvalids.erase(sharedInvalids.begin() + j);
          j--;
        }
      }
    }
  }
  for (int i : s.getInvalidValues()) {
    for (int j = 0; j < (int) sharedInvalids.size(); ++j) {
      if (i == sharedInvalids[j]) {
        sharedInvalids.erase(sharedInvalids.begin() + j);
        break;
      }
    }
  }
  if ((int) sharedInvalids.size() == 1) {
    s.setValue(sharedInvalids[0]);
    return;
  }
  //  Check column
  firstValid = true;
  for (int i = 0; i < 9; ++i) {
    if (i == s.getRow()) continue;
    if (board[i][s.getCol()].getValue() == -1) {
      if (firstValid) {
        sharedInvalids = board[i][s.getCol()].getInvalidValues();
        firstValid = false;
        continue;
      }
      auto invalids = board[i][s.getCol()].getInvalidValues();
      for (int j = 0; j < (int) sharedInvalids.size(); ++j) {
        bool seen = false;
        for (int invalid : invalids) {
          if (sharedInvalids[j] == invalid) {
            seen = true;
            break;
          }
        }
        if (!seen) {
          sharedInvalids.erase(sharedInvalids.begin() + j);
          j--;
        }
      }
    }
  }
  for (int i : s.getInvalidValues()) {
    for (int j = 0; j < (int) sharedInvalids.size(); ++j) {
      if (i == sharedInvalids[j]) {
        sharedInvalids.erase(sharedInvalids.begin() + j);
        break;
      }
    }
  }
  if ((int) sharedInvalids.size() == 1) {
    s.setValue(sharedInvalids[0]);
  }
}

std::vector<std::vector<Space>> &Board::getBoard() {
  return board;
}

bool Board::isSolved() const {
  return solved;
}

void Board::setSolved(bool s) {
  Board::solved = s;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <puzzle.txt>" << std::endl;
    exit(1);
  }

  std::ifstream puzzleFile(argv[1]);

  if (!puzzleFile.is_open()) {
    std::cerr << "Error: cannot open file " << argv[1] << std::endl;
    exit(1);
  }

  // Read in board from file and create board.
  Board b;
  std::string row;

  for (int i = 0; i < 9; ++i) {
    puzzleFile >> row;
    std::vector<Space> r;
    for (int j = 0; j < 9; ++j) {
      if (row[j] == 'x') r.emplace_back(Space(-1, i, j));
      else r.emplace_back(Space(row[j] - '0', i, j));
    }
    b.addRow(r);
  }

  b.solveBoard();

  if (!b.isSolved()) b = backtrack(b, 0, 0);

  b.printBoard();
  return 0;
}
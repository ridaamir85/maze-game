# 🎮 Maze Game (C++)

A console-based maze game built in C++ with multiple difficulty levels, real-time timer, move tracking, and a persistent leaderboard system.

---

## 📌 Features

- 3 difficulty levels — Easy (10×10), Medium (15×15), Hard (20×20)
- Real-time timer and move counter
- Colorful console display using Windows API
- Player recognition via CNIC — returning players are welcomed back automatically
- Persistent leaderboard saved to file per difficulty
- Obstacle regeneration option during gameplay
- Input validation for name and CNIC

---

## 🗺️ Map Symbols

| Symbol | Meaning       |
|--------|---------------|
| `P`    | Player        |
| `S`    | Start point   |
| `E`    | End point     |
| `#`    | Wall          |
| `X`    | Obstacle      |
| `@`    | Obstacle      |
| `.`    | Open path     |

---

## 🎯 How to Play

1. Enter your CNIC (13 digits) — returning players are recognized automatically
2. Enter your name (first time only)
3. Select difficulty level
4. Navigate the maze using **Arrow Keys**
5. Reach **E** (End) from **S** (Start) in as few moves as possible

### Controls

| Key          | Action                        |
|--------------|-------------------------------|
| `Arrow Keys` | Move player                   |
| `T`          | Regenerate obstacles & restart|
| `Q`          | Quit current game             |

---

## 📊 Scoring System

- Score = total number of moves taken
- Lower score = better performance
- Timer tracks how long you take to complete the maze
- Leaderboard saves and displays top scores per difficulty level

---

## 🔧 Requirements

- Windows OS (uses `windows.h` and `conio.h`)
- C++ compiler — g++ with MinGW recommended

---

## ▶️ How to Run

```bash
# Compile
g++ maze.cpp -o maze.exe

# Run
./maze.exe
```

---

## 📁 Project Structure

```
maze-game/
│
├── maze.cpp        # Main source file (all game logic)
└── README.md       # Project documentation
```

> Note: `easy.txt`, `medium.txt`, and `hard.txt` are generated automatically when you play and save leaderboard data.

---

## 👩‍💻 Author

**Rida Amir**  
BSCS Student @ PUCIT - FCIT, University of Punjab  
🔗 [LinkedIn](https://www.linkedin.com/in/rida-amir-b606a836b) | [GitHub](https://github.com/ridaamir85)

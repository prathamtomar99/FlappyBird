# FlappyBird 

A clone of the classic Flappy Bird game built from scratch using **C++** and the **SFML** (Simple and Fast Multimedia Library). This project features smooth physics, procedurally generated obstacles, and high-score tracking.

## Prerequisites

Before you begin, ensure you have the following installed on your system (optimized for macOS):

* **Homebrew:** To manage your packages.
* **CMake (3.16+):** To handle the build process.
* **SFML (2.6.2):** The multimedia library for graphics, windowing, and system events.

### Installation via Terminal (macOS)

```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake and SFML
brew install cmake sfml@2

```

## 📂 Project Structure

```text
FlappyBird/
├── assets/             # Game textures and fonts
├── build/              # Compiled binaries (git ignored)
├── Header Files/       # .h header files
│   ├── Bird.h
│   ├── Game.h
│   ├── Globals.h
│   └── Pipe.h
├── src/                # .cpp source files
│   ├── Bird.cpp
│   ├── Game.cpp
│   ├── main.cpp
│   └── Pipe.cpp
├── CMakeLists.txt      # Build configuration
└── README.md

```

## 🚀 How to Build and Run

1. **Clone the repository:**
```bash
git clone https://github.com/prathamtomar99/FlappyBird.git
cd FlappyBird

```


2. **Generate the build files:**
```bash
mkdir build && cd build
cmake ..

```


3. **Compile the project:**
```bash
make

```


4. **Run the game:**
```bash
./FlappyBird

```



## 🕹 Features

* **Smooth Physics:** Realistic gravity and upward impulse mechanics for the bird.
* **Dynamic Obstacles:** Procedurally generated pipes that vary in height.
* **Asset Management:** Automatic asset copying to the build directory via CMake post-build commands.
* **Object-Oriented Design:** Modular code structure for easy expansion.

## 📜 License

This project is open-source. Feel free to use it for learning or as a base for your own SFML projects!

---
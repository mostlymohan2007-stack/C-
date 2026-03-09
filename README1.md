# 🚀 Space Battle Game - C++ SFML Project

A complete 2D space shooter game built with C++ and SFML, featuring real-time gameplay, custom graphics support, and online multiplayer capabilities.

## 🎮 Features

- **Real-time space combat** with smooth animations
- **Custom graphics support** - Upload your own images!
- **Multiple enemy types** with different behaviors
- **Particle effects** and explosions
- **Score & health systems**
- **Progressive difficulty** scaling
- **Cross-platform** (Windows, Linux, Mac)
- **Online hosting** ready

## 🕹️ Game Controls

- **Arrow Keys** - Move spaceship
- **Space** - Fire lasers
- **ESC** - Exit game

## 📁 Project Structure

```
SpaceBattleGame/
├── src/                    # Source code files
│   ├── main.cpp           # Game entry point
│   ├── Game.cpp           # Main game logic
│   ├── Player.cpp         # Player class
│   ├── Enemy.cpp          # Enemy class
│   ├── Bullet.cpp         # Bullet physics
│   └── Collision.cpp      # Collision detection
├── include/                # Header files
│   ├── Game.h
│   ├── Player.h
│   ├── Enemy.h
│   ├── Bullet.h
│   └── Collision.h
├── assets/                 # Game assets
│   ├── player.png         # Player ship (40x40)
│   ├── enemy.png          # Enemy ships (30x30)
│   └── bullet.png         # Laser bullets (5x10)
├── SpaceBattleGame.exe     # Compiled Windows executable
├── index.html             # Web hosting page
├── play_console.html       # Browser console version
└── README.md              # This file
```

## 🛠️ Installation & Setup

### For Windows Users:
1. **Install SFML 2.6.1** from [SFML Website](https://www.sfml-dev.org)
2. **Compile** using `compile_sfml261_fixed.bat`
3. **Run** `SpaceBattleGame.exe`

### For Custom Graphics:
1. **Copy images** to game folder
2. **Rename** to supported names:
   - `player.png`, `my_ship.png`, `custom_player.png`
   - `enemy.png`, `alien.png`, `invader.png`
   - `bullet.png`, `laser.png`, `plasma.png`
3. **Restart game** - Images load automatically!

## 🌐 Online Hosting

### GitHub Pages (Recommended):
1. **Push all files** to your GitHub repository
2. **Enable GitHub Pages** in repository settings
3. **Game is live** at: `https://yourusername.github.io/C-`

### Alternative Hosting:
- **Netlify** - Free custom domains
- **Replit** - Play directly online
- **Glitch** - Instant deployment

## 🎯 How to Play

1. **Start game** with SPACE key
2. **Move** using arrow keys
3. **Shoot** enemies with SPACE
4. **Avoid** enemy fire and collisions
5. **Survive** as long as possible
6. **Score** points by destroying enemies

## 🏆 Scoring System

- **Basic Enemy (●)**: 10 points
- **Fast Enemy (◆)**: 15 points  
- **Heavy Enemy (■)**: 20 points
- **Difficulty increases** over time

## 🎨 Customization

### Adding Your Own Graphics:
- **Player Ship**: 40x40 pixels, transparent background
- **Enemy Ships**: 30x30 pixels, multiple types
- **Bullets**: 5x10 pixels, laser effects
- **Formats**: PNG, JPG, BMP, TGA supported

### Game automatically:
- ✅ Detects custom images
- ✅ Falls back to colored shapes
- ✅ Supports multiple naming options
- ✅ Works with any image size

## 🚀 Technical Details

### Technologies Used:
- **C++17** - Modern C++ features
- **SFML 2.6.1** - Graphics & audio
- **Object-Oriented Design** - Clean architecture
- **STL Containers** - Efficient data structures
- **Delta Time** - Frame-independent movement

### Design Patterns:
- **Game Loop Pattern** - Core game cycle
- **Entity Component** - Modular game objects
- **State Machine** - Game state management
- **Observer Pattern** - Input handling

### Algorithms:
- **AABB Collision Detection** - Efficient physics
- **Rule-based AI** - Enemy behavior
- **Randomized Spawning** - Dynamic gameplay
- **Progressive Difficulty** - Adaptive challenge

## 🐛 Troubleshooting

### Common Issues:
1. **SFML DLLs missing** → Copy from SFML/bin folder
2. **Images not loading** → Check file names & paths
3. **Compilation errors** → Verify SFML installation
4. **Game crashes** → Update graphics drivers

### Performance Tips:
- **Use Release build** for better performance
- **Keep images small** for faster loading
- **Close background applications**
- **Update graphics drivers**

## 📚 Educational Value

This project demonstrates:
- **Game development fundamentals**
- **Object-oriented programming**
- **Real-time graphics rendering**
- **Physics simulation**
- **User input handling**
- **File I/O operations**
- **Memory management**
- **Cross-platform development**

## 🤝 Contributing

Feel free to:
- **Report bugs** and issues
- **Suggest new features**
- **Submit pull requests**
- **Add new enemy types**
- **Improve graphics**
- **Optimize performance**

## 📄 License

Educational project - Open for learning and modification.

---

**🎮 Ready to play! Compile, customize, and enjoy your space battle game!**

**Made with ❤️ for C++ game development education**
